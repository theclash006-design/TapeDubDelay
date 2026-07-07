#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <cmath>
#include "DubLookAndFeel.h"

// A titled card that lays out an arbitrary set of owned controls in a
// wrapping row (via FlexBox). Used as one "hardware section" in the
// always-visible stacked panel layout (see PluginEditor's PanelStack).
namespace DubUI
{
    class SectionPanel : public juce::Component
    {
    public:
        // sectionIndex (1-based) draws a small numbered badge next to the
        // title, echoing the numbered sections on a hardware unit's fascia.
        // Pass 0 to omit the badge.
        explicit SectionPanel (const juce::String& sectionTitle, int sectionIndex = 0)
            : title (sectionTitle), index (sectionIndex) {}

        // Takes ownership of the control.
        void addControl (juce::Component* control, int width = 74, int height = 84)
        {
            controls.add (control);
            addAndMakeVisible (control);
            sizes.add ({ width, height });
        }

        void paint (juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();

            g.setColour (DubLookAndFeel::panelCard);
            g.fillRoundedRectangle (bounds, 6.0f);
            g.setColour (DubLookAndFeel::panelEdge);
            g.drawRoundedRectangle (bounds.reduced (0.75f), 6.0f, 1.5f);

            auto header = bounds.removeFromTop (36.0f).reduced (14.0f, 6.0f);

            if (index > 0)
            {
                // cycle the badge through the rasta red/gold/green trio
                static const juce::Colour badgeColours[3] = { DubLookAndFeel::accent3,
                                                                DubLookAndFeel::accent,
                                                                DubLookAndFeel::accent2 };
                auto badgeColour = badgeColours[(index - 1) % 3];

                auto badge = header.removeFromLeft (24.0f).withSizeKeepingCentre (22.0f, 22.0f);
                g.setColour (badgeColour);
                g.fillEllipse (badge);
                g.setColour (badgeColour == DubLookAndFeel::accent2 ? DubLookAndFeel::text : juce::Colours::white);
                g.setFont (juce::Font (12.0f, juce::Font::bold));
                g.drawText (juce::String (index), badge.toNearestInt(), juce::Justification::centred);
                header.removeFromLeft (10.0f);
            }

            g.setColour (DubLookAndFeel::text);
            g.setFont (juce::Font (16.0f, juce::Font::bold));
            g.drawText (title, header, juce::Justification::centredLeft);

            // rasta red/gold/green triple divider
            auto stripeW = ((float) getWidth() - 28.0f) / 3.0f;
            g.setColour (DubLookAndFeel::accent);
            g.fillRect (juce::Rectangle<float> (14.0f, 40.0f, stripeW, 2.0f));
            g.setColour (DubLookAndFeel::accent2);
            g.fillRect (juce::Rectangle<float> (14.0f + stripeW, 40.0f, stripeW, 2.0f));
            g.setColour (DubLookAndFeel::accent3);
            g.fillRect (juce::Rectangle<float> (14.0f + stripeW * 2.0f, 40.0f, stripeW, 2.0f));
        }

        void resized() override
        {
            auto area = getLocalBounds();
            area.removeFromTop (48);
            buildFlexBox (true).performLayout (area.reduced (8).toFloat());
        }

        // Simulates the same wrapping layout used in resized() at a given
        // panel width, without touching real components, so the owning
        // PanelStack can size this panel correctly before it's positioned.
        int getPreferredHeight (int width)
        {
            auto w = (float) juce::jmax (width - 16, 60);
            auto fb = buildFlexBox (false);
            fb.performLayout (juce::Rectangle<float> (0.0f, 0.0f, w, 4000.0f));

            float bottom = 0.0f;
            for (auto& item : fb.items)
                bottom = juce::jmax (bottom, item.currentBounds.getBottom());

            return (int) std::ceil (bottom) + 48 /* header + divider */ + 16 /* bottom padding */;
        }

    private:
        struct WH { int w, h; };

        juce::FlexBox buildFlexBox (bool bindComponents)
        {
            juce::FlexBox fb;
            fb.flexDirection = juce::FlexBox::Direction::row;
            fb.flexWrap = juce::FlexBox::Wrap::wrap;
            fb.alignContent = juce::FlexBox::AlignContent::flexStart;

            for (int i = 0; i < controls.size(); ++i)
            {
                juce::FlexItem item = bindComponents ? juce::FlexItem (*controls[i]) : juce::FlexItem();
                fb.items.add (item.withWidth ((float) sizes[i].w).withHeight ((float) sizes[i].h).withMargin (10.0f));
            }
            return fb;
        }

        juce::String title;
        int index = 0;
        juce::OwnedArray<juce::Component> controls;
        juce::Array<WH> sizes;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SectionPanel)
    };
}
