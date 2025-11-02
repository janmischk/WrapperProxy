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
void ProxyPluginAudioProcessor::launchStandaloneHost(int sampleRate)
{

    juce::File exeFile("C:/Users/DevPrivat/Desktop/Projects/Wrapper/build/WrapperPlugin_artefacts/Debug/Standalone/Wrapper Plugin.exe");  
    juce::StringArray args;
   
    const int blockSize = getBlockSize(); // from DAW
    const int pollIntervalMs = 10;        // optional tweak

    args.add(exeFile.getFullPathName()); // zuerst das Executable
    args.add(fromProxy);
    args.add(toProxy);
    args.add(juce::String(blockSize));
    args.add(juce::String(pollIntervalMs));
    args.add(juce::String(sampleRate));

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
        //launchButton.onClick = [this]() { processor.launchStandaloneHost(); };
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
    outputStream = std::make_unique<AudioOutputStream>(fromProxy, bufferSize);
    inputStream = std::make_unique<AudioInputStream>(toProxy, bufferSize);
 
    if (!outputStream->isValid() || !inputStream->isValid()){
        
        juce::String inputValidString = outputStream->isValid() ? "true" : "false";
        juce::String outputValidString = inputStream->isValid() ? "true" : "false";
        ToastWindow::show(juce::String("Proxy Stream is Invalid! Input valid: "+inputValidString + " Output valid: "+ outputValidString));
        return;
    }
    ToastWindow::show(juce::String("Proxy Streams succesfully launched"));
    if(!standaloneLaunched)
    {
        launchStandaloneHost(sampleRate);
        standaloneLaunched = true;
    }

}

void ProxyPluginAudioProcessor::releaseResources() {}
void ProxyPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {

    juce::ignoreUnused(midiMessages);

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // --- Send audio to standalone ---
    if (outputStream && outputStream->isValid())
    {
        const float* channelData = buffer.getReadPointer(0); // mono for simplicity
        outputStream->pushSamples(channelData, numSamples);

        // Optional debug
        float sum = 0.0f;
        for (int i = 0; i < numSamples; ++i)
            sum += std::abs(channelData[i]);
        float avg = sum / numSamples;
        if (avg > 0.001f)
            DBG("Audio detected, pushing samples to shared memory!");
    }

    // --- Read loopback audio from standalone ---
    if (inputStream && inputStream->isValid())
    {
        float* channelOut = buffer.getWritePointer(0); // mono
        int got = inputStream->getSamples(channelOut, numSamples);

        // If fewer samples available, zero the rest
        for (int i = got; i < numSamples; ++i)
            channelOut[i] = 0.0f;

        // Optional: extend to multiple channels
        for (int ch = 1; ch < numChannels; ++ch)
        {
            float* outCh = buffer.getWritePointer(ch);
            std::memcpy(outCh, channelOut, numSamples * sizeof(float));
        }
    }
    else
    {
        buffer.clear(); // fallback: silence if no input
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