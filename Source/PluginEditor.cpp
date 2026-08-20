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
    setSize(940, 620);

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

    // =================================================
    // SUBTITLE
    // =================================================

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

    keyBox.onChange =
        [this]
        {
            processor.setKeyRoot(
                keyBox.getSelectedId() - 1);
        };

    addAndMakeVisible(keyBox);

    // =================================================
    // MODE
    // =================================================

    modeBox.addItemList(
        ProgressionEngine::modeNames(),
        1);

    modeBox.setSelectedId(1);

    modeBox.onChange =
        [this]
        {
            processor.setMinorMode(
                modeBox.getSelectedId() == 2);
        };

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

    genreBox.onChange =
        [this]
        {
            processor.setGenre(
                genreBox.getText());
        };

    addAndMakeVisible(genreBox);

    // =================================================
    // MOOD
    // =================================================

    moodBox.addItemList(
        ProgressionEngine::moodNames(),
        1);

    const int smoothIndex =
        ProgressionEngine::moodNames()
            .indexOf("Smooth");

    moodBox.setSelectedId(
        smoothIndex >= 0
            ? smoothIndex + 1
            : 1);

    moodBox.onChange =
        [this]
        {
            processor.setMood(
                moodBox.getText());
        };

    addAndMakeVisible(moodBox);

    // =================================================
    // MELODY CONTROLS
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

    // =================================================
    // GENERATE CHORDS
    // =================================================

    generateProgressionButton.onClick =
        [this]
        {
            syncGeneratorSettings();

            processor.generateProgression();

            refreshProgressionText();

            status.setText(
                "New progression generated.",
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
            processor.generateMelodies();

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
                juce::FileBrowserComponent::
                    saveMode
                |
                juce::FileBrowserComponent::
                    canSelectFiles
                |
                juce::FileBrowserComponent::
                    warnAboutOverwriting;

            fileChooser->launchAsync(
                flags,

                [this](
                    const juce::FileChooser&
                        chooser)
                {
                    const auto file =
                        chooser.getResult();

                    if (file !=
                        juce::File())
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
            juce::FontOptions(22.0f)
                .withStyle("Bold")));

    progressionLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(
        progressionLabel);

    // =================================================
    // STATUS
    // =================================================

    status.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(status);

    refreshProgressionText();

    status.setText(
        "Choose key, mode, genre and mood -> Generate Chords.",
        juce::dontSendNotification);
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
        72,
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
}

// =====================================================
// UPDATE CHORD DISPLAY
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
refreshProgressionText()
{
    progressionLabel.setText(
        processor.getProgressionText(),
        juce::dontSendNotification);
}

// =====================================================
// DRAW BACKGROUND
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
paint(
    juce::Graphics& g)
{
    // Main background
    g.fillAll(
        juce::Colour(
            0xff101116));

    // Main panel
    g.setColour(
        juce::Colour(
            0xff1d2029));

    g.fillRoundedRectangle(
        18.0f,
        18.0f,
        (float)getWidth() - 36.0f,
        (float)getHeight() - 36.0f,
        16.0f);

    // Generator panel
    g.setColour(
        juce::Colour(
            0xff323745));

    g.fillRoundedRectangle(
        34.0f,
        108.0f,
        (float)getWidth() - 68.0f,
        160.0f,
        12.0f);

    g.setColour(
        juce::Colours::white);

    // Selection labels
    g.drawText(
        "KEY",
        50,
        118,
        100,
        20,
        juce::Justification::left);

    g.drawText(
        "MODE",
        190,
        118,
        100,
        20,
        juce::Justification::left);

    g.drawText(
        "GENRE",
        330,
        118,
        100,
        20,
        juce::Justification::left);

    g.drawText(
        "MOOD",
        520,
        118,
        100,
        20,
        juce::Justification::left);

    // Knob labels
    g.drawText(
        "MELODY",
        60,
        330,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "COUNTER",
        270,
        330,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "COMPLEXITY",
        480,
        330,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "HUMANISE",
        690,
        330,
        120,
        22,
        juce::Justification::left);
}

// =====================================================
// LAYOUT
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
resized()
{
    // Title
    title.setBounds(
        40,
        28,
        500,
        40);

    subtitle.setBounds(
        42,
        70,
        600,
        25);

    // Top controls
    keyBox.setBounds(
        48,
        145,
        110,
        36);

    modeBox.setBounds(
        188,
        145,
        110,
        36);

    genreBox.setBounds(
        328,
        145,
        160,
        36);

    moodBox.setBounds(
        518,
        145,
        160,
        36);

    generateProgressionButton.setBounds(
        708,
        145,
        175,
        36);

    // Generated progression
    progressionLabel.setBounds(
        60,
        200,
        820,
        48);

    // Knobs
    melodyDensity.setBounds(
        38,
        360,
        170,
        135);

    counterDensity.setBounds(
        248,
        360,
        170,
        135);

    complexity.setBounds(
        458,
        360,
        170,
        135);

    humanise.setBounds(
        668,
        360,
        170,
        135);

    // Bottom controls
    generateMelodyButton.setBounds(
        42,
        515,
        200,
        40);

    playButton.setBounds(
        258,
        515,
        90,
        40);

    stopButton.setBounds(
        358,
        515,
        90,
        40);

    exportButton.setBounds(
        458,
        515,
        150,
        40);

    status.setBounds(
        620,
        515,
        280,
        40);
}
