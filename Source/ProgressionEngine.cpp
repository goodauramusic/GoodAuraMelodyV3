#include "ProgressionEngine.h"

ProgressionEngine::ProgressionEngine()
    : rng(std::random_device{}())
{
}

// =====================================================
// MENU OPTIONS
// =====================================================

juce::StringArray ProgressionEngine::genreNames()
{
    return
    {
        "Pop",
        "R&B",
        "Neo Soul",
        "Hip-Hop",
        "Trap",
        "Afrobeats",
        "Afro House",
        "House",
        "Jazz",
        "Soul",
        "Gospel",
        "Cinematic"
    };
}

juce::StringArray ProgressionEngine::moodNames()
{
    return
    {
        "Any",
        "Bright",
        "Emotional",
        "Dark",
        "Dreamy",
        "Smooth",
        "Uplifting"
    };
}

juce::StringArray ProgressionEngine::modeNames()
{
    return
    {
        "Major",
        "Minor"
    };
}

// =====================================================
// PROGRESSION LIBRARY
// =====================================================

const std::vector<ProgressionEngine::Template>&
ProgressionEngine::templates()
{
    static const std::vector<Template> data =
    {
        // =================================================
        // POP
        // =================================================

        {
            "Pop",
            "Bright",
            "I-V-vi-IV",
            {0, 4, 5, 3},
            {0, 0, 1, 0},
            false
        },

        {
            "Pop",
            "Emotional",
            "vi-IV-I-V",
            {5, 3, 0, 4},
            {1, 0, 0, 0},
            false
        },

        {
            "Pop",
            "Uplifting",
            "I-vi-IV-V",
            {0, 5, 3, 4},
            {0, 1, 0, 0},
            false
        },

        // =================================================
        // R&B
        // =================================================

        {
            "R&B",
            "Smooth",
            "Imaj7-iii7-vi7-ii7",
            {0, 2, 5, 1},
            {2, 3, 3, 3},
            false
        },

        {
            "R&B",
            "Emotional",
            "vi7-ii7-V7-Imaj7",
            {5, 1, 4, 0},
            {3, 3, 4, 2},
            false
        },

        {
            "R&B",
            "Dreamy",
            "Imaj9-vi9-ii9-V13",
            {0, 5, 1, 4},
            {11, 12, 12, 15},
            false
        },

        // =================================================
        // NEO SOUL
        // =================================================

        {
            "Neo Soul",
            "Smooth",
            "Imaj9-iii7-vi9-ii9",
            {0, 2, 5, 1},
            {11, 3, 12, 12},
            false
        },

        {
            "Neo Soul",
            "Dreamy",
            "IVmaj7-iii7-ii9-V13",
            {3, 2, 1, 4},
            {2, 3, 12, 15},
            false
        },

        {
            "Neo Soul",
            "Emotional",
            "ii9-V13-Imaj9-VI7",
            {1, 4, 0, 5},
            {12, 15, 11, 4},
            false
        },

        // =================================================
        // HIP-HOP
        // =================================================

        {
            "Hip-Hop",
            "Dark",
            "i-VI-III-VII",
            {0, 5, 2, 6},
            {1, 0, 0, 0},
            true
        },

        {
            "Hip-Hop",
            "Emotional",
            "i-iv-VI-V",
            {0, 3, 5, 4},
            {1, 1, 0, 0},
            true
        },

        // =================================================
        // TRAP
        // =================================================

        {
            "Trap",
            "Dark",
            "i-VII-VI-VII",
            {0, 6, 5, 6},
            {1, 0, 0, 0},
            true
        },

        {
            "Trap",
            "Emotional",
            "i-VI-III-VII",
            {0, 5, 2, 6},
            {1, 0, 0, 0},
            true
        },

        {
            "Trap",
            "Dreamy",
            "i-III-VII-VI",
            {0, 2, 6, 5},
            {1, 0, 0, 0},
            true
        },

        // =================================================
        // AFROBEATS
        // =================================================

        {
            "Afrobeats",
            "Bright",
            "I-V-vi-IV",
            {0, 4, 5, 3},
            {0, 0, 1, 0},
            false
        },

        {
            "Afrobeats",
            "Smooth",
            "vi-IV-I-V",
            {5, 3, 0, 4},
            {1, 0, 0, 0},
            false
        },

        // =================================================
        // AFRO HOUSE
        // =================================================

        {
            "Afro House",
            "Dark",
            "i-VI-III-VII",
            {0, 5, 2, 6},
            {1, 0, 0, 0},
            true
        },

        {
            "Afro House",
            "Emotional",
            "i-iv-VI-V",
            {0, 3, 5, 4},
            {1, 1, 0, 0},
            true
        },

        // =================================================
        // HOUSE
        // =================================================

        {
            "House",
            "Uplifting",
            "I-vi-IV-V",
            {0, 5, 3, 4},
            {0, 1, 0, 0},
            false
        },

        {
            "House",
            "Dreamy",
            "vi-IV-I-V",
            {5, 3, 0, 4},
            {1, 0, 0, 0},
            false
        },

        {
            "House",
            "Dark",
            "i-VI-III-VII",
            {0, 5, 2, 6},
            {1, 0, 0, 0},
            true
        },

        // =================================================
        // JAZZ
        // =================================================

        {
            "Jazz",
            "Smooth",
            "ii7-V7-Imaj7-VI7",
            {1, 4, 0, 5},
            {3, 4, 2, 4},
            false
        },

        {
            "Jazz",
            "Dreamy",
            "iii7-VI7-ii7-V7",
            {2, 5, 1, 4},
            {3, 4, 3, 4},
            false
        },

        {
            "Jazz",
            "Emotional",
            "ii9-V13-Imaj9-VI7",
            {1, 4, 0, 5},
            {12, 15, 11, 4},
            false
        },

        // =================================================
        // SOUL
        // =================================================

        {
            "Soul",
            "Smooth",
            "Imaj7-vi7-ii7-V7",
            {0, 5, 1, 4},
            {2, 3, 3, 4},
            false
        },

        {
            "Soul",
            "Emotional",
            "IVmaj7-iii7-ii7-V7",
            {3, 2, 1, 4},
            {2, 3, 3, 4},
            false
        },

        // =================================================
        // GOSPEL
        // =================================================

        {
            "Gospel",
            "Uplifting",
            "Imaj7-iii7-vi7-ii7",
            {0, 2, 5, 1},
            {2, 3, 3, 3},
            false
        },

        {
            "Gospel",
            "Emotional",
            "IVmaj7-VI7-ii7-V7",
            {3, 5, 1, 4},
            {2, 4, 3, 4},
            false
        },

        // =================================================
        // CINEMATIC
        // =================================================

        {
            "Cinematic",
            "Dark",
            "i-VI-III-VII",
            {0, 5, 2, 6},
            {1, 0, 0, 0},
            true
        },

        {
            "Cinematic",
            "Emotional",
            "i-iv-VI-V",
            {0, 3, 5, 4},
            {1, 1, 0, 0},
            true
        },

        {
            "Cinematic",
            "Dreamy",
            "i-III-VII-VI",
            {0, 2, 6, 5},
            {1, 0, 0, 0},
            true
        }
    };

    return data;
}

// =====================================================
// SCALE DEGREE -> NOTE
// =====================================================

int ProgressionEngine::degreeToPitchClass(
    int keyRoot,
    int degree,
    bool minorMode) const
{
    static const int majorScale[] =
    {
        0, 2, 4, 5, 7, 9, 11
    };

    static const int naturalMinor[] =
    {
        0, 2, 3, 5, 7, 8, 10
    };

    degree =
        juce::jlimit(
            0,
            6,
            degree);

    const int interval =
        minorMode
            ? naturalMinor[degree]
            : majorScale[degree];

    return
        (keyRoot + interval)
        % 12;
}

// =====================================================
// GENERATE PROGRESSION
// =====================================================

std::array<
    ProgressionEngine::ChordChoice,
    4>
ProgressionEngine::generate(
    int keyRoot,
    bool minorMode,
    const juce::String& genre,
    const juce::String& mood)
{
    std::vector<
        const Template*>
        candidates;

    // First try to match:
    //
    // Genre + Mood + Mode

    for (const auto& t :
         templates())
    {
        const bool genreMatches =
            t.genre.equalsIgnoreCase(
                genre);

        const bool moodMatches =
            mood.equalsIgnoreCase("Any")
            ||
            t.mood.equalsIgnoreCase(
                mood);

        const bool modeMatches =
            t.minorMode
            ==
            minorMode;

        if (genreMatches
            &&
            moodMatches
            &&
            modeMatches)
        {
            candidates.push_back(
                &t);
        }
    }

    // -------------------------------------------------
    // FALLBACK 1
    //
    // If the exact mood doesn't exist,
    // use another progression from that genre/mode.
    // -------------------------------------------------

    if (candidates.empty())
    {
        for (const auto& t :
             templates())
        {
            if (t.genre.equalsIgnoreCase(
                    genre)
                &&
                t.minorMode
                    ==
                minorMode)
            {
                candidates.push_back(
                    &t);
            }
        }
    }

    // -------------------------------------------------
    // FALLBACK 2
    //
    // Use any progression with the selected mode.
    // -------------------------------------------------

    if (candidates.empty())
    {
        for (const auto& t :
             templates())
        {
            if (t.minorMode
                ==
                minorMode)
            {
                candidates.push_back(
                    &t);
            }
        }
    }

    // Safety fallback.
    if (candidates.empty())
    {
        return
        {{
            {keyRoot, 0},
            {(keyRoot + 7) % 12, 0},
            {(keyRoot + 9) % 12, 1},
            {(keyRoot + 5) % 12, 0}
        }};
    }

    std::uniform_int_distribution<int>
        distribution(
            0,
            (int)candidates.size()
                - 1);

    const auto& chosen =
        *candidates[
            (size_t)distribution(rng)];

    std::array<
        ChordChoice,
        4>
        result {};

    for (int i = 0;
         i < 4;
         ++i)
    {
        result[(size_t)i].root =
            degreeToPitchClass(
                keyRoot,
                chosen.degrees[
                    (size_t)i],
                chosen.minorMode);

        result[(size_t)i].type =
            chosen.chordTypes[
                (size_t)i];
    }

    return result;
}

// =====================================================
// DISPLAY PROGRESSION
// =====================================================

juce::String ProgressionEngine::describe(
    const std::array<
        ChordChoice,
        4>& progression) const
{
    static const juce::StringArray roots =
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

    static const juce::StringArray types =
    {
        "",
        "m",
        "maj7",
        "m7",
        "7",
        "dim",
        "sus2",
        "sus4",
        "6",
        "m6",
        "9",
        "maj9",
        "m9",
        "11",
        "m11",
        "13",
        "m13",
        "add9",
        "madd9",
        "7sus4"
    };

    juce::String result;

    for (int i = 0;
         i < 4;
         ++i)
    {
        if (i > 0)
        {
            result
                << "  ->  ";
        }

        const auto& chord =
            progression[
                (size_t)i];

        const int root =
            juce::jlimit(
                0,
                11,
                chord.root);

        const int type =
            juce::jlimit(
                0,
                types.size() - 1,
                chord.type);

        result
            << roots[root]
            << types[type];
    }

    return result;
}
