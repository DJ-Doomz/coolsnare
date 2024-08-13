/*
  ==============================================================================

    EQComponent.h
    Created: 12 Aug 2024 2:52:20pm
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "HigherOrderFilter.h"

//==============================================================================
static const float GRAPH_MIN = 20.f;
static const float GRAPH_MAX = 20000.f;

class EQNode : public juce::Component
{
public:
    enum NodeType {
        LP,
        PEAK,
        HP,
    };

    EQNode(NodeType t, juce::RangedAudioParameter& f, juce::RangedAudioParameter& g);


    void paint(juce::Graphics& g) override;
    /** @internal */
    void mouseDown(const juce::MouseEvent&) override;
    /** @internal */
    void mouseUp(const juce::MouseEvent&) override;
    /** @internal */
    void mouseDrag(const juce::MouseEvent&) override;
    /** @internal */

private:
    juce::RangedAudioParameter& freq;
    juce::RangedAudioParameter& gain;

    NodeType type;

    float screenx, screeny;
    bool dragging;
};


class EQComponent  : public juce::Component
{
public:
    EQComponent(juce::AudioProcessorValueTreeState&);
    ~EQComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    
private:
    juce::AudioProcessorValueTreeState& apvts;
    EQNode hp;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQComponent)
};




