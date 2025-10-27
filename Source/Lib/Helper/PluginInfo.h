#pragma once

class PluginInfo
{
public:
    static void setSampleRate(double rate) { sampleRate = rate; }
    static void setBlockSize(int size) { blockSize = size; }

    static double getSampleRate() { return sampleRate; }
    static int getBlockSize() { return blockSize; }

private:
    static inline double sampleRate = 48000.0;
    static inline int blockSize = 512;          
};
