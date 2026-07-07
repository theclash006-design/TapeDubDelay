#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// MVP editor: JUCE's generic parameter list. Gives every APVTS parameter a
// working slider/toggle/combo for free, so the plug-in is fully usable while
// a custom graphic UI (matching the tape-delay pedal look) is built later.
class TapeDubDelayAudioProcessorEditor : public juce::GenericAudioProcessorEditor
{
public:
    explicit TapeDubDelayAudioProcessorEditor (TapeDubDelayAudioProcessor&);
};
