#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>

//==============================================================================
// One-pole low/high cut filters living inside the feedback path only, so that
// each successive echo gets progressively darker (classic tape-delay behaviour).
class OnePoleFeedbackFilter
{
public:
    void setSampleRate (double sr) { sampleRate = sr; }

    void setHighCutHz (float hz)
    {
        lpCoeff = std::exp (-2.0f * juce::MathConstants<float>::pi * hz / (float) sampleRate);
    }

    void setLowCutHz (float hz)
    {
        hpCoeff = std::exp (-2.0f * juce::MathConstants<float>::pi * hz / (float) sampleRate);
    }

    float processHighCut (float x)
    {
        lpState = x + lpCoeff * (lpState - x);
        return lpState;
    }

    float processLowCut (float x)
    {
        hpState = x + hpCoeff * (hpState - x);
        return x - hpState;
    }

    void reset() { lpState = 0.0f; hpState = 0.0f; }

private:
    double sampleRate = 44100.0;
    float lpCoeff = 0.0f, hpCoeff = 0.0f;
    float lpState = 0.0f, hpState = 0.0f;
};

//==============================================================================
// Single-channel circular delay buffer with linear-interpolated fractional
// read position, used as the building block for the stereo engine below.
class ModulatedDelayLine
{
public:
    void prepare (double sampleRate, double maxSeconds)
    {
        buffer.assign ((size_t) (maxSeconds * sampleRate) + 8, 0.0f);
        writePos = 0;
    }

    void reset()
    {
        std::fill (buffer.begin(), buffer.end(), 0.0f);
        writePos = 0;
    }

    void push (float sample)
    {
        buffer[(size_t) writePos] = sample;
        writePos = (writePos + 1) % (int) buffer.size();
    }

    float read (double delaySamples) const
    {
        const int size = (int) buffer.size();
        double readPos = (double) writePos - delaySamples;
        while (readPos < 0.0)
            readPos += (double) size;

        int i0 = (int) readPos % size;
        const double frac = readPos - std::floor (readPos);
        int i1 = (i0 + 1) % size;

        return (float) ((1.0 - frac) * buffer[(size_t) i0] + frac * buffer[(size_t) i1]);
    }

private:
    std::vector<float> buffer;
    int writePos = 0;
};

//==============================================================================
// Full stereo tape-style delay engine: tempo-synced time (note/groove/x2/trim),
// wow & flutter, smooth time-slewing, freeze, feedback with high/low cut,
// ping-pong and drift. Roughly mirrors the "Tape Delay" + feedback-routing
// portion of the reference plug-in's signal flow.
class TapeDelayEngine
{
public:
    void prepare (double sampleRate);
    void reset();

    struct Params
    {
        bool  on = true;
        int   noteDivIndex = 1;   // 0=1/2 1=1/4 2=1/8 3=1/16
        float groove = 0.0f;      // -1..1 (triplet .. dotted)
        float trim = 0.0f;        // -1..1 (L/R time offset)
        bool  x2 = false;
        float feedback = 0.35f;   // 0..1.05
        float directMix = 0.5f;   // 0 = fully wet delay signal, 1 = fully dry
        float highCutHz = 8000.0f;
        float lowCutHz = 80.0f;
        bool  freeze = false;
        float smooth = 0.2f;      // 0..1, time-slew amount
        float flutterInt = 0.0f, flutterRateHz = 6.0f;
        float wowInt = 0.0f,     wowRateHz = 0.8f;
        bool  pingPong = false;
        float drift = 0.0f;       // -1..1
    };

    // Lets the caller (PluginProcessor) insert the Filter section's "PRE"
    // stage inside the feedback loop, mirroring the original's PRE switches.
    using FeedbackHook = std::function<void (float& l, float& r)>;

    void process (float* left, float* right, int numSamples,
                  const Params& p, double hostBpm,
                  const FeedbackHook& feedbackHook = nullptr);

    double getCurrentDelaySeconds() const { return smoothedDelaySeconds; }

private:
    double sampleRate = 44100.0;

    ModulatedDelayLine lineL, lineR;
    OnePoleFeedbackFilter fbFilterL, fbFilterR;

    double smoothedDelaySeconds = 0.25;
    double wowPhase = 0.0, flutterPhase = 0.0;
};
