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
    setSize(1240, 900);

    // =================================================
    // HEADER
    // =================================================

    title.setText(
        "GOOD AURA MELODY V6",
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
        "ADVANCED MELODY + COUNTER MELODY GENERATOR",
        juce::dontSendNotification);

    subtitle.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(subtitle);

    // =================================================
    // TOP PROGRESSION CONTROLS
    // =================================================

    keyBox.addItemList(
        MelodyEngine::rootNames(),
        1);

    keyBox.setSelectedId(1);

    modeBox.addItemList(
        ProgressionEngine::modeNames(),
        1);

    modeBox.setSelectedId(1);

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

    moodBox.addItemList(
        ProgressionEngine::moodNames(),
        1);

    const int moodIndex =
        ProgressionEngine::moodNames()
            .indexOf("Smooth");

    moodBox.setSelectedId(
        moodIndex >= 0
            ? moodIndex + 1
            : 1);

    configureComboBox(keyBox);
    configureComboBox(modeBox);
    configureComboBox(genreBox);
    configureComboBox(moodBox);

    addAndMakeVisible(keyBox);
    addAndMakeVisible(modeBox);
    addAndMakeVisible(genreBox);
    addAndMakeVisible(moodBox);

    // =================================================
    // MELODY STYLE
    // =================================================

    melodyStyleBox.addItemList(
        MelodyEngine::melodyStyleNames(),
        1);

    const int melodyStyleIndex =
        MelodyEngine::melodyStyleNames()
            .indexOf(
                processor.getMelodyStyle());

    melodyStyleBox.setSelectedId(
        melodyStyleIndex >= 0
            ? melodyStyleIndex + 1
            : 1);

    configureComboBox(
        melodyStyleBox);

    addAndMakeVisible(
        melodyStyleBox);

    // =================================================
    // SONG / ARRANGEMENT
    // =================================================

    sectionBox.addItemList(
        MelodyEngine::sectionNames(),
        1);

    const int sectionIndex =
        MelodyEngine::sectionNames()
            .indexOf(
                processor.getSection());

    sectionBox.setSelectedId(
        sectionIndex >= 0
            ? sectionIndex + 1
            : 1);

    contourBox.addItemList(
        MelodyEngine::contourNames(),
        1);

    const int contourIndex =
        MelodyEngine::contourNames()
            .indexOf(
                processor.getContour());

    contourBox.setSelectedId(
        contourIndex >= 0
            ? contourIndex + 1
            : 1);

    pocketBox.addItemList(
        MelodyEngine::pocketNames(),
        1);

    const int pocketIndex =
        MelodyEngine::pocketNames()
            .indexOf(
                processor.getPocket());

    pocketBox.setSelectedId(
        pocketIndex >= 0
            ? pocketIndex + 1
            : 1);

    counterModeBox.addItemList(
        MelodyEngine::counterModeNames(),
        1);

    const int counterModeIndex =
        MelodyEngine::counterModeNames()
            .indexOf(
                processor.getCounterMode());

    counterModeBox.setSelectedId(
        counterModeIndex >= 0
            ? counterModeIndex + 1
            : 1);

    configureComboBox(
        sectionBox);

    configureComboBox(
        contourBox);

    configureComboBox(
        pocketBox);

    configureComboBox(
        counterModeBox);

    addAndMakeVisible(sectionBox);
    addAndMakeVisible(contourBox);
    addAndMakeVisible(pocketBox);
    addAndMakeVisible(counterModeBox);

    // =================================================
    // PROGRESSION DISPLAY
    // =================================================

    progressionLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    progressionLabel.setFont(
        juce::Font(
            juce::FontOptions(20.0f)
                .withStyle("Bold")));

    progressionLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(
        progressionLabel);

    // =================================================
    // GENERATOR BUTTONS
    // =================================================

    configureButton(
        generateProgressionButton);

    configureButton(
        generateMelodyButton);

    configureButton(
        lockIdeaButton);

    configureButton(
        unlockIdeaButton);

    configureButton(
        playButton);

    configureButton(
        stopButton);

    configureButton(
        exportButton);

    addAndMakeVisible(
        generateProgressionButton);

    addAndMakeVisible(
        generateMelodyButton);

    addAndMakeVisible(
        lockIdeaButton);

    addAndMakeVisible(
        unlockIdeaButton);

    addAndMakeVisible(
        playButton);

    addAndMakeVisible(
        stopButton);

    addAndMakeVisible(
        exportButton);

    // =================================================
    // ORIGINAL CONTROLS
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
    // V6 ADVANCED CONTROLS
    // =================================================

    configureSlider(
        hookStrength,
        "Hook Strength",
        processor.hookStrength.load());

    configureSlider(
        variation,
        "Variation",
        processor.variation.load());

    configureSlider(
        surprise,
        "Surprise",
        processor.surprise.load());

    configureSlider(
        tension,
        "Tension",
        processor.tension.load());

    configureSlider(
        syncopation,
        "Syncopation",
        processor.syncopation.load());

    configureSlider(
        restAmount,
        "Rest Amount",
        processor.restAmount.load());

    configureSlider(
        registerSpread,
        "Register Spread",
        processor.registerSpread.load());

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

    hookStrength.onValueChange =
        [this]
        {
            processor.hookStrength =
                (int)hookStrength.getValue();
        };

    variation.onValueChange =
        [this]
        {
            processor.variation =
                (int)variation.getValue();
        };

    surprise.onValueChange =
        [this]
        {
            processor.surprise =
                (int)surprise.getValue();
        };

    tension.onValueChange =
        [this]
        {
            processor.tension =
                (int)tension.getValue();
        };

    syncopation.onValueChange =
        [this]
        {
            processor.syncopation =
                (int)syncopation.getValue();
        };

    restAmount.onValueChange =
        [this]
        {
            processor.restAmount =
                (int)restAmount.getValue();
        };

    registerSpread.onValueChange =
        [this]
        {
            processor.registerSpread =
                (int)registerSpread.getValue();
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
                "New progression generated.",
                juce::dontSendNotification);
        };

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
                "Advanced melody + counter generated.",
                juce::dontSendNotification);
        };

    // =================================================
    // LOCK IDEA
    // =================================================

    lockIdeaButton.onClick =
        [this]
        {
            processor.lockIdea();

            refreshIdeaStatus();

            status.setText(
                "Melodic idea locked.",
                juce::dontSendNotification);
        };

    unlockIdeaButton.onClick =
        [this]
        {
            processor.unlockIdea();

            refreshIdeaStatus();

            status.setText(
                "Melodic idea unlocked.",
                juce::dontSendNotification);
        };

    // =================================================
    // PLAY
    // =================================================

    playButton.onClick =
        [this]
        {
            processor.startPreview();

            pianoRoll.setPlaying(
                true);

            status.setText(
                "Playing preview.",
                juce::dontSendNotification);
        };

    // =================================================
    // STOP
    // =================================================

    stopButton.onClick =
        [this]
        {
            processor.stopPreview();

            pianoRoll.setPlaying(
                false);

            pianoRoll.setPlayheadBeat(
                0.0);

            status.setText(
                "Stopped.",
                juce::dontSendNotification);
        };

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
                            "GoodAuraMelodyV6.mid"),

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

    // =================================================
    // PIANO ROLL
    // =================================================

    addAndMakeVisible(
        pianoRoll);

    // =================================================
    // LAYERS
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
    // IDEA STATUS
    // =================================================

    ideaStatusLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    ideaStatusLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(
        ideaStatusLabel);

    // =================================================
    // GENERAL STATUS
    // =================================================

    status.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(
        status);

    refreshProgressionText();

    refreshPianoRoll();

    refreshIdeaStatus();

    status.setText(
        "Ready.",
        juce::dontSendNotification);

    startTimerHz(30);
}

// =====================================================
// SLIDER CONFIG
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
configureSlider(
    juce::Slider& slider,
    const juce::String& name,
    int initialValue)
{
    slider.setName(
        name);

    slider.setSliderStyle(
        juce::Slider::
            RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::
            TextBoxBelow,
        false,
        62,
        20);

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
// COMBO CONFIG
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
configureComboBox(
    juce::ComboBox& box)
{
    box.setColour(
        juce::ComboBox::backgroundColourId,
        juce::Colour(
            0xff151a23));

    box.setColour(
        juce::ComboBox::textColourId,
        juce::Colours::white);

    box.setColour(
        juce::ComboBox::outlineColourId,
        juce::Colour(
            0xff394150));
}

// =====================================================
// BUTTON CONFIG
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
configureButton(
    juce::TextButton& button)
{
    button.setColour(
        juce::TextButton::buttonColourId,
        juce::Colour(
            0xff6124a8));

    button.setColour(
        juce::TextButton::buttonOnColourId,
        juce::Colour(
            0xff8142d6));

    button.setColour(
        juce::TextButton::textColourOffId,
        juce::Colours::white);
}

// =====================================================
// SYNC SETTINGS
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
syncGeneratorSettings()
{
    processor.setKeyRoot(
        keyBox.getSelectedId()
        - 1);

    processor.setMinorMode(
        modeBox.getSelectedId()
        == 2);

    processor.setGenre(
        genreBox.getText());

    processor.setMood(
        moodBox.getText());

    processor.setMelodyStyle(
        melodyStyleBox.getText());

    processor.setSection(
        sectionBox.getText());

    processor.setContour(
        contourBox.getText());

    processor.setPocket(
        pocketBox.getText());

    processor.setCounterMode(
        counterModeBox.getText());
}

// =====================================================
// REFRESH PROGRESSION
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
refreshProgressionText()
{
    progressionLabel.setText(
        processor.getProgressionText(),
        juce::dontSendNotification);
}

// =====================================================
// REFRESH PIANO ROLL
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
// IDEA STATUS
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
refreshIdeaStatus()
{
    ideaStatusLabel.setText(
        processor.isIdeaLocked()
            ? "IDEA LOCKED"
            : "IDEA UNLOCKED",
        juce::dontSendNotification);
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
            0xff0b0e14));

    // Main background
    g.setColour(
        juce::Colour(
            0xff151a22));

    g.fillRoundedRectangle(
        12.0f,
        12.0f,
        (float)getWidth() - 24.0f,
        (float)getHeight() - 24.0f,
        14.0f);

    // Top section
    g.setColour(
        juce::Colour(
            0xff202631));

    g.fillRoundedRectangle(
        24.0f,
        96.0f,
        (float)getWidth() - 48.0f,
        130.0f,
        10.0f);

    // Song section
    g.fillRoundedRectangle(
        24.0f,
        240.0f,
        250.0f,
        220.0f,
        10.0f);

    // Melody section
    g.fillRoundedRectangle(
        286.0f,
        240.0f,
        610.0f,
        220.0f,
        10.0f);

    // Idea section
    g.fillRoundedRectangle(
        908.0f,
        240.0f,
        308.0f,
        220.0f,
        10.0f);

    g.setColour(
        juce::Colours::white);

    g.drawText(
        "SONG & ARRANGEMENT",
        38,
        250,
        220,
        24,
        juce::Justification::left);

    g.drawText(
        "MELODY CHARACTER",
        300,
        250,
        220,
        24,
        juce::Justification::left);

    g.drawText(
        "IDEA CONTROLS",
        930,
        250,
        220,
        24,
        juce::Justification::left);

    g.drawText(
        "PIANO ROLL - 4 BARS",
        28,
        480,
        220,
        24,
        juce::Justification::left);

    // Knob labels
    const char* labels[] =
    {
        "MELODY",
        "COUNTER",
        "COMPLEXITY",
        "HUMANISE",
        "REPETITION",
        "MOVEMENT",
        "HOOK",
        "VARIATION",
        "SURPRISE",
        "TENSION",
        "SYNCOPATION",
        "REST",
        "REGISTER"
    };

    const int xs[] =
    {
        300, 385, 470, 555, 640, 725,
        300, 385, 470, 555, 640, 725, 810
    };

    const int ys[] =
    {
        282, 282, 282, 282, 282, 282,
        365, 365, 365, 365, 365, 365, 365
    };

    for (int i = 0; i < 13; ++i)
    {
        g.drawText(
            labels[i],
            xs[i],
            ys[i],
            82,
            18,
            juce::Justification::centred);
    }
}

// =====================================================
// RESIZED
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
resized()
{
    // =================================================
    // HEADER
    // =================================================

    title.setBounds(
        28,
        20,
        470,
        36);

    subtitle.setBounds(
        30,
        57,
        580,
        22);

    // =================================================
    // TOP ROW
    // =================================================

    keyBox.setBounds(
        35,
        130,
        90,
        32);

    modeBox.setBounds(
        135,
        130,
        95,
        32);

    genreBox.setBounds(
        240,
        130,
        140,
        32);

    moodBox.setBounds(
        390,
        130,
        140,
        32);

    melodyStyleBox.setBounds(
        540,
        130,
        150,
        32);

    generateProgressionButton.setBounds(
        705,
        130,
        185,
        32);

    progressionLabel.setBounds(
        35,
        175,
        855,
        34);

    playButton.setBounds(
        925,
        126,
        80,
        36);

    stopButton.setBounds(
        1015,
        126,
        80,
        36);

    exportButton.setBounds(
        1105,
        126,
        100,
        36);

    // =================================================
    // SONG / ARRANGEMENT
    // =================================================

    sectionBox.setBounds(
        42,
        292,
        210,
        32);

    contourBox.setBounds(
        42,
        334,
        210,
        32);

    pocketBox.setBounds(
        42,
        376,
        210,
        32);

    counterModeBox.setBounds(
        42,
        418,
        210,
        32);

    // =================================================
    // MELODY CHARACTER
    // =================================================

    melodyDensity.setBounds(
        300,
        300,
        75,
        68);

    counterDensity.setBounds(
        385,
        300,
        75,
        68);

    complexity.setBounds(
        470,
        300,
        75,
        68);

    humanise.setBounds(
        555,
        300,
        75,
        68);

    repetition.setBounds(
        640,
        300,
        75,
        68);

    movement.setBounds(
        725,
        300,
        75,
        68);

    hookStrength.setBounds(
        300,
        383,
        75,
        68);

    variation.setBounds(
        385,
        383,
        75,
        68);

    surprise.setBounds(
        470,
        383,
        75,
        68);

    tension.setBounds(
        555,
        383,
        75,
        68);

    syncopation.setBounds(
        640,
        383,
        75,
        68);

    restAmount.setBounds(
        725,
        383,
        75,
        68);

    registerSpread.setBounds(
        810,
        383,
        75,
        68);

    // =================================================
    // IDEA CONTROLS
    // =================================================

    ideaStatusLabel.setBounds(
        930,
        292,
        260,
        36);

    lockIdeaButton.setBounds(
        932,
        350,
        120,
        36);

    unlockIdeaButton.setBounds(
        1065,
        350,
        120,
        36);

    generateMelodyButton.setBounds(
        932,
        405,
        253,
        38);

    // =================================================
    // PIANO ROLL
    // =================================================

    pianoRoll.setBounds(
        28,
        510,
        getWidth() - 56,
        285);

    // =================================================
    // LAYERS
    // =================================================

    showChordsButton.setBounds(
        35,
        807,
        90,
        28);

    showMelodyButton.setBounds(
        130,
        807,
        90,
        28);

    showCounterButton.setBounds(
        225,
        807,
        100,
        28);

    status.setBounds(
        350,
        807,
        850,
        28);
}
