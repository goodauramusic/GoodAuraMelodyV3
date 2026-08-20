#pragma once

#include <JuceHeader.h>

#include <array>
#include <random>
#include <vector>

class MelodyEngine
{
public:
    struct ChordChoice
    {
        int root = 0;
        int type = 0;
    };

    struct NoteEvent
    {
        double beat = 0.0;
        double lengthBeats = 0.25;

        int note = 60;
        int velocity = 90;
        int channel = 1;
    };

    struct Settings
    {
        int bars = 4;

        int melodyDensity = 65;
        int counterDensity = 45;
        int complexity = 55;
        int humanise = 10;

        int repetition = 60;
        int movement = 50;

        juce::String style = "Smooth";
    };

    struct PhraseCandidate
    {
        std::vector<NoteEvent> lead;
        std::vector<NoteEvent> counter;

        double score = 0.0;
    };

    MelodyEngine();

    static juce::StringArray rootNames();

    static juce::StringArray chordNames();

    static juce::StringArray melodyStyleNames();

    static std::vector<int>
    chordIntervals(
        int chordType);

    std::vector<NoteEvent>
    generate(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression);

private:
    std::mt19937 rng;

    int randomInt(
        int low,
        int high);

    double randomDouble(
        double low,
        double high);

    bool chance(
        int percent);

    std::vector<int>
    makeChordTonePool(
        const ChordChoice& chord,
        int low,
        int high) const;

    std::vector<int>
    makeMelodicPool(
        const ChordChoice& chord,
        int low,
        int high) const;

    int nearestPitch(
        int previous,
        const std::vector<int>& pool,
        int maximumJump);

    std::vector<double>
    makeRhythmPattern(
        const Settings& settings,
        int barIndex);

    std::vector<int>
    createMotif(
        const Settings& settings,
        const ChordChoice& chord,
        int startingNote);

    std::vector<int>
    mutateMotif(
        const std::vector<int>& motif,
        const Settings& settings,
        const ChordChoice& chord);

    std::vector<NoteEvent>
    generateLeadCandidate(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression);

    std::vector<NoteEvent>
    generateCounterCandidate(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression,
        const std::vector<NoteEvent>& lead);

    double scoreLead(
        const std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression,
        const Settings& settings) const;

    double scoreCounter(
        const std::vector<NoteEvent>& counter,
        const std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression,
        const Settings& settings) const;

    bool leadIsActiveNear(
        const std::vector<NoteEvent>& lead,
        double beat,
        double tolerance) const;

    int currentChordIndex(
        double beat) const;
};
