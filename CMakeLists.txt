cmake_minimum_required(VERSION 3.22)
project(JRemixAutoMixer VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(JUCE)

juce_add_plugin(JRemixAutoMixer
    COMPANY_NAME "JREMIX"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Jrmx
    PLUGIN_CODE Jram
    FORMATS VST3
    PRODUCT_NAME "JRemix Auto-Mixer"
)

juce_generate_juce_header(JRemixAutoMixer)

target_sources(JRemixAutoMixer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/TrackScanner.h
    Source/MixEngine.h
    Source/BridgeExporter.h
)

target_compile_definitions(JRemixAutoMixer PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(JRemixAutoMixer PRIVATE
    juce::juce_audio_utils
    juce::juce_gui_basics
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
)
