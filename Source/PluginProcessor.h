#pragma once

#include <JuceHeader.h>
#include "MelodyEngine.h"
#include "ProgressionEngine.h"

#include <array>
#include <atomic>
#include <vector>

// =====================================================
// SIMPLE INTERNAL PREVIEW SOUND
// =====================================================

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
    bool canPlaySound(
        juce::SynthesiserSound* sound) override
    {
        return
            dynamic_cast<PreviewSound*>(sound)
            != nullptr;
    }

    void startNote(
        int midiNoteNumber,
        float velocity,
        juce::SynthesiserSound*,
        int) override
    {
        currentAngle = 0.0;

        level =
            velocity * 0.12;

        angleDelta =
            juce::MidiMessage::
                getMidiNoteInHertz(
                    midiNoteNumber)
            *
            juce::MathConstants<double>::
                twoPi
            /
            getSampleRate();

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

    void controllerMoved(
        int,
        int) override
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
                    std::sin(
                        currentAngle)
                    *
                    level
                    *
                    (
                        tailOff > 0.0
                        ? tailOff
                        : 1.0
                    )
                );

            for (int channel = 0;
                 channel <
                    output.getNumChannels();
                 ++channel)
            {
                output.addSample(
                    channel,
                    startSample,
                    sample);
            }

            currentAngle +=
                angleDelta;

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

// =====================================================
// MAIN PROCESSOR
// =====================================================

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

    const juce::String
    getName() const override
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

    double
    getTailLengthSeconds()
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

    // =================================================
    // GENERATION
    // =================================================

    void generateProgression();

    void generateMelodies();

    // =================================================
    // PROGRESSION SETTINGS
    // =================================================

    void setKeyRoot(
        int root);

    void setMinorMode(
        bool minor);

    void setGenre(
        const juce::String& genre);

    void setMood(
        const juce::String& mood);

    juce::String
    getProgressionText() const;

    const std::array<
        MelodyEngine::ChordChoice,
        4>&
    getProgression() const
    {
        return progression;
    }

    // =================================================
    // PLAYBACK
    // =================================================

    void startPreview();

    void stopPreview();

    bool isPreviewing() const
    {
        return
            previewPlaying.load();
    }

    // =================================================
    // MIDI EXPORT
    // =================================================

    juce::File
    writeMidiToTemporaryFile();

    bool exportMidiToFile(
        const juce::File& destination);

    // =================================================
    // MELODY CONTROLS
    // =================================================

    std::atomic<int>
        melodyDensity {65};

    std::atomic<int>
        counterDensity {45};

    std::atomic<int>
        complexity {55};

    std::atomic<int>
        humanise {10};

private:
    // =================================================
    // ENGINES
    // =================================================

    MelodyEngine
        melodyEngine;

    ProgressionEngine
        progressionEngine;

    // =================================================
    // CURRENT CHORD PROGRESSION
    // =================================================

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

    // =================================================
    // GENERATED MUSICAL EVENTS
    // =================================================

    std::vector<
        MelodyEngine::NoteEvent>
    phrase;

    juce::CriticalSection
        phraseLock;

    // =================================================
    // INTERNAL PREVIEW SYNTH
    // =================================================

    juce::Synthesiser
        previewSynth;

    // =================================================
    // PROGRESSION GENERATOR SETTINGS
    // =================================================

    int keyRoot = 0;

    bool minorMode = false;

    juce::String genre =
        "R&B";

    juce::String mood =
        "Smooth";

    // =================================================
    // PLAYBACK
    // =================================================

    double sampleRateHz =
        44100.0;

    std::atomic<bool>
        previewPlaying {false};

    double previewBeatPosition =
        0.0;

    // =================================================
    // INTERNAL HELPERS
    // =================================================

    void emitEventsForWindow(
        juce::MidiBuffer& midi,
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
