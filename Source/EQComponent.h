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
#include "Globals.h"

//==============================================================================


class EQNode : public juce::Component
{
public:
    enum NodeType {
        LP,
        PEAK,
        HP,
    };

    EQNode(NodeType t, juce::RangedAudioParameter& f, juce::RangedAudioParameter& g, juce::RangedAudioParameter& inq);


    void paint(juce::Graphics& g) override;
    /** @internal */
    void mouseDown(const juce::MouseEvent&) override;
    /** @internal */
    void mouseUp(const juce::MouseEvent&) override;
    /** @internal */
    void mouseDrag(const juce::MouseEvent&) override;

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
    
    //void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

    float screenx, screeny;
    bool dragging;
    juce::RangedAudioParameter& freq;
    juce::RangedAudioParameter& q;
    juce::RangedAudioParameter& gainOrOrder; // gain if peak filter, order if hp/lp

private:
    
    NodeType type;
};


class EQComponent  : public juce::Component
{
public:
    EQComponent(juce::AudioProcessorValueTreeState&,
        juce::RangedAudioParameter& hpf, juce::RangedAudioParameter& hpr, juce::RangedAudioParameter& hpq,
        juce::RangedAudioParameter& pf, juce::RangedAudioParameter& pr, juce::RangedAudioParameter& pq, 
        juce::RangedAudioParameter& lpf, juce::RangedAudioParameter& lpr, juce::RangedAudioParameter& lpq);
    ~EQComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /** @internal */
    void mouseDown(const juce::MouseEvent&) override;
    /** @internal */
    void mouseUp(const juce::MouseEvent&) override;
    /** @internal */
    void mouseDrag(const juce::MouseEvent&) override;

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
private:
    void updateFilters();

    juce::AudioProcessorValueTreeState& apvts;
    EQNode hp, peak, lp;
    // eq component has its own copies of the filters for drawing
    HigherOrderFilter f_lp, f_hp;
    juce::dsp::IIR::Filter<float> f_peak;
    float sampleRate;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQComponent)
};




