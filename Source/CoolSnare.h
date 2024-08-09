/*
  ==============================================================================

    CoolSnare.h
    Created: 9 Aug 2024 4:55:04pm
    Author:  noone

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

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
    juce::AudioBuffer<float> synthBuffer;
    juce::AudioProcessorValueTreeState& apvts;
    void updateEnvelopes();
    float smoothit(float x, float targetx, float smooth)
    {
        return smooth * x + (1 - smooth) * targetx;
    }
};