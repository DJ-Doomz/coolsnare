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
#include "CoolSnare.h"
#include "SpectrumComponent.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

//==============================================================================
/*
*/
class HeadComponent  : public juce::Component
{
public:
    HeadComponent(juce::AudioProcessorValueTreeState& a, CoolSnare& cs) :
        apvts(a),
        headEq(a, *a.getParameter("hpFreq"), *a.getParameter("hpOrder"), *a.getParameter("hpRes"),
            *a.getParameter("peakFreq"), *a.getParameter("peakGain"), *a.getParameter("peakQ"),
            *a.getParameter("lpFreq"), *a.getParameter("lpOrder"), *a.getParameter("lpRes")),
        headSpectrum(a.processor, FFT_ORDER, cs.getHeadFFT(), cs.getHeadReady())
    {
        addAndMakeVisible(headSpectrum);
        addAndMakeVisible(headEq);
        addAndMakeVisible(delay);
        addAndMakeVisible(mix);
        addAndMakeVisible(feedback);

        feedback.getSlider().getProperties().set("bipolar", true);
    }

    ~HeadComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto lb = getLocalBounds();
        g.setColour(juce::Colours::grey);
        g.drawRect(lb);
        auto header = lb.removeFromTop(HEADER_SPACE);
        g.setColour (juce::Colours::white);
        g.setFont(HEADER_FONT_SIZE);
        g.drawText ("HEAD", header,
                    juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        auto lb = getLocalBounds();
        lb.removeFromTop(HEADER_SPACE);
        auto eqRect = lb.removeFromTop(EQ_HEIGHT);
        headSpectrum.setBounds(eqRect.reduced(MARGIN));
        headEq.setBounds(eqRect.reduced(MARGIN));
        delay.setBounds(lb.removeFromLeft(lb.getWidth() / 3.).withSizeKeepingCentre(KNOB_SIZE, KNOB_SIZE));
        feedback.setBounds(lb.removeFromLeft(lb.getWidth() / 2.).withSizeKeepingCentre(KNOB_SIZE, KNOB_SIZE));
        mix.setBounds(lb.withSizeKeepingCentre(KNOB_SIZE, KNOB_SIZE));
    }

    void addAttachment(juce::AudioProcessorValueTreeState& apvts)
    {
        delayAttachment.reset(new SliderAttachment(apvts, "delay1", delay.getSlider()));
        feedbackAttachment.reset(new SliderAttachment(apvts, "FB1", feedback.getSlider()));
        mixAttachment.reset(new SliderAttachment(apvts, "headMix", mix.getSlider()));
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    SpectrumComponent headSpectrum;
    EQComponent headEq;


    SliderAndLabel delay{ "DELAY" }, feedback{ "FEEDBACK" }, mix{ "MIX" };
    std::unique_ptr<SliderAttachment> delayAttachment, feedbackAttachment, mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeadComponent)
};
