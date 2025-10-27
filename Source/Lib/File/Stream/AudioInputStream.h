#pragma once
#include "SharedAudioBuffer.h"
#include "SharedMemoryBuffer.h"
#include <juce_core/juce_core.h>
#include <atomic>
#include <vector>

class AudioInputStream : private juce::Thread
{
public:
    AudioInputStream(const juce::String& sharedName, int bufferSize, int pollMs = 10);
    ~AudioInputStream() override;

    bool isValid() const noexcept { return valid; }

    // Thread-safe: pull up to maxSamples from internal buffer
    int getSamples(float* outBuffer, int maxSamples);

private:
    void run() override;

    SharedMemoryBuffer sharedMem;
    SharedAudioBuffer* bufferPtr{ nullptr };
    int bufferSize{ 0 };
    bool valid{ false };
    int pollIntervalMs{ 10 };

    std::vector<float> localBuffer;
    std::atomic<int> localWriteIndex{ 0 };
    std::atomic<int> localReadIndex{ 0 };
};
