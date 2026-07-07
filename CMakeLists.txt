cmake_minimum_required(VERSION 3.15)

project(TapeDubDelay VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Fetch JUCE automatically so you don't need a separate JUCE install ---
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.13
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(TapeDubDelay
    COMPANY_NAME "YourName"
    PLUGIN_MANUFACTURER_CODE Ymnf
    PLUGIN_CODE  Tdd1
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Tape Dub Delay"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(TapeDubDelay PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/TapeDelayEngine.cpp
    Source/UI/DubLookAndFeel.cpp
)

target_include_directories(TapeDubDelay PRIVATE Source)

target_compile_definitions(TapeDubDelay PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(TapeDubDelay PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
