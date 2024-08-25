/*
  ==============================================================================

    SpectrumComponent.cpp
    Created: 20 Aug 2024 11:44:26am
    Author:  noone

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpectrumComponent.h"

void SpectrumComponent::paint (juce::Graphics& g)
{
    // turn down the alpha for funnies
    g.fillAll(juce::Colour(0.f, 0.f, 0.f, 1.f));

    drawNextFrameOfSpectrum(g);
    repaint();
}

void SpectrumComponent::drawNextFrameOfSpectrum(juce::Graphics& g)
{
    juce::Rectangle<float> lb = getLocalBounds().toFloat();
    float bottom = lb.getBottom();
    float top = lb.getY();
    float w = lb.getWidth();
    const auto factor = lb.getWidth() / 10.0f;

    const float smoothing = 0.96;
    // do smooth towards next scope data
    for (int i = 0; i < scopeSize; ++i)                         // [3]
    {
        if (nextScopeData[i] >= scopeData[i])
        {
            scopeData[i] = nextScopeData[i];
        }
        else
        {
            scopeData[i] = smoothing * scopeData[i] + (1 - smoothing) * nextScopeData[i];
        }
    }

    // first apply a windowing function to our data
    // make a local copy of the data
    if (nextFFTBlockReady)
    {
        juce::zeromem(fftData, 2*fftSize*sizeof(fftData[0]) );
        memcpy(fftData, fifo, fftSize * sizeof(fftData[0]));

        nextFFTBlockReady = false;

        window.multiplyWithWindowingTable(fftData, fftSize);       // [1]

        // then render our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform(fftData);  // [2]

        //TODO: redo this in a not-stupid way
        for (int i = 0; i < scopeSize; ++i)                         // [3]
        {
            nextScopeData[i] = binToY(fftData[i], lb);
        }
    }

    // actual drawing stuff

    // only drawing every other screen pixel
    spectrumPath.clear();
    spectrumPath.preallocateSpace(8 + w * 3);
    spectrumPath.startNewSubPath(lb.getX(), juce::jmap(scopeData[0], bottom, top));
    for (int i = 0; i < w; i+=2)
    {
        float x = lb.getX() + i;
        float y = juce::jmap(scopeData[xToIndex(i)], bottom, top);
        spectrumPath.lineTo(x, y);
    }
    g.setColour(juce::Colour::fromFloatRGBA(1.f, 1.f, 1.f, .6f));
    g.strokePath(spectrumPath, juce::PathStrokeType(2.0));
}

void SpectrumComponent::resized()
{
}
