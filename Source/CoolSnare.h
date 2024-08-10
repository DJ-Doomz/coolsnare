/*
  ==============================================================================

    CoolSnare.h
    Created: 9 Aug 2024 4:55:04pm
    Author:  noone

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "CircularBuffer.h"

class CoolSnare
{
public:
    CoolSnare(juce::AudioProcessorValueTreeState& v);

    void processBlock(juce::AudioBuffer<float>& outputAudio, juce::MidiBuffer& midiData);
    void renderVoices(juce::AudioBuffer<float>& outputBuffer,
        int startSample, int numSamples);

    void handleMidiEvent(const juce::MidiMessage&);

    void prepareToPlay(double newRate, int samplePerBlock);
private:
    float get_impulse();
    float get_noise();
    void do_resonance();
    void updateEnvelopes();
    void updateFilters();

    float impulse;
    float impulse_phase;
    float impulse_vol;
    float noise_vol;
    CircularBuffer head;

    juce::dsp::IIR::Filter<float> lp, peak, hp, noisepeak;

    juce::Random random;

    juce::AudioBuffer<float> synthBuffer,
        impulseBuffer;
    juce::AudioProcessorValueTreeState& apvts;

    // todo: make this not stupid
    float c_hpFreq, c_lpFreq;

    // parameters
    std::atomic<float>
        * delay1,
        * FB1,
        * headMix,
        * hpFreq,
        * lpFreq,
        * peakFreq,
        * peakQ,
        * peakGain;

    void init_params() {
        delay1 = apvts.getRawParameterValue("delay1");
        FB1 = apvts.getRawParameterValue("FB1");
        headMix = apvts.getRawParameterValue("headMix");

        hpFreq = apvts.getRawParameterValue("hpFreq");
        lpFreq = apvts.getRawParameterValue("lpFreq");
        peakFreq = apvts.getRawParameterValue("peakFreq");
        peakQ = apvts.getRawParameterValue("peakQ");
        peakGain = apvts.getRawParameterValue("peakGain");
    }
    
    float smoothit(float x, float targetx, float smooth)
    {
        return smooth * x + (1 - smooth) * targetx;
    }

    float sampleRate;
    
    int minimumSubBlockSize = 32;
    bool subBlockSubdivisionIsStrict = false;
};