/*
  ==============================================================================

    CoolSnare.cpp
    Created: 9 Aug 2024 4:55:04pm
    Author:  noone

  ==============================================================================
*/

#include "CoolSnare.h"


CoolSnare::CoolSnare(juce::AudioProcessorValueTreeState& v): apvts(v)
{
}

void CoolSnare::processBlock(juce::AudioBuffer<float>& outputAudio, juce::MidiBuffer& midiData)
{
    // must set the sample rate before using this!
    const int targetChannels = outputAudio.getNumChannels();
    int startSample = 0;
    int numSamples = outputAudio.getNumSamples();

    auto midiIterator = midiData.findNextSamplePosition(startSample);

    bool firstEvent = true;

    for (; numSamples > 0; ++midiIterator)
    {
        if (midiIterator == midiData.cend())
        {
            if (targetChannels > 0)
                renderVoices(outputAudio, startSample, numSamples);

            return;
        }

        const auto metadata = *midiIterator;
        const int samplesToNextMidiMessage = metadata.samplePosition - startSample;

        if (samplesToNextMidiMessage >= numSamples)
        {
            if (targetChannels > 0)
                renderVoices(outputAudio, startSample, numSamples);

            handleMidiEvent(metadata.getMessage());
            break;
        }

        if (samplesToNextMidiMessage < ((firstEvent && !subBlockSubdivisionIsStrict) ? 1 : minimumSubBlockSize))
        {
            handleMidiEvent(metadata.getMessage());
            continue;
        }

        firstEvent = false;

        if (targetChannels > 0)
            renderVoices(outputAudio, startSample, samplesToNextMidiMessage);

        handleMidiEvent(metadata.getMessage());
        startSample += samplesToNextMidiMessage;
        numSamples -= samplesToNextMidiMessage;
    }

    std::for_each(midiIterator,
        midiData.cend(),
        [&](const juce::MidiMessageMetadata& meta) { handleMidiEvent(meta.getMessage()); });
    
}


void CoolSnare::renderVoices(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();

    for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
    {
        // actual per-sample processing goes here
        updateEnvelopes();

        float o = 0.;

        for (int ch = 0; ch < synthBuffer.getNumChannels(); ch++)
        {
            synthBuffer.addSample(ch, s, o);
        }
    }

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
    }
}

void CoolSnare::prepareToPlay(double newRate)
{
}

void CoolSnare::updateEnvelopes()
{
}

void CoolSnare::handleMidiEvent(const juce::MidiMessage& m)
{
    if (m.isNoteOn())
    {
       
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {

    }
}
