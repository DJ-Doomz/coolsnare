/*
  ==============================================================================

    SpectrumComponent.cpp
    Created: 20 Aug 2024 11:44:26am
    Author:  noone

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpectrumComponent.h"

void SpectrumComponent::paint (juce::Graphics& m)
{
    juce::Rectangle<float> shrunk = getLocalBounds().toFloat().expanded(2, 2).translated(0, 0);
    juce::Graphics g(buffer);
    buffer.multiplyAllAlphas(.4);
    g.drawImage(buffer, shrunk);
    // turn down the alpha for funnies
    g.fillAll(juce::Colour(0.f, 0.f, 0.f, .4f));

    auto lb = getLocalBounds();
    auto responseArea = lb;
    auto w = lb.getWidth();
    auto h = lb.getHeight();

    // draw background image
    //m.fillCheckerBoard(lb.toFloat(), h / 6, h / 6, juce::Colour::fromFloatRGBA(0.1, 0.1, 0.1, 1), juce::Colours::black);

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();

    drawNextFrameOfSpectrum(g);
    //g.setColour(Colour(hue, 1.f, 1.f, 1.f));
    //float thue = hue;
    /*
    g.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .6f));
    for (int i = 1; i < BandModAudioProcessor::scopeSize; ++i)
    {
        g.drawLine({ (float)jmap(i - 1, 0, BandModAudioProcessor::scopeSize - 1, 0, w),
                              jmap(scopeData[i - 1], 0.0f, 1.0f, (float)h, 0.0f),
                      (float)jmap(i,     0, BandModAudioProcessor::scopeSize - 1, 0, w),
                              jmap(scopeData[i],     0.0f, 1.0f, (float)h, 0.0f) }, 2);
    }
    */

    //double trash;
    //hue = modf(thue + .003, &trash);

    m.drawImageAt(buffer, 0, 0);
    repaint();
}

void SpectrumComponent::drawNextFrameOfSpectrum(juce::Graphics& g)
{
    juce::Rectangle<float> lb = getLocalBounds().toFloat();
    float bottom = lb.getBottom();
    float top = lb.getY();
    const auto factor = lb.getWidth() / 10.0f;

    const float smoothing = 0.7;
    // do smooth towards next scope data
    for (int i = 0; i < scopeSize; ++i)                         // [3]
    {
        scopeData[i] = smoothing * scopeData[i] + (1 - smoothing) * nextScopeData[i];
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
    spectrumPath.clear();
    spectrumPath.preallocateSpace(8 + scopeSize * 3);
    spectrumPath.startNewSubPath(lb.getX() + factor * indexToX(0, 20.f), juce::jmap(scopeData[0], bottom, top));
    for (int i = 0; i < scopeSize; ++i)                         // [3]
    {
        spectrumPath.lineTo(lb.getX() + factor * indexToX(float(i), 20.f), juce::jmap(scopeData[i], bottom, top));
    }
    g.setColour(juce::Colour::fromFloatRGBA(1.f, 1.f, 1.f, .6f));
    g.strokePath(spectrumPath.createPathWithRoundedCorners(5), juce::PathStrokeType(2.0));
}

void SpectrumComponent::resized()
{
    auto lb = getLocalBounds();
    auto w = lb.getWidth();
    auto h = lb.getHeight();

    buffer = buffer.rescaled(lb.getWidth(), lb.getHeight());
}
