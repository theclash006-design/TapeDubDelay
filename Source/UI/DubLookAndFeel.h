#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// "Sunsplash Console" theme for Tape Dub Delay: warm cream panels with a
// rasta red/gold/green accent trio, and plain matte convex knobs (soft
// directional shading + drop shadow, no recess, no gear flutes, no gloss).
// A 60s-sci-fi-console feel inspired by vintage tape-echo hardware, but
// drawn entirely from scratch here. No colours, artwork, or layout are
// traced or copied from any reference plug-in or product.
class DubLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DubLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    static const juce::Colour panelBg;    // outer window background (light cream)
    static const juce::Colour panelCard;  // section-panel card background (deeper cream)
    static const juce::Colour panelEdge;  // borders (warm tan-brown)
    static const juce::Colour accent;     // primary accent (rasta red)
    static const juce::Colour accent2;    // secondary accent (rasta gold)
    static const juce::Colour accent3;    // tertiary accent (rasta green)
    static const juce::Colour text;       // ink (dark warm brown, near-black)
};
