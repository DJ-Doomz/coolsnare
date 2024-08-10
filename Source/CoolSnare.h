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
    void do_resonance();
    void updateEnvelopes();
    void updateFilters();

    float impulse;
    float impulse_vol;
    CircularBuffer head1, head2;

    juce::dsp::IIR::Filter<float> lp1, peak1, hp1, lp2, peak2, hp2;

    juce::Random random;

    juce::AudioBuffer<float> synthBuffer;
    juce::AudioProcessorValueTreeState& apvts;

    // todo: make this not stupid
    float c_hp1Freq, c_hp2Freq, c_lp1Freq, c_lp2Freq;

    // parameters
    std::atomic<float>
        * head1Delay,
        * head1FB,
        * head1Mix,
        * hp1Freq,
        * lp1Freq,
        * peak1Freq,
        * peak1Q,
        * peak1Gain,
        * head2Delay,
        * head2FB,
        * head2Mix,
        * hp2Freq,
        * lp2Freq,
        * peak2Freq,
        * peak2Q,
        * peak2Gain,
        * depth,
        * sympathy;

    void init_params() {
        head1Delay = apvts.getRawParameterValue("head1Delay");
        head1FB = apvts.getRawParameterValue("head1FB");
        head1Mix = apvts.getRawParameterValue("head1Mix");
        hp1Freq = apvts.getRawParameterValue("hp1Freq");
        lp1Freq = apvts.getRawParameterValue("lp1Freq");

        head2Delay = apvts.getRawParameterValue("head2Delay");
        head2FB = apvts.getRawParameterValue("head2FB");
        head2Mix = apvts.getRawParameterValue("head2Mix");
        hp2Freq = apvts.getRawParameterValue("hp2Freq");
        lp2Freq = apvts.getRawParameterValue("lp2Freq");

        depth = apvts.getRawParameterValue("depth");
        sympathy = apvts.getRawParameterValue("sympathy");
    }
    
    float smoothit(float x, float targetx, float smooth)
    {
        return smooth * x + (1 - smooth) * targetx;
    }

    float sampleRate;
    
    int minimumSubBlockSize = 32;
    bool subBlockSubdivisionIsStrict = false;
};