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
using SVF implementation for actual filtering because supposedly it is faster and better for automation
IIR filter for calculating magnitudes (hopefully they're similar enough no one will notice)
*/
class HigherOrderFilter
{
public:
    enum FilterType {
        LP,
        PEAK,
        HP,
    };
    static const int MAX_ORDER = 4;
    HigherOrderFilter() :
        order(1),
        c_freq(5000),
        c_res(.7),
        type(LP),
        dirty(true),
        sampleRate(44100){};

    ~HigherOrderFilter() {};

    void setType(FilterType t)
    {
        type = t;
        dirty = true;
        for (int i = 0; i < MAX_ORDER; i++)
        {
            if (type == LP)
                filters[i].parameters.get()->type = juce::dsp::StateVariableFilter::StateVariableFilterType::lowPass;
            else if (type == HP)
                filters[i].parameters.get()->type = juce::dsp::StateVariableFilter::StateVariableFilterType::highPass;
        }
        updateCoefficients();
    }

    void setCutoffFrequency(float f)
    {
        c_freq = f;
        dirty = true;
        updateCoefficients();
    }

    void setResonance(float r)
    {
        c_res = r;
        dirty = true;
        updateCoefficients();
    }

    void updateCoefficients()
    {
        for (int i = 0; i < MAX_ORDER; i++)
        {
            filters[i].parameters.get()->setCutOffFrequency(sampleRate, c_freq, c_res);
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

    float magnitude(double frequency)
    {
        if (dirty)
        {
            if (type == LP)
                IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, c_freq, c_res);
            else if(type == HP)
                IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, c_freq, c_res);
            dirty = false;
        }
        float m = IIRfilter.coefficients.get()->getMagnitudeForFrequency(frequency, sampleRate);

        return pow(m, order);
    }


private:
    juce::dsp::StateVariableFilter::Filter<float> filters[MAX_ORDER];
    juce::dsp::IIR::Filter<float> IIRfilter;
    FilterType type;
    float c_freq;
    float c_res;
    double sampleRate;
    int order;
    bool dirty;
};