#pragma once

#include <JuceHeader.h>
#include "MelodyEngine.h"

#include <array>
#include <atomic>
#include <vector>

class PreviewSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override
    {
        return true;
    }

    bool appliesToChannel(int) override
    {
        return true;
    }
};

class PreviewVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<PreviewSound*>(sound) != nullptr;
    }

    void startNote(
        int midiNoteNumber,
        float velocity,
        juce::SynthesiserSound*,
        int) override
    {
        currentAngle = 0.0;
        level = velocity * 0.14;

        angleDelta =
            juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber)
            * juce::MathConstants<double>::twoPi
            / getSampleRate();

        tailOff = 0.0;
    }

    void stopNote(
        float,
        bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved(int) override
    {
    }

    void controllerMoved(int, int) override
    {
    }

    void renderNextBlock(
        juce::AudioBuffer<float>& output,
        int startSample,
        int numSamples) override
    {
        if (angleDelta == 0.0)
            return;

        while (--numSamples >= 0)
        {
            const auto sample =
                (float)(
                    std::sin(currentAngle)
                    * level
                    * (tailOff > 0.0 ? tailOff : 1.0)
                );

            for (int channel = 0;
                 channel < output.getNumChannels();
                 ++channel)
            {
                output.addSample(
                    channel,
                    startSample,
                    sample);
            }

            currentAngle += angleDelta;
            ++startSample;

            if (tailOff > 0.0)
            {
                tailOff *= 0.994;

                if (tailOff <= 0.005)
                {
                    clearCurrentNote();
                    angleDelta = 0.0;
                    break;
                }
            }
        }
    }

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};

class GoodAuraMelodyAudioProcessor
    : public juce::AudioProcessor
{
public:
    GoodAuraMelodyAudioProcessor();

    ~GoodAuraMelodyAudioProcessor()
        override = default;

    void prepareToPlay(
        double sampleRate,
        int samplesPerBlock) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(
        const BusesLayout& layouts)
        const override;

    void processBlock(
        juce::AudioBuffer<float>&,
        juce::MidiBuffer&) override;

    juce::AudioProcessorEditor*
    createEditor() override;

    bool hasEditor() const override
    {
        return true;
    }

    const juce::String getName()
        const override
    {
        return "Good Aura Melody";
    }

    bool acceptsMidi() const override
    {
        return true;
    }

    bool producesMidi() const override
    {
        return true;
    }

    bool isMidiEffect() const override
    {
        return false;
    }

    double getTailLengthSeconds()
        const override
    {
        return 0.0;
    }

    int getNumPrograms() override
    {
        return 1;
    }

    int getCurrentProgram() override
    {
        return 0;
    }

    void setCurrentProgram(int) override
    {
    }

    const juce::String
    getProgramName(int) override
    {
        return {};
    }

    void changeProgramName(
        int,
        const juce::String&) override
    {
    }

    void getStateInformation(
        juce::MemoryBlock&) override;

    void setStateInformation(
        const void*,
        int) override;

    void generateNewPhrase();

    void setChord(
        int slot,
        int root,
        int type);

    const std::array<
        MelodyEngine::ChordChoice,
        4>&
    getProgression() const
    {
        return progression;
    }

    void startPreview();

    void stopPreview();

    bool isPreviewing() const
    {
        return previewPlaying.load();
    }

    juce::File writeMidiToTemporaryFile();

    bool exportMidiToFile(
        const juce::File& destination);

    std::atomic<int>
        melodyDensity {65};

    std::atomic<int>
        counterDensity {45};

    std::atomic<int>
        complexity {55};

    std::atomic<int>
        humanise {10};

private:
    MelodyEngine engine;

    std::array<
        MelodyEngine::ChordChoice,
        4>
    progression
    {{
        {0, 2},
        {9, 3},
        {2, 3},
        {7, 4}
    }};

    std::vector<
        MelodyEngine::NoteEvent>
    phrase;

    juce::CriticalSection
        phraseLock;

    juce::Synthesiser
        previewSynth;

    double sampleRateHz =
        44100.0;

    double fallbackBeatPosition =
        0.0;

    std::atomic<bool>
        previewPlaying {false};

    double previewBeatPosition =
        0.0;

    void emitEventsForWindow(
        juce::MidiBuffer&,
        double blockStartBeat,
        double blockEndBeat,
        double samplesPerBeat,
        int numSamples);

    bool writeMidiFile(
        const juce::File& file);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        GoodAuraMelodyAudioProcessor
    )
};
