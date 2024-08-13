/*
  ==============================================================================

    EQComponent.cpp
    Created: 12 Aug 2024 2:52:20pm
    Author:  noone

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EQComponent.h"

//==============================================================================
EQComponent::EQComponent(juce::AudioProcessorValueTreeState& a) :
    apvts(a),
    hp(EQNode::NodeType::HP, *a.getParameter("hpFreq"), *a.getParameter("hpFreq"))
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(hp);
}

EQComponent::~EQComponent()
{
}

void EQComponent::paint (juce::Graphics& g)
{
    
}

void EQComponent::resized()
{
    auto lb = getLocalBounds();
    hp.setBounds(lb);

}

EQNode::EQNode(NodeType t, juce::RangedAudioParameter& f, juce::RangedAudioParameter& g) :
    freq(f),
    gain(g),
    type(t)
{
    screenx = 0;
    screeny = 0;
    dragging = false;
}

void EQNode::paint(juce::Graphics& g)
{
    auto lb = getLocalBounds();
    auto w = lb.getWidth();
    auto h = lb.getHeight();
    float freqx = juce::jmap(freq.getValue(), GRAPH_MIN, GRAPH_MAX);
    freqx = juce::mapFromLog10(freqx, GRAPH_MIN, GRAPH_MAX);
    screenx = juce::jmap(freqx, 0.f, float(w));
    screeny = h / 2.;
    if(type == NodeType::PEAK)
        screeny = juce::jmap(gain.getValue(), float(h), 0.f);

    if (dragging)
        g.setColour(juce::Colours::rebeccapurple);
    else
        g.setColour(juce::Colours::white);

    g.fillEllipse(screenx, screeny, 2, 2);
}

void EQNode::mouseDown(const juce::MouseEvent& e)
{
    auto p = e.getPosition();
    // detect if the mouse was over this when clicked
    if (p.getDistanceFrom(juce::Point<int>(screenx, screeny)) < 6 && !dragging)
    {
        dragging = true;
        //freq.beginChangeGesture();
        //gain.beginChangeGesture();
        mouseDrag(e);
    }
}

void EQNode::mouseUp(const juce::MouseEvent& e)
{
    if(dragging)
    {
        //freq.endChangeGesture();
        //gain.endChangeGesture();
    }

    dragging = false;
    repaint();
}

void EQNode::mouseDrag(const juce::MouseEvent& e)
{
    if (dragging && e.eventComponent == this)
    {
        auto lb = getLocalBounds();
        auto w = lb.getWidth();
        auto h = lb.getHeight();
        auto p = e.getPosition();

        float s = juce::jmap(float(p.getX()), 0.f, float(w), 0.f, 1.f);
        float hertz = juce::mapToLog10(s, GRAPH_MIN, GRAPH_MAX);
        float newv = juce::jmap(hertz, GRAPH_MIN, GRAPH_MAX, 0.f, 1.f);
        
        freq.setValueNotifyingHost(newv);

        repaint();
    }
}
