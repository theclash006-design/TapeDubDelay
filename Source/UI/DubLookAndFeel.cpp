#include "DubLookAndFeel.h"
#include <cmath>

const juce::Colour DubLookAndFeel::panelBg   { 0xfff7eedb };
const juce::Colour DubLookAndFeel::panelCard { 0xffefe0bd };
const juce::Colour DubLookAndFeel::panelEdge { 0xffb69a63 };
const juce::Colour DubLookAndFeel::accent    { 0xffc8102e };
const juce::Colour DubLookAndFeel::accent2   { 0xffe0b043 };
const juce::Colour DubLookAndFeel::accent3   { 0xff1e7145 };
const juce::Colour DubLookAndFeel::text      { 0xff2c2013 };

DubLookAndFeel::DubLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, panelBg);
    setColour (juce::Slider::textBoxTextColourId, text);
    setColour (juce::Slider::textBoxOutlineColourId, panelEdge);
    setColour (juce::Slider::textBoxBackgroundColourId, panelCard.darker (0.03f));
    setColour (juce::Label::textColourId, text);
    setColour (juce::ComboBox::backgroundColourId, panelCard);
    setColour (juce::ComboBox::outlineColourId, panelEdge);
    setColour (juce::ComboBox::textColourId, text);
    setColour (juce::PopupMenu::backgroundColourId, panelCard);
    setColour (juce::PopupMenu::textColourId, text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha (0.35f));
    setColour (juce::TextButton::buttonColourId, panelCard);

    // default pointer/tick colour when a component doesn't set its own
    // (Controls.h overrides these per-instance to cycle red/gold/green)
    setColour (juce::Slider::rotarySliderFillColourId, accent);
    setColour (juce::ToggleButton::tickColourId, accent);
}

void DubLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                        juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (4.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f * 0.82f;
    auto centre = bounds.getCentre();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Plain matte convex knob: no recess, no gear flutes, no glossy
    // highlight - just a rounded dome lifted off the panel with a soft
    // drop shadow and a single directional light-to-dark gradient
    // (light upper-left, darker lower-right), plus a thin pointer.
    juce::Path knobPath;
    knobPath.addEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    juce::DropShadow shadow (juce::Colours::black.withAlpha (0.4f),
                              (int) juce::jmax (2.0f, radius * 0.3f),
                              { 0, (int) juce::jmax (1.0f, radius * 0.16f) });
    shadow.drawForPath (g, knobPath);

    auto highlightPos = centre.translated (radius * -0.22f, radius * -0.30f);
    juce::ColourGradient dome (juce::Colour (0xfff2e4c1), highlightPos.x, highlightPos.y,
                                juce::Colour (0xffa3893f), centre.x + radius, centre.y, true);
    dome.addColour (0.55, juce::Colour (0xffd9c290));
    g.setGradientFill (dome);
    g.fillPath (knobPath);

    g.setColour (juce::Colour (0xff5c4a1f).withAlpha (0.55f));
    g.strokePath (knobPath, juce::PathStrokeType (1.3f));

    // pointer (colour is per-component - see Controls.h - so it can cycle
    // through the rasta red/gold/green trio instead of always being red)
    juce::Path pointer;
    auto pointerLength = radius * 0.78f;
    pointer.addRectangle (-1.7f, -pointerLength, 3.4f, pointerLength * 0.9f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId));
    g.fillPath (pointer);
}

void DubLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);
    auto switchW = juce::jmin (bounds.getWidth(), 34.0f);
    auto switchBounds = juce::Rectangle<float> (switchW, switchW * 0.55f)
                            .withCentre ({ bounds.getX() + switchW * 0.5f, bounds.getCentreY() });

    bool on = button.getToggleState();
    g.setColour (on ? button.findColour (juce::ToggleButton::tickColourId) : panelCard.darker (0.06f));
    g.fillRoundedRectangle (switchBounds, switchBounds.getHeight() * 0.5f);
    g.setColour (panelEdge);
    g.drawRoundedRectangle (switchBounds, switchBounds.getHeight() * 0.5f, 1.2f);

    auto knobD = switchBounds.getHeight() - 4.0f;
    auto knobX = on ? switchBounds.getRight() - knobD - 2.0f : switchBounds.getX() + 2.0f;
    g.setColour (juce::Colours::white);
    g.fillEllipse (knobX, switchBounds.getY() + 2.0f, knobD, knobD);
    g.setColour (panelEdge.withAlpha (0.6f));
    g.drawEllipse (knobX, switchBounds.getY() + 2.0f, knobD, knobD, 1.0f);

    g.setColour (text);
    g.setFont (juce::Font (13.0f, juce::Font::bold));
    g.drawText (button.getButtonText(), bounds.withTrimmedLeft (switchW + 8.0f), juce::Justification::centredLeft);

    juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}
