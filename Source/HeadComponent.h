/*
  ==============================================================================

    HeadComponent.h
    Created: 14 Aug 2024 11:44:27pm
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EQComponent.h"

//==============================================================================
/*
*/
class HeadComponent  : public juce::Component
{
public:
    HeadComponent(juce::AudioProcessorValueTreeState& a) :
        apvts(a),
        headEq(a, *a.getParameter("hpFreq"), *a.getParameter("hpOrder"), *a.getParameter("hpRes"),
            *a.getParameter("peakFreq"), *a.getParameter("peakGain"), *a.getParameter("peakQ"),
            *a.getParameter("lpFreq"), *a.getParameter("lpOrder"), *a.getParameter("lpRes"))
    {
        addAndMakeVisible(headEq);
    }

    ~HeadComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto lb = getLocalBounds();
        auto header = lb.removeFromTop(HEADER_SPACE);
        g.setColour (juce::Colours::white);
        g.drawText ("Head", header,
                    juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        auto lb = getLocalBounds();
        lb.removeFromTop(HEADER_SPACE);
        auto eqRect = lb.removeFromTop(EQ_HEIGHT);
        headEq.setBounds(eqRect);
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    EQComponent headEq;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeadComponent)
};
