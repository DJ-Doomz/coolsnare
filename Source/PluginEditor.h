/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ImpulseComponent.h"
#include "HeadComponent.h"
#include "NoiseComponent.h"

//==============================================================================
/**
*/
class CoolsnareAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CoolsnareAudioProcessorEditor (CoolsnareAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~CoolsnareAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CoolsnareAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    HeadComponent headComponent;
    ImpulseComponent impulseComponent;
    NoiseComponent noiseComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoolsnareAudioProcessorEditor)
};
