/*
  ==============================================================================

    ImpulseComponent.h
    Created: 14 Aug 2024 11:43:59pm
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Globals.h"
#include "SliderAndLabel.h"

//==============================================================================
/*
*/
class ImpulseComponent  : public juce::Component
{
public:
    ImpulseComponent()
    {
    }

    ~ImpulseComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto lb = getLocalBounds();
        auto header = lb.removeFromTop(HEADER_SPACE);
        g.setColour (juce::Colours::white);
        g.drawText ("Impulse", header,
                    juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }

private:


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImpulseComponent)
};
