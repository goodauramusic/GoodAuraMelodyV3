#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "PianoRollView.h"

#include <memory>

// =====================================================
// GOOD AURA MELODY V6 EDITOR
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
    // =================================================

    GoodAuraMelodyAudioProcessor&
        processor;

    // =================================================
    // HEADER
    // =================================================

    juce::Label title;
    juce::Label subtitle;

    // =================================================
    // PROGRESSION
    // =================================================

    juce::ComboBox keyBox;
    juce::ComboBox modeBox;
    juce::ComboBox genreBox;
    juce::ComboBox moodBox;

    juce::Label progressionLabel;

    juce::TextButton
        generateProgressionButton
        {
            "GENERATE CHORDS"
        };

    // =================================================
    // SONG / ARRANGEMENT
    // =================================================

    juce::ComboBox sectionBox;
    juce::ComboBox contourBox;
    juce::ComboBox pocketBox;
    juce::ComboBox counterModeBox;

    // =================================================
    // MELODY STYLE
    // =================================================

    juce::ComboBox melodyStyleBox;

    // =================================================
    // ORIGINAL MELODY CONTROLS
    // =================================================

    juce::Slider melodyDensity;
    juce::Slider counterDensity;
    juce::Slider complexity;
    juce::Slider humanise;
    juce::Slider repetition;
    juce::Slider movement;

    // =================================================
    // V6 ADVANCED CONTROLS
    // =================================================

    juce::Slider hookStrength;
    juce::Slider variation;
    juce::Slider surprise;
    juce::Slider tension;

    juce::Slider syncopation;
    juce::Slider restAmount;
    juce::Slider registerSpread;

    // =================================================
    // MELODY GENERATION
    // =================================================

    juce::TextButton
        generateMelodyButton
        {
            "GENERATE MELODIES"
        };

    // =================================================
    // IDEA LOCK
    // =================================================

    juce::TextButton
        lockIdeaButton
        {
            "LOCK IDEA"
        };

    juce::TextButton
        unlockIdeaButton
        {
            "UNLOCK"
        };

    juce::Label
        ideaStatusLabel;

    // =================================================
    // PREVIEW
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
    // PIANO ROLL
    // =================================================

    PianoRollView
        pianoRoll;

    // =================================================
    // PIANO ROLL LAYERS
    // =================================================

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
    // =================================================

    juce::Label status;

    // =================================================
    // FILE CHOOSER
    // =================================================

    std::unique_ptr<
        juce::FileChooser>
        fileChooser;

    // =================================================
    // GUI HELPERS
    // =================================================

    void configureSlider(
        juce::Slider& slider,
        const juce::String& name,
        int initialValue);

    void configureComboBox(
        juce::ComboBox& box);

    void configureButton(
        juce::TextButton& button);

    // =================================================
    // PROCESSOR SYNC
    // =================================================

    void syncGeneratorSettings();

    void refreshProgressionText();

    void refreshPianoRoll();

    void refreshIdeaStatus();

    // =================================================
    // TIMER
    // =================================================

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        GoodAuraMelodyAudioProcessorEditor
    )
};
