#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/DubLookAndFeel.h"
#include "UI/SectionPanel.h"

// Single always-visible panel editor (no tabs): a fixed header above a
// scrollable vertical stack of the five DSP section panels, echoing the
// layout of a physical dub-delay hardware unit's fascia. All colours and
// drawing are original - nothing traced or copied from any reference
// product's artwork or layout.
class TapeDubDelayAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit TapeDubDelayAudioProcessorEditor (TapeDubDelayAudioProcessor&);
    ~TapeDubDelayAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Owns and vertically stacks the five section panels, recomputing each
    // one's needed height whenever the width changes so nothing clips as
    // the window is resized.
    class PanelStack : public juce::Component
    {
    public:
        void addSection (DubUI::SectionPanel* panel)
        {
            sections.add (panel);
            addAndMakeVisible (panel);
        }

        void resized() override
        {
            auto width = getWidth();
            int y = 12;

            for (auto* s : sections)
            {
                auto h = s->getPreferredHeight (width - 24);
                s->setBounds (12, y, width - 24, h);
                y += h + 14;
            }

            auto total = y + 12;
            if (getHeight() != total)
                setSize (width, total);
        }

    private:
        juce::OwnedArray<DubUI::SectionPanel> sections;
    };

    TapeDubDelayAudioProcessor& processorRef;
    DubLookAndFeel lookAndFeel;

    juce::Label titleLabel;
    juce::Label taglineLabel;

    juce::Viewport viewport;
    PanelStack panelStack;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeDubDelayAudioProcessorEditor)
};
