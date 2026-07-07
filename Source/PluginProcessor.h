#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/TapeDelayEngine.h"
#include "DSP/ModFilter.h"
#include "DSP/PitchShift.h"
#include "DSP/ReverbBank.h"

class TapeDubDelayAudioProcessor : public juce::AudioProcessor
{
public:
    TapeDubDelayAudioProcessor();
    ~TapeDubDelayAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Tape Dub Delay"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    TapeDelayEngine tapeDelay;
    ModFilter filter;
    GranularPitchShifter pitchShiftL, pitchShiftR;
    ReverbBank reverb;

    double currentBpm = 120.0;
    double autoPanPhase = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeDubDelayAudioProcessor)
};
