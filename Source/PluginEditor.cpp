/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CoolsnareAudioProcessorEditor::CoolsnareAudioProcessorEditor (CoolsnareAudioProcessor& p, juce::AudioProcessorValueTreeState& a)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(a), headComponent(a), noiseComponent(a)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (670, 370);
    addAndMakeVisible(headComponent);
    addAndMakeVisible(impulseComponent);
    addAndMakeVisible(noiseComponent);

    impulseComponent.addAttachment(apvts);
    headComponent.addAttachment(apvts);
    noiseComponent.addAttachment(apvts);

    juce::LookAndFeel::setDefaultLookAndFeel(&myLnF);
}

CoolsnareAudioProcessorEditor::~CoolsnareAudioProcessorEditor()
{
}

//==============================================================================
void CoolsnareAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CoolsnareAudioProcessorEditor::resized()
{
    auto lb = getLocalBounds();
    const int impulseWidth = 150;
    auto impulseRect = lb.removeFromLeft(impulseWidth);
    impulseComponent.setBounds(impulseRect.reduced(MARGIN, MARGIN));
    headComponent.setBounds(lb.removeFromLeft(lb.getWidth() / 2.).reduced(MARGIN, MARGIN));
    noiseComponent.setBounds(lb.reduced(MARGIN, MARGIN));
}
