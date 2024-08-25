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
class ImpulseComponent  : public juce::Component, public juce::ChangeListener
{
public:
    ImpulseComponent()
        : thumbnailCache(5),
        thumbnail(512, formatManager, thumbnailCache)
    {
        addAndMakeVisible(type);
        addAndMakeVisible(mixSlider);

        formatManager.registerBasicFormats();

        thumbnail.addChangeListener(this);
    }

    ~ImpulseComponent() override
    {
    }

    void loadFile(const juce::String& s)
    {
        juce::File f(s);
        if (f.exists())
        {
            auto* reader = formatManager.createReaderFor(f);

            if (reader != nullptr)
            {
                auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                thumbnail.setSource(new juce::FileInputSource(f));                            // [7]
                readerSource.reset(newSource.release());
            }
        }
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)       thumbnailChanged();
    }

    void thumbnailChanged()
    {
        repaint();
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
                    juce::Justification::centred, true);
        lb = lb.removeFromTop(EQ_HEIGHT);
        lb.removeFromTop(SELECTION_HEIGHT);

        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded(g, lb.reduced(MARGIN));
        else
            paintIfFileLoaded(g, lb.reduced(MARGIN));
    }

    void paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour(juce::Colours::black);
        g.fillRect(thumbnailBounds);
        g.setColour(juce::Colours::darkgrey);
        g.drawRect(thumbnailBounds);
    }

    void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour(juce::Colours::black);
        g.fillRect(thumbnailBounds);

        g.setColour(juce::Colours::red);                               // [8]

        thumbnail.drawChannel(g,
            thumbnailBounds,
            0.,
            thumbnail.getTotalLength(),
            0,
            1.0f);
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        auto lb = getLocalBounds();
        auto header = lb.removeFromTop(HEADER_SPACE);
        juce::Rectangle<int> typeRect = lb.removeFromTop(EQ_HEIGHT);
        type.setBounds(typeRect.withSize(125, SELECTION_HEIGHT));
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
    //waveform drawing stuffs:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    const int SELECTION_HEIGHT = 20;
    SliderAndLabel mixSlider{ "MIX" };
    std::unique_ptr<SliderAttachment> mixAttachment;
    juce::ComboBox type;
    std::unique_ptr<ComboBoxAttachment> typeAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImpulseComponent)
};
