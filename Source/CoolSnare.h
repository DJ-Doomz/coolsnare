/*
  ==============================================================================

    CoolSnare.h
    Created: 9 Aug 2024 4:55:04pm
    Author:  noone

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "HigherOrderFilter.h"
#include "CircularBuffer.h"
#include "MyEQ.h"

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
    float note_in_hertz;
    float noise_vol;
    float vel;
    CircularBuffer head;

    MyEQ headEQ, noiseEQ;

    juce::Random random;

    juce::AudioBuffer<float> synthBuffer,
        impulseBuffer;
    juce::AudioProcessorValueTreeState& apvts;

    // parameters
    std::atomic<float>
        * delay1,
        * FB1,
        * noiseMix,
        * noiseRelease,
        * headMix,
        * impulseMix,
        * impulseType,
        * accent;

    void init_params() {
        delay1 = apvts.getRawParameterValue("delay1");
        FB1 = apvts.getRawParameterValue("FB1");
        headMix = apvts.getRawParameterValue("headMix");
        noiseMix = apvts.getRawParameterValue("noiseMix");
        noiseRelease = apvts.getRawParameterValue( "noiseRelease");
        accent = apvts.getRawParameterValue("accent");
        impulseMix = apvts.getRawParameterValue("impulseMix");
        impulseType = apvts.getRawParameterValue("impulseType");
    }
    
    float smoothit(float x, float targetx, float smooth)
    {
        return smooth * x + (1 - smooth) * targetx;
    }

    float sampleRate;

    int minimumSubBlockSize = 32;
    bool subBlockSubdivisionIsStrict = false;
};