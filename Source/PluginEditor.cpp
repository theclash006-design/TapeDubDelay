#include "PluginEditor.h"
#include "UI/Controls.h"

using namespace DubUI;

namespace
{
    SectionPanel* buildTapeDelayPanel (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("TAPE DELAY", 1);
        panel->addControl (new Toggle (apvts, "delayOn", "ON/OFF"), 96, 40);
        panel->addControl (new Choice (apvts, "noteDiv", "NOTE"), 96, 60);
        panel->addControl (new Knob (apvts, "groove", "GROOVE"));
        panel->addControl (new Knob (apvts, "trim", "TRIM"));
        panel->addControl (new Toggle (apvts, "x2", "X2"), 90, 40);
        panel->addControl (new Knob (apvts, "feedback", "FEEDBACK"));
        panel->addControl (new Knob (apvts, "directMix", "DIRECT MIX"));
        panel->addControl (new Knob (apvts, "highCut", "HIGH CUT"));
        panel->addControl (new Knob (apvts, "lowCut", "LOW CUT"));
        panel->addControl (new Toggle (apvts, "freeze", "FREEZE"), 90, 40);
        panel->addControl (new Knob (apvts, "smooth", "SMOOTH"));
        panel->addControl (new Knob (apvts, "flutterInt", "FLUTTER INT"));
        panel->addControl (new Knob (apvts, "flutterRate", "FLUTTER RATE"));
        panel->addControl (new Knob (apvts, "wowInt", "WOW INT"));
        panel->addControl (new Knob (apvts, "wowRate", "WOW RATE"));
        return panel;
    }

    SectionPanel* buildStereoPanel (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("STEREO", 2);
        panel->addControl (new Knob (apvts, "balance", "BALANCE"));
        panel->addControl (new Knob (apvts, "width", "WIDTH"));
        panel->addControl (new Toggle (apvts, "pingpong", "PINGPONG"), 110, 40);
        panel->addControl (new Knob (apvts, "drift", "DRIFT"));
        panel->addControl (new Knob (apvts, "autoPanInt", "AUTO PAN INT"));
        panel->addControl (new Choice (apvts, "autoPanRate", "AP RATE"), 96, 60);
        return panel;
    }

    SectionPanel* buildFilterPanel (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("FILTER", 3);
        panel->addControl (new Toggle (apvts, "filterOn", "ON/OFF"), 96, 40);
        panel->addControl (new Choice (apvts, "filterType", "TYPE"), 96, 60);
        panel->addControl (new Knob (apvts, "cutoff", "CUTOFF"));
        panel->addControl (new Knob (apvts, "resonance", "RESONANCE"));
        panel->addControl (new Toggle (apvts, "filterLfoOn", "LFO ON"), 96, 40);
        panel->addControl (new Knob (apvts, "filterLfoInt", "LFO INT"));
        panel->addControl (new Choice (apvts, "filterLfoRate", "LFO RATE"), 96, 60);
        panel->addControl (new Toggle (apvts, "filterPre", "PRE"), 90, 40);
        return panel;
    }

    SectionPanel* buildPitchPanel (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("PITCH SHIFT", 4);
        panel->addControl (new Toggle (apvts, "pitchOn", "ON/OFF"), 96, 40);
        panel->addControl (new Choice (apvts, "pitchAlgo", "ALGO"), 90, 60);
        panel->addControl (new Knob (apvts, "pitchSemi", "SEMI"));
        panel->addControl (new Knob (apvts, "pitchFine", "FINE"));
        panel->addControl (new Knob (apvts, "pitchGrain", "GRAIN"));
        panel->addControl (new Knob (apvts, "pitchMix", "MIX"));
        panel->addControl (new Toggle (apvts, "pitchPre", "PRE"), 90, 40);
        return panel;
    }

    SectionPanel* buildReverbPanel (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("REVERB", 5);
        panel->addControl (new Toggle (apvts, "reverbOn", "ON/OFF"), 96, 40);
        panel->addControl (new Knob (apvts, "reverbSizeA", "SIZE A"));
        panel->addControl (new Knob (apvts, "reverbDampA", "DAMP A"));
        panel->addControl (new Knob (apvts, "reverbSizeB", "SIZE B"));
        panel->addControl (new Knob (apvts, "reverbDampB", "DAMP B"));
        panel->addControl (new Knob (apvts, "reverbMixAB", "MIX A/B"));
        panel->addControl (new Knob (apvts, "reverbWidth", "WIDTH"));
        panel->addControl (new Knob (apvts, "reverbMix", "MIX"));
        panel->addControl (new Toggle (apvts, "reverbPre", "PRE"), 90, 40);
        return panel;
    }
}

TapeDubDelayAudioProcessorEditor::TapeDubDelayAudioProcessorEditor (TapeDubDelayAudioProcessor& p)
    : juce::AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (titleLabel);
    titleLabel.setText ("TAPE DUB DELAY", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setColour (juce::Label::textColourId, DubLookAndFeel::accent);
    titleLabel.setFont (juce::Font (23.0f, juce::Font::bold));

    addAndMakeVisible (taglineLabel);
    taglineLabel.setText ("DUB TAPE ECHO SYSTEM", juce::dontSendNotification);
    taglineLabel.setJustificationType (juce::Justification::centredLeft);
    taglineLabel.setColour (juce::Label::textColourId, DubLookAndFeel::accent3);
    taglineLabel.setFont (juce::Font (12.5f, juce::Font::bold));

    panelStack.addSection (buildTapeDelayPanel (processorRef.apvts));
    panelStack.addSection (buildStereoPanel (processorRef.apvts));
    panelStack.addSection (buildFilterPanel (processorRef.apvts));
    panelStack.addSection (buildPitchPanel (processorRef.apvts));
    panelStack.addSection (buildReverbPanel (processorRef.apvts));

    viewport.setViewedComponent (&panelStack, false);
    viewport.setScrollBarsShown (true, false);
    addAndMakeVisible (viewport);

    setResizable (true, true);
    setResizeLimits (620, 460, 1200, 900);
    setSize (860, 640);
}

TapeDubDelayAudioProcessorEditor::~TapeDubDelayAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void TapeDubDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (DubLookAndFeel::panelBg);

    // rasta red/gold/green triple-rule under the header, echoing each
    // section panel's divider
    auto width = getWidth();
    auto third = width / 3;
    g.setColour (DubLookAndFeel::accent);
    g.fillRect (juce::Rectangle<int> (0, 62, third, 3));
    g.setColour (DubLookAndFeel::accent2);
    g.fillRect (juce::Rectangle<int> (third, 62, third, 3));
    g.setColour (DubLookAndFeel::accent3);
    g.fillRect (juce::Rectangle<int> (third * 2, 62, width - third * 2, 3));
}

void TapeDubDelayAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto header = area.removeFromTop (62).reduced (16, 4);
    titleLabel.setBounds (header.removeFromTop (30));
    taglineLabel.setBounds (header);

    area.removeFromTop (9); // room for the double-rule drawn in paint()

    viewport.setBounds (area.reduced (4));
    panelStack.setSize (viewport.getWidth() - 10, panelStack.getHeight());
}
