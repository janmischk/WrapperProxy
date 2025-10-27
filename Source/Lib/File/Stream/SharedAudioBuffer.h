#pragma once
#include <atomic>

constexpr int MAX_AUDIO_SAMPLES = 48000 * 2;

struct SharedAudioBuffer
{
    std::atomic<int> writeIndex{ 0 };
    std::atomic<int> readIndex{ 0 };
    int bufferSize = MAX_AUDIO_SAMPLES;
    float audioData[MAX_AUDIO_SAMPLES];
};