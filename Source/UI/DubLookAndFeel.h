#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Original dark/amber "hardware pedal" theme for Tape Dub Delay. All colours
// and knob/switch drawing are done from scratch here - nothing traced or
// copied from any reference plug-in's artwork or layout.
class DubLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DubLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawTabButton (juce::TabBarButton&, juce::Graphics&, bool isMouseOver, bool isMouseDown) override;

    static const juce::Colour panelBg;
    static const juce::Colour panelEdge;
    static const juce::Colour accent;
    static const juce::Colour text;
};
