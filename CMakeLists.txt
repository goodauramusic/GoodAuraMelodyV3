cmake_minimum_required(VERSION 3.22)

project(GoodAuraMelodyV3 VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(GoodAuraMelodyV3
    COMPANY_NAME "Good Aura Music"
    BUNDLE_ID "com.goodauramusic.goodauramelody"

    PLUGIN_MANUFACTURER_CODE Gaur
    PLUGIN_CODE Gam3

    FORMATS VST3

    PRODUCT_NAME "Good Aura Melody"
    PLUGIN_NAME "Good Aura Melody"

    IS_SYNTH TRUE
    IS_MIDI_EFFECT FALSE

    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE

    VST3_CATEGORIES
        Instrument
        Generator
        Synth

    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(
    GoodAuraMelodyV3

    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h

        Source/PluginEditor.cpp
        Source/PluginEditor.h

        Source/MelodyEngine.cpp
        Source/MelodyEngine.h
)

juce_generate_juce_header(
    GoodAuraMelodyV3
)

target_compile_definitions(
    GoodAuraMelodyV3

    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(
    GoodAuraMelodyV3

    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_basics
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
