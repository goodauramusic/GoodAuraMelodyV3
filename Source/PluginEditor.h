#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include <memory>

// =====================================================
// GOOD AURA MELODY V5 EDITOR
// =====================================================

class GoodAuraMelodyAudioProcessorEditor
    : public juce::AudioProcessorEditor
{
public:
    explicit GoodAuraMelodyAudioProcessorEditor(
        GoodAuraMelodyAudioProcessor&);

    ~GoodAuraMelodyAudioProcessorEditor()
        override = default;

    void paint(
        juce::Graphics&) override;

    void resized() override;

private:
    // =================================================
    // PROCESSOR
    // =================================================

    GoodAuraMelodyAudioProcessor&
        processor;

    // =================================================
    // TITLES
    // =================================================

    juce::Label title;
    juce::Label subtitle;

    // =================================================
    // PROGRESSION CONTROLS
    // =================================================

    juce::ComboBox keyBox;
    juce::ComboBox modeBox;
    juce::ComboBox genreBox;
    juce::ComboBox moodBox;

    // =================================================
    // V5 MELODY STYLE
    // =================================================

    juce::ComboBox melodyStyleBox;

    // =================================================
    // CURRENT PROGRESSION
    // =================================================

    juce::Label progressionLabel;

    // =================================================
    // MELODY CONTROLS
    // =================================================

    juce::Slider melodyDensity;
    juce::Slider counterDensity;
    juce::Slider complexity;
    juce::Slider humanise;

    // =================================================
    // NEW V5 CONTROLS
    // =================================================

    juce::Slider repetition;
    juce::Slider movement;

    // =================================================
    // GENERATION BUTTONS
    // =================================================

    juce::TextButton
        generateProgressionButton
        {
            "GENERATE CHORDS"
        };

    juce::TextButton
        generateMelodyButton
        {
            "GENERATE MELODIES"
        };

    // =================================================
    // PLAYBACK
    // =================================================

    juce::TextButton
        playButton
        {
            "PLAY"
        };

    juce::TextButton
        stopButton
        {
            "STOP"
        };

    // =================================================
    // MIDI EXPORT
    // =================================================

    juce::TextButton
        exportButton
        {
            "EXPORT MIDI"
        };

    // =================================================
    // STATUS
    // =================================================

    juce::Label status;

    // =================================================
    // FILE CHOOSER
    // =================================================

    std::unique_ptr<
        juce::FileChooser>
        fileChooser;

    // =================================================
    // HELPERS
    // =================================================

    void configureSlider(
        juce::Slider& slider,
        const juce::String& name,
        int initialValue);

    void syncGeneratorSettings();

    void refreshProgressionText();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        GoodAuraMelodyAudioProcessorEditor
    )
};
