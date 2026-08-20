#include "PluginEditor.h"

GoodAuraMelodyAudioProcessorEditor::
GoodAuraMelodyAudioProcessorEditor(
    GoodAuraMelodyAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p)
{
    setSize(900, 560);

    title.setText(
        "GOOD AURA MELODY",
        juce::dontSendNotification);

    title.setFont(
        juce::Font(
            juce::FontOptions(28.0f)
                .withStyle("Bold")));

    title.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    addAndMakeVisible(title);

    subtitle.setText(
        "CHORDS + MAIN MELODY + COUNTER MELODY",
        juce::dontSendNotification);

    subtitle.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(subtitle);

    auto roots =
        MelodyEngine::rootNames();

    auto chords =
        MelodyEngine::chordNames();

    const auto& progression =
        processor.getProgression();

    for (int i = 0;
         i < 4;
         ++i)
    {
        chordLabels[(size_t) i]
            .setText(
                "CHORD " +
                juce::String(i + 1),
                juce::dontSendNotification);

        chordLabels[(size_t) i]
            .setColour(
                juce::Label::textColourId,
                juce::Colours::white);

        addAndMakeVisible(
            chordLabels[(size_t) i]);

        rootBoxes[(size_t) i]
            .addItemList(
                roots,
                1);

        chordBoxes[(size_t) i]
            .addItemList(
                chords,
                1);

        rootBoxes[(size_t) i]
            .setSelectedId(
                progression[(size_t) i]
                    .root + 1);

        chordBoxes[(size_t) i]
            .setSelectedId(
                progression[(size_t) i]
                    .type + 1);

        rootBoxes[(size_t) i]
            .onChange =
            [this, i]
            {
                syncChord(i);
            };

        chordBoxes[(size_t) i]
            .onChange =
            [this, i]
            {
                syncChord(i);
            };

        addAndMakeVisible(
            rootBoxes[(size_t) i]);

        addAndMakeVisible(
            chordBoxes[(size_t) i]);
    }

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
                (int) melodyDensity.getValue();
        };

    counterDensity.onValueChange =
        [this]
        {
            processor.counterDensity =
                (int) counterDensity.getValue();
        };

    complexity.onValueChange =
        [this]
        {
            processor.complexity =
                (int) complexity.getValue();
        };

    humanise.onValueChange =
        [this]
        {
            processor.humanise =
                (int) humanise.getValue();
        };

    generateButton.onClick =
        [this]
        {
            for (int i = 0;
                 i < 4;
                 ++i)
            {
                syncChord(i);
            }

            processor.generateNewPhrase();

            info.setText(
                "New phrase generated.",
                juce::dontSendNotification);
        };

    playButton.onClick =
        [this]
        {
            processor.startPreview();

            refreshStatus();
        };

    stopButton.onClick =
        [this]
        {
            processor.stopPreview();

            refreshStatus();
        };

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

                        info.setText(
                            success
                                ? "MIDI exported successfully."
                                : "Could not export MIDI.",
                            juce::dontSendNotification);
                    }
                });
        };

    dragButton.onClick =
        [this]
        {
            const auto file =
                processor
                    .writeMidiToTemporaryFile();

            if (!file.existsAsFile())
            {
                info.setText(
                    "Could not create MIDI file.",
                    juce::dontSendNotification);

                return;
            }

            juce::StringArray paths;

            paths.add(
                file.getFullPathName());

            const bool started =
                juce::DragAndDropContainer::
                    performExternalDragDropOfFiles(
                        paths,
                        false,
                        this);

            info.setText(
                started
                    ? "Drag started — drop MIDI into FL Studio."
                    : "External drag unavailable — use EXPORT MIDI.",
                juce::dontSendNotification);
        };

    addAndMakeVisible(
        generateButton);

    addAndMakeVisible(
        playButton);

    addAndMakeVisible(
        stopButton);

    addAndMakeVisible(
        exportButton);

    addAndMakeVisible(
        dragButton);

    info.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    refreshStatus();

    addAndMakeVisible(info);
}

void
GoodAuraMelodyAudioProcessorEditor::
configureSlider(
    juce::Slider& slider,
    const juce::String& name,
    int initial)
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
        initial);

    addAndMakeVisible(
        slider);
}

void
GoodAuraMelodyAudioProcessorEditor::
syncChord(
    int index)
{
    processor.setChord(
        index,

        rootBoxes[(size_t) index]
            .getSelectedId() - 1,

        chordBoxes[(size_t) index]
            .getSelectedId() - 1);
}

void
GoodAuraMelodyAudioProcessorEditor::
refreshStatus()
{
    info.setText(
        processor.isPreviewing()
            ? "Preview playing inside Good Aura Melody."
            : "Choose chords → Generate → Play.",
        juce::dontSendNotification);
}

void
GoodAuraMelodyAudioProcessorEditor::
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
        (float) getWidth() - 36.0f,
        (float) getHeight() - 36.0f,
        16.0f);

    g.setColour(
        juce::Colour(
            0xff323745));

    g.fillRoundedRectangle(
        32.0f,
        100.0f,
        (float) getWidth() - 64.0f,
        130.0f,
        12.0f);

    g.setColour(
        juce::Colours::white);

    g.drawText(
        "MELODY",
        54,
        260,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "COUNTER",
        244,
        260,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "COMPLEXITY",
        434,
        260,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "HUMANISE",
        624,
        260,
        120,
        22,
        juce::Justification::left);
}

void
GoodAuraMelodyAudioProcessorEditor::
resized()
{
    title.setBounds(
        38,
        28,
        420,
        38);

    subtitle.setBounds(
        40,
        64,
        560,
        24);

    for (int i = 0;
         i < 4;
         ++i)
    {
        const int x =
            45 +
            i * 200;

        chordLabels[(size_t) i]
            .setBounds(
                x,
                112,
                120,
                20);

        rootBoxes[(size_t) i]
            .setBounds(
                x,
                140,
                72,
                34);

        chordBoxes[(size_t) i]
            .setBounds(
                x + 78,
                140,
                108,
                34);
    }

    melodyDensity.setBounds(
        38,
        288,
        160,
        130);

    counterDensity.setBounds(
        228,
        288,
        160,
        130);

    complexity.setBounds(
        418,
        288,
        160,
        130);

    humanise.setBounds(
        608,
        288,
        160,
        130);

    generateButton.setBounds(
        42,
        430,
        235,
        40);

    playButton.setBounds(
        292,
        430,
        90,
        40);

    stopButton.setBounds(
        392,
        430,
        90,
        40);

    exportButton.setBounds(
        492,
        430,
        145,
        40);

    dragButton.setBounds(
        647,
        430,
        145,
        40);

    info.setBounds(
        42,
        485,
        750,
        34);
}
