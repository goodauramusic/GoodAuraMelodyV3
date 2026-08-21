#include "PluginEditor.h"

// =====================================================
// CONSTRUCTOR
// =====================================================

GoodAuraMelodyAudioProcessorEditor::
GoodAuraMelodyAudioProcessorEditor(
    GoodAuraMelodyAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p)
{
    setSize(1120, 820);

    // =================================================
    // TITLE
    // =================================================

    title.setText(
        "GOOD AURA MELODY",
        juce::dontSendNotification);

    title.setFont(
        juce::Font(
            juce::FontOptions(30.0f)
                .withStyle("Bold")));

    title.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    addAndMakeVisible(title);

    subtitle.setText(
        "PROGRESSION -> MELODY -> COUNTER MELODY",
        juce::dontSendNotification);

    subtitle.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(subtitle);

    // =================================================
    // KEY
    // =================================================

    keyBox.addItemList(
        MelodyEngine::rootNames(),
        1);

    keyBox.setSelectedId(1);

    addAndMakeVisible(keyBox);

    // =================================================
    // MODE
    // =================================================

    modeBox.addItemList(
        ProgressionEngine::modeNames(),
        1);

    modeBox.setSelectedId(1);

    addAndMakeVisible(modeBox);

    // =================================================
    // GENRE
    // =================================================

    genreBox.addItemList(
        ProgressionEngine::genreNames(),
        1);

    const int rnbIndex =
        ProgressionEngine::genreNames()
            .indexOf("R&B");

    genreBox.setSelectedId(
        rnbIndex >= 0
            ? rnbIndex + 1
            : 1);

    addAndMakeVisible(genreBox);

    // =================================================
    // MOOD
    // =================================================

    moodBox.addItemList(
        ProgressionEngine::moodNames(),
        1);

    const int smoothMoodIndex =
        ProgressionEngine::moodNames()
            .indexOf("Smooth");

    moodBox.setSelectedId(
        smoothMoodIndex >= 0
            ? smoothMoodIndex + 1
            : 1);

    addAndMakeVisible(moodBox);

    // =================================================
    // MELODY STYLE
    // =================================================

    melodyStyleBox.addItemList(
        MelodyEngine::melodyStyleNames(),
        1);

    const int styleIndex =
        MelodyEngine::melodyStyleNames()
            .indexOf(
                processor.getMelodyStyle());

    melodyStyleBox.setSelectedId(
        styleIndex >= 0
            ? styleIndex + 1
            : 1);

    addAndMakeVisible(
        melodyStyleBox);

    // =================================================
    // SLIDERS
    // =================================================

    configureSlider(
        melodyDensity,
        "Melody Density",
        processor.melodyDensity.load());

    configureSlider(
        counterDensity,
        "Counter Density",
        processor.counterDensity.load());

    configureSlider(
        complexity,
        "Complexity",
        processor.complexity.load());

    configureSlider(
        humanise,
        "Humanise",
        processor.humanise.load());

    configureSlider(
        repetition,
        "Repetition",
        processor.repetition.load());

    configureSlider(
        movement,
        "Movement",
        processor.movement.load());

    // =================================================
    // SLIDER CONNECTIONS
    // =================================================

    melodyDensity.onValueChange =
        [this]
        {
            processor.melodyDensity =
                (int)melodyDensity.getValue();
        };

    counterDensity.onValueChange =
        [this]
        {
            processor.counterDensity =
                (int)counterDensity.getValue();
        };

    complexity.onValueChange =
        [this]
        {
            processor.complexity =
                (int)complexity.getValue();
        };

    humanise.onValueChange =
        [this]
        {
            processor.humanise =
                (int)humanise.getValue();
        };

    repetition.onValueChange =
        [this]
        {
            processor.repetition =
                (int)repetition.getValue();
        };

    movement.onValueChange =
        [this]
        {
            processor.movement =
                (int)movement.getValue();
        };

    // =================================================
    // GENERATE CHORDS
    // =================================================

    generateProgressionButton.onClick =
        [this]
        {
            syncGeneratorSettings();

            processor.generateProgression();

            refreshProgressionText();

            refreshPianoRoll();

            status.setText(
                "New progression + matching melodies generated.",
                juce::dontSendNotification);
        };

    addAndMakeVisible(
        generateProgressionButton);

    // =================================================
    // GENERATE MELODIES
    // =================================================

    generateMelodyButton.onClick =
        [this]
        {
            syncGeneratorSettings();

            processor.generateMelodies();

            refreshPianoRoll();

            status.setText(
                "New melody + counter melody generated.",
                juce::dontSendNotification);
        };

    addAndMakeVisible(
        generateMelodyButton);

    // =================================================
    // PLAY
    // =================================================

    playButton.onClick =
        [this]
        {
            processor.startPreview();

            pianoRoll.setPlaying(true);

            status.setText(
                "Playing preview.",
                juce::dontSendNotification);
        };

    addAndMakeVisible(playButton);

    // =================================================
    // STOP
    // =================================================

    stopButton.onClick =
        [this]
        {
            processor.stopPreview();

            pianoRoll.setPlaying(false);

            pianoRoll.setPlayheadBeat(0.0);

            status.setText(
                "Stopped.",
                juce::dontSendNotification);
        };

    addAndMakeVisible(stopButton);

    // =================================================
    // EXPORT MIDI
    // =================================================

    exportButton.onClick =
        [this]
        {
            fileChooser =
                std::make_unique<
                    juce::FileChooser>(
                        "Save generated MIDI",

                        juce::File::
                            getSpecialLocation(
                                juce::File::
                                    userDocumentsDirectory)
                            .getChildFile(
                                "GoodAuraMelody.mid"),

                        "*.mid");

            const auto flags =
                juce::FileBrowserComponent::saveMode
                |
                juce::FileBrowserComponent::canSelectFiles
                |
                juce::FileBrowserComponent::warnAboutOverwriting;

            fileChooser->launchAsync(
                flags,

                [this](
                    const juce::FileChooser&
                        chooser)
                {
                    const auto file =
                        chooser.getResult();

                    if (file != juce::File())
                    {
                        const bool success =
                            processor
                                .exportMidiToFile(
                                    file);

                        status.setText(
                            success
                                ? "MIDI exported."
                                : "MIDI export failed.",
                            juce::dontSendNotification);
                    }
                });
        };

    addAndMakeVisible(
        exportButton);

    // =================================================
    // PROGRESSION DISPLAY
    // =================================================

    progressionLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    progressionLabel.setFont(
        juce::Font(
            juce::FontOptions(21.0f)
                .withStyle("Bold")));

    progressionLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(
        progressionLabel);

    // =================================================
    // PIANO ROLL
    // =================================================

    addAndMakeVisible(
        pianoRoll);

    // =================================================
    // LAYER TOGGLES
    // =================================================

    showChordsButton.setToggleState(
        true,
        juce::dontSendNotification);

    showMelodyButton.setToggleState(
        true,
        juce::dontSendNotification);

    showCounterButton.setToggleState(
        true,
        juce::dontSendNotification);

    showChordsButton.onClick =
        [this]
        {
            pianoRoll.setShowChords(
                showChordsButton.getToggleState());
        };

    showMelodyButton.onClick =
        [this]
        {
            pianoRoll.setShowMelody(
                showMelodyButton.getToggleState());
        };

    showCounterButton.onClick =
        [this]
        {
            pianoRoll.setShowCounter(
                showCounterButton.getToggleState());
        };

    addAndMakeVisible(
        showChordsButton);

    addAndMakeVisible(
        showMelodyButton);

    addAndMakeVisible(
        showCounterButton);

    // =================================================
    // STATUS
    // =================================================

    status.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(
        status);

    // =================================================
    // INITIAL VIEW
    // =================================================

    refreshProgressionText();

    refreshPianoRoll();

    status.setText(
        "Choose settings, generate, then press Play.",
        juce::dontSendNotification);

    // GUI updates at 30 FPS.
    startTimerHz(30);
}

// =====================================================
// SLIDER SETUP
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
configureSlider(
    juce::Slider& slider,
    const juce::String& name,
    int initialValue)
{
    slider.setName(name);

    slider.setSliderStyle(
        juce::Slider::
            RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::
            TextBoxBelow,
        false,
        68,
        22);

    slider.setRange(
        0,
        100,
        1);

    slider.setValue(
        initialValue);

    addAndMakeVisible(
        slider);
}

// =====================================================
// COPY GUI SETTINGS TO PROCESSOR
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
syncGeneratorSettings()
{
    processor.setKeyRoot(
        keyBox.getSelectedId() - 1);

    processor.setMinorMode(
        modeBox.getSelectedId() == 2);

    processor.setGenre(
        genreBox.getText());

    processor.setMood(
        moodBox.getText());

    processor.setMelodyStyle(
        melodyStyleBox.getText());
}

// =====================================================
// UPDATE PROGRESSION LABEL
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
refreshProgressionText()
{
    progressionLabel.setText(
        processor.getProgressionText(),
        juce::dontSendNotification);
}

// =====================================================
// UPDATE PIANO ROLL
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
refreshPianoRoll()
{
    pianoRoll.setNotes(
        processor.getGeneratedNotes());

    pianoRoll.setProgression(
        processor.getProgression());

    pianoRoll.setPlayheadBeat(
        processor.getPreviewBeat());

    pianoRoll.setPlaying(
        processor.isPreviewing());
}

// =====================================================
// TIMER
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
timerCallback()
{
    pianoRoll.setPlayheadBeat(
        processor.getPreviewBeat());

    pianoRoll.setPlaying(
        processor.isPreviewing());
}

// =====================================================
// PAINT
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
paint(
    juce::Graphics& g)
{
    g.fillAll(
        juce::Colour(
            0xff0d1016));

    // Main panel
    g.setColour(
        juce::Colour(
            0xff171c25));

    g.fillRoundedRectangle(
        16.0f,
        16.0f,
        (float)getWidth() - 32.0f,
        (float)getHeight() - 32.0f,
        16.0f);

    // Top controls panel
    g.setColour(
        juce::Colour(
            0xff242b36));

    g.fillRoundedRectangle(
        30.0f,
        105.0f,
        (float)getWidth() - 60.0f,
        158.0f,
        12.0f);

    // Melody controls panel
    g.setColour(
        juce::Colour(
            0xff202631));

    g.fillRoundedRectangle(
        30.0f,
        285.0f,
        (float)getWidth() - 60.0f,
        175.0f,
        12.0f);

    g.setColour(
        juce::Colours::white);

    // Top labels
    g.drawText(
        "KEY",
        50,
        116,
        80,
        20,
        juce::Justification::left);

    g.drawText(
        "MODE",
        165,
        116,
        80,
        20,
        juce::Justification::left);

    g.drawText(
        "GENRE",
        280,
        116,
        90,
        20,
        juce::Justification::left);

    g.drawText(
        "MOOD",
        455,
        116,
        90,
        20,
        juce::Justification::left);

    g.drawText(
        "MELODY STYLE",
        630,
        116,
        140,
        20,
        juce::Justification::left);

    // Knob labels
    g.drawText(
        "MELODY",
        45,
        300,
        100,
        20,
        juce::Justification::left);

    g.drawText(
        "COUNTER",
        215,
        300,
        100,
        20,
        juce::Justification::left);

    g.drawText(
        "COMPLEXITY",
        385,
        300,
        110,
        20,
        juce::Justification::left);

    g.drawText(
        "HUMANISE",
        555,
        300,
        100,
        20,
        juce::Justification::left);

    g.drawText(
        "REPETITION",
        725,
        300,
        110,
        20,
        juce::Justification::left);

    g.drawText(
        "MOVEMENT",
        895,
        300,
        100,
        20,
        juce::Justification::left);

    // Piano roll title
    g.drawText(
        "PIANO ROLL",
        38,
        480,
        160,
        24,
        juce::Justification::left);
}

// =====================================================
// LAYOUT
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
resized()
{
    // =================================================
    // TITLE
    // =================================================

    title.setBounds(
        38,
        25,
        480,
        40);

    subtitle.setBounds(
        40,
        66,
        650,
        25);

    // =================================================
    // TOP CONTROLS
    // =================================================

    keyBox.setBounds(
        48,
        143,
        95,
        34);

    modeBox.setBounds(
        163,
        143,
        95,
        34);

    genreBox.setBounds(
        278,
        143,
        155,
        34);

    moodBox.setBounds(
        453,
        143,
        155,
        34);

    melodyStyleBox.setBounds(
        628,
        143,
        155,
        34);

    generateProgressionButton.setBounds(
        803,
        143,
        270,
        34);

    progressionLabel.setBounds(
        55,
        198,
        1010,
        42);

    // =================================================
    // KNOBS
    // =================================================

    melodyDensity.setBounds(
        25,
        323,
        150,
        120);

    counterDensity.setBounds(
        195,
        323,
        150,
        120);

    complexity.setBounds(
        365,
        323,
        150,
        120);

    humanise.setBounds(
        535,
        323,
        150,
        120);

    repetition.setBounds(
        705,
        323,
        150,
        120);

    movement.setBounds(
        875,
        323,
        150,
        120);

    // =================================================
    // PIANO ROLL
    // =================================================

    pianoRoll.setBounds(
        35,
        510,
        getWidth() - 70,
        220);

    // =================================================
    // LAYER TOGGLES
    // =================================================

    showChordsButton.setBounds(
        40,
        742,
        100,
        28);

    showMelodyButton.setBounds(
        145,
        742,
        100,
        28);

    showCounterButton.setBounds(
        250,
        742,
        100,
        28);

    // =================================================
    // BOTTOM BUTTONS
    // =================================================

    generateMelodyButton.setBounds(
        375,
        740,
        185,
        34);

    playButton.setBounds(
        575,
        740,
        85,
        34);

    stopButton.setBounds(
        670,
        740,
        85,
        34);

    exportButton.setBounds(
        765,
        740,
        140,
        34);

    status.setBounds(
        915,
        740,
        175,
        34);
}
