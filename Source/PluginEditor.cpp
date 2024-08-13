/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CoolsnareAudioProcessorEditor::CoolsnareAudioProcessorEditor (CoolsnareAudioProcessor& p, juce::AudioProcessorValueTreeState& a)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(a),
    headEq(a)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible(headEq);
}

CoolsnareAudioProcessorEditor::~CoolsnareAudioProcessorEditor()
{
}

//==============================================================================
void CoolsnareAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CoolsnareAudioProcessorEditor::resized()
{
    auto lb = getLocalBounds();
    headEq.setBounds(lb);
}
