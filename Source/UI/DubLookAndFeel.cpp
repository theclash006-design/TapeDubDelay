#include "DubLookAndFeel.h"
#include <cmath>

const juce::Colour DubLookAndFeel::panelBg   { 0xfff7eedb };
const juce::Colour DubLookAndFeel::panelCard { 0xffefe0bd };
const juce::Colour DubLookAndFeel::panelEdge { 0xffb69a63 };
const juce::Colour DubLookAndFeel::accent    { 0xffd9622e };
const juce::Colour DubLookAndFeel::accent2   { 0xff2c8c86 };
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
}

void DubLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                        juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (4.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // analog-gauge tick marks around the dial
    for (int i = 0; i <= 10; ++i)
    {
        auto t = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (i / 10.0f);
        auto inner = radius * 0.92f;
        auto outer = radius * 1.02f;
        juce::Point<float> p1 (centre.x + inner * std::sin (t), centre.y - inner * std::cos (t));
        juce::Point<float> p2 (centre.x + outer * std::sin (t), centre.y - outer * std::cos (t));
        g.setColour (panelEdge.withAlpha (0.8f));
        g.drawLine ({ p1, p2 }, 1.2f);
    }

    // outer track
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, radius * 0.86f, radius * 0.86f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (panelEdge.withAlpha (0.55f));
    g.strokePath (track, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // value arc (teal)
    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, radius * 0.86f, radius * 0.86f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (accent2);
    g.strokePath (valueArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // knob body
    auto knobRadius = radius * 0.66f;
    g.setColour (panelCard.brighter (0.08f));
    g.fillEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
    g.setColour (panelEdge);
    g.drawEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.5f);

    // pointer (orange)
    juce::Path pointer;
    auto pointerLength = knobRadius * 0.82f;
    pointer.addRectangle (-1.6f, -pointerLength, 3.2f, pointerLength * 0.88f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    g.setColour (accent);
    g.fillPath (pointer);

    // centre cap
    g.setColour (accent);
    g.fillEllipse (centre.x - 3.0f, centre.y - 3.0f, 6.0f, 6.0f);

    juce::ignoreUnused (slider);
}

void DubLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);
    auto switchW = juce::jmin (bounds.getWidth(), 34.0f);
    auto switchBounds = juce::Rectangle<float> (switchW, switchW * 0.55f)
                            .withCentre ({ bounds.getX() + switchW * 0.5f, bounds.getCentreY() });

    bool on = button.getToggleState();
    g.setColour (on ? accent : panelCard.darker (0.06f));
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
