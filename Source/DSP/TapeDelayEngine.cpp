#include "TapeDelayEngine.h"
#include "TempoSync.h"

void TapeDelayEngine::prepare (double sr)
{
    sampleRate = sr;
    lineL.prepare (sr, 10.0);   // 10s max covers the slowest note + dotted + x2 combo
    lineR.prepare (sr, 10.0);
    fbFilterL.setSampleRate (sr);
    fbFilterR.setSampleRate (sr);
    reset();
}

void TapeDelayEngine::reset()
{
    lineL.reset();
    lineR.reset();
    fbFilterL.reset();
    fbFilterR.reset();
    wowPhase = flutterPhase = 0.0;
}

void TapeDelayEngine::process (float* left, float* right, int numSamples,
                                const Params& p, double hostBpm,
                                const FeedbackHook& feedbackHook)
{
    fbFilterL.setHighCutHz (p.highCutHz);
    fbFilterR.setHighCutHz (p.highCutHz);
    fbFilterL.setLowCutHz (p.lowCutHz);
    fbFilterR.setLowCutHz (p.lowCutHz);

    // --- target delay time from note division / groove / x2 ---
    double baseSeconds = TempoSync::noteDivisionSeconds (hostBpm, p.noteDivIndex);
    baseSeconds = TempoSync::applyGroove (baseSeconds, p.groove);
    if (p.x2)
        baseSeconds *= 2.0;
    baseSeconds = juce::jlimit (0.001, 9.5, baseSeconds);

    // Smoothing time constant: ~5ms (near instant) .. ~2.5s (slow tape-motor style)
    const double smoothMs = juce::jmap ((double) p.smooth, 0.0, 1.0, 5.0, 2500.0);
    const double smoothCoeff = std::exp (-1.0 / (0.001 * smoothMs * sampleRate));

    // TRIM: single control that pushes L/R delay times apart symmetrically (+/-50%)
    const double trimOffset = (double) p.trim * 0.5;
    const double baseLeftRatio  = 1.0 - trimOffset;
    const double baseRightRatio = 1.0 + trimOffset;

    const double wowInc     = juce::MathConstants<double>::twoPi * p.wowRateHz / sampleRate;
    const double flutterInc = juce::MathConstants<double>::twoPi * p.flutterRateHz / sampleRate;

    const double fbBase   = juce::jlimit (0.0, 1.05, (double) p.feedback);
    const double driftAmt = juce::jlimit (-1.0, 1.0, (double) p.drift);
    const double fbLeftGain  = fbBase * (1.0 - driftAmt * 0.5);
    const double fbRightGain = fbBase * (1.0 + driftAmt * 0.5);

    for (int n = 0; n < numSamples; ++n)
    {
        // Slew the delay time towards the target (SMOOTH control)
        smoothedDelaySeconds = smoothedDelaySeconds * smoothCoeff + baseSeconds * (1.0 - smoothCoeff);
        const double slewRatio = smoothedDelaySeconds / baseSeconds;

        wowPhase += wowInc;
        if (wowPhase > juce::MathConstants<double>::twoPi) wowPhase -= juce::MathConstants<double>::twoPi;
        flutterPhase += flutterInc;
        if (flutterPhase > juce::MathConstants<double>::twoPi) flutterPhase -= juce::MathConstants<double>::twoPi;

        // WOW/FLUTTER: small modulation on top of the (already slewed) time.
        // INT=1 gives roughly +/-8ms depth for wow and +/-3ms for flutter.
        const double modSeconds = p.wowInt * 0.008 * std::sin (wowPhase)
                                 + p.flutterInt * 0.003 * std::sin (flutterPhase);

        const double timeL = juce::jmax (0.0005, baseSeconds * baseLeftRatio  * slewRatio + modSeconds);
        const double timeR = juce::jmax (0.0005, baseSeconds * baseRightRatio * slewRatio + modSeconds);

        const double delaySamplesL = timeL * sampleRate;
        const double delaySamplesR = timeR * sampleRate;

        const float readL = lineL.read (delaySamplesL);
        const float readR = lineR.read (delaySamplesR);

        if (p.freeze)
        {
            // FREEZE: recirculate the loop content at unity gain, no new input,
            // no tape-eq degradation.
            lineL.push (readL);
            lineR.push (readR);
        }
        else
        {
            float sendL = readL, sendR = readR;

            if (p.pingPong)
                std::swap (sendL, sendR);

            // High/low cut live inside the feedback path only.
            sendL = fbFilterL.processLowCut (fbFilterL.processHighCut (sendL));
            sendR = fbFilterR.processLowCut (fbFilterR.processHighCut (sendR));

            if (feedbackHook)
                feedbackHook (sendL, sendR);

            // PINGPONG mutes the right input, feeding a mono sum into the left line only,
            // matching the manual's note about the right input channel being muted.
            const float inL = p.pingPong ? (left[n] + right[n]) * 0.5f : left[n];
            const float inR = p.pingPong ? 0.0f : right[n];

            lineL.push (inL + sendL * (float) fbLeftGain);
            lineR.push (inR + sendR * (float) fbRightGain);
        }

        // DIRECT MIX: balance between the (wet) delay signal and the unprocessed input.
        left[n]  = juce::jmap (p.directMix, readL, left[n]);
        right[n] = juce::jmap (p.directMix, readR, right[n]);
    }
}
