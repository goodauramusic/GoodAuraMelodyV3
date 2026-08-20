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
    setSize(1040, 700);

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

    const int smoothMoodIndex =
        ProgressionEngine::moodNames()
            .indexOf("Smooth");

    moodBox.setSelectedId(
        smoothMoodIndex >= 0
            ? smoothMoodIndex + 1
            : 1);

    moodBox.onChange =
        [this]
        {
            processor.setMood(
                moodBox.getText());
        };

    addAndMakeVisible(moodBox);

    // =================================================
    // MELODY STYLE
    // =================================================

    melodyStyleBox.addItemList(
        MelodyEngine::melodyStyleNames(),
        1);

    const int smoothStyleIndex =
        MelodyEngine::melodyStyleNames()
            .indexOf(
                processor.getMelodyStyle());

    melodyStyleBox.setSelectedId(
        smoothStyleIndex >= 0
            ? smoothStyleIndex + 1
            : 1);

    melodyStyleBox.onChange =
        [this]
        {
            processor.setMelodyStyle(
                melodyStyleBox.getText());
        };

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
            processor.setMelodyStyle(
                melodyStyleBox.getText());

            processor.generateMelodies();

            status.setText(
                "New V5 melody + counter melody generated.",
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

    addAndMakeVisible(
        playButton);

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

    addAndMakeVisible(
        stopButton);

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
                            processor.exportMidiToFile(
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

    addAndMakeVisible(
        status);

    refreshProgressionText();

    status.setText(
        "Choose progression + melody style, then generate.",
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
// SYNC SETTINGS
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
// PROGRESSION DISPLAY
// =====================================================

void GoodAuraMelodyAudioProcessorEditor::
refreshProgressionText()
{
    progressionLabel.setText(
        processor.getProgressionText(),
        juce::dontSendNotification);
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
            0xff101116));

    g.setColour(
        juce::Colour(
            0xff1d2029));

    g.fillRoundedRectangle(
        18.0f,
        18.0f,
        (float)getWidth() - 36.0f,
        (float)getHeight() - 36.0f,
        16.0f);

    // Top generator panel
    g.setColour(
        juce::Colour(
            0xff323745));

    g.fillRoundedRectangle(
        34.0f,
        108.0f,
        (float)getWidth() - 68.0f,
        180.0f,
        12.0f);

    // Melody control panel
    g.setColour(
        juce::Colour(
            0xff282c36));

    g.fillRoundedRectangle(
        34.0f,
        315.0f,
        (float)getWidth() - 68.0f,
        245.0f,
        12.0f);

    g.setColour(
        juce::Colours::white);

    g.drawText(
        "KEY",
        50,
        118,
        90,
        20,
        juce::Justification::left);

    g.drawText(
        "MODE",
        170,
        118,
        90,
        20,
        juce::Justification::left);

    g.drawText(
        "GENRE",
        290,
        118,
        90,
        20,
        juce::Justification::left);

    g.drawText(
        "MOOD",
        470,
        118,
        90,
        20,
        juce::Justification::left);

    g.drawText(
        "MELODY STYLE",
        650,
        118,
        150,
        20,
        juce::Justification::left);

    g.drawText(
        "MELODY",
        55,
        335,
        110,
        22,
        juce::Justification::left);

    g.drawText(
        "COUNTER",
        215,
        335,
        110,
        22,
        juce::Justification::left);

    g.drawText(
        "COMPLEXITY",
        375,
        335,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "HUMANISE",
        535,
        335,
        110,
        22,
        juce::Justification::left);

    g.drawText(
        "REPETITION",
        695,
        335,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "MOVEMENT",
        855,
        335,
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
    title.setBounds(
        40,
        28,
        500,
        40);

    subtitle.setBounds(
        42,
        70,
        620,
        25);

    // =================================================
    // TOP ROW
    // =================================================

    keyBox.setBounds(
        48,
        145,
        100,
        36);

    modeBox.setBounds(
        168,
        145,
        100,
        36);

    genreBox.setBounds(
        288,
        145,
        160,
        36);

    moodBox.setBounds(
        468,
        145,
        160,
        36);

    melodyStyleBox.setBounds(
        648,
        145,
        160,
        36);

    generateProgressionButton.setBounds(
        828,
        145,
        160,
        36);

    // =================================================
    // PROGRESSION
    // =================================================

    progressionLabel.setBounds(
        60,
        210,
        920,
        48);

    // =================================================
    // SIX V5 KNOBS
    // =================================================

    melodyDensity.setBounds(
        35,
        365,
        145,
        135);

    counterDensity.setBounds(
        195,
        365,
        145,
        135);

    complexity.setBounds(
        355,
        365,
        145,
        135);

    humanise.setBounds(
        515,
        365,
        145,
        135);

    repetition.setBounds(
        675,
        365,
        145,
        135);

    movement.setBounds(
        835,
        365,
        145,
        135);

    // =================================================
    // BOTTOM BUTTONS
    // =================================================

    generateMelodyButton.setBounds(
        42,
        585,
        205,
        42);

    playButton.setBounds(
        265,
        585,
        90,
        42);

    stopButton.setBounds(
        365,
        585,
        90,
        42);

    exportButton.setBounds(
        465,
        585,
        150,
        42);

    status.setBounds(
        635,
        585,
        350,
        42);
}
