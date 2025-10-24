#pragma once
#include "ProxyPlugin.h"
#include "JuceHeader.h"
#include <iostream>
#include "../Lib/File/sharedMemoryBuffer.h"



// =========================
// Constructor / Destructor
// =========================
ProxyPluginAudioProcessor::ProxyPluginAudioProcessor() {
    launchStandaloneHost();
}
ProxyPluginAudioProcessor::~ProxyPluginAudioProcessor() {}

// =========================
// Standalone Host
// =========================
struct SharedAudioBuffer
{
    std::atomic<int> writeIndex{ 0 };
    std::atomic<int> readIndex{ 0 };
    int bufferSize;
    float audioData[48000 * 2];
};

void ProxyPluginAudioProcessor::launchStandaloneHost()
{
    SharedMemoryBuffer shared("MyAudioBuffer", sizeof(SharedAudioBuffer), true);

    if (!shared.isValid())
    {
        DBG("Shared memory creation failed!");
        return;
    }
    DBG("Shared memory creation succeeded!");
    auto* buf = static_cast<SharedAudioBuffer*>(shared.getData());
    buf->bufferSize = 48000 * 2;

    juce::File exeFile("C:/Users/DevPrivat/Desktop/Projects/Wrapper/build/WrapperPlugin_artefacts/Debug/Standalone/Wrapper Plugin.exe");  
    juce::StringArray args;
   
    args.add(exeFile.getFullPathName()); // zuerst das Executable
    args.add(shared.getSharedBufferName());

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

            // Polling (z.B. 100 ms Pause)
            while (hostProcess.isRunning())
            {
                juce::String output = hostProcess.readAllProcessOutput();
                if (output.isNotEmpty())
                    DBG("From child:\n" + output);

                juce::Thread::sleep(100);
            }

            // Letzte Ausgabe nach Beendigung
            juce::String output = hostProcess.readAllProcessOutput();
            if (output.isNotEmpty())
                DBG("From child:\n" + output);
        
        }
        
    }
    else
    {
        DBG("Standalone host executable not found!");
    }
    /*
    juce::File exeFile("C:/Users/DevPrivat/Desktop/Projects/Wrapper/build/WrapperPlugin_artefacts/Debug/Standalone/Wrapper Plugin.exe");

    if (exeFile.existsAsFile())
    {
        juce::ChildProcess hostProcess;
        if (!hostProcess.start(exeFile.getFullPathName()))
            DBG("Failed to start standalone host");
        else
            DBG("Standalone host launched successfully");
    }
    else
    {
        DBG("Standalone host executable not found!");
    }
    {
        DBG("Fehler: Shared Memory konnte nicht geöffnet werden!");
        return;
    }
    */
    
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
        setSize(200, 50);
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
void ProxyPluginAudioProcessor::prepareToPlay(double /*sampleRate*/, int /*samplesPerBlock*/) {}
void ProxyPluginAudioProcessor::releaseResources() {}
void ProxyPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& /*buffer*/, juce::MidiBuffer& /*midiMessages*/) {}

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