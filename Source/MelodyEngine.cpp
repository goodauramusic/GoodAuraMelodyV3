#include "MelodyEngine.h"

#include <algorithm>
#include <cmath>
#include <numeric>

// =====================================================
// CONSTRUCTOR
// =====================================================

MelodyEngine::MelodyEngine()
    : rng(std::random_device{}())
{
}

// =====================================================
// MENU / DISPLAY DATA
// =====================================================

juce::StringArray MelodyEngine::rootNames()
{
    return
    {
        "C", "C#", "D", "Eb", "E", "F",
        "F#", "G", "Ab", "A", "Bb", "B"
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

juce::StringArray MelodyEngine::melodyStyleNames()
{
    return
    {
        "Smooth",
        "Catchy",
        "Emotional",
        "Dark",
        "Bouncy",
        "Complex",
        "Dreamy",
        "Soulful",
        "Afro",
        "Trap",
        "R&B",
        "Pop"
    };
}

juce::StringArray MelodyEngine::sectionNames()
{
    return
    {
        "Intro",
        "Verse",
        "Pre-Chorus",
        "Chorus",
        "Post-Chorus",
        "Bridge",
        "Outro"
    };
}

juce::StringArray MelodyEngine::contourNames()
{
    return
    {
        "Wave",
        "Rising",
        "Falling",
        "Arch",
        "Inverted Arch",
        "Stable"
    };
}

juce::StringArray MelodyEngine::pocketNames()
{
    return
    {
        "Ahead",
        "Centre",
        "Behind"
    };
}

juce::StringArray MelodyEngine::counterModeNames()
{
    return
    {
        "Fill The Gaps",
        "Answer",
        "Harmony",
        "Opposite Motion",
        "High Counter",
        "Low Counter",
        "Sparse",
        "Atmospheric"
    };
}

// =====================================================
// CHORD DEFINITIONS
// =====================================================

std::vector<int>
MelodyEngine::chordIntervals(
    int chordType)
{
    static const std::vector<
        std::vector<int>> table =
    {
        {0, 4, 7},
        {0, 3, 7},
        {0, 4, 7, 11},
        {0, 3, 7, 10},
        {0, 4, 7, 10},
        {0, 3, 6},
        {0, 2, 7},
        {0, 5, 7},
        {0, 4, 7, 9},
        {0, 3, 7, 9},
        {0, 4, 7, 10, 14},
        {0, 4, 7, 11, 14},
        {0, 3, 7, 10, 14},
        {0, 4, 7, 10, 14, 17},
        {0, 3, 7, 10, 14, 17},
        {0, 4, 7, 10, 14, 17, 21},
        {0, 3, 7, 10, 14, 17, 21},
        {0, 4, 7, 14},
        {0, 3, 7, 14},
        {0, 5, 7, 10}
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
    if (high <= low)
        return low;

    std::uniform_int_distribution<int>
        distribution(
            low,
            high);

    return distribution(rng);
}

double MelodyEngine::randomDouble(
    double low,
    double high)
{
    std::uniform_real_distribution<double>
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
// MOTIF LOCKING
// =====================================================

void MelodyEngine::lockCurrentMotif()
{
    if (currentMotif.valid)
    {
        lockedMotif =
            currentMotif;

        motifLocked =
            true;
    }
}

void MelodyEngine::unlockMotif()
{
    motifLocked =
        false;
}

bool MelodyEngine::isMotifLocked() const
{
    return motifLocked;
}

// =====================================================
// CHORD / SCALE POOLS
// =====================================================

std::vector<int>
MelodyEngine::makeChordTonePool(
    const ChordChoice& chord,
    int low,
    int high) const
{
    std::vector<int> pool;

    const auto intervals =
        chordIntervals(
            chord.type);

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pc =
            (midi
             - chord.root
             + 120)
            % 12;

        for (int interval :
             intervals)
        {
            if ((interval % 12)
                == pc)
            {
                pool.push_back(
                    midi);

                break;
            }
        }
    }

    return pool;
}

std::vector<int>
MelodyEngine::makeExtensionPool(
    const ChordChoice& chord,
    int low,
    int high) const
{
    std::vector<int> pool;

    static const int extensions[] =
    {
        2,
        5,
        9,
        10,
        11
    };

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pc =
            (midi
             - chord.root
             + 120)
            % 12;

        for (int extension :
             extensions)
        {
            if (pc == extension)
            {
                pool.push_back(
                    midi);

                break;
            }
        }
    }

    return pool;
}

std::vector<int>
MelodyEngine::makeMelodicPool(
    const ChordChoice& chord,
    int low,
    int high) const
{
    auto pool =
        makeChordTonePool(
            chord,
            low,
            high);

    const auto extensions =
        makeExtensionPool(
            chord,
            low,
            high);

    pool.insert(
        pool.end(),
        extensions.begin(),
        extensions.end());

    std::sort(
        pool.begin(),
        pool.end());

    pool.erase(
        std::unique(
            pool.begin(),
            pool.end()),
        pool.end());

    return pool;
}

bool MelodyEngine::isChordTone(
    int midiNote,
    const ChordChoice& chord) const
{
    const int pc =
        (midiNote
         - chord.root
         + 120)
        % 12;

    for (int interval :
         chordIntervals(
             chord.type))
    {
        if ((interval % 12)
            == pc)
        {
            return true;
        }
    }

    return false;
}

bool MelodyEngine::isExtensionTone(
    int midiNote,
    const ChordChoice& chord) const
{
    const int pc =
        (midiNote
         - chord.root
         + 120)
        % 12;

    return
        pc == 2
        || pc == 5
        || pc == 9
        || pc == 10
        || pc == 11;
}

// =====================================================
// PITCH HELPERS
// =====================================================

int MelodyEngine::nearestPitch(
    int previous,
    const std::vector<int>& pool,
    int maximumJump)
{
    if (pool.empty())
        return previous;

    std::vector<int> candidates;

    for (int note :
         pool)
    {
        if (std::abs(
                note - previous)
            <= maximumJump)
        {
            candidates.push_back(
                note);
        }
    }

    if (candidates.empty())
        candidates = pool;

    std::sort(
        candidates.begin(),
        candidates.end(),

        [previous](
            int a,
            int b)
        {
            return
                std::abs(
                    a - previous)
                <
                std::abs(
                    b - previous);
        });

    const int choices =
        juce::jmin(
            6,
            (int)candidates.size());

    return candidates[
        (size_t)randomInt(
            0,
            choices - 1)];
}

int MelodyEngine::voiceLeadToNextChord(
    int previousNote,
    const ChordChoice&,
    const ChordChoice& nextChord,
    int low,
    int high)
{
    const auto pool =
        makeChordTonePool(
            nextChord,
            low,
            high);

    return nearestPitch(
        previousNote,
        pool,
        7);
}

int MelodyEngine::chooseTargetTone(
    const ChordChoice& chord,
    int previousNote,
    const Settings& settings,
    bool strongBeat)
{
    auto chordPool =
        makeChordTonePool(
            chord,
            58,
            92);

    auto melodicPool =
        makeMelodicPool(
            chord,
            58,
            92);

    if (strongBeat
        || chance(
            70
            - settings.surprise / 3))
    {
        return nearestPitch(
            previousNote,
            chordPool,
            settings.movement > 65
                ? 12
                : 7);
    }

    return nearestPitch(
        previousNote,
        melodicPool,
        settings.movement > 65
            ? 12
            : 7);
}

// =====================================================
// CONTOUR
// =====================================================

int MelodyEngine::applyContour(
    int note,
    int phrasePosition,
    int totalPositions,
    const Settings& settings)
{
    if (totalPositions <= 1)
        return note;

    const double position =
        (double)phrasePosition
        /
        (double)(totalPositions - 1);

    int offset = 0;

    if (settings.contour
        .equalsIgnoreCase("Rising"))
    {
        offset =
            (int)std::round(
                position
                *
                settings.movement
                / 12.0);
    }
    else if (
        settings.contour
            .equalsIgnoreCase("Falling"))
    {
        offset =
            -(int)std::round(
                position
                *
                settings.movement
                / 12.0);
    }
    else if (
        settings.contour
            .equalsIgnoreCase("Arch"))
    {
        const double curve =
            1.0
            -
            std::abs(
                position * 2.0
                - 1.0);

        offset =
            (int)std::round(
                curve
                *
                settings.movement
                / 10.0);
    }
    else if (
        settings.contour
            .equalsIgnoreCase(
                "Inverted Arch"))
    {
        const double curve =
            1.0
            -
            std::abs(
                position * 2.0
                - 1.0);

        offset =
            -(int)std::round(
                curve
                *
                settings.movement
                / 10.0);
    }
    else if (
        settings.contour
            .equalsIgnoreCase("Wave"))
    {
        offset =
            (int)std::round(
                std::sin(
                    position
                    *
                    juce::MathConstants<
                        double>::twoPi)
                *
                settings.movement
                / 12.0);
    }

    return juce::jlimit(
        55,
        94,
        note + offset);
}

// =====================================================
// RHYTHM
// =====================================================

std::vector<double>
MelodyEngine::createRhythmPattern(
    const Settings& settings,
    int barIndex)
{
    std::vector<
        std::vector<double>>
    patterns;

    const auto style =
        settings.style
            .toLowerCase();

    if (style == "afro"
        || style == "bouncy")
    {
        patterns =
        {
            {0.0, 0.75, 1.5, 2.5, 3.25},
            {0.5, 1.0, 1.75, 2.75, 3.5},
            {0.0, 1.0, 1.5, 2.25, 3.25},
            {0.5, 1.25, 2.0, 3.0}
        };
    }
    else if (
        style == "trap"
        || style == "dark")
    {
        patterns =
        {
            {0.0, 1.5, 3.0},
            {0.5, 2.0, 3.5},
            {0.0, 2.0, 3.25},
            {1.0, 2.5}
        };
    }
    else if (
        style == "r&b"
        || style == "smooth"
        || style == "soulful")
    {
        patterns =
        {
            {0.0, 0.75, 1.5, 2.75},
            {0.5, 1.25, 2.0, 3.25},
            {0.0, 1.5, 2.25, 3.5},
            {0.5, 1.0, 2.5, 3.0}
        };
    }
    else if (
        style == "complex")
    {
        patterns =
        {
            {0.0, 0.5, 1.0, 1.75, 2.5, 3.25, 3.75},
            {0.25, 0.75, 1.5, 2.0, 2.75, 3.5},
            {0.0, 0.75, 1.25, 2.25, 2.75, 3.25},
            {0.5, 1.0, 1.5, 2.0, 3.0, 3.5}
        };
    }
    else if (
        style == "emotional"
        || style == "dreamy")
    {
        patterns =
        {
            {0.0, 1.5, 3.0},
            {0.5, 2.0, 3.5},
            {0.0, 1.0, 2.5},
            {0.0, 2.0, 3.0}
        };
    }
    else
    {
        patterns =
        {
            {0.0, 1.0, 2.0, 3.0},
            {0.5, 1.5, 2.5, 3.5},
            {0.0, 1.5, 2.5},
            {0.5, 1.0, 2.0, 3.0}
        };
    }

    auto rhythm =
        patterns[
            (size_t)randomInt(
                0,
                (int)patterns.size() - 1)];

    std::vector<double>
        filtered;

    for (double beat :
         rhythm)
    {
        const int probability =
            juce::jlimit(
                15,
                100,
                settings.melodyDensity
                - settings.restAmount / 3);

        if (chance(probability))
        {
            filtered.push_back(
                beat);
        }
    }

    if (filtered.empty()
        && !rhythm.empty())
    {
        filtered.push_back(
            rhythm.front());
    }

    return applySectionRhythm(
        applySyncopation(
            filtered,
            settings),
        settings,
        barIndex);
}

std::vector<double>
MelodyEngine::applySyncopation(
    const std::vector<double>& rhythm,
    const Settings& settings)
{
    auto output =
        rhythm;

    for (auto& beat :
         output)
    {
        if (chance(
            settings.syncopation
            / 3))
        {
            beat +=
                chance(50)
                ? 0.25
                : -0.25;

            beat =
                juce::jlimit(
                    0.0,
                    3.75,
                    beat);
        }
    }

    std::sort(
        output.begin(),
        output.end());

    return output;
}

std::vector<double>
MelodyEngine::applySectionRhythm(
    const std::vector<double>& rhythm,
    const Settings& settings,
    int barIndex)
{
    auto output =
        rhythm;

    if (settings.section
        .equalsIgnoreCase("Intro"))
    {
        if (output.size() > 2)
            output.resize(2);
    }
    else if (
        settings.section
            .equalsIgnoreCase("Verse"))
    {
        if (chance(35)
            && output.size() > 1)
        {
            output.pop_back();
        }
    }
    else if (
        settings.section
            .equalsIgnoreCase(
                "Pre-Chorus"))
    {
        if (barIndex >= 2)
        {
            output.push_back(
                3.5);
        }
    }
    else if (
        settings.section
            .equalsIgnoreCase("Chorus"))
    {
        if (output.size() < 4)
        {
            output.push_back(
                2.0);
        }
    }
    else if (
        settings.section
            .equalsIgnoreCase("Outro"))
    {
        if (barIndex == 3
            && output.size() > 2)
        {
            output.resize(2);
        }
    }

    std::sort(
        output.begin(),
        output.end());

    output.erase(
        std::unique(
            output.begin(),
            output.end()),
        output.end());

    return output;
}

double MelodyEngine::applyPocketOffset(
    double beat,
    const Settings& settings)
{
    if (settings.pocket
        .equalsIgnoreCase("Ahead"))
    {
        return
            beat
            -
            randomDouble(
                0.005,
                0.025);
    }

    if (settings.pocket
        .equalsIgnoreCase("Behind"))
    {
        return
            beat
            +
            randomDouble(
                0.005,
                0.030);
    }

    return beat;
}

// =====================================================
// MOTIF CREATION
// =====================================================

MelodyEngine::Motif
MelodyEngine::createMotif(
    const Settings& settings,
    const ChordChoice& chord,
    int startingNote)
{
    Motif motif;

    const auto pool =
        makeMelodicPool(
            chord,
            60,
            90);

    int note =
        nearestPitch(
            startingNote,
            pool,
            7);

    const int motifLength =
        settings.complexity >= 75
        ? 5
        : settings.complexity >= 45
            ? 4
            : 3;

    motif.anchorNote =
        note;

    motif.intervals.push_back(
        0);

    int previous =
        note;

    for (int i = 1;
         i < motifLength;
         ++i)
    {
        const int jump =
            settings.movement > 70
            ? 10
            : settings.movement > 40
                ? 6
                : 4;

        const int next =
            nearestPitch(
                previous,
                pool,
                jump);

        motif.intervals.push_back(
            next - note);

        previous =
            next;
    }

    motif.rhythm =
        createRhythmPattern(
            settings,
            0);

    while (motif.rhythm.size()
           <
           motif.intervals.size())
    {
        motif.rhythm.push_back(
            juce::jlimit(
                0.0,
                3.75,
                motif.rhythm.empty()
                    ? 0.0
                    : motif.rhythm.back()
                      + 0.5));
    }

    if (motif.rhythm.size()
        >
        motif.intervals.size())
    {
        motif.rhythm.resize(
            motif.intervals.size());
    }

    for (size_t i = 0;
         i < motif.intervals.size();
         ++i)
    {
        double length =
            0.45;

        if (settings.style
            .equalsIgnoreCase("Smooth")
            ||
            settings.style
            .equalsIgnoreCase("R&B")
            ||
            settings.style
            .equalsIgnoreCase("Soulful"))
        {
            length =
                chance(35)
                ? 0.85
                : 0.5;
        }
        else if (
            settings.style
            .equalsIgnoreCase("Trap")
            ||
            settings.style
            .equalsIgnoreCase("Bouncy"))
        {
            length =
                chance(60)
                ? 0.3
                : 0.45;
        }

        motif.lengths.push_back(
            length);
    }

    motif.valid =
        !motif.intervals.empty();

    currentMotif =
        motif;

    return motif;
}

MelodyEngine::Motif
MelodyEngine::mutateMotif(
    const Motif& source,
    const Settings& settings,
    const ChordChoice&,
    int barIndex)
{
    Motif result =
        source;

    if (!result.valid)
        return result;

    const int mutationChance =
        juce::jlimit(
            5,
            90,
            settings.variation);

    for (size_t i = 0;
         i < result.intervals.size();
         ++i)
    {
        if (chance(
            mutationChance / 2))
        {
            result.intervals[i] +=
                chance(50)
                ? randomInt(1, 3)
                : -randomInt(1, 3);
        }
    }

    if (chance(
        mutationChance / 3))
    {
        for (auto& beat :
             result.rhythm)
        {
            beat +=
                chance(50)
                ? 0.25
                : -0.25;

            beat =
                juce::jlimit(
                    0.0,
                    3.75,
                    beat);
        }
    }

    if (barIndex == 3
        && !result.intervals.empty())
    {
        result.intervals.back() =
            chance(60)
            ? 0
            : result.intervals.back();
    }

    return result;
}

MelodyEngine::Motif
MelodyEngine::transposeMotifToChord(
    const Motif& source,
    const ChordChoice& chord,
    int startingNote)
{
    Motif result =
        source;

    const auto pool =
        makeChordTonePool(
            chord,
            58,
            92);

    result.anchorNote =
        nearestPitch(
            startingNote,
            pool,
            7);

    return result;
}

MelodyEngine::Motif
MelodyEngine::invertMotif(
    const Motif& source)
{
    Motif result =
        source;

    for (auto& interval :
         result.intervals)
    {
        interval =
            -interval;
    }

    return result;
}

MelodyEngine::Motif
MelodyEngine::reverseMotif(
    const Motif& source)
{
    Motif result =
        source;

    std::reverse(
        result.intervals.begin(),
        result.intervals.end());

    return result;
}

// =====================================================
// LEAD GENERATION
// =====================================================

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateLeadCandidate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression)
{
    std::vector<NoteEvent>
        lead;

    int previousNote =
        72;

    Motif baseMotif;

    if (motifLocked
        && lockedMotif.valid)
    {
        baseMotif =
            lockedMotif;
    }
    else
    {
        baseMotif =
            createMotif(
                settings,
                progression[0],
                previousNote);
    }

    for (int bar = 0;
         bar < settings.bars;
         ++bar)
    {
        const auto chord =
            progression[
                (size_t)(bar % 4)];

        Motif motif =
            baseMotif;

        if (bar > 0)
        {
            motif =
                mutateMotif(
                    baseMotif,
                    settings,
                    chord,
                    bar);

            if (bar == 2
                &&
                chance(
                    settings.variation))
            {
                if (chance(50))
                {
                    motif =
                        invertMotif(
                            motif);
                }
                else
                {
                    motif =
                        reverseMotif(
                            motif);
                }
            }
        }

        motif =
            transposeMotifToChord(
                motif,
                chord,
                previousNote);

        const auto rhythm =
            createRhythmPattern(
                settings,
                bar);

        if (rhythm.empty())
            continue;

        const int totalPositions =
            (int)rhythm.size();

        for (int i = 0;
             i < totalPositions;
             ++i)
        {
            const bool strongBeat =
                std::abs(
                    rhythm[(size_t)i]
                    -
                    std::round(
                        rhythm[(size_t)i]))
                <
                0.01;

            int note =
                motif.anchorNote;

            if (!motif.intervals.empty())
            {
                note +=
                    motif.intervals[
                        (size_t)i
                        %
                        motif.intervals.size()];
            }

            const int target =
                chooseTargetTone(
                    chord,
                    note,
                    settings,
                    strongBeat);

            if (chance(
                75
                -
                settings.surprise / 2))
            {
                note =
                    target;
            }

            note =
                applyContour(
                    note,
                    i,
                    totalPositions,
                    settings);

            if (settings.surprise > 0
                &&
                chance(
                    settings.surprise / 6))
            {
                note +=
                    chance(50)
                    ? 12
                    : -12;
            }

            note =
                juce::jlimit(
                    57,
                    94,
                    note);

            double beat =
                bar * 4.0
                +
                rhythm[(size_t)i];

            beat =
                applyPocketOffset(
                    beat,
                    settings);

            double length =
                0.45;

            if (i
                <
                (int)motif.lengths.size())
            {
                length =
                    motif.lengths[
                        (size_t)i];
            }

            if (settings.section
                .equalsIgnoreCase("Chorus")
                &&
                i == totalPositions - 1
                &&
                chance(45))
            {
                length *=
                    1.5;
            }

            int velocity =
                randomInt(
                    78,
                    108);

            lead.push_back(
            {
                juce::jmax(
                    bar * 4.0,
                    beat),

                length,

                note,

                velocity,

                2
            });

            previousNote =
                note;
        }

        if (bar < 3
            &&
            chance(
                settings.tension / 3))
        {
            previousNote =
                voiceLeadToNextChord(
                    previousNote,
                    chord,
                    progression[
                        (size_t)(bar + 1)],
                    58,
                    92);
        }
    }

    addPassingNotes(
        lead,
        progression,
        settings);

    addApproachNotes(
        lead,
        progression,
        settings);

    addEnclosures(
        lead,
        progression,
        settings);

    addAnticipations(
        lead,
        progression,
        settings);

    applyPhraseResolution(
        lead,
        progression,
        settings);

    humaniseNotes(
        lead,
        settings);

    sortEvents(
        lead);

    return lead;
}

// =====================================================
// PASSING / APPROACH / ENCLOSURES
// =====================================================

void MelodyEngine::addPassingNotes(
    std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>&,
    const Settings& settings)
{
    if (lead.size() < 2)
        return;

    std::vector<NoteEvent>
        additions;

    for (size_t i = 1;
         i < lead.size();
         ++i)
    {
        const auto& previous =
            lead[i - 1];

        const auto& current =
            lead[i];

        const int interval =
            current.note
            -
            previous.note;

        const double gap =
            current.beat
            -
            (
                previous.beat
                +
                previous.lengthBeats);

        if (std::abs(interval)
                >= 4
            &&
            gap >= 0.2
            &&
            chance(
                settings.complexity / 5))
        {
            const int passingNote =
                previous.note
                +
                (
                    interval > 0
                    ? 2
                    : -2
                );

            additions.push_back(
            {
                previous.beat
                    +
                    previous.lengthBeats
                    +
                    gap * 0.35,

                juce::jmin(
                    0.25,
                    gap * 0.45),

                passingNote,

                juce::jlimit(
                    45,
                    100,
                    previous.velocity - 12),

                2
            });
        }
    }

    lead.insert(
        lead.end(),
        additions.begin(),
        additions.end());
}

void MelodyEngine::addApproachNotes(
    std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>& progression,
    const Settings& settings)
{
    std::vector<NoteEvent>
        additions;

    for (const auto& note :
         lead)
    {
        const int chordIndex =
            currentChordIndex(
                note.beat);

        if (!isChordTone(
                note.note,
                progression[
                    (size_t)chordIndex]))
        {
            continue;
        }

        if (chance(
            settings.tension / 8))
        {
            const double approachBeat =
                note.beat
                -
                0.25;

            if (approachBeat
                >= chordIndex * 4.0)
            {
                additions.push_back(
                {
                    approachBeat,

                    0.20,

                    note.note
                    +
                    (
                        chance(50)
                        ? 1
                        : -1
                    ),

                    juce::jlimit(
                        45,
                        105,
                        note.velocity - 10),

                    2
                });
            }
        }
    }

    lead.insert(
        lead.end(),
        additions.begin(),
        additions.end());
}

void MelodyEngine::addEnclosures(
    std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>& progression,
    const Settings& settings)
{
    if (settings.complexity < 65)
        return;

    std::vector<NoteEvent>
        additions;

    for (const auto& note :
         lead)
    {
        if (!chance(
            settings.surprise / 12))
        {
            continue;
        }

        const int chordIndex =
            currentChordIndex(
                note.beat);

        if (!isChordTone(
                note.note,
                progression[
                    (size_t)chordIndex]))
        {
            continue;
        }

        const double start =
            note.beat
            -
            0.50;

        if (start
            <
            chordIndex * 4.0)
        {
            continue;
        }

        additions.push_back(
        {
            start,
            0.20,
            note.note + 1,
            note.velocity - 15,
            2
        });

        additions.push_back(
        {
            start + 0.25,
            0.20,
            note.note - 1,
            note.velocity - 12,
            2
        });
    }

    lead.insert(
        lead.end(),
        additions.begin(),
        additions.end());
}

void MelodyEngine::addAnticipations(
    std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>& progression,
    const Settings& settings)
{
    for (int bar = 0;
         bar < 3;
         ++bar)
    {
        if (!chance(
            settings.tension / 4))
        {
            continue;
        }

        const auto& currentChord =
            progression[
                (size_t)bar];

        const auto& nextChord =
            progression[
                (size_t)(bar + 1)];

        int previous =
            72;

        if (!lead.empty())
            previous =
                lead.back().note;

        const int target =
            voiceLeadToNextChord(
                previous,
                currentChord,
                nextChord,
                58,
                92);

        lead.push_back(
        {
            bar * 4.0
                + 3.75,

            0.20,

            target,

            82,

            2
        });
    }
}

void MelodyEngine::applyPhraseResolution(
    std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>& progression,
    const Settings&)
{
    if (lead.empty())
        return;

    auto& last =
        lead.back();

    const auto finalChord =
        progression[3];

    const auto pool =
        makeChordTonePool(
            finalChord,
            58,
            90);

    last.note =
        nearestPitch(
            last.note,
            pool,
            5);

    last.lengthBeats =
        juce::jmax(
            last.lengthBeats,
            0.75);
}

// =====================================================
// COUNTER HELPERS
// =====================================================

bool MelodyEngine::leadIsActiveNear(
    const std::vector<NoteEvent>& lead,
    double beat,
    double tolerance) const
{
    for (const auto& event :
         lead)
    {
        const double start =
            event.beat
            -
            tolerance;

        const double end =
            event.beat
            +
            event.lengthBeats
            +
            tolerance;

        if (beat >= start
            &&
            beat <= end)
        {
            return true;
        }
    }

    return false;
}

int MelodyEngine::findNearestLeadNote(
    const std::vector<NoteEvent>& lead,
    double beat) const
{
    if (lead.empty())
        return 72;

    double best =
        999.0;

    int note =
        lead.front().note;

    for (const auto& event :
         lead)
    {
        const double distance =
            std::abs(
                event.beat
                -
                beat);

        if (distance < best)
        {
            best =
                distance;

            note =
                event.note;
        }
    }

    return note;
}

int MelodyEngine::currentChordIndex(
    double beat) const
{
    return
        juce::jlimit(
            0,
            3,
            (int)std::floor(
                beat / 4.0));
}

// =====================================================
// COUNTER MODES
// =====================================================

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateGapFillCounter(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression,
    const std::vector<NoteEvent>& lead)
{
    std::vector<NoteEvent>
        counter;

    int previous =
        55;

    static const double positions[] =
    {
        0.5,
        1.0,
        1.5,
        2.5,
        3.0,
        3.5
    };

    for (int bar = 0;
         bar < 4;
         ++bar)
    {
        const auto pool =
            makeMelodicPool(
                progression[
                    (size_t)bar],
                43,
                72);

        for (double localBeat :
             positions)
        {
            const double beat =
                bar * 4.0
                +
                localBeat;

            if (leadIsActiveNear(
                    lead,
                    beat,
                    0.12))
            {
                continue;
            }

            if (!chance(
                settings.counterDensity))
            {
                continue;
            }

            int note =
                nearestPitch(
                    previous,
                    pool,
                    6);

            const int leadNote =
                findNearestLeadNote(
                    lead,
                    beat);

            if (leadNote > 74)
                note =
                    juce::jmin(
                        note,
                        62);

            note =
                juce::jlimit(
                    43,
                    72,
                    note);

            counter.push_back(
            {
                beat,
                0.45,
                note,
                randomInt(
                    48,
                    76),
                3
            });

            previous =
                note;
        }
    }

    return counter;
}

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateAnswerCounter(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression,
    const std::vector<NoteEvent>& lead)
{
    std::vector<NoteEvent>
        counter;

    int previous =
        55;

    for (int bar = 0;
         bar < 4;
         ++bar)
    {
        const auto pool =
            makeMelodicPool(
                progression[
                    (size_t)bar],
                45,
                72);

        static const double answerPoints[] =
        {
            2.5,
            3.0,
            3.5
        };

        for (double point :
             answerPoints)
        {
            const double beat =
                bar * 4.0
                +
                point;

            if (leadIsActiveNear(
                    lead,
                    beat,
                    0.12))
            {
                continue;
            }

            if (!chance(
                settings.counterDensity))
            {
                continue;
            }

            const int leadNote =
                findNearestLeadNote(
                    lead,
                    beat - 1.0);

            int note =
                nearestPitch(
                    leadNote - 12,
                    pool,
                    7);

            note =
                nearestPitch(
                    previous,
                    pool,
                    8);

            counter.push_back(
            {
                beat,
                0.55,
                note,
                randomInt(
                    52,
                    78),
                3
            });

            previous =
                note;
        }
    }

    return counter;
}

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateHarmonyCounter(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression,
    const std::vector<NoteEvent>& lead)
{
    std::vector<NoteEvent>
        counter;

    for (const auto& leadNote :
         lead)
    {
        if (!chance(
            settings.counterDensity))
        {
            continue;
        }

        const int chordIndex =
            currentChordIndex(
                leadNote.beat);

        const auto pool =
            makeChordTonePool(
                progression[
                    (size_t)chordIndex],
                43,
                84);

        int target =
            leadNote.note
            -
            (
                chance(65)
                ? 3
                : 4
            );

        target =
            nearestPitch(
                target,
                pool,
                4);

        if (target
            >= leadNote.note)
        {
            target -=
                12;
        }

        target =
            juce::jlimit(
                43,
                76,
                target);

        counter.push_back(
        {
            leadNote.beat,
            leadNote.lengthBeats
                * 0.9,
            target,
            juce::jlimit(
                40,
                90,
                leadNote.velocity - 18),
            3
        });
    }

    return counter;
}

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateOppositeMotionCounter(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression,
    const std::vector<NoteEvent>& lead)
{
    std::vector<NoteEvent>
        counter;

    int previousCounter =
        55;

    int previousLead =
        lead.empty()
        ? 72
        : lead.front().note;

    for (const auto& leadEvent :
         lead)
    {
        if (!chance(
            settings.counterDensity))
        {
            previousLead =
                leadEvent.note;

            continue;
        }

        const int chordIndex =
            currentChordIndex(
                leadEvent.beat);

        const auto pool =
            makeMelodicPool(
                progression[
                    (size_t)chordIndex],
                43,
                72);

        const int leadDirection =
            leadEvent.note
            -
            previousLead;

        int target =
            previousCounter;

        if (leadDirection > 0)
        {
            target -=
                randomInt(
                    2,
                    5);
        }
        else if (
            leadDirection < 0)
        {
            target +=
                randomInt(
                    2,
                    5);
        }

        const int note =
            nearestPitch(
                target,
                pool,
                7);

        counter.push_back(
        {
            leadEvent.beat
                + 0.25,
            0.40,
            note,
            randomInt(
                50,
                76),
            3
        });

        previousCounter =
            note;

        previousLead =
            leadEvent.note;
    }

    return counter;
}

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateCounterCandidate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression,
    const std::vector<NoteEvent>& lead)
{
    if (settings.counterMode
        .equalsIgnoreCase("Answer"))
    {
        return generateAnswerCounter(
            settings,
            progression,
            lead);
    }

    if (settings.counterMode
        .equalsIgnoreCase("Harmony"))
    {
        return generateHarmonyCounter(
            settings,
            progression,
            lead);
    }

    if (settings.counterMode
        .equalsIgnoreCase(
            "Opposite Motion"))
    {
        return
            generateOppositeMotionCounter(
                settings,
                progression,
                lead);
    }

    auto counter =
        generateGapFillCounter(
            settings,
            progression,
            lead);

    if (settings.counterMode
        .equalsIgnoreCase(
            "High Counter"))
    {
        for (auto& note :
             counter)
        {
            note.note =
                juce::jlimit(
                    60,
                    84,
                    note.note + 12);
        }
    }
    else if (
        settings.counterMode
            .equalsIgnoreCase(
                "Low Counter"))
    {
        for (auto& note :
             counter)
        {
            note.note =
                juce::jlimit(
                    36,
                    64,
                    note.note - 5);
        }
    }
    else if (
        settings.counterMode
            .equalsIgnoreCase(
                "Sparse"))
    {
        std::vector<NoteEvent>
            sparse;

        for (size_t i = 0;
             i < counter.size();
             i += 2)
        {
            sparse.push_back(
                counter[i]);
        }

        counter =
            std::move(
                sparse);
    }
    else if (
        settings.counterMode
            .equalsIgnoreCase(
                "Atmospheric"))
    {
        for (auto& note :
             counter)
        {
            note.lengthBeats =
                juce::jmax(
                    0.9,
                    note.lengthBeats
                    * 2.0);

            note.velocity =
                juce::jlimit(
                    35,
                    70,
                    note.velocity - 8);
        }
    }

    humaniseNotes(
        counter,
        settings);

    return counter;
}

// =====================================================
// SCORING
// =====================================================

double MelodyEngine::scoreMotifStrength(
    const std::vector<NoteEvent>& lead,
    const Settings& settings) const
{
    if (lead.size() < 4)
        return -10.0;

    double score =
        0.0;

    for (size_t i = 1;
         i < lead.size();
         ++i)
    {
        const int interval =
            std::abs(
                lead[i].note
                -
                lead[i - 1].note);

        if (interval <= 5)
            score += 1.0;
    }

    score +=
        settings.hookStrength
        * 0.05;

    return score;
}

double MelodyEngine::scoreVoiceLeading(
    const std::vector<NoteEvent>& lead) const
{
    if (lead.size() < 2)
        return 0.0;

    double score =
        0.0;

    for (size_t i = 1;
         i < lead.size();
         ++i)
    {
        const int jump =
            std::abs(
                lead[i].note
                -
                lead[i - 1].note);

        if (jump <= 2)
            score += 2.0;
        else if (jump <= 5)
            score += 1.0;
        else if (jump > 12)
            score -= 5.0;
    }

    return score;
}

double MelodyEngine::scoreRhythmicInterest(
    const std::vector<NoteEvent>& lead) const
{
    if (lead.empty())
        return -10.0;

    int offbeats =
        0;

    for (const auto& note :
         lead)
    {
        const double fraction =
            std::fmod(
                note.beat,
                1.0);

        if (fraction > 0.20
            &&
            fraction < 0.80)
        {
            ++offbeats;
        }
    }

    return
        juce::jmin(
            12.0,
            offbeats * 1.2);
}

double MelodyEngine::scoreTensionResolution(
    const std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>& progression) const
{
    if (lead.empty())
        return 0.0;

    double score =
        0.0;

    for (const auto& note :
         lead)
    {
        const int chordIndex =
            currentChordIndex(
                note.beat);

        if (isChordTone(
                note.note,
                progression[
                    (size_t)chordIndex]))
        {
            score +=
                1.5;
        }
        else if (
            isExtensionTone(
                note.note,
                progression[
                    (size_t)chordIndex]))
        {
            score +=
                0.7;
        }
    }

    const auto& last =
        lead.back();

    if (isChordTone(
            last.note,
            progression[3]))
    {
        score +=
            8.0;
    }

    return score;
}

double MelodyEngine::scoreRegisterControl(
    const std::vector<NoteEvent>& notes) const
{
    if (notes.empty())
        return 0.0;

    int low =
        127;

    int high =
        0;

    for (const auto& note :
         notes)
    {
        low =
            juce::jmin(
                low,
                note.note);

        high =
            juce::jmax(
                high,
                note.note);
    }

    const int range =
        high - low;

    if (range >= 7
        &&
        range <= 22)
    {
        return 8.0;
    }

    if (range > 30)
        return -10.0;

    return 2.0;
}

double MelodyEngine::scoreCounterSeparation(
    const std::vector<NoteEvent>& counter,
    const std::vector<NoteEvent>& lead) const
{
    double score =
        0.0;

    for (const auto& c :
         counter)
    {
        if (!leadIsActiveNear(
                lead,
                c.beat,
                0.12))
        {
            score +=
                2.5;
        }

        const int leadNote =
            findNearestLeadNote(
                lead,
                c.beat);

        const int distance =
            std::abs(
                leadNote
                -
                c.note);

        if (distance >= 5)
            score += 1.5;

        if (distance == 0)
            score -= 5.0;
    }

    return score;
}

double MelodyEngine::scoreLead(
    const std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>& progression,
    const Settings& settings) const
{
    if (lead.empty())
        return -1000.0;

    double score =
        0.0;

    const double idealCount =
        8.0
        +
        settings.melodyDensity
        * 0.14;

    score -=
        std::abs(
            (double)lead.size()
            -
            idealCount)
        * 0.8;

    score +=
        scoreMotifStrength(
            lead,
            settings);

    score +=
        scoreVoiceLeading(
            lead);

    score +=
        scoreRhythmicInterest(
            lead);

    score +=
        scoreTensionResolution(
            lead,
            progression);

    score +=
        scoreRegisterControl(
            lead);

    return score;
}

double MelodyEngine::scoreCounter(
    const std::vector<NoteEvent>& counter,
    const std::vector<NoteEvent>& lead,
    const std::array<
        ChordChoice,
        4>& progression,
    const Settings&) const
{
    if (counter.empty())
        return -15.0;

    double score =
        scoreCounterSeparation(
            counter,
            lead);

    for (const auto& note :
         counter)
    {
        const int chordIndex =
            currentChordIndex(
                note.beat);

        if (isChordTone(
                note.note,
                progression[
                    (size_t)chordIndex]))
        {
            score +=
                1.5;
        }
    }

    return score;
}

// =====================================================
// HUMANISATION
// =====================================================

void MelodyEngine::humaniseNotes(
    std::vector<NoteEvent>& notes,
    const Settings& settings)
{
    if (settings.humanise <= 0)
        return;

    for (auto& note :
         notes)
    {
        const double timingAmount =
            settings.humanise
            / 100.0
            *
            0.035;

        note.beat +=
            randomDouble(
                -timingAmount,
                timingAmount);

        note.beat =
            juce::jmax(
                0.0,
                note.beat);

        note.velocity +=
            randomInt(
                -settings.humanise / 4,
                settings.humanise / 4);

        note.velocity =
            juce::jlimit(
                1,
                127,
                note.velocity);

        note.lengthBeats *=
            randomDouble(
                0.92,
                1.08);
    }
}

// =====================================================
// CHORD LAYER
// =====================================================

void MelodyEngine::addChordLayer(
    std::vector<NoteEvent>& result,
    const std::array<
        ChordChoice,
        4>& progression,
    int bars)
{
    for (int bar = 0;
         bar < bars;
         ++bar)
    {
        const auto chord =
            progression[
                (size_t)(bar % 4)];

        const auto intervals =
            chordIntervals(
                chord.type);

        const size_t noteCount =
            juce::jmin(
                (size_t)4,
                intervals.size());

        for (size_t i = 0;
             i < noteCount;
             ++i)
        {
            int note =
                48
                +
                chord.root
                +
                intervals[i];

            while (note > 67)
                note -= 12;

            while (note < 43)
                note += 12;

            result.push_back(
            {
                bar * 4.0,

                3.80,

                note,

                66
                -
                (int)i * 3,

                1
            });
        }
    }
}

// =====================================================
// SORT
// =====================================================

void MelodyEngine::sortEvents(
    std::vector<NoteEvent>& events)
{
    std::sort(
        events.begin(),
        events.end(),

        [](
            const NoteEvent& a,
            const NoteEvent& b)
        {
            if (a.beat
                ==
                b.beat)
            {
                if (a.channel
                    ==
                    b.channel)
                {
                    return
                        a.note
                        <
                        b.note;
                }

                return
                    a.channel
                    <
                    b.channel;
            }

            return
                a.beat
                <
                b.beat;
        });
}

// =====================================================
// MAIN GENERATION
// =====================================================

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression)
{
    PhraseCandidate
        best;

    best.totalScore =
        -1000000.0;

    // More complexity = consider more candidate
    // compositions internally.
    const int candidateCount =
        settings.complexity >= 75
        ? 20
        : settings.complexity >= 50
            ? 16
            : 12;

    for (int i = 0;
         i < candidateCount;
         ++i)
    {
        PhraseCandidate
            candidate;

        candidate.lead =
            generateLeadCandidate(
                settings,
                progression);

        candidate.counter =
            generateCounterCandidate(
                settings,
                progression,
                candidate.lead);

        candidate.leadScore =
            scoreLead(
                candidate.lead,
                progression,
                settings);

        candidate.counterScore =
            scoreCounter(
                candidate.counter,
                candidate.lead,
                progression,
                settings);

        candidate.totalScore =
            candidate.leadScore
            +
            candidate.counterScore;

        if (candidate.totalScore
            >
            best.totalScore)
        {
            best =
                std::move(
                    candidate);
        }
    }

    std::vector<NoteEvent>
        result;

    addChordLayer(
        result,
        progression,
        settings.bars);

    result.insert(
        result.end(),
        best.lead.begin(),
        best.lead.end());

    result.insert(
        result.end(),
        best.counter.begin(),
        best.counter.end());

    sortEvents(
        result);

    return result;
}
