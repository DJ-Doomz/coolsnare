/*
  ==============================================================================

    SpectrumComponent.h
    Created: 20 Aug 2024 11:44:26am
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
/*
  ==============================================================================

    BandGraphComponent.h
    Created: 17 Jun 2024 11:06:42am
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class SpectrumComponent : public juce::Component, juce::Timer
{
public:
    SpectrumComponent(juce::AudioProcessor& p, int order, float* i_fifo, std::atomic<bool>& i_nextFFTBlockReady)
        :
        processor(p),
        fifo(i_fifo),
        nextFFTBlockReady(i_nextFFTBlockReady),
        fftOrder(order),
        forwardFFT(fftOrder),
        fftSize(1 << fftOrder),
        scopeSize(fftSize/2),
        window(fftSize, juce::dsp::WindowingFunction<float>::hann),
        scopeData{ 0 },
        nextScopeData{ 0 }
    {
        startTimerHz(120);

        // data allocation

        // maybe use vectors??

        fftData = new float[2 * fftSize];
        scopeData = new float[scopeSize];
        nextScopeData = new float[scopeSize];

        for (int i = 0; i < 2 * fftSize; i++)
        {
            fftData[i] = 0;
        }

        for (int i = 0; i < scopeSize; i++)
        {
            scopeData[i] = 0;
            nextScopeData[i] = 0;
        }

        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        setOpaque(true);
    }

    void timerCallback() override {
        repaint();
    }

    ~SpectrumComponent() override
    {
        delete[] fftData;
        delete[] scopeData;
        delete[] nextScopeData;
    }

    void paint(juce::Graphics& m) override;

    void resized() override;

private:
    juce::AudioProcessor& processor;
    const float mindB = -100.0f;
    const float maxdB = 0.0f;
    // data transfer stuff
    std::atomic<bool>& nextFFTBlockReady;
    float* fifo;

    // FFT stuff
    int fftOrder;
    int fftSize;
    int scopeSize;
    
    float* fftData;

    juce::dsp::FFT forwardFFT;                      // [4]
    juce::dsp::WindowingFunction<float> window;     // [5]
    float* scopeData;
    float* nextScopeData;
    juce::Path spectrumPath;

    // drawing stuff
    void drawNextFrameOfSpectrum(juce::Graphics& g);

    // helper functions (should probably make my own little library of these at one point)
    float smoothit(float x, float targetx, float smooth)
    {
        return smooth * x + (1 - smooth) * targetx;
    }

    float indexToX(float index, float minFreq) const
    {
        const auto freq = (processor.getSampleRate() * index) / forwardFFT.getSize();
        return (freq > 0.01f) ? std::log(freq / minFreq) / std::log(2.0f) : 0.0f;
    }

    float binToY(float bin, juce::Rectangle<float> bounds) const
    {
        const float infinity = -80.0f;
        return juce::jmap(juce::Decibels::gainToDecibels(bin, infinity) - juce::Decibels::gainToDecibels(fftSize),
            infinity, 0.0f, 0.f, 1.0f);
    }

    int xToIndex(int i)
    {
        float w = getLocalBounds().getWidth();
        float s = forwardFFT.getSize();
        float freq = juce::mapToLog10(i / w, 20.f, 20000.f);
        int ret = ((freq - 20.f) / (20000.f - 20.f)) * s / 2.f;
        return ret;
    }



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumComponent)
};

