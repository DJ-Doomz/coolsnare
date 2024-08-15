/*
  ==============================================================================

    MyEQ.cpp
    Created: 15 Aug 2024 1:23:40pm
    Author:  noone

  ==============================================================================
*/

#include "MyEQ.h"

MyEQ::MyEQ(std::atomic<float>* in_hpFreq,
    std::atomic<float>* in_hpOrder,
    std::atomic<float>* in_hpRes,
    std::atomic<float>* in_lpFreq,
    std::atomic<float>* in_lpOrder,
    std::atomic<float>* in_lpRes,
    std::atomic<float>* in_peakFreq,
    std::atomic<float>* in_peakQ,
    std::atomic<float>* in_peakGain) :
    hpFreq    (in_hpFreq),
    hpOrder   (in_hpOrder),
    hpRes     (in_hpRes),
    lpFreq    (in_lpFreq),
    lpOrder   (in_lpOrder),
    lpRes     (in_lpRes),
    peakFreq  (in_peakFreq),
    peakQ     (in_peakQ),
    peakGain  (in_peakGain)
{
    sampleRate = 44100;
    ready = false;
    c_hpFreq     = -1;
    c_hpOrder    = -1;
    c_hpRes      = -1;
    c_lpFreq     = -1;
    c_lpOrder    = -1;
    c_lpRes      = -1;
    c_peakFreq   = 500;
    c_peakQ      = .707;
    c_peakGain   = 1;
}

void MyEQ::prepare(juce::dsp::ProcessSpec ps)
{
    lp.prepare(ps);
    peak.prepare(ps);
    hp.prepare(ps);

    lp.setType(HigherOrderFilter::LP);
    hp.setType(HigherOrderFilter::HP);

    sampleRate = ps.sampleRate;

    update();
}

// todo: de-ewify this
void MyEQ::update()
{
    // hp
    if (*hpFreq != c_hpFreq)
    {
        c_hpFreq = *hpFreq;
        hp.setCutoffFrequency(c_hpFreq);
    }
    if (*hpOrder != c_hpOrder)
    {
        c_hpOrder = *hpOrder;
        hp.setOrder(c_hpOrder);
    }
    if (*hpRes != c_hpRes)
    {
        c_hpRes = *hpRes;
        hp.setResonance(c_hpRes);
    }

    // lp
    if (*lpFreq != c_lpFreq)
    {
        c_lpFreq = *lpFreq;
        lp.setCutoffFrequency(c_lpFreq);
    }
    if (*lpOrder != c_lpOrder)
    {
        c_lpOrder = *lpOrder;
        lp.setOrder(c_lpOrder);
    }
    if (*lpRes != c_lpRes)
    {
        c_lpRes = *lpRes;
        lp.setResonance(c_lpRes);
    }

    // peak
    if (*peakFreq != c_peakFreq)
    {
        c_peakFreq = *peakFreq;
        peak.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, c_peakFreq, c_peakQ, juce::Decibels::decibelsToGain(c_peakGain));
    }
    if (*peakQ != c_peakQ)
    {
        c_peakQ = *peakQ;
        peak.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, c_peakFreq, c_peakQ, juce::Decibels::decibelsToGain(c_peakGain));
    }
    if (*peakGain != c_peakGain)
    {
        c_peakGain = *peakGain;
        peak.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, c_peakFreq, c_peakQ, juce::Decibels::decibelsToGain(c_peakGain));
    }
}

float MyEQ::magnitude(float f)
{
    float m = 1.0;
    m = lp.magnitude(f);
    m *= peak.coefficients.get()->getMagnitudeForFrequency(f, sampleRate);
    m *= hp.magnitude(f);

    return m;
}

float MyEQ::process(float s)
{
    return peak.processSample(lp.processSample(hp.processSample(s)));
}
