#include "AudioOutputStream.h"
#include <algorithm>
#include "sharedMemoryBuffer.h"

AudioOutputStream::AudioOutputStream(const juce::String& sharedName, int bufferSize_)
    : bufferSize(bufferSize_),
    sharedMem(sharedName, sizeof(SharedAudioBuffer), true)
{
  
    valid = sharedMem.isValid();
    if (!valid) return;

    bufferPtr = static_cast<SharedAudioBuffer*>(sharedMem.getData());
    bufferPtr->bufferSize = bufferSize;
    bufferPtr->writeIndex.store(0);
    bufferPtr->readIndex.store(0);
}

AudioOutputStream::~AudioOutputStream()
{
    bufferPtr = nullptr;
}

void AudioOutputStream::pushSamples(const float* samples, int numSamples)
{
    if (!valid || !bufferPtr) return;

    int writeIdx = bufferPtr->writeIndex.load();
    for (int i = 0; i < numSamples; ++i)
    {
        bufferPtr->audioData[writeIdx] = samples[i];
        writeIdx = (writeIdx + 1) % bufferSize;
    }
    bufferPtr->writeIndex.store(writeIdx);
}
