#include "MelodyEngine.h"

#include <algorithm>
#include <cmath>

MelodyEngine::MelodyEngine()
    : rng(std::random_device{}())
{
}

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

std::vector<int>
MelodyEngine::chordIntervals(int chordType)
{
    static const std::vector<std::vector<int>> table =
    {
        {0,4,7},
        {0,3,7},
        {0,4,7,11},
        {0,3,7,10},
        {0,4,7,10},
        {0,3,6},
        {0,2,7},
        {0,5,7},
        {0,4,7,9},
        {0,3,7,9},
        {0,4,7,10,14},
        {0,4,7,11,14},
        {0,3,7,10,14},
        {0,4,7,10,14,17},
        {0,3,7,10,14,17},
        {0,4,7,10,14,17,21},
        {0,3,7,10,14,17,21},
        {0,4,7,14},
        {0,3,7,14},
        {0,5,7,10}
    };

    return table[
        (size_t) juce::jlimit(
            0,
            (int) table.size() - 1,
            chordType)];
}

int MelodyEngine::randomInt(
    int low,
    int high)
{
    std::uniform_int_distribution<int>
        distribution(low, high);

    return distribution(rng);
}

bool MelodyEngine::chance(
    int percent)
{
    return
        randomInt(0, 99)
        < juce::jlimit(
            0,
            100,
            percent);
}

std::vector<int>
MelodyEngine::makePool(
    const ChordChoice& chord,
    int low,
    int high) const
{
    std::vector<int> pool;

    const auto intervals =
        chordIntervals(chord.type);

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pitchClass =
            (midi
             - chord.root
             + 120) % 12;

        for (int interval :
             intervals)
        {
            if ((interval % 12)
                == pitchClass)
            {
                pool.push_back(midi);

                // Chord tones get extra weight.
                pool.push_back(midi);

                break;
            }
        }
    }

    static const int colourIntervals[] =
    {
        2,
        5,
        9,
        11
    };

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pitchClass =
            (midi
             - chord.root
             + 120) % 12;

        for (int colour :
             colourIntervals)
        {
            if (pitchClass == colour)
                pool.push_back(midi);
        }
    }

    return pool;
}

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
            (size_t) randomInt(
                0,
                (int) pool.size() - 1)];
    }

    std::vector<
        std::pair<int, int>>
        scored;

    scored.reserve(
        pool.size());

    for (int note : pool)
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
            auto a,
            auto b)
        {
            return
                a.first
                < b.first;
        });

    const int choices =
        juce::jmin(
            6,
            (int) scored.size());

    return scored[
        (size_t) randomInt(
            0,
            choices - 1)]
        .second;
}

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

    int previousLead = 72;
    int previousCounter = 55;

    for (int bar = 0;
         bar < settings.bars;
         ++bar)
    {
        const auto chord =
            progression[
                (size_t) (bar % 4)];

        const auto intervals =
            chordIntervals(
                chord.type);

        const double barBeat =
            bar * 4.0;

        // ==========================================
        // CHORD LAYER
        // MIDI CHANNEL 1
        // ==========================================

        for (size_t i = 0;
             i < juce::jmin(
                 (size_t) 4,
                 intervals.size());
             ++i)
        {
            int note =
                48
                + chord.root
                + intervals[i];

            while (note > 67)
                note -= 12;

            while (note < 43)
                note += 12;

            output.push_back(
            {
                barBeat,
                3.80,
                note,
                66 - (int) i * 3,
                1
            });
        }

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

        const int divisions =
            settings.complexity >= 70
                ? 16
                : settings.complexity >= 40
                    ? 8
                    : 4;

        const double step =
            4.0 / divisions;

        // ==========================================
        // MAIN MELODY
        // MIDI CHANNEL 2
        // ==========================================

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

            // Give the phrase some breathing room.
            if ((index == 0
                 || index ==
                    divisions - 1)
                && chance(42))
            {
                continue;
            }

            int note =
                nearestPitchFromPool(
                    previousLead,
                    leadPool,
                    true);

            // Add occasional chromatic approach notes.
            if (settings.complexity > 65
                && chance(18))
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

            previousLead = note;

            double startBeat =
                barBeat
                + index * step;

            double noteLength =
                step
                * (chance(25)
                    ? 1.65
                    : 0.86);

            int velocity =
                randomInt(
                    78,
                    105);

            // ======================================
            // HUMANISE
            // ======================================

            if (settings.humanise > 0)
            {
                const double maxShift =
                    (settings.humanise
                     / 100.0)
                    * step
                    * 0.20;

                const double shift =
                    (randomInt(
                        -100,
                        100)
                     / 100.0)
                    * maxShift;

                startBeat += shift;

                startBeat =
                    juce::jmax(
                        barBeat,
                        startBeat);

                velocity +=
                    randomInt(
                        -settings.humanise / 3,
                        settings.humanise / 3);
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

        // ==========================================
        // COUNTER MELODY
        // MIDI CHANNEL 3
        // ==========================================

        for (int index = 1;
             index < divisions;
             index += 2)
        {
            if (!chance(
                settings.counterDensity))
            {
                continue;
            }

            if (settings.melodyDensity > 75
                && chance(45))
            {
                continue;
            }

            int note =
                nearestPitchFromPool(
                    previousCounter,
                    counterPool,
                    true);

            // Basic contrary-motion behaviour.
            if (previousLead > 74
                && chance(55))
            {
                note -= 2;
            }
            else if (previousLead < 68
                     && chance(55))
            {
                note += 2;
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
                + index * step;

            int velocity =
                randomInt(
                    52,
                    78);

            if (settings.humanise > 0)
            {
                const double maxShift =
                    (settings.humanise
                     / 100.0)
                    * step
                    * 0.15;

                startBeat +=
                    (randomInt(
                        -100,
                        100)
                     / 100.0)
                    * maxShift;

                velocity +=
                    randomInt(
                        -settings.humanise / 4,
                        settings.humanise / 4);
            }

            output.push_back(
            {
                startBeat,
                step * 0.82,
                note,
                juce::jlimit(
                    1,
                    127,
                    velocity),
                3
            });
        }
    }

    std::sort(
        output.begin(),
        output.end(),
        [](
            const NoteEvent& a,
            const NoteEvent& b)
        {
            if (a.beat == b.beat)
                return a.note < b.note;

            return a.beat < b.beat;
        });

    return output;
}
