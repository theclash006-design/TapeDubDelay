#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

// Multi-mode filter (TPT / "zero-delay feedback" State Variable Filter,
// Vadim Zavalishin / Andrew Simper topology) with an optional tempo-synced
// LFO on the cutoff frequency. Stands in for the reference plug-in's
// "Filter" section (currently LP/HP/BP/Notch; the original's full 12-type,
// dual-algorithm filter bank can be added on top of this same structure
// later - see README "Roadmap").
class ModFilter
{
public:
    enum class Type { LowPass, HighPass, BandPass, Notch };

    void prepare (double sampleRate)
    {
        sr = sampleRate;
        reset();
    }

    void reset()
    {
        icL1 = icL2 = icR1 = icR2 = 0.0f;
        lfoPhase = 0.0;
    }

    struct Params
    {
        bool   on = false;
        Type   type = Type::LowPass;
        float  cutoffHz = 2000.0f;
        float  resonance = 0.7f;       // roughly 0.1 (wide) .. 10 (sharp)
        bool   lfoOn = false;
        float  lfoInt = 0.0f;          // 0..1, scales up to +/-3 octaves
        double lfoCycleSeconds = 0.5;  // tempo-synced cycle length, set by caller
    };

    void processSample (float& l, float& r, const Params& p)
    {
        if (! p.on)
            return;

        float cutoff = p.cutoffHz;

        if (p.lfoOn && p.lfoCycleSeconds > 0.0001)
        {
            const double inc = juce::MathConstants<double>::twoPi / (p.lfoCycleSeconds * sr);
            lfoPhase += inc;
            if (lfoPhase > juce::MathConstants<double>::twoPi)
                lfoPhase -= juce::MathConstants<double>::twoPi;

            const float octaves = p.lfoInt * 3.0f * (float) std::sin (lfoPhase);
            cutoff = juce::jlimit (20.0f, 18000.0f, cutoff * std::pow (2.0f, octaves));
        }

        const float g  = std::tan (juce::MathConstants<float>::pi * cutoff / (float) sr);
        const float k  = 1.0f / juce::jmax (0.05f, p.resonance);
        const float a1 = 1.0f / (1.0f + g * (g + k));
        const float a2 = g * a1;
        const float a3 = g * a2;

        l = tick (l, icL1, icL2, a1, a2, a3, k, p.type);
        r = tick (r, icR1, icR2, a1, a2, a3, k, p.type);
    }

private:
    static float tick (float x, float& ic1, float& ic2,
                        float a1, float a2, float a3, float k, Type type)
    {
        const float v3 = x - ic2;
        const float v1 = a1 * ic1 + a2 * v3;
        const float v2 = ic2 + a2 * ic1 + a3 * v3;
        ic1 = 2.0f * v1 - ic1;
        ic2 = 2.0f * v2 - ic2;

        switch (type)
        {
            case Type::LowPass:  return v2;
            case Type::HighPass: return x - k * v1 - v2;
            case Type::BandPass: return v1;
            case Type::Notch:    return x - k * v1;
        }
        return v2;
    }

    double sr = 44100.0;
    float icL1 = 0.0f, icL2 = 0.0f, icR1 = 0.0f, icR2 = 0.0f;
    double lfoPhase = 0.0;
};
