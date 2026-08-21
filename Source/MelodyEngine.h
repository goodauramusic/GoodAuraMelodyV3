#pragma once

#include <JuceHeader.h>

#include <array>
#include <random>
#include <vector>

class MelodyEngine
{
public:
    // =====================================================
    // BASIC MUSICAL DATA
    // =====================================================

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

    // =====================================================
    // HIGH-LEVEL MUSICAL SETTINGS
    // =====================================================

    struct Settings
    {
        int bars = 4;

        int melodyDensity = 65;
        int counterDensity = 45;
        int complexity = 55;
        int humanise = 10;

        int repetition = 60;
        int movement = 50;

        // New V6 controls
        int hookStrength = 65;
        int variation = 45;
        int surprise = 20;
        int tension = 40;
        int syncopation = 50;
        int restAmount = 35;
        int registerSpread = 45;

        juce::String style = "Smooth";

        juce::String section = "Verse";

        juce::String contour = "Wave";

        juce::String pocket = "Centre";

        juce::String counterMode =
            "Fill The Gaps";
    };

    // =====================================================
    // MOTIF
    // =====================================================

    struct Motif
    {
        std::vector<int> intervals;

        std::vector<double> rhythm;

        std::vector<double> lengths;

        int anchorNote = 72;

        bool valid = false;
    };

    // =====================================================
    // CANDIDATE
    // =====================================================

    struct PhraseCandidate
    {
        std::vector<NoteEvent> lead;

        std::vector<NoteEvent> counter;

        double leadScore = 0.0;

        double counterScore = 0.0;

        double totalScore = 0.0;
    };

    // =====================================================
    // PUBLIC
    // =====================================================

    MelodyEngine();

    static juce::StringArray rootNames();

    static juce::StringArray chordNames();

    static juce::StringArray melodyStyleNames();

    static juce::StringArray sectionNames();

    static juce::StringArray contourNames();

    static juce::StringArray pocketNames();

    static juce::StringArray counterModeNames();

    static std::vector<int>
    chordIntervals(
        int chordType);

    std::vector<NoteEvent>
    generate(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression);

    // =====================================================
    // LOCK IDEA
    // =====================================================

    void lockCurrentMotif();

    void unlockMotif();

    bool isMotifLocked() const;

private:
    // =====================================================
    // RANDOM
    // =====================================================

    std::mt19937 rng;

    int randomInt(
        int low,
        int high);

    double randomDouble(
        double low,
        double high);

    bool chance(
        int percent);

    // =====================================================
    // MOTIF MEMORY
    // =====================================================

    Motif currentMotif;

    Motif lockedMotif;

    bool motifLocked =
        false;

    // =====================================================
    // SCALE / CHORD HELPERS
    // =====================================================

    std::vector<int>
    makeChordTonePool(
        const ChordChoice& chord,
        int low,
        int high) const;

    std::vector<int>
    makeExtensionPool(
        const ChordChoice& chord,
        int low,
        int high) const;

    std::vector<int>
    makeMelodicPool(
        const ChordChoice& chord,
        int low,
        int high) const;

    bool isChordTone(
        int midiNote,
        const ChordChoice& chord) const;

    bool isExtensionTone(
        int midiNote,
        const ChordChoice& chord) const;

    // =====================================================
    // PITCH / VOICE LEADING
    // =====================================================

    int nearestPitch(
        int previous,
        const std::vector<int>& pool,
        int maximumJump);

    int voiceLeadToNextChord(
        int previousNote,
        const ChordChoice& currentChord,
        const ChordChoice& nextChord,
        int low,
        int high);

    int chooseTargetTone(
        const ChordChoice& chord,
        int previousNote,
        const Settings& settings,
        bool strongBeat);

    int applyContour(
        int note,
        int phrasePosition,
        int totalPositions,
        const Settings& settings);

    // =====================================================
    // MOTIF CREATION
    // =====================================================

    Motif createMotif(
        const Settings& settings,
        const ChordChoice& chord,
        int startingNote);

    Motif mutateMotif(
        const Motif& source,
        const Settings& settings,
        const ChordChoice& chord,
        int barIndex);

    Motif transposeMotifToChord(
        const Motif& source,
        const ChordChoice& chord,
        int startingNote);

    Motif invertMotif(
        const Motif& source);

    Motif reverseMotif(
        const Motif& source);

    // =====================================================
    // RHYTHM
    // =====================================================

    std::vector<double>
    createRhythmPattern(
        const Settings& settings,
        int barIndex);

    std::vector<double>
    applySyncopation(
        const std::vector<double>& rhythm,
        const Settings& settings);

    std::vector<double>
    applySectionRhythm(
        const std::vector<double>& rhythm,
        const Settings& settings,
        int barIndex);

    double applyPocketOffset(
        double beat,
        const Settings& settings);

    // =====================================================
    // LEAD GENERATION
    // =====================================================

    std::vector<NoteEvent>
    generateLeadCandidate(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression);

    void addPassingNotes(
        std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression,
        const Settings& settings);

    void addApproachNotes(
        std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression,
        const Settings& settings);

    void addEnclosures(
        std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression,
        const Settings& settings);

    void addAnticipations(
        std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression,
        const Settings& settings);

    void applyPhraseResolution(
        std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression,
        const Settings& settings);

    // =====================================================
    // COUNTER GENERATION
    // =====================================================

    std::vector<NoteEvent>
    generateCounterCandidate(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression,
        const std::vector<NoteEvent>& lead);

    std::vector<NoteEvent>
    generateAnswerCounter(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression,
        const std::vector<NoteEvent>& lead);

    std::vector<NoteEvent>
    generateHarmonyCounter(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression,
        const std::vector<NoteEvent>& lead);

    std::vector<NoteEvent>
    generateOppositeMotionCounter(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression,
        const std::vector<NoteEvent>& lead);

    std::vector<NoteEvent>
    generateGapFillCounter(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression,
        const std::vector<NoteEvent>& lead);

    // =====================================================
    // ANALYSIS HELPERS
    // =====================================================

    bool leadIsActiveNear(
        const std::vector<NoteEvent>& lead,
        double beat,
        double tolerance) const;

    int findNearestLeadNote(
        const std::vector<NoteEvent>& lead,
        double beat) const;

    int currentChordIndex(
        double beat) const;

    // =====================================================
    // SCORING
    // =====================================================

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

    double scoreMotifStrength(
        const std::vector<NoteEvent>& lead,
        const Settings& settings) const;

    double scoreVoiceLeading(
        const std::vector<NoteEvent>& lead) const;

    double scoreRhythmicInterest(
        const std::vector<NoteEvent>& lead) const;

    double scoreTensionResolution(
        const std::vector<NoteEvent>& lead,
        const std::array<
            ChordChoice,
            4>& progression) const;

    double scoreRegisterControl(
        const std::vector<NoteEvent>& notes) const;

    double scoreCounterSeparation(
        const std::vector<NoteEvent>& counter,
        const std::vector<NoteEvent>& lead) const;

    // =====================================================
    // HUMANISATION
    // =====================================================

    void humaniseNotes(
        std::vector<NoteEvent>& notes,
        const Settings& settings);

    // =====================================================
    // FINAL ASSEMBLY
    // =====================================================

    void addChordLayer(
        std::vector<NoteEvent>& result,
        const std::array<
            ChordChoice,
            4>& progression,
        int bars);

    void sortEvents(
        std::vector<NoteEvent>& events);
};
