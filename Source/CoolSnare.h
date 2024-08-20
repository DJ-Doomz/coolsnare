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

    float* getHeadFFT()
    {
        return headFFT;
    };
    std::atomic<bool>& getHeadReady() {
        return headReady;
    };

    float* getNoiseFFT() {
        return noiseFFT;
    };
    std::atomic<bool>& getNoiseReady()
    {
        return noiseReady;
    };

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

    // spectrum stuff
    std::atomic<bool> headReady, noiseReady;
    const int fftSize = 1 << FFT_ORDER;
    int headFifoIndex = 0;
    int noiseFifoIndex = 0;
    float headFifo[1 << FFT_ORDER];
    float headFFT[1 << FFT_ORDER];
    float noiseFifo[1 << FFT_ORDER];
    float noiseFFT[1 << FFT_ORDER];

    void pushSampleIntoFifo(float* fifo, float* fftData, int& fifoIndex, std::atomic<bool>& nextFFTBlockReady, float sample)
    {
        // if the fifo contains enough data, set a flag to say
        // that the next frame should now be rendered..
        if (fifoIndex == fftSize)               // [11]
        {
            if (!nextFFTBlockReady)            // [12]
            {
                juce::zeromem(fftData, fftSize);
                memcpy(fftData, fifo, fftSize);
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;             // [12]
    }


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