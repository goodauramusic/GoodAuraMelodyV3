#pragma once

#include <JuceHeader.h>
#include "MelodyEngine.h"

#include <array>
#include <vector>

// =====================================================
// GOOD AURA MELODY - PIANO ROLL VISUALIZER
// =====================================================

class PianoRollView
    : public juce::Component,
      private juce::Timer
{
public:
    PianoRollView();

    ~PianoRollView() override = default;

    void paint(
        juce::Graphics& g) override;

    void resized() override;

    // =================================================
    // GENERATED NOTE DATA
    // =================================================

    void setNotes(
        const std::vector<
            MelodyEngine::NoteEvent>& newNotes);

    // =================================================
    // CURRENT CHORD PROGRESSION
    // =================================================

    void setProgression(
        const std::array<
            MelodyEngine::ChordChoice,
            4>& newProgression);

    // =================================================
    // PLAYHEAD
    // =================================================

    void setPlayheadBeat(
        double beat);

    void setPlaying(
        bool shouldPlay);

    // =================================================
    // SHOW / HIDE NOTE LAYERS
    // =================================================

    void setShowChords(
        bool shouldShow);

    void setShowMelody(
        bool shouldShow);

    void setShowCounter(
        bool shouldShow);

private:
    // =================================================
    // MUSICAL DATA
    // =================================================

    std::vector<
        MelodyEngine::NoteEvent>
        notes;

    std::array<
        MelodyEngine::ChordChoice,
        4>
        progression {};

    // =================================================
    // PLAYBACK
    // =================================================

    double playheadBeat =
        0.0;

    bool playing =
        false;

    // =================================================
    // LAYER VISIBILITY
    // =================================================

    bool showChords =
        true;

    bool showMelody =
        true;

    bool showCounter =
        true;

    // =================================================
    // PIANO ROLL RANGE
    // =================================================

    int lowestNote =
        40;

    int highestNote =
        92;

    static constexpr double
        totalBeats =
            16.0;

    // =================================================
    // VISUAL LAYOUT
    // =================================================

    float keyboardWidth =
        58.0f;

    float chordHeaderHeight =
        34.0f;

    // =================================================
    // TIMER
    // =================================================

    void timerCallback() override;

    // =================================================
    // DRAWING HELPERS
    // =================================================

    float beatToX(
        double beat) const;

    float noteToY(
        int midiNote) const;

    float noteHeight() const;

    juce::Colour colourForChannel(
        int midiChannel) const;

    juce::String chordName(
        const MelodyEngine::ChordChoice&
            chord) const;

    bool shouldDrawChannel(
        int midiChannel) const;

    void drawBackground(
        juce::Graphics& g);

    void drawKeyboard(
        juce::Graphics& g);

    void drawGrid(
        juce::Graphics& g);

    void drawChordHeaders(
        juce::Graphics& g);

    void drawNotes(
        juce::Graphics& g);

    void drawPlayhead(
        juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PianoRollView
    )
};
