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
    headEq(a, *a.getParameter("hpFreq"), *a.getParameter("hpOrder"), *a.getParameter("hpRes"),
        *a.getParameter("peakFreq"), *a.getParameter("peakGain"), *a.getParameter("peakQ"),
        *a.getParameter("lpFreq"), *a.getParameter("lpOrder"), *a.getParameter("lpRes"))
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
}

void CoolsnareAudioProcessorEditor::resized()
{
    auto lb = getLocalBounds();
    headEq.setBounds(lb);
}
