#include "AudioInputStream.h"
#include <algorithm>


AudioInputStream::AudioInputStream(const juce::String& sharedName, int bufferSize_, int pollMs)
    : juce::Thread("AudioInputStreamThread"),
    bufferSize(bufferSize_),
    pollIntervalMs(pollMs),
    sharedMem(sharedName, sizeof(SharedAudioBuffer), false)
{
    valid = sharedMem.isValid();
    if (!valid) return;

    bufferPtr = static_cast<SharedAudioBuffer*>(sharedMem.getData());
    localBuffer.resize(bufferSize);

    startThread();
}

AudioInputStream::~AudioInputStream()
{
    stopThread(100);
}

void AudioInputStream::run()
{
    if (!valid || !bufferPtr) return;

    while (!threadShouldExit())
    {
        int readIdx = bufferPtr->readIndex.load();
        int writeIdx = bufferPtr->writeIndex.load();
        int available = (writeIdx - readIdx + bufferPtr->bufferSize) % bufferPtr->bufferSize;

        for (int i = 0; i < available; ++i)
        {
            int idx = (readIdx + i) % bufferPtr->bufferSize;
            int localIdx = localWriteIndex.load() % bufferSize;
            localBuffer[localIdx] = bufferPtr->audioData[idx];
            localWriteIndex.fetch_add(1);
        }

        if (available > 0)
            bufferPtr->readIndex.store((readIdx + available) % bufferPtr->bufferSize);

        wait(pollIntervalMs);
    }
}

int AudioInputStream::getSamples(float* outBuffer, int maxSamples)
{
    int available = (localWriteIndex.load() - localReadIndex.load() + bufferSize) % bufferSize;
    int toRead = std::min(maxSamples, available);

    for (int i = 0; i < toRead; ++i)
    {
        int idx = (localReadIndex.load() + i) % bufferSize;
        outBuffer[i] = localBuffer[idx];
    }

    localReadIndex.store((localReadIndex.load() + toRead) % bufferSize);
    return toRead;
}
