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

    void prepareToPlay(double newRate);
private:
    float impulse();
    void do_resonance();
    void updateEnvelopes();

    CircularBuffer head1, head2;

    juce::Random random;

    juce::AudioBuffer<float> synthBuffer;
    juce::AudioProcessorValueTreeState& apvts;


    // parameters
    std::atomic<float>
        * head1Delay,
        * head1FB,
        * head1Mix,
        * head2Delay,
        * head2FB,
        * head2Mix;

    void init_params() {
        head1Delay = apvts.getRawParameterValue("head1Delay");
        head1FB = apvts.getRawParameterValue("head1FB");
        head1Mix = apvts.getRawParameterValue("head1Mix");
        head2Delay = apvts.getRawParameterValue("head2Delay");
        head2FB = apvts.getRawParameterValue("head2FB");
        head2Mix = apvts.getRawParameterValue("head2Mix");
    }
    
    float smoothit(float x, float targetx, float smooth)
    {
        return smooth * x + (1 - smooth) * targetx;
    }

    
    int minimumSubBlockSize = 32;
    bool subBlockSubdivisionIsStrict = false;
};