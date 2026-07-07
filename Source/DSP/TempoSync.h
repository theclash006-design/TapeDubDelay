#pragma once

#include <juce_core/juce_core.h>

// Small helpers to convert host tempo + note-division / groove / rate-multiplier
// choices into concrete time values, mirroring how the original plug-in
// syncs the delay time and the various LFOs to the host tempo.
namespace TempoSync
{
    // noteDivIndex: 0 = 1/2, 1 = 1/4, 2 = 1/8, 3 = 1/16
    inline double noteDivisionSeconds (double bpm, int noteDivIndex)
    {
        const double quarterNoteSec = 60.0 / juce::jmax (1.0, bpm);
        static const double multipliers[4] = { 2.0, 1.0, 0.5, 0.25 };
        const int idx = juce::jlimit (0, 3, noteDivIndex);
        return quarterNoteSec * multipliers[idx];
    }

    // groove: -1 = full triplet (2/3), 0 = straight note, +1 = full dotted (3/2)
    inline double applyGroove (double baseSeconds, float groove)
    {
        if (groove < 0.0f)
            return juce::jmap ((double) -groove, 0.0, 1.0, baseSeconds, baseSeconds * (2.0 / 3.0));

        return juce::jmap ((double) groove, 0.0, 1.0, baseSeconds, baseSeconds * 1.5);
    }

    // Parses rate-multiplier choice strings like ":4", ":2", "x1", "x2", "x4"
    // into a plain multiplication factor relative to one full cycle.
    inline double rateChoiceToFactor (const juce::String& choice)
    {
        if (choice.startsWithChar ('x'))
            return juce::jmax (0.01, choice.substring (1).getDoubleValue());

        if (choice.startsWithChar (':'))
        {
            const double d = choice.substring (1).getDoubleValue();
            return d > 0.0 ? 1.0 / d : 1.0;
        }

        return 1.0;
    }
}
