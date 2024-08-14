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
        sampleRate(44100){};

    ~HigherOrderFilter() {};

    void setType(FilterType t)
    {
        type = t;
        updateCoefficients();
    }

    void setCutoffFrequency(float f)
    {
        c_freq = f;
        updateCoefficients();
    }

    void setResonance(float r)
    {
        c_res = r;
        updateCoefficients();
    }

    void updateCoefficients()
    {
        for (int i = 0; i < MAX_ORDER; i++)
        {
            filters[i].coefficients =
                type == LP ? juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, c_freq, c_res) :
                juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, c_freq, c_res);
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
        float m = 1.;
        for (int i = 0; i < order; i++)
        {
            m *= filters[i].coefficients.get()->getMagnitudeForFrequency(frequency, sampleRate);
        }
        return m;
    }

private:
    juce::dsp::IIR::Filter<float> filters[MAX_ORDER];
    FilterType type;
    float c_freq;
    float c_res;
    double sampleRate;
    int order;
};