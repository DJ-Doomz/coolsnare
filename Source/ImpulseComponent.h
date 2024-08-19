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

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

//==============================================================================
/*
*/
class ImpulseComponent  : public juce::Component
{
public:
    ImpulseComponent()
    {
        addAndMakeVisible(type);
        addAndMakeVisible(mixSlider);
    }

    ~ImpulseComponent() override
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
        g.drawText ("IMPULSE", header,
                    juce::Justification::centred, true);   // draw some placeholder text
        
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        auto lb = getLocalBounds();
        auto header = lb.removeFromTop(HEADER_SPACE);
        juce::Rectangle<int> typeRect = lb.removeFromTop(EQ_HEIGHT);
        type.setBounds(typeRect.withSizeKeepingCentre(125, 20));
        mixSlider.setBounds(lb.withSizeKeepingCentre(KNOB_SIZE, KNOB_SIZE));
    }

    void addAttachment(juce::AudioProcessorValueTreeState& apvts)
    {
        auto* parameter = apvts.getParameter("impulseType");
        type.addItemList(parameter->getAllValueStrings(), 1);

        mixAttachment.reset(new SliderAttachment(apvts, "impulseMix", mixSlider.getSlider()));
        typeAttachment.reset(new ComboBoxAttachment(apvts, "impulseType", type));
    }

private:
    SliderAndLabel mixSlider{ "MIX" };
    std::unique_ptr<SliderAttachment> mixAttachment;
    juce::ComboBox type;
    std::unique_ptr<ComboBoxAttachment> typeAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImpulseComponent)
};
