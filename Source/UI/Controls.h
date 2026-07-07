#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "DubLookAndFeel.h"

// Small reusable control wrappers, each bundling a label + widget + APVTS
// attachment into one Component so the section-builder code in
// PluginEditor.cpp stays short.
namespace DubUI
{
    using APVTS = juce::AudioProcessorValueTreeState;

    // A rotary knob with a label above and a numeric readout below.
    // accentColour lets callers cycle the pointer through the rasta
    // red/gold/green trio instead of every knob defaulting to the same one.
    class Knob : public juce::Component
    {
    public:
        Knob (APVTS& state, const juce::String& paramID, const juce::String& labelText,
              juce::Colour accentColour = DubLookAndFeel::accent)
        {
            addAndMakeVisible (label);
            label.setText (labelText, juce::dontSendNotification);
            label.setJustificationType (juce::Justification::centred);
            label.setFont (juce::Font (12.0f, juce::Font::bold));
            label.setColour (juce::Label::textColourId, DubLookAndFeel::text);

            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                         juce::MathConstants<float>::pi * 2.8f, true);
            slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 16);
            // set explicitly on the component (not just the shared LookAndFeel
            // scheme) so the value readout is always dark ink on a cream box,
            // never the default light JUCE text colour
            slider.setColour (juce::Slider::textBoxTextColourId, DubLookAndFeel::text);
            slider.setColour (juce::Slider::textBoxBackgroundColourId, DubLookAndFeel::panelCard.darker (0.05f));
            slider.setColour (juce::Slider::textBoxOutlineColourId, DubLookAndFeel::panelEdge);
            slider.setColour (juce::Slider::rotarySliderFillColourId, accentColour);
            addAndMakeVisible (slider);

            attachment = std::make_unique<APVTS::SliderAttachment> (state, paramID, slider);
        }

        void resized() override
        {
            auto b = getLocalBounds();
            label.setBounds (b.removeFromTop (16));
            slider.setBounds (b);
        }

    private:
        juce::Label label;
        juce::Slider slider;
        std::unique_ptr<APVTS::SliderAttachment> attachment;
    };

    // An on/off rocker-style toggle switch. accentColour cycles the same way as Knob.
    class Toggle : public juce::Component
    {
    public:
        Toggle (APVTS& state, const juce::String& paramID, const juce::String& labelText,
                juce::Colour accentColour = DubLookAndFeel::accent)
        {
            button.setButtonText (labelText);
            button.setColour (juce::ToggleButton::tickColourId, accentColour);
            addAndMakeVisible (button);
            attachment = std::make_unique<APVTS::ButtonAttachment> (state, paramID, button);
        }

        void resized() override { button.setBounds (getLocalBounds()); }

    private:
        juce::ToggleButton button;
        std::unique_ptr<APVTS::ButtonAttachment> attachment;
    };

    // A labelled dropdown, auto-populated from the AudioParameterChoice's own list.
    class Choice : public juce::Component
    {
    public:
        Choice (APVTS& state, const juce::String& paramID, const juce::String& labelText)
        {
            addAndMakeVisible (label);
            label.setText (labelText, juce::dontSendNotification);
            label.setJustificationType (juce::Justification::centred);
            label.setFont (juce::Font (12.0f, juce::Font::bold));
            label.setColour (juce::Label::textColourId, DubLookAndFeel::text);

            if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*> (state.getParameter (paramID)))
                for (int i = 0; i < choiceParam->choices.size(); ++i)
                    box.addItem (choiceParam->choices[i], i + 1);

            addAndMakeVisible (box);
            attachment = std::make_unique<APVTS::ComboBoxAttachment> (state, paramID, box);
        }

        void resized() override
        {
            auto b = getLocalBounds();
            label.setBounds (b.removeFromTop (16));
            box.setBounds (b.reduced (0, 8));
        }

    private:
        juce::Label label;
        juce::ComboBox box;
        std::unique_ptr<APVTS::ComboBoxAttachment> attachment;
    };
}
