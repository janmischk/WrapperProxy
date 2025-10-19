#include "ProxyPlugin.h"

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
void ProxyPluginAudioProcessor::launchStandaloneHost()
{
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