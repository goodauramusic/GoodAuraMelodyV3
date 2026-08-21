#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

// =====================================================
// CONSTRUCTOR
// =====================================================

GoodAuraMelodyAudioProcessor::
GoodAuraMelodyAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withOutput(
                "Output",
                juce::AudioChannelSet::stereo(),
                true))
{
    for (int i = 0; i < 12; ++i)
    {
        previewSynth.addVoice(
            new PreviewVoice());
    }

    previewSynth.addSound(
        new PreviewSound());

    generateProgression();
}

// =====================================================
// PREPARE
// =====================================================

void GoodAuraMelodyAudioProcessor::
prepareToPlay(
    double sampleRate,
    int)
{
    sampleRateHz =
        sampleRate;

    previewBeatPosition.store(
        0.0);

    previewSynth
        .setCurrentPlaybackSampleRate(
            sampleRateHz);
}

// =====================================================
// RELEASE
// =====================================================

void GoodAuraMelodyAudioProcessor::
releaseResources()
{
}

// =====================================================
// BUS LAYOUT
// =====================================================

bool GoodAuraMelodyAudioProcessor::
isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    return
        layouts
            .getMainOutputChannelSet()
        ==
        juce::AudioChannelSet::stereo();
}

// =====================================================
// PROGRESSION SETTINGS
// =====================================================

void GoodAuraMelodyAudioProcessor::
setKeyRoot(
    int root)
{
    keyRoot =
        juce::jlimit(
            0,
            11,
            root);
}

void GoodAuraMelodyAudioProcessor::
setMinorMode(
    bool minor)
{
    minorMode =
        minor;
}

void GoodAuraMelodyAudioProcessor::
setGenre(
    const juce::String& newGenre)
{
    genre =
        newGenre;
}

void GoodAuraMelodyAudioProcessor::
setMood(
    const juce::String& newMood)
{
    mood =
        newMood;
}

// =====================================================
// ADVANCED MELODY SETTINGS
// =====================================================

void GoodAuraMelodyAudioProcessor::
setMelodyStyle(
    const juce::String& style)
{
    melodyStyle =
        style;
}

void GoodAuraMelodyAudioProcessor::
setSection(
    const juce::String& newSection)
{
    section =
        newSection;
}

void GoodAuraMelodyAudioProcessor::
setContour(
    const juce::String& newContour)
{
    contour =
        newContour;
}

void GoodAuraMelodyAudioProcessor::
setPocket(
    const juce::String& newPocket)
{
    pocket =
        newPocket;
}

void GoodAuraMelodyAudioProcessor::
setCounterMode(
    const juce::String& newCounterMode)
{
    counterMode =
        newCounterMode;
}

// =====================================================
// GENERATE PROGRESSION
// =====================================================

void GoodAuraMelodyAudioProcessor::
generateProgression()
{
    const auto generated =
        progressionEngine.generate(
            keyRoot,
            minorMode,
            genre,
            mood);

    for (int i = 0;
         i < 4;
         ++i)
    {
        progression[
            (size_t)i]
            .root =
                generated[
                    (size_t)i]
                    .root;

        progression[
            (size_t)i]
            .type =
                generated[
                    (size_t)i]
                    .type;
    }

    generateMelodies();
}

// =====================================================
// GENERATE ADVANCED V6 MELODY + COUNTER
// =====================================================

void GoodAuraMelodyAudioProcessor::
generateMelodies()
{
    MelodyEngine::Settings
        settings;

    settings.bars =
        4;

    settings.melodyDensity =
        melodyDensity.load();

    settings.counterDensity =
        counterDensity.load();

    settings.complexity =
        complexity.load();

    settings.humanise =
        humanise.load();

    settings.repetition =
        repetition.load();

    settings.movement =
        movement.load();

    settings.hookStrength =
        hookStrength.load();

    settings.variation =
        variation.load();

    settings.surprise =
        surprise.load();

    settings.tension =
        tension.load();

    settings.syncopation =
        syncopation.load();

    settings.restAmount =
        restAmount.load();

    settings.registerSpread =
        registerSpread.load();

    settings.style =
        melodyStyle;

    settings.section =
        section;

    settings.contour =
        contour;

    settings.pocket =
        pocket;

    settings.counterMode =
        counterMode;

    auto generated =
        melodyEngine.generate(
            settings,
            progression);

    {
        const juce::ScopedLock lock(
            phraseLock);

        phrase =
            std::move(
                generated);
    }

    previewBeatPosition.store(
        0.0);
}

// =====================================================
// PROGRESSION TEXT
// =====================================================

juce::String
GoodAuraMelodyAudioProcessor::
getProgressionText() const
{
    std::array<
        ProgressionEngine::ChordChoice,
        4>
    converted {};

    for (int i = 0;
         i < 4;
         ++i)
    {
        converted[
            (size_t)i]
            .root =
                progression[
                    (size_t)i]
                    .root;

        converted[
            (size_t)i]
            .type =
                progression[
                    (size_t)i]
                    .type;
    }

    return
        progressionEngine
            .describe(
                converted);
}

// =====================================================
// PREVIEW
// =====================================================

void GoodAuraMelodyAudioProcessor::
startPreview()
{
    previewSynth.allNotesOff(
        0,
        false);

    previewBeatPosition.store(
        0.0);

    previewPlaying.store(
        true);
}

void GoodAuraMelodyAudioProcessor::
stopPreview()
{
    previewPlaying.store(
        false);

    previewBeatPosition.store(
        0.0);

    previewSynth.allNotesOff(
        0,
        false);
}

// =====================================================
// EMIT MIDI
// =====================================================

void GoodAuraMelodyAudioProcessor::
emitEventsForWindow(
    juce::MidiBuffer& midi,
    double blockStartBeat,
    double blockEndBeat,
    double samplesPerBeat,
    int numSamples)
{
    constexpr double loopLength =
        16.0;

    auto emitSection =
        [&](
            double localStart,
            double localEnd,
            double absoluteStart)
        {
            for (const auto& event :
                 phrase)
            {
                const double noteOnBeat =
                    event.beat;

                const double noteOffBeat =
                    event.beat
                    +
                    event.lengthBeats;

                if (noteOnBeat >= localStart
                    &&
                    noteOnBeat < localEnd)
                {
                    const double absoluteBeat =
                        absoluteStart
                        +
                        (
                            noteOnBeat
                            -
                            localStart
                        );

                    const int samplePosition =
                        juce::jlimit(
                            0,
                            juce::jmax(
                                0,
                                numSamples - 1),

                            (int)std::floor(
                                (
                                    absoluteBeat
                                    -
                                    blockStartBeat
                                )
                                *
                                samplesPerBeat));

                    midi.addEvent(
                        juce::MidiMessage::
                            noteOn(
                                event.channel,
                                event.note,
                                (juce::uint8)
                                    event.velocity),

                        samplePosition);
                }

                if (noteOffBeat >= localStart
                    &&
                    noteOffBeat < localEnd)
                {
                    const double absoluteBeat =
                        absoluteStart
                        +
                        (
                            noteOffBeat
                            -
                            localStart
                        );

                    const int samplePosition =
                        juce::jlimit(
                            0,
                            juce::jmax(
                                0,
                                numSamples - 1),

                            (int)std::floor(
                                (
                                    absoluteBeat
                                    -
                                    blockStartBeat
                                )
                                *
                                samplesPerBeat));

                    midi.addEvent(
                        juce::MidiMessage::
                            noteOff(
                                event.channel,
                                event.note),

                        samplePosition);
                }
            }
        };

    double cursor =
        blockStartBeat;

    while (cursor <
           blockEndBeat)
    {
        const double localStart =
            std::fmod(
                std::fmod(
                    cursor,
                    loopLength)
                +
                loopLength,

                loopLength);

        const double remaining =
            loopLength
            -
            localStart;

        const double sectionEnd =
            juce::jmin(
                blockEndBeat,
                cursor
                +
                remaining);

        const double localEnd =
            localStart
            +
            (
                sectionEnd
                -
                cursor
            );

        emitSection(
            localStart,
            localEnd,
            cursor);

        cursor =
            sectionEnd;
    }
}

// =====================================================
// AUDIO + MIDI PROCESSING
// =====================================================

void GoodAuraMelodyAudioProcessor::
processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals
        noDenormals;

    buffer.clear();

    double bpm =
        120.0;

    if (auto* playHead =
        getPlayHead())
    {
        if (auto position =
            playHead->getPosition())
        {
            if (auto currentBpm =
                position->getBpm())
            {
                bpm =
                    *currentBpm;
            }
        }
    }

    bpm =
        juce::jmax(
            1.0,
            bpm);

    const double samplesPerBeat =
        sampleRateHz
        *
        60.0
        /
        bpm;

    const double blockBeats =
        buffer.getNumSamples()
        /
        samplesPerBeat;

    juce::MidiBuffer
        generatedMidi;

    if (previewPlaying.load())
    {
        const double startBeat =
            previewBeatPosition.load();

        {
            const juce::ScopedLock lock(
                phraseLock);

            emitEventsForWindow(
                generatedMidi,
                startBeat,
                startBeat + blockBeats,
                samplesPerBeat,
                buffer.getNumSamples());
        }

        double nextBeat =
            startBeat
            +
            blockBeats;

        if (nextBeat >= 16.0)
        {
            nextBeat =
                std::fmod(
                    nextBeat,
                    16.0);
        }

        previewBeatPosition.store(
            nextBeat);
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
}

// =====================================================
// MIDI EXPORT
// =====================================================

bool GoodAuraMelodyAudioProcessor::
writeMidiFile(
    const juce::File& file)
{
    std::vector<
        MelodyEngine::NoteEvent>
        localPhrase;

    {
        const juce::ScopedLock lock(
            phraseLock);

        localPhrase =
            phrase;
    }

    if (localPhrase.empty())
        return false;

    constexpr int ticksPerQuarter =
        960;

    juce::MidiMessageSequence
        chordTrack;

    juce::MidiMessageSequence
        melodyTrack;

    juce::MidiMessageSequence
        counterTrack;

    for (const auto& event :
         localPhrase)
    {
        auto noteOn =
            juce::MidiMessage::
                noteOn(
                    event.channel,
                    event.note,
                    (juce::uint8)
                        event.velocity);

        noteOn.setTimeStamp(
            event.beat
            *
            ticksPerQuarter);

        auto noteOff =
            juce::MidiMessage::
                noteOff(
                    event.channel,
                    event.note);

        noteOff.setTimeStamp(
            (
                event.beat
                +
                event.lengthBeats
            )
            *
            ticksPerQuarter);

        juce::MidiMessageSequence*
            destinationTrack =
                nullptr;

        if (event.channel == 1)
        {
            destinationTrack =
                &chordTrack;
        }
        else if (
            event.channel == 2)
        {
            destinationTrack =
                &melodyTrack;
        }
        else
        {
            destinationTrack =
                &counterTrack;
        }

        destinationTrack
            ->addEvent(
                noteOn);

        destinationTrack
            ->addEvent(
                noteOff);
    }

    chordTrack
        .updateMatchedPairs();

    melodyTrack
        .updateMatchedPairs();

    counterTrack
        .updateMatchedPairs();

    juce::MidiFile
        midiFile;

    midiFile
        .setTicksPerQuarterNote(
            ticksPerQuarter);

    midiFile.addTrack(
        chordTrack);

    midiFile.addTrack(
        melodyTrack);

    midiFile.addTrack(
        counterTrack);

    if (file.existsAsFile())
    {
        if (!file.deleteFile())
            return false;
    }

    juce::FileOutputStream
        stream(
            file);

    if (!stream.openedOk())
        return false;

    return midiFile.writeTo(
        stream);
}

// =====================================================
// TEMP MIDI
// =====================================================

juce::File
GoodAuraMelodyAudioProcessor::
writeMidiToTemporaryFile()
{
    const auto file =
        juce::File::
            getSpecialLocation(
                juce::File::
                    tempDirectory)
            .getNonexistentChildFile(
                "GoodAuraMelody",
                ".mid");

    if (writeMidiFile(file))
        return file;

    return {};
}

// =====================================================
// EXPORT MIDI
// =====================================================

bool GoodAuraMelodyAudioProcessor::
exportMidiToFile(
    const juce::File& destination)
{
    return writeMidiFile(
        destination);
}

// =====================================================
// SAVE STATE
// =====================================================

void GoodAuraMelodyAudioProcessor::
getStateInformation(
    juce::MemoryBlock& destination)
{
    juce::ValueTree state(
        "GoodAuraMelody");

    state.setProperty(
        "keyRoot",
        keyRoot,
        nullptr);

    state.setProperty(
        "minorMode",
        minorMode,
        nullptr);

    state.setProperty(
        "genre",
        genre,
        nullptr);

    state.setProperty(
        "mood",
        mood,
        nullptr);

    state.setProperty(
        "melodyStyle",
        melodyStyle,
        nullptr);

    state.setProperty(
        "section",
        section,
        nullptr);

    state.setProperty(
        "contour",
        contour,
        nullptr);

    state.setProperty(
        "pocket",
        pocket,
        nullptr);

    state.setProperty(
        "counterMode",
        counterMode,
        nullptr);

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

    state.setProperty(
        "repetition",
        repetition.load(),
        nullptr);

    state.setProperty(
        "movement",
        movement.load(),
        nullptr);

    state.setProperty(
        "hookStrength",
        hookStrength.load(),
        nullptr);

    state.setProperty(
        "variation",
        variation.load(),
        nullptr);

    state.setProperty(
        "surprise",
        surprise.load(),
        nullptr);

    state.setProperty(
        "tension",
        tension.load(),
        nullptr);

    state.setProperty(
        "syncopation",
        syncopation.load(),
        nullptr);

    state.setProperty(
        "restAmount",
        restAmount.load(),
        nullptr);

    state.setProperty(
        "registerSpread",
        registerSpread.load(),
        nullptr);

    for (int i = 0;
         i < 4;
         ++i)
    {
        state.setProperty(
            "chordRoot"
                +
                juce::String(i),

            progression[
                (size_t)i]
                .root,

            nullptr);

        state.setProperty(
            "chordType"
                +
                juce::String(i),

            progression[
                (size_t)i]
                .type,

            nullptr);
    }

    if (auto xml =
        state.createXml())
    {
        copyXmlToBinary(
            *xml,
            destination);
    }
}

// =====================================================
// LOAD STATE
// =====================================================

void GoodAuraMelodyAudioProcessor::
setStateInformation(
    const void* data,
    int size)
{
    auto xml =
        getXmlFromBinary(
            data,
            size);

    if (xml == nullptr)
        return;

    auto state =
        juce::ValueTree::
            fromXml(
                *xml);

    if (!state.isValid())
        return;

    keyRoot =
        (int)state.getProperty(
            "keyRoot",
            0);

    minorMode =
        (bool)state.getProperty(
            "minorMode",
            false);

    genre =
        state.getProperty(
            "genre",
            "R&B")
            .toString();

    mood =
        state.getProperty(
            "mood",
            "Smooth")
            .toString();

    melodyStyle =
        state.getProperty(
            "melodyStyle",
            "Smooth")
            .toString();

    section =
        state.getProperty(
            "section",
            "Verse")
            .toString();

    contour =
        state.getProperty(
            "contour",
            "Wave")
            .toString();

    pocket =
        state.getProperty(
            "pocket",
            "Centre")
            .toString();

    counterMode =
        state.getProperty(
            "counterMode",
            "Fill The Gaps")
            .toString();

    melodyDensity =
        (int)state.getProperty(
            "melodyDensity",
            65);

    counterDensity =
        (int)state.getProperty(
            "counterDensity",
            45);

    complexity =
        (int)state.getProperty(
            "complexity",
            55);

    humanise =
        (int)state.getProperty(
            "humanise",
            10);

    repetition =
        (int)state.getProperty(
            "repetition",
            60);

    movement =
        (int)state.getProperty(
            "movement",
            50);

    hookStrength =
        (int)state.getProperty(
            "hookStrength",
            65);

    variation =
        (int)state.getProperty(
            "variation",
            45);

    surprise =
        (int)state.getProperty(
            "surprise",
            20);

    tension =
        (int)state.getProperty(
            "tension",
            40);

    syncopation =
        (int)state.getProperty(
            "syncopation",
            50);

    restAmount =
        (int)state.getProperty(
            "restAmount",
            35);

    registerSpread =
        (int)state.getProperty(
            "registerSpread",
            45);

    for (int i = 0;
         i < 4;
         ++i)
    {
        progression[
            (size_t)i]
            .root =
                (int)state.getProperty(
                    "chordRoot"
                        +
                        juce::String(i),

                    progression[
                        (size_t)i]
                        .root);

        progression[
            (size_t)i]
            .type =
                (int)state.getProperty(
                    "chordType"
                        +
                        juce::String(i),

                    progression[
                        (size_t)i]
                        .type);
    }

    generateMelodies();
}

// =====================================================
// EDITOR
// =====================================================

juce::AudioProcessorEditor*
GoodAuraMelodyAudioProcessor::
createEditor()
{
    return
        new GoodAuraMelodyAudioProcessorEditor(
            *this);
}

// =====================================================
// PLUGIN ENTRY POINT
// =====================================================

juce::AudioProcessor*
JUCE_CALLTYPE
createPluginFilter()
{
    return
        new GoodAuraMelodyAudioProcessor();
}
