cmake_minimum_required(VERSION 3.20)

project(ProxyPlugin VERSION 1.0.0 LANGUAGES CXX)

# 1️⃣ JUCE Subdirectory
add_subdirectory("C:/Users/DevPrivat/Lib/JUCE" "${CMAKE_BINARY_DIR}/JUCE")

# VST2 deaktivieren
add_definitions(-DJUCE_DISABLE_VST2=1)
add_definitions(-DJUCE_VST3_CAN_REPLACE_VST2=0)
add_definitions(-DJUCE_FORCE_USE_LEGACY_PARAM_IDS=0)
add_definitions(-DJUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1)

# 2️⃣ Plugin
juce_add_plugin(ProxyPlugin
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE           PRXY
    FORMATS               Standalone VST3
    PRODUCT_NAME          "Standalone Proxy"
)

# --- 🔍 Automatically include all .cpp/.h files under Source/ ---
# Recursively gather all source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Add them to the target
target_sources(ProxyPlugin PRIVATE ${SOURCE_FILES})

# Mirror actual folder structure in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${SOURCE_FILES})

# 4️⃣ Link JUCE modules
target_link_libraries(ProxyPlugin
    PRIVATE
            juce::juce_audio_processors
        juce::juce_gui_extra
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_audio_utils      # <- wichtig für Standalone
        juce::juce_audio_devices    # <- optional, oft für Standalone nötig
)

# 5️⃣ C++17
target_compile_features(ProxyPlugin PRIVATE cxx_std_17)

# 6️⃣ Output folder
set_target_properties(ProxyPlugin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/VST3"
)
# ✅ WICHTIG: Generiert JuceHeader.h für dein Target
juce_generate_juce_header(ProxyPlugin)