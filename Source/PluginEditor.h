#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/DubLookAndFeel.h"

// Custom tabbed editor: one tab per DSP section (Delay/Stereo/Filter/Pitch/
// Reverb), each built from the reusable Knob/Toggle/Choice controls in
// UI/Controls.h laid out via UI/SectionPanel.h. Replaces the earlier
// GenericAudioProcessorEditor placeholder.
class TapeDubDelayAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit TapeDubDelayAudioProcessorEditor (TapeDubDelayAudioProcessor&);
    ~TapeDubDelayAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TapeDubDelayAudioProcessor& processorRef;
    DubLookAndFeel lookAndFeel;

    juce::Label titleLabel;
    juce::TabbedComponent tabs { juce::TabbedButtonBar::Orientation::TabsAtTop };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeDubDelayAudioProcessorEditor)
};
