#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "PianoRollView.h"

#include <memory>

// =====================================================
// GOOD AURA MELODY EDITOR
// =====================================================

class GoodAuraMelodyAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::Timer
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
    // =====================================================

    GoodAuraMelodyAudioProcessor&
        processor;

    // =================================================
    // TITLE
    // =====================================================

    juce::Label title;
    juce::Label subtitle;

    // =================================================
    // PROGRESSION CONTROLS
    // =====================================================

    juce::ComboBox keyBox;
    juce::ComboBox modeBox;
    juce::ComboBox genreBox;
    juce::ComboBox moodBox;

    // =================================================
    // MELODY STYLE
    // =====================================================

    juce::ComboBox melodyStyleBox;

    // =================================================
    // PROGRESSION DISPLAY
    // =====================================================

    juce::Label progressionLabel;

    // =================================================
    // GENERATOR KNOBS
    // =====================================================

    juce::Slider melodyDensity;
    juce::Slider counterDensity;
    juce::Slider complexity;
    juce::Slider humanise;
    juce::Slider repetition;
    juce::Slider movement;

    // =================================================
    // GENERATOR BUTTONS
    // =====================================================

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
    // =====================================================

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
    // =====================================================

    juce::TextButton
        exportButton
        {
            "EXPORT MIDI"
        };

    // =================================================
    // PIANO ROLL
    // =====================================================

    PianoRollView
        pianoRoll;

    // =================================================
    // LAYER VISIBILITY
    // =====================================================

    juce::ToggleButton
        showChordsButton
        {
            "Chords"
        };

    juce::ToggleButton
        showMelodyButton
        {
            "Melody"
        };

    juce::ToggleButton
        showCounterButton
        {
            "Counter"
        };

    // =================================================
    // STATUS
    // =====================================================

    juce::Label status;

    // =================================================
    // FILE CHOOSER
    // =====================================================

    std::unique_ptr<
        juce::FileChooser>
        fileChooser;

    // =================================================
    // HELPERS
    // =====================================================

    void configureSlider(
        juce::Slider& slider,
        const juce::String& name,
        int initialValue);

    void syncGeneratorSettings();

    void refreshProgressionText();

    void refreshPianoRoll();

    // =================================================
    // TIMER
    //
    // Reads the live preview beat from the
    // processor and updates the playhead.
    // =====================================================

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        GoodAuraMelodyAudioProcessorEditor
    )
};
