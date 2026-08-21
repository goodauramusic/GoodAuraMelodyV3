#include "PianoRollView.h"

// =====================================================
// CONSTRUCTOR
// =====================================================

PianoRollView::PianoRollView()
{
    setOpaque(true);

    startTimerHz(30);
}

// =====================================================
// DATA
// =====================================================

void PianoRollView::setNotes(
    const std::vector<
        MelodyEngine::NoteEvent>& newNotes)
{
    notes = newNotes;

    repaint();
}

void PianoRollView::setProgression(
    const std::array<
        MelodyEngine::ChordChoice,
        4>& newProgression)
{
    progression = newProgression;

    repaint();
}

void PianoRollView::setPlayheadBeat(
    double beat)
{
    playheadBeat =
        std::fmod(
            std::fmod(
                beat,
                totalBeats)
            +
            totalBeats,
            totalBeats);

    repaint();
}

void PianoRollView::setPlaying(
    bool shouldPlay)
{
    playing =
        shouldPlay;

    repaint();
}

void PianoRollView::setShowChords(
    bool shouldShow)
{
    showChords =
        shouldShow;

    repaint();
}

void PianoRollView::setShowMelody(
    bool shouldShow)
{
    showMelody =
        shouldShow;

    repaint();
}

void PianoRollView::setShowCounter(
    bool shouldShow)
{
    showCounter =
        shouldShow;

    repaint();
}

// =====================================================
// TIMER
// =====================================================

void PianoRollView::timerCallback()
{
    if (playing)
        repaint();
}

// =====================================================
// LAYOUT
// =====================================================

void PianoRollView::resized()
{
}

// =====================================================
// CONVERT BEAT -> X
// =====================================================

float PianoRollView::beatToX(
    double beat) const
{
    const float availableWidth =
        (float)getWidth()
        -
        keyboardWidth;

    return
        keyboardWidth
        +
        (float)(
            beat
            /
            totalBeats)
        *
        availableWidth;
}

// =====================================================
// NOTE HEIGHT
// =====================================================

float PianoRollView::noteHeight() const
{
    const float usableHeight =
        (float)getHeight()
        -
        chordHeaderHeight;

    const int noteCount =
        highestNote
        -
        lowestNote
        +
        1;

    return
        usableHeight
        /
        (float)noteCount;
}

// =====================================================
// MIDI NOTE -> Y
// =====================================================

float PianoRollView::noteToY(
    int midiNote) const
{
    const int clampedNote =
        juce::jlimit(
            lowestNote,
            highestNote,
            midiNote);

    const float h =
        noteHeight();

    return
        chordHeaderHeight
        +
        (
            highestNote
            -
            clampedNote
        )
        *
        h;
}

// =====================================================
// CHANNEL COLOUR
//
// CHANNEL 1 = CHORDS
// CHANNEL 2 = MELODY
// CHANNEL 3 = COUNTER
// =====================================================

juce::Colour
PianoRollView::colourForChannel(
    int midiChannel) const
{
    if (midiChannel == 1)
    {
        return
            juce::Colour(
                0xff8f4dff);
    }

    if (midiChannel == 2)
    {
        return
            juce::Colour(
                0xff2979ff);
    }

    return
        juce::Colour(
            0xff17c3b2);
}

// =====================================================
// SHOULD CHANNEL BE DRAWN?
// =====================================================

bool PianoRollView::shouldDrawChannel(
    int midiChannel) const
{
    if (midiChannel == 1)
        return showChords;

    if (midiChannel == 2)
        return showMelody;

    if (midiChannel == 3)
        return showCounter;

    return false;
}

// =====================================================
// CHORD NAME
// =====================================================

juce::String PianoRollView::chordName(
    const MelodyEngine::ChordChoice&
        chord) const
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

    const int root =
        juce::jlimit(
            0,
            roots.size() - 1,
            chord.root);

    const int type =
        juce::jlimit(
            0,
            types.size() - 1,
            chord.type);

    return
        roots[root]
        +
        types[type];
}

// =====================================================
// BACKGROUND
// =====================================================

void PianoRollView::drawBackground(
    juce::Graphics& g)
{
    g.fillAll(
        juce::Colour(
            0xff0c1118));

    g.setColour(
        juce::Colour(
            0xff131a24));

    g.fillRoundedRectangle(
        0.0f,
        0.0f,
        (float)getWidth(),
        (float)getHeight(),
        10.0f);
}

// =====================================================
// KEYBOARD
// =====================================================

void PianoRollView::drawKeyboard(
    juce::Graphics& g)
{
    const float h =
        noteHeight();

    for (int note = lowestNote;
         note <= highestNote;
         ++note)
    {
        const float y =
            noteToY(note);

        const int pitchClass =
            note % 12;

        const bool blackKey =
            pitchClass == 1
            ||
            pitchClass == 3
            ||
            pitchClass == 6
            ||
            pitchClass == 8
            ||
            pitchClass == 10;

        if (blackKey)
        {
            g.setColour(
                juce::Colour(
                    0xff242a34));
        }
        else
        {
            g.setColour(
                juce::Colour(
                    0xffd8dde5));
        }

        g.fillRect(
            0.0f,
            y,
            keyboardWidth,
            h + 1.0f);

        g.setColour(
            juce::Colour(
                0xff3a414c));

        g.drawRect(
            0.0f,
            y,
            keyboardWidth,
            h + 1.0f,
            0.5f);

        // Show C note labels.
        if (pitchClass == 0)
        {
            g.setColour(
                blackKey
                    ? juce::Colours::white
                    : juce::Colours::black);

            const int octave =
                (note / 12)
                - 1;

            g.drawText(
                "C"
                +
                juce::String(octave),
                4,
                (int)y,
                (int)keyboardWidth - 8,
                (int)h,
                juce::Justification::
                    centredLeft);
        }
    }
}

// =====================================================
// GRID
// =====================================================

void PianoRollView::drawGrid(
    juce::Graphics& g)
{
    const float startX =
        keyboardWidth;

    const float usableWidth =
        (float)getWidth()
        -
        keyboardWidth;

    const float bottom =
        (float)getHeight();

    // Horizontal note grid.
    const float h =
        noteHeight();

    for (int note = lowestNote;
         note <= highestNote;
         ++note)
    {
        const float y =
            noteToY(note);

        const int pitchClass =
            note % 12;

        if (pitchClass == 0)
        {
            g.setColour(
                juce::Colour(
                    0xff283342));
        }
        else
        {
            g.setColour(
                juce::Colour(
                    0xff1b2430));
        }

        g.drawHorizontalLine(
            (int)y,
            startX,
            (float)getWidth());
    }

    // Quarter beat grid.
    for (int division = 0;
         division <= 64;
         ++division)
    {
        const double beat =
            division * 0.25;

        const float x =
            beatToX(beat);

        if (division % 16 == 0)
        {
            // Bar line.
            g.setColour(
                juce::Colour(
                    0xff596579));

            g.drawLine(
                x,
                chordHeaderHeight,
                x,
                bottom,
                1.4f);
        }
        else if (division % 4 == 0)
        {
            // Beat line.
            g.setColour(
                juce::Colour(
                    0xff344052));

            g.drawLine(
                x,
                chordHeaderHeight,
                x,
                bottom,
                0.9f);
        }
        else
        {
            // Subdivision line.
            g.setColour(
                juce::Colour(
                    0xff202a36));

            g.drawLine(
                x,
                chordHeaderHeight,
                x,
                bottom,
                0.5f);
        }
    }
}

// =====================================================
// CHORD HEADERS
// =====================================================

void PianoRollView::drawChordHeaders(
    juce::Graphics& g)
{
    const float availableWidth =
        (float)getWidth()
        -
        keyboardWidth;

    const float barWidth =
        availableWidth
        /
        4.0f;

    g.setColour(
        juce::Colour(
            0xff161e29));

    g.fillRect(
        keyboardWidth,
        0.0f,
        availableWidth,
        chordHeaderHeight);

    for (int bar = 0;
         bar < 4;
         ++bar)
    {
        const float x =
            keyboardWidth
            +
            bar * barWidth;

        g.setColour(
            juce::Colour(
                0xff303b4b));

        g.drawRect(
            x,
            0.0f,
            barWidth,
            chordHeaderHeight,
            1.0f);

        g.setColour(
            juce::Colours::white);

        g.setFont(
            juce::Font(
                juce::FontOptions(
                    15.0f)
                    .withStyle(
                        "Bold")));

        g.drawText(
            chordName(
                progression[
                    (size_t)bar]),

            (int)x,
            0,
            (int)barWidth,
            (int)chordHeaderHeight,

            juce::Justification::
                centred);
    }
}

// =====================================================
// DRAW NOTES
// =====================================================

void PianoRollView::drawNotes(
    juce::Graphics& g)
{
    const float h =
        noteHeight();

    for (const auto& event :
         notes)
    {
        if (!shouldDrawChannel(
                event.channel))
        {
            continue;
        }

        const float x =
            beatToX(
                event.beat);

        const float endX =
            beatToX(
                event.beat
                +
                event.lengthBeats);

        const float y =
            noteToY(
                event.note);

        float width =
            juce::jmax(
                3.0f,
                endX - x - 1.0f);

        const float noteBlockHeight =
            juce::jmax(
                3.0f,
                h - 1.0f);

        auto colour =
            colourForChannel(
                event.channel);

        // Slight transparency
        // for chord layer.
        if (event.channel == 1)
        {
            colour =
                colour.withAlpha(
                    0.80f);
        }

        g.setColour(
            colour);

        g.fillRoundedRectangle(
            x + 1.0f,
            y + 1.0f,
            width,
            noteBlockHeight,
            2.0f);

        g.setColour(
            colour.brighter(
                0.25f));

        g.drawRoundedRectangle(
            x + 1.0f,
            y + 1.0f,
            width,
            noteBlockHeight,
            2.0f,
            0.7f);
    }
}

// =====================================================
// PLAYHEAD
// =====================================================

void PianoRollView::drawPlayhead(
    juce::Graphics& g)
{
    if (!playing)
        return;

    const float x =
        beatToX(
            playheadBeat);

    g.setColour(
        juce::Colours::white);

    g.drawLine(
        x,
        0.0f,
        x,
        (float)getHeight(),
        2.0f);

    // Playhead triangle
    juce::Path triangle;

    triangle.addTriangle(
        x - 6.0f,
        0.0f,

        x + 6.0f,
        0.0f,

        x,
        9.0f);

    g.fillPath(
        triangle);
}

// =====================================================
// MAIN PAINT
// =====================================================

void PianoRollView::paint(
    juce::Graphics& g)
{
    drawBackground(g);

    drawChordHeaders(g);

    drawGrid(g);

    drawKeyboard(g);

    drawNotes(g);

    drawPlayhead(g);
}
