/*
  ==============================================================================

    NoiseComponent.h
    Created: 14 Aug 2024 11:44:39pm
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class NoiseComponent  : public juce::Component
{
public:
    NoiseComponent(juce::AudioProcessorValueTreeState& a) :
        apvts(a)
    {
        
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
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoiseComponent)
};
