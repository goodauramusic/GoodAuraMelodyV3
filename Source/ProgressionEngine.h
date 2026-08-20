#pragma once

#include <JuceHeader.h>

#include <array>
#include <random>
#include <vector>

class ProgressionEngine
{
public:
    struct ChordChoice
    {
        // MIDI pitch class:
        // C = 0, C# = 1 ... B = 11
        int root = 0;

        // Matches the chord types used
        // by MelodyEngine.
        int type = 0;
    };

    struct Template
    {
        juce::String genre;
        juce::String mood;
        juce::String name;

        // Scale degrees:
        // I = 0
        // ii = 1
        // iii = 2
        // IV = 3
        // V = 4
        // vi = 5
        // vii = 6
        std::array<int, 4> degrees {};

        // Chord quality/extension
        // for each of the four chords.
        std::array<int, 4> chordTypes {};

        bool minorMode = false;
    };

    ProgressionEngine();

    // =====================================================
    // MENU OPTIONS
    // =====================================================

    static juce::StringArray genreNames();

    static juce::StringArray moodNames();

    static juce::StringArray modeNames();

    // =====================================================
    // PROGRESSION GENERATOR
    // =====================================================

    std::array<ChordChoice, 4> generate(
        int keyRoot,
        bool minorMode,
        const juce::String& genre,
        const juce::String& mood);

    // Converts the generated progression into
    // readable text such as:
    //
    // Cm9 → AbMaj7 → EbMaj7 → Bb7
    //
    juce::String describe(
        const std::array<
            ChordChoice,
            4>& progression) const;

private:
    std::mt19937 rng;

    // The progression template library.
    static const std::vector<Template>&
        templates();

    // Converts a scale degree into the
    // correct pitch class for the chosen key.
    int degreeToPitchClass(
        int keyRoot,
        int degree,
        bool minorMode) const;
};
