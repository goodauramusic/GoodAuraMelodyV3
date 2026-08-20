#include "MelodyEngine.h"

#include <algorithm>
#include <cmath>
#include <numeric>

MelodyEngine::MelodyEngine()
    : rng(std::random_device{}())
{
}

juce::StringArray MelodyEngine::rootNames()
{
    return
    {
        "C","C#","D","Eb","E","F",
        "F#","G","Ab","A","Bb","B"
    };
}

juce::StringArray MelodyEngine::chordNames()
{
    return
    {
        "Major","Minor","Maj7","Min7","7",
        "Dim","Sus2","Sus4","6","m6",
        "9","Maj9","m9","11","m11",
        "13","m13","add9","madd9","7sus4"
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
        "Complex"
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
        (size_t)juce::jlimit(
            0,
            (int)table.size() - 1,
            chordType)];
}

int MelodyEngine::randomInt(
    int low,
    int high)
{
    std::uniform_int_distribution<int> d(
        low,
        high);

    return d(rng);
}

double MelodyEngine::randomDouble(
    double low,
    double high)
{
    std::uniform_real_distribution<double> d(
        low,
        high);

    return d(rng);
}

bool MelodyEngine::chance(
    int percent)
{
    return
        randomInt(0,99)
        <
        juce::jlimit(
            0,
            100,
            percent);
}

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

    for (int note = low;
         note <= high;
         ++note)
    {
        const int pitchClass =
            (note - chord.root + 120)
            % 12;

        for (int interval : intervals)
        {
            if ((interval % 12)
                == pitchClass)
            {
                pool.push_back(note);
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

    // Useful extension / passing colours.
    static const int colours[] =
    {
        2,
        5,
        9,
        10,
        11
    };

    for (int note = low;
         note <= high;
         ++note)
    {
        const int pc =
            (note - chord.root + 120)
            % 12;

        for (int colour : colours)
        {
            if (pc == colour)
            {
                pool.push_back(note);
                break;
            }
        }
    }

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

int MelodyEngine::nearestPitch(
    int previous,
    const std::vector<int>& pool,
    int maximumJump)
{
    if (pool.empty())
        return previous;

    std::vector<int> candidates;

    for (int note : pool)
    {
        if (std::abs(
                note - previous)
            <= maximumJump)
        {
            candidates.push_back(note);
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
                std::abs(a - previous)
                <
                std::abs(b - previous);
        });

    const int limit =
        juce::jmin(
            5,
            (int)candidates.size());

    return candidates[
        (size_t)randomInt(
            0,
            limit - 1)];
}

std::vector<double>
MelodyEngine::makeRhythmPattern(
    const Settings& settings,
    int barIndex)
{
    std::vector<double> rhythm;

    const auto style =
        settings.style.toLowerCase();

    // Each value is a beat offset
    // inside one 4/4 bar.

    if (style == "catchy")
    {
        static const std::vector<
            std::vector<double>>
        patterns =
        {
            {0.0, 1.0, 2.0, 3.0},
            {0.0, 0.5, 1.5, 2.5},
            {0.5, 1.0, 2.0, 3.0},
            {0.0, 1.5, 2.0, 3.5}
        };

        rhythm =
            patterns[
                (size_t)randomInt(
                    0,
                    (int)patterns.size()-1)];
    }
    else if (style == "bouncy")
    {
        static const std::vector<
            std::vector<double>>
        patterns =
        {
            {0.0,0.5,1.5,2.0,3.0},
            {0.5,1.0,1.75,2.5,3.5},
            {0.0,0.75,1.5,2.25,3.25},
            {0.5,1.5,2.0,2.75,3.5}
        };

        rhythm =
            patterns[
                (size_t)randomInt(
                    0,
                    (int)patterns.size()-1)];
    }
    else if (style == "complex")
    {
        static const std::vector<
            std::vector<double>>
        patterns =
        {
            {0.0,0.5,1.0,1.5,2.25,3.0,3.5},
            {0.25,0.75,1.25,2.0,2.5,3.0,3.75},
            {0.0,0.75,1.5,2.0,2.75,3.25},
            {0.5,1.0,1.5,2.25,2.75,3.5}
        };

        rhythm =
            patterns[
                (size_t)randomInt(
                    0,
                    (int)patterns.size()-1)];
    }
    else if (style == "emotional")
    {
        static const std::vector<
            std::vector<double>>
        patterns =
        {
            {0.0,1.0,2.5},
            {0.5,1.5,3.0},
            {0.0,1.5,2.0,3.5},
            {0.0,2.0,3.0}
        };

        rhythm =
            patterns[
                (size_t)randomInt(
                    0,
                    (int)patterns.size()-1)];
    }
    else if (style == "dark")
    {
        static const std::vector<
            std::vector<double>>
        patterns =
        {
            {0.0,1.5,3.0},
            {0.5,2.0,3.5},
            {0.0,2.5},
            {1.0,2.0,3.0}
        };

        rhythm =
            patterns[
                (size_t)randomInt(
                    0,
                    (int)patterns.size()-1)];
    }
    else
    {
        // Smooth
        static const std::vector<
            std::vector<double>>
        patterns =
        {
            {0.0,1.0,2.0,3.0},
            {0.5,1.5,2.5,3.5},
            {0.0,1.5,2.5},
            {0.5,1.0,2.0,3.0}
        };

        rhythm =
            patterns[
                (size_t)randomInt(
                    0,
                    (int)patterns.size()-1)];
    }

    // Density trims some rhythm positions.
    std::vector<double> filtered;

    for (double beat : rhythm)
    {
        if (chance(
            juce::jlimit(
                20,
                100,
                settings.melodyDensity)))
        {
            filtered.push_back(beat);
        }
    }

    // Keep at least one note in most bars.
    if (filtered.empty()
        && chance(80))
    {
        filtered.push_back(
            rhythm.empty()
                ? 0.0
                : rhythm.front());
    }

    // Phrase endings get more space.
    if (barIndex == 3
        && filtered.size() > 2
        && chance(60))
    {
        filtered.pop_back();
    }

    return filtered;
}

std::vector<int>
MelodyEngine::createMotif(
    const Settings& settings,
    const ChordChoice& chord,
    int startingNote)
{
    std::vector<int> motif;

    const auto pool =
        makeMelodicPool(
            chord,
            60,
            88);

    int current =
        nearestPitch(
            startingNote,
            pool,
            7);

    int motifLength = 3;

    if (settings.complexity >= 70)
        motifLength = 5;
    else if (settings.complexity >= 45)
        motifLength = 4;

    motif.push_back(current);

    for (int i = 1;
         i < motifLength;
         ++i)
    {
        int maxJump = 5;

        if (settings.movement > 70)
            maxJump = 12;
        else if (settings.movement < 35)
            maxJump = 3;

        current =
            nearestPitch(
                current,
                pool,
                maxJump);

        motif.push_back(current);
    }

    return motif;
}

std::vector<int>
MelodyEngine::mutateMotif(
    const std::vector<int>& motif,
    const Settings& settings,
    const ChordChoice& chord)
{
    if (motif.empty())
        return {};

    auto result =
        motif;

    const auto pool =
        makeMelodicPool(
            chord,
            60,
            90);

    for (size_t i = 0;
         i < result.size();
         ++i)
    {
        // Repetition control:
        // high repetition = fewer changes.
        const int mutationChance =
            100
            -
            juce::jlimit(
                10,
                90,
                settings.repetition);

        if (chance(mutationChance))
        {
            result[i] =
                nearestPitch(
                    result[i],
                    pool,
                    settings.movement > 60
                        ? 9
                        : 5);
        }
    }

    // Occasionally alter just the ending.
    if (result.size() >= 2
        && chance(35))
    {
        result.back() =
            nearestPitch(
                result[
                    result.size()-2],
                pool,
                7);
    }

    return result;
}

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateLeadCandidate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression)
{
    std::vector<NoteEvent> lead;

    int previousNote = 72;

    std::vector<int> motifA;

    for (int bar = 0;
         bar < settings.bars;
         ++bar)
    {
        const auto chord =
            progression[
                (size_t)(bar % 4)];

        const auto rhythm =
            makeRhythmPattern(
                settings,
                bar);

        std::vector<int> motif;

        if (bar == 0)
        {
            motif =
                createMotif(
                    settings,
                    chord,
                    previousNote);

            motifA = motif;
        }
        else if (bar == 1)
        {
            motif =
                mutateMotif(
                    motifA,
                    settings,
                    chord);
        }
        else if (bar == 2)
        {
            if (chance(
                settings.repetition))
            {
                motif =
                    mutateMotif(
                        motifA,
                        settings,
                        chord);
            }
            else
            {
                motif =
                    createMotif(
                        settings,
                        chord,
                        previousNote);
            }
        }
        else
        {
            // Final bar tends to resolve.
            motif =
                mutateMotif(
                    motifA,
                    settings,
                    chord);
        }

        if (motif.empty())
            continue;

        const auto chordTones =
            makeChordTonePool(
                chord,
                60,
                90);

        for (size_t i = 0;
             i < rhythm.size();
             ++i)
        {
            int note =
                motif[
                    i % motif.size()];

            // Strong beats prefer chord tones.
            const double localBeat =
                rhythm[i];

            const bool strongBeat =
                std::abs(
                    localBeat
                    - std::round(localBeat))
                < 0.01;

            if (strongBeat
                && !chordTones.empty()
                && chance(70))
            {
                note =
                    nearestPitch(
                        note,
                        chordTones,
                        5);
            }

            // Final phrase note resolves strongly.
            if (bar == 3
                && i == rhythm.size()-1
                && !chordTones.empty())
            {
                note =
                    nearestPitch(
                        note,
                        chordTones,
                        4);
            }

            // Style-specific octave behavior.
            if (settings.style.equalsIgnoreCase(
                    "Emotional")
                && chance(12))
            {
                note += 12;
            }

            if (settings.style.equalsIgnoreCase(
                    "Dark")
                && chance(20))
            {
                note -= 12;
            }

            note =
                juce::jlimit(
                    58,
                    92,
                    note);

            const double globalBeat =
                bar * 4.0
                + rhythm[i];

            double length =
                0.42;

            if (settings.style.equalsIgnoreCase(
                    "Smooth")
                ||
                settings.style.equalsIgnoreCase(
                    "Emotional"))
            {
                length =
                    chance(40)
                        ? 0.9
                        : 0.48;
            }
            else if (settings.style.equalsIgnoreCase(
                    "Bouncy"))
            {
                length =
                    chance(70)
                        ? 0.28
                        : 0.48;
            }

            int velocity =
                randomInt(
                    78,
                    108);

            double startBeat =
                globalBeat;

            if (settings.humanise > 0)
            {
                startBeat +=
                    randomDouble(
                        -0.02,
                        0.02)
                    *
                    (settings.humanise
                     / 25.0);

                velocity +=
                    randomInt(
                        -settings.humanise / 3,
                        settings.humanise / 3);
            }

            lead.push_back(
            {
                juce::jmax(
                    bar * 4.0,
                    startBeat),

                length,

                note,

                juce::jlimit(
                    1,
                    127,
                    velocity),

                2
            });

            previousNote =
                note;
        }
    }

    return lead;
}

bool MelodyEngine::leadIsActiveNear(
    const std::vector<NoteEvent>& lead,
    double beat,
    double tolerance) const
{
    for (const auto& event : lead)
    {
        const double start =
            event.beat;

        const double end =
            event.beat
            + event.lengthBeats;

        if (beat >= start - tolerance
            &&
            beat <= end + tolerance)
        {
            return true;
        }
    }

    return false;
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

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generateCounterCandidate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression,
    const std::vector<NoteEvent>& lead)
{
    std::vector<NoteEvent> counter;

    int previousNote = 55;

    for (int bar = 0;
         bar < settings.bars;
         ++bar)
    {
        const auto chord =
            progression[
                (size_t)(bar % 4)];

        const auto pool =
            makeMelodicPool(
                chord,
                43,
                72);

        // Counter melody probes offbeats
        // and spaces between lead phrases.
        static const double positions[] =
        {
            0.5,
            1.5,
            2.5,
            3.5
        };

        for (double localBeat :
             positions)
        {
            if (!chance(
                settings.counterDensity))
            {
                continue;
            }

            const double beat =
                bar * 4.0
                + localBeat;

            // Prefer spaces where lead is not active.
            if (leadIsActiveNear(
                    lead,
                    beat,
                    0.18))
            {
                if (chance(80))
                    continue;
            }

            int maxJump =
                settings.movement > 60
                ? 8
                : 5;

            int note =
                nearestPitch(
                    previousNote,
                    pool,
                    maxJump);

            // Find nearby lead note for contrary motion.
            int nearbyLead =
                72;

            double bestDistance =
                999.0;

            for (const auto& leadEvent :
                 lead)
            {
                const double distance =
                    std::abs(
                        leadEvent.beat
                        - beat);

                if (distance <
                    bestDistance)
                {
                    bestDistance =
                        distance;

                    nearbyLead =
                        leadEvent.note;
                }
            }

            if (nearbyLead > 74)
            {
                note =
                    juce::jmin(
                        note,
                        62);
            }
            else if (nearbyLead < 66)
            {
                note =
                    juce::jmax(
                        note,
                        50);
            }

            // Avoid exact unisons with lead.
            for (const auto& leadEvent :
                 lead)
            {
                if (std::abs(
                        leadEvent.beat
                        - beat)
                    < 0.15
                    &&
                    leadEvent.note
                    == note)
                {
                    note +=
                        chance(50)
                        ? 3
                        : -3;
                }
            }

            note =
                juce::jlimit(
                    43,
                    72,
                    note);

            double length =
                settings.style.equalsIgnoreCase(
                    "Smooth")
                ? 0.75
                : 0.45;

            int velocity =
                randomInt(
                    48,
                    76);

            counter.push_back(
            {
                beat,
                length,
                note,
                velocity,
                3
            });

            previousNote =
                note;
        }
    }

    return counter;
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

    // Reward sensible total density.
    const double idealNotes =
        10.0
        +
        settings.melodyDensity
        * 0.12;

    score -=
        std::abs(
            (double)lead.size()
            - idealNotes)
        * 1.5;

    int largeJumps = 0;
    int repeatedNotes = 0;
    int chordToneHits = 0;

    for (size_t i = 0;
         i < lead.size();
         ++i)
    {
        const auto& event =
            lead[i];

        const int chordIndex =
            juce::jlimit(
                0,
                3,
                (int)(event.beat / 4.0));

        const auto intervals =
            chordIntervals(
                progression[
                    (size_t)chordIndex]
                    .type);

        const int pc =
            (event.note
             -
             progression[
                (size_t)chordIndex]
                .root
             +
             120)
            % 12;

        for (int interval :
             intervals)
        {
            if ((interval % 12)
                == pc)
            {
                ++chordToneHits;
                break;
            }
        }

        if (i > 0)
        {
            const int jump =
                std::abs(
                    event.note
                    -
                    lead[i-1].note);

            if (jump > 12)
                ++largeJumps;

            if (event.note
                ==
                lead[i-1].note)
            {
                ++repeatedNotes;
            }
        }
    }

    score +=
        chordToneHits
        * 2.2;

    score -=
        largeJumps
        * 6.0;

    // Some repeated notes are catchy,
    // too many are dull.
    if (repeatedNotes > 0)
        score += 2.0;

    if (repeatedNotes
        > (int)lead.size()/2)
    {
        score -= 10.0;
    }

    // Reward phrase span without huge chaos.
    int minNote = 127;
    int maxNote = 0;

    for (const auto& event : lead)
    {
        minNote =
            juce::jmin(
                minNote,
                event.note);

        maxNote =
            juce::jmax(
                maxNote,
                event.note);
    }

    const int range =
        maxNote - minNote;

    if (range >= 5
        && range <= 19)
    {
        score += 10.0;
    }
    else if (range > 24)
    {
        score -= 10.0;
    }

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
        return -50.0;

    double score =
        0.0;

    int overlaps = 0;
    int freeSpaceHits = 0;
    int chordToneHits = 0;
    int unisons = 0;

    for (const auto& event :
         counter)
    {
        if (leadIsActiveNear(
                lead,
                event.beat,
                0.12))
        {
            ++overlaps;
        }
        else
        {
            ++freeSpaceHits;
        }

        const int chordIndex =
            juce::jlimit(
                0,
                3,
                (int)(event.beat / 4.0));

        const auto intervals =
            chordIntervals(
                progression[
                    (size_t)chordIndex]
                    .type);

        const int pc =
            (event.note
             -
             progression[
                (size_t)chordIndex]
                .root
             +
             120)
            % 12;

        for (int interval :
             intervals)
        {
            if ((interval % 12)
                == pc)
            {
                ++chordToneHits;
                break;
            }
        }

        for (const auto& leadEvent :
             lead)
        {
            if (std::abs(
                    leadEvent.beat
                    - event.beat)
                < 0.12
                &&
                leadEvent.note
                == event.note)
            {
                ++unisons;
            }
        }
    }

    score +=
        freeSpaceHits
        * 5.0;

    score +=
        chordToneHits
        * 2.0;

    score -=
        overlaps
        * 3.5;

    score -=
        unisons
        * 8.0;

    return score;
}

std::vector<MelodyEngine::NoteEvent>
MelodyEngine::generate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression)
{
    PhraseCandidate bestCandidate;

    bestCandidate.score =
        -100000.0;

    // Generate several full compositions,
    // score them, then keep the best one.
    constexpr int numberOfCandidates =
        12;

    for (int candidateIndex = 0;
         candidateIndex
            < numberOfCandidates;
         ++candidateIndex)
    {
        PhraseCandidate candidate;

        candidate.lead =
            generateLeadCandidate(
                settings,
                progression);

        candidate.counter =
            generateCounterCandidate(
                settings,
                progression,
                candidate.lead);

        candidate.score =
            scoreLead(
                candidate.lead,
                progression,
                settings)
            +
            scoreCounter(
                candidate.counter,
                candidate.lead,
                progression,
                settings);

        if (candidate.score
            >
            bestCandidate.score)
        {
            bestCandidate =
                std::move(candidate);
        }
    }

    std::vector<NoteEvent> result;

    // Add chord layer first.
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

        const size_t count =
            juce::jmin(
                (size_t)4,
                intervals.size());

        for (size_t i = 0;
             i < count;
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
                barBeat,
                3.80,
                note,
                66 - (int)i*3,
                1
            });
        }
    }

    result.insert(
        result.end(),
        bestCandidate.lead.begin(),
        bestCandidate.lead.end());

    result.insert(
        result.end(),
        bestCandidate.counter.begin(),
        bestCandidate.counter.end());

    std::sort(
        result.begin(),
        result.end(),
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

    return result;
}
