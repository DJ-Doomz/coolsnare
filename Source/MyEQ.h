/*
  ==============================================================================

    MyEQ.h
    Created: 15 Aug 2024 1:23:40pm
    Author:  noone

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HigherOrderFilter.h"
#include "Globals.h"

#pragma once
// 3 band EQ that handles params and updates and such
class MyEQ
{
public:
    MyEQ(
        std::atomic<float> *in_hpFreq,
        std::atomic<float> *in_hpOrder,
        std::atomic<float> *in_hpRes,
        std::atomic<float> *in_lpFreq,
        std::atomic<float> *in_lpOrder,
        std::atomic<float> *in_lpRes,
        std::atomic<float> *in_peakFreq,
        std::atomic<float> *in_peakQ,
        std::atomic<float> *in_peakGain    
    );

    void prepare(juce::dsp::ProcessSpec ps);

    void update();

    float magnitude(float f);

    float process(float s);

private:
    HigherOrderFilter lp, hp;
    juce::dsp::IIR::Filter<float> peak;

    double sampleRate;

    std::atomic<float>
        * hpFreq,
        * hpOrder,
        * hpRes,
        * lpFreq,
        * lpOrder,
        * lpRes,
        * peakFreq,
        * peakQ,
        * peakGain;

    float 
        c_hpFreq,
        c_hpOrder,
        c_hpRes,
        c_lpFreq,
        c_lpOrder,
        c_lpRes,
        c_peakFreq,
        c_peakQ,
        c_peakGain;
    
    bool ready;
};