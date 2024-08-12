/*
  ==============================================================================

    HigherOrderFilter.h
    Created: 11 Aug 2024 7:06:44pm
    Author:  noone

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
/*
I'm sure this is not the correct way to do this but whatever it sounds cool
*/
class HigherOrderFilter
{
public:
    static const int MAX_ORDER = 4;
    HigherOrderFilter() :
        order(1),
        sampleRate(44100){};

    ~HigherOrderFilter() {};

    void setType(juce::dsp::StateVariableFilter::StateVariableFilterType t)
    {
        for (int i = 0; i < MAX_ORDER; i++)
        {
            filters[i].parameters.get()->type = t;
        }
    }

    void setCutoffFrequency(float f)
    {
        for (int i = 0; i < MAX_ORDER; i++)
        {
            filters[i].parameters.get()->setCutOffFrequency(sampleRate, f);
        }
    }

    void prepare(const juce::dsp::ProcessSpec& ps)
    {
        sampleRate = ps.sampleRate;
        for (int i = 0; i < MAX_ORDER; i++)
        {
            filters[i].prepare(ps);
        }
    }

    float processSample(float s)
    {
        for (int i = 0; i < order; i++)
        {
            s = filters[i].processSample(s);
        }
        return s;
    }

    void setOrder(int o)
    {
        if (o != order)
        {
            order = o;
            for (int i = 0; i < order; i++)
            {
                filters[i].reset();
            }
        }
    }

private:
    juce::dsp::StateVariableFilter::Filter<float> filters[MAX_ORDER];
    double sampleRate;
    int order;
};