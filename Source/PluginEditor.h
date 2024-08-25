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
#include "myLookAndFeel.h"

//==============================================================================
/**
*/
class CoolsnareAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::FileDragAndDropTarget
{
public:
    CoolsnareAudioProcessorEditor (CoolsnareAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~CoolsnareAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool loadFile(const juce::String& s);

    void filesDropped(const juce::StringArray& files, int x, int y) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;

    void fileDragEnter(const juce::StringArray& files, int x, int y) override;

    void fileDragMove(const juce::StringArray& files, int x, int y) override;

    void fileDragExit(const juce::StringArray& files) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CoolsnareAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    myLookAndFeel myLnF;

    HeadComponent headComponent;
    ImpulseComponent impulseComponent;
    NoiseComponent noiseComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoolsnareAudioProcessorEditor)
};
