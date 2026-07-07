#include "PluginEditor.h"
#include "UI/Controls.h"
#include "UI/SectionPanel.h"

using namespace DubUI;

namespace
{
    juce::Component* buildTapeDelayTab (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("TAPE DELAY");
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

    juce::Component* buildStereoTab (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("STEREO");
        panel->addControl (new Knob (apvts, "balance", "BALANCE"));
        panel->addControl (new Knob (apvts, "width", "WIDTH"));
        panel->addControl (new Toggle (apvts, "pingpong", "PINGPONG"), 110, 40);
        panel->addControl (new Knob (apvts, "drift", "DRIFT"));
        panel->addControl (new Knob (apvts, "autoPanInt", "AUTO PAN INT"));
        panel->addControl (new Choice (apvts, "autoPanRate", "AP RATE"), 96, 60);
        return panel;
    }

    juce::Component* buildFilterTab (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("FILTER");
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

    juce::Component* buildPitchTab (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("PITCH SHIFT");
        panel->addControl (new Toggle (apvts, "pitchOn", "ON/OFF"), 96, 40);
        panel->addControl (new Choice (apvts, "pitchAlgo", "ALGO"), 90, 60);
        panel->addControl (new Knob (apvts, "pitchSemi", "SEMI"));
        panel->addControl (new Knob (apvts, "pitchFine", "FINE"));
        panel->addControl (new Knob (apvts, "pitchGrain", "GRAIN"));
        panel->addControl (new Knob (apvts, "pitchMix", "MIX"));
        panel->addControl (new Toggle (apvts, "pitchPre", "PRE"), 90, 40);
        return panel;
    }

    juce::Component* buildReverbTab (juce::AudioProcessorValueTreeState& apvts)
    {
        auto* panel = new SectionPanel ("REVERB");
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
    titleLabel.setFont (juce::Font (20.0f, juce::Font::bold));

    addAndMakeVisible (tabs);
    tabs.addTab ("DELAY",  DubLookAndFeel::panelBg, buildTapeDelayTab (processorRef.apvts), true);
    tabs.addTab ("STEREO", DubLookAndFeel::panelBg, buildStereoTab (processorRef.apvts), true);
    tabs.addTab ("FILTER", DubLookAndFeel::panelBg, buildFilterTab (processorRef.apvts), true);
    tabs.addTab ("PITCH",  DubLookAndFeel::panelBg, buildPitchTab (processorRef.apvts), true);
    tabs.addTab ("REVERB", DubLookAndFeel::panelBg, buildReverbTab (processorRef.apvts), true);

    setResizable (true, true);
    setResizeLimits (620, 420, 1100, 800);
    setSize (760, 520);
}

TapeDubDelayAudioProcessorEditor::~TapeDubDelayAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void TapeDubDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (DubLookAndFeel::panelBg.darker (0.2f));
}

void TapeDubDelayAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    titleLabel.setBounds (area.removeFromTop (32).reduced (12, 0));
    tabs.setBounds (area);
}
