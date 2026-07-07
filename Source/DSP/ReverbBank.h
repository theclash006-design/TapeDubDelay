#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

// Algorithm A: a compact, deliberately small comb + allpass reverberator.
// Fewer combs and a brighter damping range than a typical Freeverb-style
// network give it a more resonant/metallic "boingy" character, standing in
// for the reference plug-in's "old school" reverb algorithm A.
class CombAllpassReverb
{
public:
    // spreadSamples offsets every comb/allpass length slightly, so running one
    // instance per channel with a different spread gives natural stereo decorrelation.
    void prepare (double sampleRate, int spreadSamples = 0)
    {
        static const double combBaseMs[NumCombs]         = { 25.3, 26.9, 28.9, 30.7 };
        static const double allpassBaseMs[NumAllpasses]  = { 5.0, 1.7 };

        for (int i = 0; i < NumCombs; ++i)
            combs[(size_t) i].prepare ((size_t) (combBaseMs[i] * 0.001 * sampleRate) + (size_t) spreadSamples + 1);

        for (int i = 0; i < NumAllpasses; ++i)
            allpasses[(size_t) i].prepare ((size_t) (allpassBaseMs[i] * 0.001 * sampleRate) + (size_t) spreadSamples + 1);

        reset();
    }

    void reset()
    {
        for (auto& c : combs) c.reset();
        for (auto& a : allpasses) a.reset();
    }

    // size: 0..1 controls comb feedback (decay length / resonance)
    // damp: 0..1 controls high-frequency damping inside the comb feedback path
    float processSample (float x, float size, float damp)
    {
        const float fb = juce::jmap (juce::jlimit (0.0f, 1.0f, size), 0.0f, 1.0f, 0.75f, 0.97f);
        const float dampCoeff = juce::jmap (juce::jlimit (0.0f, 1.0f, damp), 0.0f, 1.0f, 0.15f, 0.85f);

        float sum = 0.0f;
        for (auto& c : combs)
            sum += c.process (x, fb, dampCoeff);
        sum *= 1.0f / (float) NumCombs;

        for (auto& a : allpasses)
            sum = a.process (sum, 0.5f);

        return sum;
    }

private:
    static constexpr int NumCombs = 4;
    static constexpr int NumAllpasses = 2;

    struct Comb
    {
        void prepare (size_t sizeSamples) { buffer.assign (juce::jmax ((size_t) 4, sizeSamples), 0.0f); pos = 0; damp1 = 0.0f; }
        void reset() { std::fill (buffer.begin(), buffer.end(), 0.0f); damp1 = 0.0f; }

        float process (float x, float feedback, float dampCoeff)
        {
            const float y = buffer[pos];
            damp1 = y * (1.0f - dampCoeff) + damp1 * dampCoeff;
            buffer[pos] = x + damp1 * feedback;
            pos = (pos + 1) % buffer.size();
            return y;
        }

        std::vector<float> buffer;
        size_t pos = 0;
        float damp1 = 0.0f;
    };

    struct Allpass
    {
        void prepare (size_t sizeSamples) { buffer.assign (juce::jmax ((size_t) 4, sizeSamples), 0.0f); pos = 0; }
        void reset() { std::fill (buffer.begin(), buffer.end(), 0.0f); }

        float process (float x, float g)
        {
            const float bufOut = buffer[pos];
            const float y = -x + bufOut;
            buffer[pos] = x + bufOut * g;
            pos = (pos + 1) % buffer.size();
            return y;
        }

        std::vector<float> buffer;
        size_t pos = 0;
    };

    std::array<Comb, NumCombs> combs;
    std::array<Allpass, NumAllpasses> allpasses;
};

//==============================================================================
// Full Reverb section: Algorithm A (above) and Algorithm B (JUCE's built-in
// Freeverb-derived juce::Reverb, smoother/more natural) blended via
// MIX AB, plus WIDTH and dry/wet MIX. Mirrors the reference plug-in's
// two-algorithm reverb section, including its PRE routing switch (handled
// by the caller via the same FeedbackHook pattern used for the Filter).
class ReverbBank
{
public:
    void prepare (double sampleRate)
    {
        sr = sampleRate;
        const int spread = (int) (23.0 * sampleRate / 44100.0); // classic ~0.5ms stereo spread
        algoAL.prepare (sampleRate, 0);
        algoAR.prepare (sampleRate, spread);
        algoB.setSampleRate (sampleRate);
        reset();
    }

    void reset()
    {
        algoAL.reset();
        algoAR.reset();
        algoB.reset();
    }

    struct Params
    {
        bool  on = false;
        float sizeA = 0.5f, dampA = 0.3f;   // Algorithm A: boingy/old-school
        float sizeB = 0.5f, dampB = 0.5f;   // Algorithm B: natural/smooth
        float mixAB = 0.5f;                 // 0 = full A, 1 = full B
        float width = 1.0f;                 // 0..1.5, applied to the wet signal
        float mix = 0.3f;                   // 0 = dry, 1 = fully wet
    };

    void processSample (float& l, float& r, const Params& p)
    {
        if (! p.on)
            return;

        const float dryL = l, dryR = r;

        // Algorithm A
        const float aL = algoAL.processSample (dryL, p.sizeA, p.dampA);
        const float aR = algoAR.processSample (dryR, p.sizeA, p.dampA);

        // Algorithm B
        juce::Reverb::Parameters bp;
        bp.roomSize   = juce::jlimit (0.0f, 1.0f, p.sizeB);
        bp.damping    = juce::jlimit (0.0f, 1.0f, p.dampB);
        bp.width      = 1.0f;   // WIDTH is applied ourselves below, after the A/B blend
        bp.wetLevel   = 1.0f;
        bp.dryLevel   = 0.0f;
        bp.freezeMode = 0.0f;
        algoB.setParameters (bp);

        float bL = dryL, bR = dryR;
        algoB.processStereo (&bL, &bR, 1);

        // MIX AB blend
        float wetL = aL + (bL - aL) * p.mixAB;
        float wetR = aR + (bR - aR) * p.mixAB;

        // WIDTH (mid/side) on the wet signal
        const float mid  = 0.5f * (wetL + wetR);
        const float side = 0.5f * (wetL - wetR) * p.width;
        wetL = mid + side;
        wetR = mid - side;

        // MIX (dry/wet)
        l = dryL + (wetL - dryL) * p.mix;
        r = dryR + (wetR - dryR) * p.mix;
    }

private:
    double sr = 44100.0;
    CombAllpassReverb algoAL, algoAR;
    juce::Reverb algoB;
};
