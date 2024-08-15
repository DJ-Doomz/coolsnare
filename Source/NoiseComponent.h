/*
  ==============================================================================

    NoiseComponent.h
    Created: 14 Aug 2024 11:44:39pm
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EQComponent.h"

//==============================================================================
/*
*/
class NoiseComponent  : public juce::Component
{
public:
    NoiseComponent(juce::AudioProcessorValueTreeState& a) :
        apvts(a),
        noiseEq(a, *a.getParameter("noisehpFreq"), *a.getParameter("noisehpOrder"), *a.getParameter("noisehpRes"),
            *a.getParameter("noisepeakFreq"), *a.getParameter("noisepeakGain"), *a.getParameter("noisepeakQ"),
            *a.getParameter("noiselpFreq"), *a.getParameter("noiselpOrder"), *a.getParameter("noiselpRes"))
    {
        addAndMakeVisible(noiseEq);
    }

    ~NoiseComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto lb = getLocalBounds();
        auto header = lb.removeFromTop(HEADER_SPACE);
        g.setColour(juce::Colours::white);
        g.drawText("Noise", header,
            juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        auto lb = getLocalBounds();
        auto header = lb.removeFromTop(HEADER_SPACE);
        auto eqRect = lb.removeFromTop(EQ_HEIGHT);
        noiseEq.setBounds(eqRect);
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    EQComponent noiseEq;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoiseComponent)
};
