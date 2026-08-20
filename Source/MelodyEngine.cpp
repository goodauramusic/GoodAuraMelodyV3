#include "MelodyEngine.h"

#include <algorithm>
#include <cmath>

// =====================================================
// CONSTRUCTOR
// =====================================================

MelodyEngine::MelodyEngine()
    : rng(std::random_device{}())
{
}

// =====================================================
// NOTE NAMES
// =====================================================

juce::StringArray MelodyEngine::rootNames()
{
    return
    {
        "C",
        "C#",
        "D",
        "Eb",
        "E",
        "F",
        "F#",
        "G",
        "Ab",
        "A",
        "Bb",
        "B"
    };
}

// =====================================================
// CHORD TYPES
// =====================================================

juce::StringArray MelodyEngine::chordNames()
{
    return
    {
        "Major",
        "Minor",
        "Maj7",
        "Min7",
        "7",
        "Dim",
        "Sus2",
        "Sus4",
        "6",
        "m6",
        "9",
        "Maj9",
        "m9",
        "11",
        "m11",
        "13",
        "m13",
        "add9",
        "madd9",
        "7sus4"
    };
}

// =====================================================
// CHORD INTERVALS
// =====================================================

std::vector<int>
MelodyEngine::chordIntervals(
    int chordType)
{
    static const std::vector<
        std::vector<int>>
        table =
    {
        {0, 4, 7},                 // Major
        {0, 3, 7},                 // Minor
        {0, 4, 7, 11},             // Maj7
        {0, 3, 7, 10},             // Min7
        {0, 4, 7, 10},             // Dominant 7
        {0, 3, 6},                 // Diminished
        {0, 2, 7},                 // Sus2
        {0, 5, 7},                 // Sus4
        {0, 4, 7, 9},              // 6
        {0, 3, 7, 9},              // m6
        {0, 4, 7, 10, 14},         // 9
        {0, 4, 7, 11, 14},         // Maj9
        {0, 3, 7, 10, 14},         // m9
        {0, 4, 7, 10, 14, 17},     // 11
        {0, 3, 7, 10, 14, 17},     // m11
        {0, 4, 7, 10, 14, 17, 21}, // 13
        {0, 3, 7, 10, 14, 17, 21}, // m13
        {0, 4, 7, 14},             // add9
        {0, 3, 7, 14},             // madd9
        {0, 5, 7, 10}              // 7sus4
    };

    return table[
        (size_t)juce::jlimit(
            0,
            (int)table.size() - 1,
            chordType)];
}

// =====================================================
// RANDOM HELPERS
// =====================================================

int MelodyEngine::randomInt(
    int low,
    int high)
{
    std::uniform_int_distribution<int>
        distribution(
            low,
            high);

    return distribution(rng);
}

bool MelodyEngine::chance(
    int percent)
{
    return
        randomInt(0, 99)
        <
        juce::jlimit(
            0,
            100,
            percent);
}

// =====================================================
// CREATE NOTE POOL FROM CURRENT CHORD
// =====================================================

std::vector<int>
MelodyEngine::makePool(
    const ChordChoice& chord,
    int low,
    int high) const
{
    std::vector<int> pool;

    const auto intervals =
        chordIntervals(
            chord.type);

    // -----------------------------------------------
    // CHORD TONES
    //
    // These receive extra weighting so the melody
    // strongly follows each generated chord.
    // -----------------------------------------------

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pitchClass =
            (midi
             - chord.root
             + 120)
            % 12;

        for (int interval :
             intervals)
        {
            if ((interval % 12)
                == pitchClass)
            {
                // Add multiple times to increase
                // probability of chord tones.
                pool.push_back(midi);
                pool.push_back(midi);
                pool.push_back(midi);

                break;
            }
        }
    }

    // -----------------------------------------------
    // COLOUR NOTES
    //
    // These give the melodies more R&B / Neo Soul /
    // modern melodic movement.
    // -----------------------------------------------

    static const int colourIntervals[] =
    {
        2,   // 9th
        5,   // 11th
        9,   // 13th / 6th
        11   // major 7 colour
    };

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pitchClass =
            (midi
             - chord.root
             + 120)
            % 12;

        for (int colour :
             colourIntervals)
        {
            if (pitchClass == colour)
            {
                pool.push_back(midi);
            }
        }
    }

    return pool;
}

// =====================================================
// CHOOSE NOTE NEAR PREVIOUS NOTE
// =====================================================

int MelodyEngine::nearestPitchFromPool(
    int previous,
    const std::vector<int>& pool,
    bool favorStep)
{
    if (pool.empty())
        return previous;

    if (!favorStep)
    {
        return pool[
            (size_t)randomInt(
                0,
                (int)pool.size() - 1)];
    }

    std::vector<
        std::pair<int, int>>
        scored;

    scored.reserve(
        pool.size());

    for (int note :
         pool)
    {
        scored.push_back(
        {
            std::abs(
                note - previous),

            note
        });
    }

    std::sort(
        scored.begin(),
        scored.end(),

        [](
            const auto& a,
            const auto& b)
        {
            return
                a.first
                <
                b.first;
        });

    const int numberOfChoices =
        juce::jmin(
            6,
            (int)scored.size());

    return scored[
        (size_t)randomInt(
            0,
            numberOfChoices - 1)]
        .second;
}

// =====================================================
// GENERATE COMPLETE MUSICAL PHRASE
//
// MIDI CHANNEL 1 = CHORDS
// MIDI CHANNEL 2 = MAIN MELODY
// MIDI CHANNEL 3 = COUNTER MELODY
// =====================================================

std::vector<
    MelodyEngine::NoteEvent>
MelodyEngine::generate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression)
{
    std::vector<
        NoteEvent>
        output;

    int previousLead =
        72;

    int previousCounter =
        55;

    // =================================================
    // ONE CHORD PER BAR
    // =================================================

    for (int bar = 0;
         bar < settings.bars;
         ++bar)
    {
        const auto chord =
            progression[
                (size_t)(bar % 4)];

        const auto intervals =
            chordIntervals(
                chord.type);

        const double barBeat =
            bar * 4.0;

        // =================================================
        // CHORD LAYER
        // CHANNEL 1
        // =================================================

        const size_t notesInVoicing =
            juce::jmin(
                (size_t)4,
                intervals.size());

        for (size_t i = 0;
             i < notesInVoicing;
             ++i)
        {
            int note =
                48
                + chord.root
                + intervals[i];

            // Keep chords in a useful
            // middle register.
            while (note > 67)
                note -= 12;

            while (note < 43)
                note += 12;

            output.push_back(
            {
                barBeat,

                3.80,

                note,

                juce::jlimit(
                    1,
                    127,
                    70 - (int)i * 3),

                1
            });
        }

        // =================================================
        // NOTE POOLS FOR THIS EXACT CHORD
        // =================================================

        const auto leadPool =
            makePool(
                chord,
                60,
                88);

        const auto counterPool =
            makePool(
                chord,
                43,
                69);

        // =================================================
        // RHYTHMIC COMPLEXITY
        // =================================================

        int divisions =
            4;

        if (settings.complexity
            >= 70)
        {
            divisions = 16;
        }
        else if (
            settings.complexity
            >= 40)
        {
            divisions = 8;
        }

        const double step =
            4.0
            /
            (double)divisions;

        // =================================================
        // MAIN MELODY
        // CHANNEL 2
        // =================================================

        for (int index = 0;
             index < divisions;
             ++index)
        {
            if (!chance(
                juce::jlimit(
                    5,
                    95,
                    settings.melodyDensity)))
            {
                continue;
            }

            // Leave occasional spaces so the melody
            // doesn't constantly play.
            if ((index == 0
                 ||
                 index == divisions - 1)
                &&
                chance(38))
            {
                continue;
            }

            int note =
                nearestPitchFromPool(
                    previousLead,
                    leadPool,
                    true);

            // -----------------------------------------
            // OCCASIONAL OCTAVE MOVEMENT
            // -----------------------------------------

            if (settings.complexity
                    > 55
                &&
                chance(12))
            {
                const int candidate =
                    note
                    +
                    (chance(50)
                        ? 12
                        : -12);

                if (candidate >= 60
                    &&
                    candidate <= 90)
                {
                    note =
                        candidate;
                }
            }

            // -----------------------------------------
            // OCCASIONAL APPROACH NOTE
            // -----------------------------------------

            if (settings.complexity
                    > 70
                &&
                chance(12))
            {
                note +=
                    chance(50)
                        ? 1
                        : -1;
            }

            note =
                juce::jlimit(
                    60,
                    90,
                    note);

            previousLead =
                note;

            double startBeat =
                barBeat
                +
                index * step;

            double noteLength =
                step * 0.86;

            if (chance(20))
            {
                noteLength =
                    step * 1.65;
            }

            if (chance(12))
            {
                noteLength =
                    step * 0.45;
            }

            int velocity =
                randomInt(
                    78,
                    108);

            // -----------------------------------------
            // HUMANISATION
            // -----------------------------------------

            if (settings.humanise
                > 0)
            {
                const double
                    maxShift =
                        (settings.humanise
                         / 100.0)
                        *
                        step
                        *
                        0.20;

                const double shift =
                    (randomInt(
                        -100,
                        100)
                     / 100.0)
                    *
                    maxShift;

                startBeat +=
                    shift;

                startBeat =
                    juce::jmax(
                        barBeat,
                        startBeat);

                velocity +=
                    randomInt(
                        -settings.humanise
                            / 3,

                        settings.humanise
                            / 3);
            }

            output.push_back(
            {
                startBeat,

                noteLength,

                note,

                juce::jlimit(
                    1,
                    127,
                    velocity),

                2
            });
        }

        // =================================================
        // COUNTER MELODY
        // CHANNEL 3
        //
        // Counter notes are placed more often between
        // the main melody positions.
        // =================================================

        for (int index = 1;
             index < divisions;
             index += 2)
        {
            if (!chance(
                juce::jlimit(
                    0,
                    100,
                    settings.counterDensity)))
            {
                continue;
            }

            // If lead is already very dense,
            // create more room.
            if (settings.melodyDensity
                    > 75
                &&
                chance(48))
            {
                continue;
            }

            int note =
                nearestPitchFromPool(
                    previousCounter,
                    counterPool,
                    true);

            // -----------------------------------------
            // CONTRARY MOTION
            //
            // If the lead is high, counter tends lower.
            // If lead is lower, counter can rise.
            // -----------------------------------------

            if (previousLead > 76
                &&
                chance(60))
            {
                note -=
                    randomInt(
                        1,
                        3);
            }
            else if (
                previousLead < 68
                &&
                chance(60))
            {
                note +=
                    randomInt(
                        1,
                        3);
            }

            note =
                juce::jlimit(
                    43,
                    71,
                    note);

            previousCounter =
                note;

            double startBeat =
                barBeat
                +
                index * step;

            double length =
                step * 0.82;

            if (chance(22))
            {
                length =
                    step * 1.45;
            }

            int velocity =
                randomInt(
                    50,
                    78);

            // -----------------------------------------
            // COUNTER HUMANISATION
            // -----------------------------------------

            if (settings.humanise
                > 0)
            {
                const double
                    maxShift =
                        (settings.humanise
                         / 100.0)
                        *
                        step
                        *
                        0.15;

                startBeat +=
                    (randomInt(
                        -100,
                        100)
                     / 100.0)
                    *
                    maxShift;

                startBeat =
                    juce::jmax(
                        barBeat,
                        startBeat);

                velocity +=
                    randomInt(
                        -settings.humanise
                            / 4,

                        settings.humanise
                            / 4);
            }

            output.push_back(
            {
                startBeat,

                length,

                note,

                juce::jlimit(
                    1,
                    127,
                    velocity),

                3
            });
        }
    }

    // =================================================
    // SORT EVERYTHING INTO PLAYBACK ORDER
    // =================================================

    std::sort(
        output.begin(),
        output.end(),

        [](
            const NoteEvent& a,
            const NoteEvent& b)
        {
            if (a.beat
                ==
                b.beat)
            {
                return
                    a.note
                    <
                    b.note;
            }

            return
                a.beat
                <
                b.beat;
        });

    return output;
}
