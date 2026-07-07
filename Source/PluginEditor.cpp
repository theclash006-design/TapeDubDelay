#include "PluginEditor.h"

TapeDubDelayAudioProcessorEditor::TapeDubDelayAudioProcessorEditor (TapeDubDelayAudioProcessor& p)
    : juce::GenericAudioProcessorEditor (p)
{
    setSize (420, 640);
}
