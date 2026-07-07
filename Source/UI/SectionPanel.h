#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "DubLookAndFeel.h"

// A titled panel that lays out an arbitrary set of owned controls in a
// wrapping row (via FlexBox). Used as the content Component for each tab
// in the editor.
namespace DubUI
{
    class SectionPanel : public juce::Component
    {
    public:
        explicit SectionPanel (const juce::String& sectionTitle) : title (sectionTitle) {}

        // Takes ownership of the control.
        void addControl (juce::Component* control, int width = 74, int height = 84)
        {
            controls.add (control);
            addAndMakeVisible (control);
            sizes.add ({ width, height });
        }

        void paint (juce::Graphics& g) override
        {
            g.fillAll (DubLookAndFeel::panelBg);
            g.setColour (DubLookAndFeel::accent);
            g.setFont (juce::Font (16.0f, juce::Font::bold));
            g.drawText (title, getLocalBounds().removeFromTop (32).reduced (12, 0),
                        juce::Justification::centredLeft);
            g.setColour (DubLookAndFeel::panelEdge);
            g.drawHorizontalLine (34, 12.0f, (float) getWidth() - 12.0f);
        }

        void resized() override
        {
            auto area = getLocalBounds();
            area.removeFromTop (40);

            juce::FlexBox fb;
            fb.flexDirection = juce::FlexBox::Direction::row;
            fb.flexWrap = juce::FlexBox::Wrap::wrap;
            fb.alignContent = juce::FlexBox::AlignContent::flexStart;

            for (int i = 0; i < controls.size(); ++i)
                fb.items.add (juce::FlexItem (*controls[i])
                                  .withWidth ((float) sizes[i].w)
                                  .withHeight ((float) sizes[i].h)
                                  .withMargin (10.0f));

            fb.performLayout (area.reduced (8).toFloat());
        }

    private:
        struct WH { int w, h; };

        juce::String title;
        juce::OwnedArray<juce::Component> controls;
        juce::Array<WH> sizes;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SectionPanel)
    };
}
