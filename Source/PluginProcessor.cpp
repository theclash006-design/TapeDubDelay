#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/TempoSync.h"

using APVTS = juce::AudioProcessorValueTreeState;

TapeDubDelayAudioProcessor::TapeDubDelayAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

APVTS::ParameterLayout TapeDubDelayAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // --- Tape Delay -----------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "delayOn", "Delay On/Off", true));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "noteDiv", "Note Division", juce::StringArray { "1/2", "1/4", "1/8", "1/16" }, 1));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "groove", "Groove", juce::NormalisableRange<float> (-1.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "trim", "Trim", juce::NormalisableRange<float> (-1.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "x2", "x2", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "feedback", "Feedback", juce::NormalisableRange<float> (0.0f, 1.05f), 0.35f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "directMix", "Direct Mix", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "highCut", "High Cut", juce::NormalisableRange<float> (200.0f, 20000.0f, 1.0f, 0.3f), 8000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "lowCut", "Low Cut", juce::NormalisableRange<float> (20.0f, 2000.0f, 1.0f, 0.3f), 80.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "freeze", "Freeze", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "smooth", "Smooth", juce::NormalisableRange<float> (0.0f, 1.0f), 0.2f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "flutterInt", "Flutter Int", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "flutterRate", "Flutter Rate", juce::NormalisableRange<float> (0.1f, 20.0f, 0.0f, 0.4f), 6.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "wowInt", "Wow Int", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "wowRate", "Wow Rate", juce::NormalisableRange<float> (0.05f, 5.0f, 0.0f, 0.4f), 0.8f));

    // --- Stereo -----------------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "balance", "Balance", juce::NormalisableRange<float> (-1.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "width", "Width", juce::NormalisableRange<float> (0.0f, 1.5f), 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "pingpong", "Ping Pong", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "drift", "Drift", juce::NormalisableRange<float> (-1.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "autoPanInt", "Auto Pan Int", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "autoPanRate", "Auto Pan Rate", juce::StringArray { ":4", ":2", "x1", "x2", "x4" }, 2));

    // --- Filter -------------------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "filterOn", "Filter On/Off", false));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "filterType", "Filter Type", juce::StringArray { "LP", "HP", "BP", "Notch" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "cutoff", "Cutoff", juce::NormalisableRange<float> (20.0f, 18000.0f, 1.0f, 0.3f), 2000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "resonance", "Resonance", juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.4f), 0.7f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "filterLfoOn", "Filter LFO On", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "filterLfoInt", "Filter LFO Int", juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "filterLfoRate", "Filter LFO Rate", juce::StringArray { ":4", ":2", "x1", "x2", "x4" }, 2));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "filterPre", "Filter Pre (in feedback loop)", false));

    // --- Pitch Shift ------------------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "pitchOn", "Pitch Shift On/Off", false));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "pitchAlgo", "Pitch Algo", juce::StringArray { "A", "B", "C", "D", "X" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        "pitchSemi", "Pitch Semi", -24, 24, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitchFine", "Pitch Fine", juce::NormalisableRange<float> (-1.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitchGrain", "Pitch Grain", juce::NormalisableRange<float> (0.0f, 1.0f), 0.4f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitchMix", "Pitch Mix", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "pitchPre", "Pitch Pre (in feedback loop)", false));

    // --- Reverb ---------------------------------------------------------------
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "reverbOn", "Reverb On/Off", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverbSizeA", "Reverb Size A", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverbDampA", "Reverb Damp A", juce::NormalisableRange<float> (0.0f, 1.0f), 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverbSizeB", "Reverb Size B", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverbDampB", "Reverb Damp B", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverbMixAB", "Reverb Mix A/B", juce::NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverbWidth", "Reverb Width", juce::NormalisableRange<float> (0.0f, 1.5f), 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverbMix", "Reverb Mix", juce::NormalisableRange<float> (0.0f, 1.0f), 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "reverbPre", "Reverb Pre (in feedback loop)", false));

    return { params.begin(), params.end() };
}

void TapeDubDelayAudioProcessor::prepareToPlay (double sampleRate, int)
{
    tapeDelay.prepare (sampleRate);
    filter.prepare (sampleRate);
    pitchShiftL.prepare (sampleRate);
    pitchShiftR.prepare (sampleRate);
    reverb.prepare (sampleRate);
    autoPanPhase = 0.0;
}

bool TapeDubDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo();
}

void TapeDubDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
            if (auto bpm = pos->getBpm())
                currentBpm = *bpm;

    auto* left  = buffer.getWritePointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : left;
    const int numSamples = buffer.getNumSamples();

    // ---- gather Tape Delay params ----
    TapeDelayEngine::Params dp;
    dp.on            = apvts.getRawParameterValue ("delayOn")->load() > 0.5f;
    dp.noteDivIndex  = (int) apvts.getRawParameterValue ("noteDiv")->load();
    dp.groove        = apvts.getRawParameterValue ("groove")->load();
    dp.trim          = apvts.getRawParameterValue ("trim")->load();
    dp.x2            = apvts.getRawParameterValue ("x2")->load() > 0.5f;
    dp.feedback      = apvts.getRawParameterValue ("feedback")->load();
    dp.directMix     = apvts.getRawParameterValue ("directMix")->load();
    dp.highCutHz     = apvts.getRawParameterValue ("highCut")->load();
    dp.lowCutHz      = apvts.getRawParameterValue ("lowCut")->load();
    dp.freeze        = apvts.getRawParameterValue ("freeze")->load() > 0.5f;
    dp.smooth        = apvts.getRawParameterValue ("smooth")->load();
    dp.flutterInt    = apvts.getRawParameterValue ("flutterInt")->load();
    dp.flutterRateHz = apvts.getRawParameterValue ("flutterRate")->load();
    dp.wowInt        = apvts.getRawParameterValue ("wowInt")->load();
    dp.wowRateHz     = apvts.getRawParameterValue ("wowRate")->load();
    dp.pingPong      = apvts.getRawParameterValue ("pingpong")->load() > 0.5f;
    dp.drift         = apvts.getRawParameterValue ("drift")->load();

    // ---- gather Filter params ----
    const bool filterOn  = apvts.getRawParameterValue ("filterOn")->load() > 0.5f;
    const bool filterPre = apvts.getRawParameterValue ("filterPre")->load() > 0.5f;

    ModFilter::Params fp;
    fp.on        = filterOn;
    fp.type      = (ModFilter::Type) (int) apvts.getRawParameterValue ("filterType")->load();
    fp.cutoffHz  = apvts.getRawParameterValue ("cutoff")->load();
    fp.resonance = apvts.getRawParameterValue ("resonance")->load();
    fp.lfoOn     = apvts.getRawParameterValue ("filterLfoOn")->load() > 0.5f;
    fp.lfoInt    = apvts.getRawParameterValue ("filterLfoInt")->load();

    auto* filterLfoRateParam = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter ("filterLfoRate"));
    const double filterLfoFactor = TempoSync::rateChoiceToFactor (filterLfoRateParam->getCurrentChoiceName());
    fp.lfoCycleSeconds = juce::jmax (0.001, tapeDelay.getCurrentDelaySeconds() * filterLfoFactor);

    // ---- gather Pitch Shift params ----
    const bool pitchOn  = apvts.getRawParameterValue ("pitchOn")->load() > 0.5f;
    const bool pitchPre = apvts.getRawParameterValue ("pitchPre")->load() > 0.5f;

    GranularPitchShifter::Params pp;
    pp.on    = pitchOn;
    pp.algo  = (int) apvts.getRawParameterValue ("pitchAlgo")->load();
    pp.semi  = (int) apvts.getRawParameterValue ("pitchSemi")->load();
    pp.fine  = apvts.getRawParameterValue ("pitchFine")->load();
    pp.grain = apvts.getRawParameterValue ("pitchGrain")->load();
    pp.mix   = apvts.getRawParameterValue ("pitchMix")->load();

    // ---- gather Reverb params ----
    const bool reverbOn  = apvts.getRawParameterValue ("reverbOn")->load() > 0.5f;
    const bool reverbPre = apvts.getRawParameterValue ("reverbPre")->load() > 0.5f;

    ReverbBank::Params rp;
    rp.on     = reverbOn;
    rp.sizeA  = apvts.getRawParameterValue ("reverbSizeA")->load();
    rp.dampA  = apvts.getRawParameterValue ("reverbDampA")->load();
    rp.sizeB  = apvts.getRawParameterValue ("reverbSizeB")->load();
    rp.dampB  = apvts.getRawParameterValue ("reverbDampB")->load();
    rp.mixAB  = apvts.getRawParameterValue ("reverbMixAB")->load();
    rp.width  = apvts.getRawParameterValue ("reverbWidth")->load();
    rp.mix    = apvts.getRawParameterValue ("reverbMix")->load();

    // ---- Delay bypassed: filter/pitch/reverb (if any) still run on the dry signal ----
    if (! dp.on)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            if (filterOn)
                filter.processSample (left[n], right[n], fp);
            if (pitchOn)
            {
                left[n]  = pitchShiftL.processSample (left[n], pp);
                right[n] = pitchShiftR.processSample (right[n], pp);
            }
            if (reverbOn)
                reverb.processSample (left[n], right[n], rp);
        }
        return;
    }

    // "PRE" = inserted inside the tape delay's feedback loop, Filter -> Pitch Shift -> Reverb.
    // A pitch shifter in the feedback loop shifts a little more with every repeat -
    // the classic "pitch escalator" dub delay effect.
    TapeDelayEngine::FeedbackHook hook = nullptr;
    if ((filterOn && filterPre) || (pitchOn && pitchPre) || (reverbOn && reverbPre))
    {
        hook = [this, fp, filterOn, filterPre, pp, pitchOn, pitchPre, rp, reverbOn, reverbPre] (float& l, float& r)
        {
            if (filterOn && filterPre)
                filter.processSample (l, r, fp);
            if (pitchOn && pitchPre)
            {
                l = pitchShiftL.processSample (l, pp);
                r = pitchShiftR.processSample (r, pp);
            }
            if (reverbOn && reverbPre)
                reverb.processSample (l, r, rp);
        };
    }

    tapeDelay.process (left, right, numSamples, dp, currentBpm, hook);

    // ---- Stereo section (balance / width / auto-pan), post-delay ----
    const float balance    = apvts.getRawParameterValue ("balance")->load();
    const float width      = apvts.getRawParameterValue ("width")->load();
    const float autoPanInt = apvts.getRawParameterValue ("autoPanInt")->load();

    auto* autoPanRateParam = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter ("autoPanRate"));
    const double autoPanFactor = TempoSync::rateChoiceToFactor (autoPanRateParam->getCurrentChoiceName());
    const double autoPanCycle  = juce::jmax (0.001, tapeDelay.getCurrentDelaySeconds() * autoPanFactor);
    const double autoPanInc    = juce::MathConstants<double>::twoPi / (autoPanCycle * getSampleRate());

    const float bnorm = (balance + 1.0f) * 0.5f;
    const float gL = std::cos (bnorm * juce::MathConstants<float>::halfPi) * 1.4142136f;
    const float gR = std::sin (bnorm * juce::MathConstants<float>::halfPi) * 1.4142136f;

    for (int n = 0; n < numSamples; ++n)
    {
        // WIDTH via mid/side
        const float mid  = 0.5f * (left[n] + right[n]);
        const float side = 0.5f * (left[n] - right[n]) * width;
        float l = mid + side;
        float r = mid - side;

        // BALANCE (equal power)
        l *= gL;
        r *= gR;

        // AUTO PAN (LFO synced to the current delay time)
        if (autoPanInt > 0.0001f)
        {
            autoPanPhase += autoPanInc;
            if (autoPanPhase > juce::MathConstants<double>::twoPi)
                autoPanPhase -= juce::MathConstants<double>::twoPi;

            const float pan = autoPanInt * (float) std::sin (autoPanPhase);
            const float pnorm = (pan + 1.0f) * 0.5f;
            l *= std::cos (pnorm * juce::MathConstants<float>::halfPi) * 1.4142136f;
            r *= std::sin (pnorm * juce::MathConstants<float>::halfPi) * 1.4142136f;
        }

        left[n]  = l;
        right[n] = r;

        // Filter, "post" routing (after delay + stereo stage)
        if (filterOn && ! filterPre)
            filter.processSample (left[n], right[n], fp);

        // Pitch Shift, "post" routing (after delay + stereo stage, and after Filter)
        if (pitchOn && ! pitchPre)
        {
            left[n]  = pitchShiftL.processSample (left[n], pp);
            right[n] = pitchShiftR.processSample (right[n], pp);
        }

        // Reverb, "post" routing (after delay + stereo stage, and after Filter/Pitch Shift)
        if (reverbOn && ! reverbPre)
            reverb.processSample (left[n], right[n], rp);
    }
}

juce::AudioProcessorEditor* TapeDubDelayAudioProcessor::createEditor()
{
    return new TapeDubDelayAudioProcessorEditor (*this);
}

void TapeDubDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TapeDubDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapeDubDelayAudioProcessor();
}
