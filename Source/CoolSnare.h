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

    juce::dsp::IIR::Filter<float> peak, noisepeak;

    HigherOrderFilter lp, hp;

    juce::Random random;

    juce::AudioBuffer<float> synthBuffer,
        impulseBuffer;
    juce::AudioProcessorValueTreeState& apvts;

    // todo: make this not stupid
    float c_hpFreq, c_lpFreq, c_order;

    // parameters
    std::atomic<float>
        * delay1,
        * FB1,
        * noiseMix,
        * noiseRelease,
        * headMix,
        * impulseMix,
        * impulseType,
        * hpFreq,
        * hpOrder,
        * hpRes,
        * lpFreq,
        * lpOrder,
        * lpRes,
        * peakFreq,
        * peakQ,
        * peakGain,
        * noisehpFreq,
        * noisehpOrder,
        * noisehpRes,
        * noiselpFreq,
        * noiselpOrder,
        * noiselpRes,
        * noisepeakFreq,
        * noisepeakQ,
        * noisepeakGain,
        * accent;

    void init_params() {
        delay1 = apvts.getRawParameterValue("delay1");
        FB1 = apvts.getRawParameterValue("FB1");
        headMix = apvts.getRawParameterValue("headMix");
        noiseMix = apvts.getRawParameterValue("noiseMix");
        noiseRelease = apvts.getRawParameterValue( "noiseRelease");
        noisehpFreq = apvts.getRawParameterValue(  "noisehpFreq");
        noisehpOrder = apvts.getRawParameterValue( "noisehpOrder");
        noisehpRes = apvts.getRawParameterValue(   "noisehpRes");
        noiselpFreq = apvts.getRawParameterValue(  "noiselpFreq");
        noiselpOrder = apvts.getRawParameterValue( "noiselpOrder");
        noiselpRes = apvts.getRawParameterValue(   "noiselpRes");
        noisepeakFreq = apvts.getRawParameterValue("noisepeakFreq");
        noisepeakQ = apvts.getRawParameterValue(   "noisepeakQ");
        noisepeakGain = apvts.getRawParameterValue("noisepeakGain");


        hpFreq = apvts.getRawParameterValue("hpFreq");
        hpOrder = apvts.getRawParameterValue("hpOrder");
        hpRes = apvts.getRawParameterValue("hpRes");
        lpFreq = apvts.getRawParameterValue("lpFreq");
        lpOrder = apvts.getRawParameterValue("lpOrder");
        lpRes = apvts.getRawParameterValue("lpRes");
        peakFreq = apvts.getRawParameterValue("peakFreq");
        peakQ = apvts.getRawParameterValue("peakQ");
        peakGain = apvts.getRawParameterValue("peakGain");


        accent = apvts.getRawParameterValue("accent");
        impulseMix = apvts.getRawParameterValue("impulseMix");
        impulseType = apvts.getRawParameterValue("impulseType");
    }
    
    float smoothit(float x, float targetx, float smooth)
    {
        return smooth * x + (1 - smooth) * targetx;
    }

    float sampleRate;

    MyEQ headEQ, noiseEQ;
    
    int minimumSubBlockSize = 32;
    bool subBlockSubdivisionIsStrict = false;
};