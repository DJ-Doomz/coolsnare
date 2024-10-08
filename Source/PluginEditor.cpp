/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CoolsnareAudioProcessorEditor::CoolsnareAudioProcessorEditor (CoolsnareAudioProcessor& p, juce::AudioProcessorValueTreeState& a)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(a), headComponent(a, p.cs), noiseComponent(a, p.cs)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 350);
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

bool CoolsnareAudioProcessorEditor::loadFile(const juce::String& s)
{
    bool ret = false;
    if (audioProcessor.cs.loadSample(s))
    {
        ret = true;
        impulseComponent.loadFile(s);
        apvts.getParameter("impulseType")->setValueNotifyingHost(1.0);
    }
    return ret;
}

void CoolsnareAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    if (loadFile(files[0]))
    {
        apvts.state.setProperty("loadedSample", files[0], nullptr);
    }
}

bool CoolsnareAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    return true;
}

void CoolsnareAudioProcessorEditor::fileDragEnter(const juce::StringArray& files, int x, int y)
{
}

void CoolsnareAudioProcessorEditor::fileDragMove(const juce::StringArray& files, int x, int y)
{
}

void CoolsnareAudioProcessorEditor::fileDragExit(const juce::StringArray& files)
{
}
