#pragma once
#include "ProxyPlugin.h"
#include "JuceHeader.h"
#include <iostream>
#include "../Lib/File/Stream/sharedMemoryBuffer.h"

#include "../Lib/Helper/ToastWindow.h";


// =========================
// Constructor / Destructor
// =========================
ProxyPluginAudioProcessor::ProxyPluginAudioProcessor() {
    
}
ProxyPluginAudioProcessor::~ProxyPluginAudioProcessor() {}

// =========================
// Standalone Host
// =========================
/* Definiert im Host
struct SharedAudioBuffer
{
    std::atomic<int> writeIndex{ 0 };
    std::atomic<int> readIndex{ 0 };
    int bufferSize;
    float audioData[48000 * 2];
};
*/
void ProxyPluginAudioProcessor::launchStandaloneHost()
{

    juce::File exeFile("C:/Users/DevPrivat/Desktop/Projects/Wrapper/build/WrapperPlugin_artefacts/Debug/Standalone/Wrapper Plugin.exe");  
    juce::StringArray args;
   
    args.add(exeFile.getFullPathName()); // zuerst das Executable
    args.add(outputStreamSharedMemoryName);

    if (exeFile.existsAsFile())
    {
        juce::ChildProcess hostProcess;
        // StreamFlags aktivieren
        int streamFlags = juce::ChildProcess::wantStdOut | juce::ChildProcess::wantStdErr;
        if (!hostProcess.start(args, streamFlags)) {
            DBG("Failed to start standalone host");
        }
        else {
            
            DBG("Standalone host launched successfully");
        }  
    }
    else
    {
        DBG("Standalone host executable not found!");
    }
    
}

// =========================
// Editor
// =========================
class ProxyPluginEditor : public juce::AudioProcessorEditor
{
public:
    ProxyPluginEditor(ProxyPluginAudioProcessor& p) : AudioProcessorEditor(&p), processor(p)
    {
        addAndMakeVisible(launchButton);
        launchButton.setButtonText("Open Standalone GUI");
        launchButton.onClick = [this]() { processor.launchStandaloneHost(); };
        setSize(500, 500);
    }

private:
    ProxyPluginAudioProcessor& processor;
    juce::TextButton launchButton;
};

// Create editor
juce::AudioProcessorEditor* ProxyPluginAudioProcessor::createEditor()
{
    return new ProxyPluginEditor(*this);
}

// =========================
// Mandatory AudioProcessor overrides
// =========================
void ProxyPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {

    const int bufferSize = static_cast<int>(sampleRate * 2.0); // 2 seconds of audio buffer, for example
    outputStream = std::make_unique<AudioOutputStream>(outputStreamSharedMemoryName, bufferSize);

    if (!outputStream->isValid()) {
        ToastWindow::show(juce::String("Output Stream Failed"));
        DBG("Output Stream Failed");
    }else{
        ToastWindow::show(juce::String("Output Stream is valid"));

        launchStandaloneHost();
    }

}

void ProxyPluginAudioProcessor::releaseResources() {}
void ProxyPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {

    // Only send if stream is valid
    if (outputStream && outputStream->isValid())
    {
        const float* samples = buffer.getReadPointer(0);
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        // Example: take just the first channel for simplicity
        const float* channelData = buffer.getReadPointer(0);

        // Push samples into shared memory
        outputStream->pushSamples(channelData, numSamples);

        // Quick check: calculate an average level
        float sum = 0.0f;
        for (int i = 0; i < numSamples; ++i)
            sum += std::abs(samples[i]);

        float avg = sum / numSamples;

        if (avg > 0.001f) // roughly "non-silent"
            DBG("Audio detected, pushing samples to shared memory!");
    }

}

// =========================
// Abstract class overrides
// =========================
bool ProxyPluginAudioProcessor::acceptsMidi() const { return true; }
bool ProxyPluginAudioProcessor::producesMidi() const { return true; }
bool ProxyPluginAudioProcessor::isMidiEffect() const { return false; }
double ProxyPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

bool ProxyPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo();
}

// =========================
// Programs / Presets
// =========================
int ProxyPluginAudioProcessor::getNumPrograms() { return 1; }
int ProxyPluginAudioProcessor::getCurrentProgram() { return 0; }
void ProxyPluginAudioProcessor::setCurrentProgram(int /*index*/) {}
const juce::String ProxyPluginAudioProcessor::getProgramName(int /*index*/) { return "Default"; }
void ProxyPluginAudioProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/) {}

// =========================
// State
// =========================
void ProxyPluginAudioProcessor::getStateInformation(juce::MemoryBlock& /*destData*/) {}
void ProxyPluginAudioProcessor::setStateInformation(const void* /*data*/, int /*sizeInBytes*/) {}

// =========================
// Plugin entry point
// =========================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProxyPluginAudioProcessor();
}