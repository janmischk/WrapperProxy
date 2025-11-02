
#pragma once
#include <JuceHeader.h>
#include "../Lib/File/Stream/AudioOutputStream.h"
#include "../Lib/File/Stream/AudioInputStream.h"

class ProxyPluginAudioProcessor : public juce::AudioProcessor
    {
    public:
        ProxyPluginAudioProcessor();
        ~ProxyPluginAudioProcessor() override;

        // =========================
        // Mandatory overrides
        // =========================
        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override { return true; }

        const juce::String getName() const override { return "Standalone Proxy"; }

        void getStateInformation(juce::MemoryBlock&) override;
        void setStateInformation(const void*, int) override;

        // =========================
        // Abstract class overrides
        // =========================
        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;
        bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

        // =========================
        // Programs / Presets
        // =========================
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram(int index) override;
        const juce::String getProgramName(int index) override;
        void changeProgramName(int index, const juce::String& newName) override;

        // =========================
        // Custom function
        // =========================
        void launchStandaloneHost(int sampleRate);

    private:
        juce::String outputStreamSharedMemoryName = "AnnyChannelWrapperAudioProxyOutput";
        juce::String inputStreamSharedMemoryName = "AnnyChannelWrapperAudioProxyInput";
        bool standaloneLaunched = false;
        std::unique_ptr<AudioOutputStream> outputStream;
        std::unique_ptr<AudioInputStream> inputStream;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProxyPluginAudioProcessor)
    };