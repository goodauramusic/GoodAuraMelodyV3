#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

GoodAuraMelodyAudioProcessor::GoodAuraMelodyAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withOutput(
                "Output",
                juce::AudioChannelSet::stereo(),
                true))
{
    for (int i = 0; i < 12; ++i)
        previewSynth.addVoice(new PreviewVoice());

    previewSynth.addSound(new PreviewSound());

    generateNewPhrase();
}

void GoodAuraMelodyAudioProcessor::prepareToPlay(
    double sampleRate,
    int)
{
    sampleRateHz = sampleRate;
    fallbackBeatPosition = 0.0;
    previewBeatPosition = 0.0;

    previewSynth.setCurrentPlaybackSampleRate(sampleRateHz);
}

void GoodAuraMelodyAudioProcessor::releaseResources()
{
}

bool GoodAuraMelodyAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet()
        == juce::AudioChannelSet::stereo();
}

void GoodAuraMelodyAudioProcessor::setChord(
    int slot,
    int root,
    int type)
{
    if (slot < 0 || slot >= 4)
        return;

    progression[(size_t) slot].root =
        juce::jlimit(0, 11, root);

    progression[(size_t) slot].type =
        juce::jmax(0, type);
}

void GoodAuraMelodyAudioProcessor::generateNewPhrase()
{
    MelodyEngine::Settings settings;

    settings.bars = 4;
    settings.melodyDensity = melodyDensity.load();
    settings.counterDensity = counterDensity.load();
    settings.complexity = complexity.load();
    settings.humanise = humanise.load();

    auto generated =
        engine.generate(settings, progression);

    const juce::ScopedLock lock(phraseLock);

    phrase = std::move(generated);
}

void GoodAuraMelodyAudioProcessor::startPreview()
{
    previewBeatPosition = 0.0;
    previewPlaying = true;
}

void GoodAuraMelodyAudioProcessor::stopPreview()
{
    previewPlaying = false;
    previewBeatPosition = 0.0;

    previewSynth.allNotesOff(0, false);
}

void GoodAuraMelodyAudioProcessor::emitEventsForWindow(
    juce::MidiBuffer& midi,
    double blockStart,
    double blockEnd,
    double samplesPerBeat,
    int numSamples)
{
    constexpr double loopLength = 16.0;

    auto emit =
        [&](
            double localStart,
            double localEnd,
            double absoluteStart)
        {
            for (const auto& event : phrase)
            {
                const double noteOnBeat =
                    event.beat;

                const double noteOffBeat =
                    event.beat +
                    event.lengthBeats;

                if (noteOnBeat >= localStart
                    && noteOnBeat < localEnd)
                {
                    const double absoluteBeat =
                        absoluteStart +
                        (noteOnBeat - localStart);

                    const int samplePosition =
                        juce::jlimit(
                            0,
                            numSamples - 1,
                            (int) std::floor(
                                (absoluteBeat - blockStart)
                                * samplesPerBeat));

                    midi.addEvent(
                        juce::MidiMessage::noteOn(
                            event.channel,
                            event.note,
                            (juce::uint8) event.velocity),
                        samplePosition);
                }

                if (noteOffBeat >= localStart
                    && noteOffBeat < localEnd)
                {
                    const double absoluteBeat =
                        absoluteStart +
                        (noteOffBeat - localStart);

                    const int samplePosition =
                        juce::jlimit(
                            0,
                            numSamples - 1,
                            (int) std::floor(
                                (absoluteBeat - blockStart)
                                * samplesPerBeat));

                    midi.addEvent(
                        juce::MidiMessage::noteOff(
                            event.channel,
                            event.note),
                        samplePosition);
                }
            }
        };

    double cursor = blockStart;

    while (cursor < blockEnd)
    {
        const double localStart =
            std::fmod(
                std::fmod(cursor, loopLength)
                    + loopLength,
                loopLength);

        const double remaining =
            loopLength - localStart;

        const double thisEnd =
            juce::jmin(
                blockEnd,
                cursor + remaining);

        const double localEnd =
            localStart +
            (thisEnd - cursor);

        emit(
            localStart,
            localEnd,
            cursor);

        cursor = thisEnd;
    }
}

void GoodAuraMelodyAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    double bpm = 120.0;

    double blockStart =
        fallbackBeatPosition;

    bool hostPlaying = false;

    if (auto* playHead = getPlayHead())
    {
        if (auto position =
                playHead->getPosition())
        {
            if (auto hostBpm =
                    position->getBpm())
            {
                bpm = *hostBpm;
            }

            if (auto ppq =
                    position->getPpqPosition())
            {
                blockStart = *ppq;
            }

            hostPlaying =
                position->getIsPlaying();
        }
    }

    const double samplesPerBeat =
        sampleRateHz
        * 60.0
        / juce::jmax(1.0, bpm);

    const double blockBeats =
        buffer.getNumSamples()
        / samplesPerBeat;

    juce::MidiBuffer generatedMidi;

    {
        const juce::ScopedLock lock(
            phraseLock);

        if (previewPlaying.load())
        {
            blockStart =
                previewBeatPosition;

            emitEventsForWindow(
                generatedMidi,
                blockStart,
                blockStart + blockBeats,
                samplesPerBeat,
                buffer.getNumSamples());

            previewBeatPosition =
                std::fmod(
                    previewBeatPosition
                        + blockBeats,
                    16.0);
        }
        else if (hostPlaying)
        {
            emitEventsForWindow(
                generatedMidi,
                blockStart,
                blockStart + blockBeats,
                samplesPerBeat,
                buffer.getNumSamples());
        }
    }

    midi.addEvents(
        generatedMidi,
        0,
        buffer.getNumSamples(),
        0);

    previewSynth.renderNextBlock(
        buffer,
        generatedMidi,
        0,
        buffer.getNumSamples());

    fallbackBeatPosition =
        blockStart + blockBeats;
}

bool GoodAuraMelodyAudioProcessor::writeMidiFile(
    const juce::File& file)
{
    std::vector<
        MelodyEngine::NoteEvent>
        localPhrase;

    {
        const juce::ScopedLock lock(
            phraseLock);

        localPhrase = phrase;
    }

    juce::MidiMessageSequence sequence;

    constexpr int ticksPerQuarter = 960;

    for (const auto& event : localPhrase)
    {
        auto noteOn =
            juce::MidiMessage::noteOn(
                event.channel,
                event.note,
                (juce::uint8) event.velocity);

        noteOn.setTimeStamp(
            event.beat * ticksPerQuarter);

        auto noteOff =
            juce::MidiMessage::noteOff(
                event.channel,
                event.note);

        noteOff.setTimeStamp(
            (event.beat + event.lengthBeats)
            * ticksPerQuarter);

        sequence.addEvent(noteOn);
        sequence.addEvent(noteOff);
    }

    sequence.updateMatchedPairs();

    juce::MidiFile midiFile;

    midiFile.setTicksPerQuarterNote(
        ticksPerQuarter);

    midiFile.addTrack(sequence);

    file.deleteFile();

    juce::FileOutputStream stream(file);

    if (!stream.openedOk())
        return false;

    return midiFile.writeTo(stream);
}

juce::File
GoodAuraMelodyAudioProcessor::writeMidiToTemporaryFile()
{
    auto file =
        juce::File::getSpecialLocation(
            juce::File::tempDirectory)
            .getNonexistentChildFile(
                "GoodAuraMelody",
                ".mid");

    if (writeMidiFile(file))
        return file;

    return {};
}

bool GoodAuraMelodyAudioProcessor::exportMidiToFile(
    const juce::File& destination)
{
    return writeMidiFile(destination);
}

void GoodAuraMelodyAudioProcessor::getStateInformation(
    juce::MemoryBlock& destination)
{
    juce::ValueTree state("GoodAuraMelody");

    state.setProperty(
        "melodyDensity",
        melodyDensity.load(),
        nullptr);

    state.setProperty(
        "counterDensity",
        counterDensity.load(),
        nullptr);

    state.setProperty(
        "complexity",
        complexity.load(),
        nullptr);

    state.setProperty(
        "humanise",
        humanise.load(),
        nullptr);

    for (int i = 0; i < 4; ++i)
    {
        state.setProperty(
            "root" + juce::String(i),
            progression[(size_t) i].root,
            nullptr);

        state.setProperty(
            "type" + juce::String(i),
            progression[(size_t) i].type,
            nullptr);
    }

    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destination);
}

void GoodAuraMelodyAudioProcessor::setStateInformation(
    const void* data,
    int size)
{
    if (auto xml =
            getXmlFromBinary(data, size))
    {
        auto state =
            juce::ValueTree::fromXml(*xml);

        if (state.isValid())
        {
            melodyDensity =
                (int) state.getProperty(
                    "melodyDensity",
                    65);

            counterDensity =
                (int) state.getProperty(
                    "counterDensity",
                    45);

            complexity =
                (int) state.getProperty(
                    "complexity",
                    55);

            humanise =
                (int) state.getProperty(
                    "humanise",
                    10);

            for (int i = 0; i < 4; ++i)
            {
                progression[(size_t) i].root =
                    (int) state.getProperty(
                        "root" + juce::String(i),
                        0);

                progression[(size_t) i].type =
                    (int) state.getProperty(
                        "type" + juce::String(i),
                        0);
            }

            generateNewPhrase();
        }
    }
}

juce::AudioProcessorEditor*
GoodAuraMelodyAudioProcessor::createEditor()
{
    return new GoodAuraMelodyAudioProcessorEditor(
        *this);
}

juce::AudioProcessor*
JUCE_CALLTYPE createPluginFilter()
{
    return new GoodAuraMelodyAudioProcessor();
}
