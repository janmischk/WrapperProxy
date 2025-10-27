#pragma once
#include "SharedAudioBuffer.h"
#include "SharedMemoryBuffer.h"
#include <juce_core/juce_core.h>

class AudioOutputStream
{
public:
    AudioOutputStream(const juce::String& sharedName, int bufferSize);
    ~AudioOutputStream();

    bool isValid() const noexcept { return valid; }

    // Push audio samples directly
    void pushSamples(const float* samples, int numSamples);

private:
    SharedMemoryBuffer sharedMem;
    SharedAudioBuffer* bufferPtr{ nullptr };
    int bufferSize{ 0 };
    bool valid{ false };
};
