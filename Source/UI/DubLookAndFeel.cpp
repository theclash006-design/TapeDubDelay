#include "DubLookAndFeel.h"

const juce::Colour DubLookAndFeel::panelBg   { 0xff23282d };
const juce::Colour DubLookAndFeel::panelEdge { 0xff454d55 };
const juce::Colour DubLookAndFeel::accent    { 0xffe0a44a };
const juce::Colour DubLookAndFeel::text      { 0xffe8e6df };

DubLookAndFeel::DubLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, panelBg);
    setColour (juce::Slider::textBoxTextColourId, text);
    setColour (juce::Slider::textBoxOutlineColourId, panelEdge);
    setColour (juce::Slider::textBoxBackgroundColourId, panelBg.darker (0.2f));
    setColour (juce::Label::textColourId, text);
    setColour (juce::ComboBox::backgroundColourId, panelBg.darker (0.1f));
    setColour (juce::ComboBox::outlineColourId, panelEdge);
    setColour (juce::ComboBox::textColourId, text);
    setColour (juce::PopupMenu::backgroundColourId, panelBg);
    setColour (juce::PopupMenu::textColourId, text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha (0.3f));
    setColour (juce::TabbedComponent::backgroundColourId, panelBg);
    setColour (juce::TabbedButtonBar::tabOutlineColourId, panelEdge);
    setColour (juce::TextButton::buttonColourId, panelBg.brighter (0.1f));
}

void DubLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                        juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (4.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // outer track
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (panelEdge);
    g.strokePath (track, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // value arc
    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (accent);
    g.strokePath (valueArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // knob body
    auto knobRadius = radius * 0.68f;
    g.setColour (panelBg.brighter (0.15f));
    g.fillEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
    g.setColour (panelEdge);
    g.drawEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.5f);

    // pointer
    juce::Path pointer;
    auto pointerLength = knobRadius * 0.85f;
    pointer.addRectangle (-1.5f, -pointerLength, 3.0f, pointerLength * 0.9f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    g.setColour (accent);
    g.fillPath (pointer);

    juce::ignoreUnused (slider);
}

void DubLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);
    auto switchW = juce::jmin (bounds.getWidth(), 34.0f);
    auto switchBounds = juce::Rectangle<float> (switchW, switchW * 0.55f)
                            .withCentre ({ bounds.getX() + switchW * 0.5f, bounds.getCentreY() });

    g.setColour (button.getToggleState() ? accent.withAlpha (0.85f) : panelBg.darker (0.3f));
    g.fillRoundedRectangle (switchBounds, switchBounds.getHeight() * 0.5f);
    g.setColour (panelEdge);
    g.drawRoundedRectangle (switchBounds, switchBounds.getHeight() * 0.5f, 1.2f);

    auto knobD = switchBounds.getHeight() - 4.0f;
    auto knobX = button.getToggleState() ? switchBounds.getRight() - knobD - 2.0f : switchBounds.getX() + 2.0f;
    g.setColour (text);
    g.fillEllipse (knobX, switchBounds.getY() + 2.0f, knobD, knobD);

    g.setColour (text);
    g.setFont (juce::Font (13.0f, juce::Font::bold));
    g.drawText (button.getButtonText(), bounds.withTrimmedLeft (switchW + 8.0f), juce::Justification::centredLeft);

    juce::ignoreUnused (shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

void DubLookAndFeel::drawTabButton (juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown)
{
    auto area = button.getActiveArea().toFloat();
    bool selected = button.getToggleState();

    g.setColour (selected ? accent.withAlpha (0.9f) : panelBg.brighter (isMouseOver ? 0.08f : 0.0f));
    g.fillRect (area);
    g.setColour (panelEdge);
    g.drawRect (area, 1.0f);
    g.setColour (selected ? juce::Colours::black : text);
    g.setFont (juce::Font (14.0f, juce::Font::bold));
    g.drawText (button.getButtonText(), area.toNearestInt(), juce::Justification::centred);

    juce::ignoreUnused (isMouseDown);
}
