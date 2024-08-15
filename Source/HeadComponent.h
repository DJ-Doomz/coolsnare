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

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

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
        addAndMakeVisible(delay);
        addAndMakeVisible(mix);
        addAndMakeVisible(feedback);
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
        delay.setBounds(lb.removeFromLeft(lb.getWidth() / 3.));
        feedback.setBounds(lb.removeFromLeft(lb.getWidth() / 2.));
        mix.setBounds(lb);
    }

    void addAttachment(juce::AudioProcessorValueTreeState& apvts)
    {
        delayAttachment.reset(new SliderAttachment(apvts, "delay1", delay.getSlider()));
        feedbackAttachment.reset(new SliderAttachment(apvts, "FB1", feedback.getSlider()));
        mixAttachment.reset(new SliderAttachment(apvts, "headMix", mix.getSlider()));
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    EQComponent headEq;

    SliderAndLabel delay{ "Delay" }, feedback{ "Feedback" }, mix{ "Mix" };
    std::unique_ptr<SliderAttachment> delayAttachment, feedbackAttachment, mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeadComponent)
};
