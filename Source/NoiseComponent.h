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

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
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
        addAndMakeVisible(release);
        addAndMakeVisible(mix);
    }

    ~NoiseComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto lb = getLocalBounds();
        g.setColour(juce::Colours::grey);
        g.drawRect(lb);
        auto header = lb.removeFromTop(HEADER_SPACE);
        g.setColour(juce::Colours::white);
        g.setFont(HEADER_FONT_SIZE);
        g.drawText("NOISE", header,
            juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        auto lb = getLocalBounds();
        
        auto header = lb.removeFromTop(HEADER_SPACE);
        auto eqRect = lb.removeFromTop(EQ_HEIGHT);
        noiseEq.setBounds(eqRect.reduced(MARGIN));
        release.setBounds(lb.removeFromLeft(lb.getWidth() / 2).withSizeKeepingCentre(KNOB_SIZE, KNOB_SIZE));
        mix.setBounds(lb.withSizeKeepingCentre(KNOB_SIZE, KNOB_SIZE));
    }

    void addAttachment(juce::AudioProcessorValueTreeState& apvts)
    {
        releaseAttachment.reset(new SliderAttachment(apvts, "noiseRelease", release.getSlider()));
        mixAttachment.reset(new SliderAttachment(apvts, "noiseMix", mix.getSlider()));
    }

private:
    SliderAndLabel release{ "RELEASE" }, mix{ "MIX" };

    std::unique_ptr<SliderAttachment> releaseAttachment, mixAttachment;


    juce::AudioProcessorValueTreeState& apvts;
    EQComponent noiseEq;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoiseComponent)
};
