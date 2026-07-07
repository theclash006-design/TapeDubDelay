#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// "Atomic Console" theme for Tape Dub Delay: warm cream panels, burnt-orange
// primary accent, dusty teal secondary accent - a 60s-sci-fi-console palette
// inspired by the look of vintage tape-echo hardware, but drawn entirely
// from scratch here. No colours, artwork, or layout are traced or copied
// from any reference plug-in or product.
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
    static const juce::Colour panelEdge;  // borders, tick marks (warm tan-brown)
    static const juce::Colour accent;     // primary accent (burnt orange)
    static const juce::Colour accent2;    // secondary accent (dusty teal)
    static const juce::Colour text;       // ink (dark warm brown, near-black)
};
