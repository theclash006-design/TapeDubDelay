#pragma once

#include "TapeDelayEngine.h"   // reuses ModulatedDelayLine as the grain buffer
#include <cmath>

// Time-domain granular pitch shifter: two overlapping "grains" read from a
// delay line at a speed offset from the write speed, spaced half a grain
// apart and crossfaded with a Hann window so the discontinuity when a grain
// wraps is masked (sum of two 50%-overlapped Hann windows is ~constant).
// This is the same family of technique used by classic delay-based / rack
// harmonizer pitch shifters, and stands in for the reference plug-in's
// "Pitch Shift" section (5 algorithms A/B/C/D/X - see README "Roadmap" for
// how ALGO differentiation can be extended further).
class GranularPitchShifter
{
public:
    void prepare (double sampleRate)
    {
        sr = sampleRate;
        line.prepare (sampleRate, 1.0); // 1s buffer comfortably covers grain sizes up to ~150ms
        reset();
    }

    void reset()
    {
        line.reset();
        delayA = 0.0f;
        delayB = 0.0f;
        grainInitialised = false;
        lofiCounter = 0;
        lofiHeld = 0.0f;
    }

    enum Algo { A = 0, B, C, D, X };

    struct Params
    {
        bool  on = false;
        int   algo = A;
        int   semi = 0;        // -24..24 semitones
        float fine = 0.0f;     // -1..1, non-linear, scaled to +/-2 octaves at the extremes
        float grain = 0.4f;    // 0..1 -> ~20..150ms grain length
        float mix = 0.5f;      // 0 = dry, 1 = fully shifted
    };

    float processSample (float x, const Params& p)
    {
        if (! p.on)
        {
            line.push (x);
            return x;
        }

        // SEMI + FINE -> playback ratio. FINE is cubic so small moves near the
        // centre give fine detuning, while the extremes reach +/-2 octaves.
        const float fineSemis = (p.fine >= 0.0f ? 1.0f : -1.0f) * 24.0f * std::pow (std::abs (p.fine), 3.0f);
        const float totalSemis = (float) p.semi + fineSemis;
        const float ratio = std::pow (2.0f, totalSemis / 12.0f);

        // GRAIN: B/D favour small/smooth shifts (shorter grains suit that use),
        // A/C are the general-purpose/larger-shift algorithms (longer grains).
        const bool smallShiftAlgo = (p.algo == B || p.algo == D);
        const float grainMsRange = smallShiftAlgo ? 60.0f : 130.0f;
        const float grainMs = juce::jmap (juce::jlimit (0.0f, 1.0f, p.grain), 0.0f, 1.0f, 15.0f, 15.0f + grainMsRange);
        float grainSizeSamples = (float) (grainMs * 0.001 * sr);
        grainSizeSamples = juce::jmax (8.0f, grainSizeSamples);

        if (! grainInitialised)
        {
            delayA = 0.0f;
            delayB = grainSizeSamples * 0.5f;
            grainInitialised = true;
        }

        auto stepGrain = [ratio, grainSizeSamples] (float& delay)
        {
            delay -= (ratio - 1.0f);
            if (delay < 0.0f)               delay += grainSizeSamples;
            else if (delay >= grainSizeSamples) delay -= grainSizeSamples;
        };
        stepGrain (delayA);
        stepGrain (delayB);

        const float readA = line.read (delayA);
        const float readB = line.read (delayB);

        const float wa = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * (delayA / grainSizeSamples));
        const float wb = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * (delayB / grainSizeSamples));

        float shifted = readA * wa + readB * wb;

        if (p.algo == X)
        {
            // Lo-Fi: crude sample & hold downsampling + light quantization for grit.
            ++lofiCounter;
            if (lofiCounter % 3 == 0)
                lofiHeld = shifted;
            shifted = lofiHeld;
            shifted = std::round (shifted * 32.0f) / 32.0f;
        }

        line.push (x);

        // MIX: balance between the original and the pitch-shifted signal
        // (use a low SEMI + low MIX for a chorus-like effect, per the manual).
        return x + (shifted - x) * p.mix;
    }

private:
    double sr = 44100.0;
    ModulatedDelayLine line;
    float delayA = 0.0f, delayB = 0.0f;
    bool grainInitialised = false;
    int lofiCounter = 0;
    float lofiHeld = 0.0f;
};
