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
    sampleRate = 44100;
    impulse = 0;
    impulse_vol = 0;
    init_params();
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


void CoolSnare::renderVoices(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();
    updateFilters();

    for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
    {
        // actual per-sample processing goes here
        updateEnvelopes();
        impulse = get_impulse();
        do_resonance();
        float o = impulse + *head1Mix * head1.get(0) + *head2Mix * head2.get(0);
        o = juce::jlimit(-1.f, 1.f, o);

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

void CoolSnare::prepareToPlay(double newRate, int samplePerBlock)
{
    sampleRate = newRate;
    juce::dsp::ProcessSpec spec_mono;
    spec_mono.maximumBlockSize = samplePerBlock;
    spec_mono.numChannels = 1;
    spec_mono.sampleRate = sampleRate;
    lp1.prepare(spec_mono);
    peak1.prepare(spec_mono);
    hp1.prepare(spec_mono);
    lp2.prepare(spec_mono);
    peak2.prepare(spec_mono);
    hp2.prepare(spec_mono);
}

void CoolSnare::updateEnvelopes()
{
    impulse_vol = smoothit(impulse_vol, 0., 0.9);
}

float CoolSnare::get_impulse()
{
    float n = 2. * random.nextFloat() - 1.;
    return n * impulse_vol;
}

void CoolSnare::do_resonance()
{
    float h1s = 0., h2s = 0.;
    /*
    h1s += impulse + *head1FB * lp1.processSample(hp1.processSample(head1.get(*head1Delay))) + *sympathy * head2.get(*depth);
    h2s += *head2FB * lp1.processSample(hp1.processSample(head2.get(*head2Delay))) + *sympathy * head1.get(*depth);
    */
    h1s += impulse + *head1FB * head1.get(*head1Delay) + *sympathy * head2.get(*depth);
    h2s += *head2FB * head2.get(*head2Delay) + *sympathy * head1.get(*depth);
    if (isnan(h1s) || isnan(h2s) || isinf(h1s) || isinf(h2s))
    {
        h1s = 0, h2s = 0;
        hp1.reset();
        hp2.reset();
        lp1.reset();
        lp2.reset();
    }

    h1s = juce::jlimit(-1.f, 1.f, h1s);
    h2s = juce::jlimit(-1.f, 1.f, h2s);

    h1s = lp1.processSample(hp1.processSample(h1s));
    h2s = lp2.processSample(hp2.processSample(h2s));

    head1.put(h1s);
    head2.put(h2s);
}

void CoolSnare::updateFilters()
{

    if (*hp1Freq != c_hp1Freq)
    {
        c_hp1Freq = *hp1Freq;
        float f = juce::jlimit(1., sampleRate / 2. - 1., *hp1Freq * sampleRate / 2.);
        hp1.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, f);
    }

    if (*hp2Freq != c_hp2Freq)
    {
        c_hp2Freq = *hp2Freq;
        float f = juce::jlimit(1., sampleRate / 2. - 1., *hp2Freq * sampleRate / 2.);
        hp2.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, f);
    }

    if (*lp1Freq != c_lp1Freq)
    {
        c_lp1Freq = *lp1Freq;
        float f = juce::jlimit(1., sampleRate / 2. - 1., *lp1Freq * sampleRate / 2.);
        lp1.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, f);
    }

    if (*lp2Freq != c_lp2Freq)
    {
        c_lp2Freq = *lp2Freq;
        float f = juce::jlimit(1., sampleRate / 2. - 1., *lp2Freq * sampleRate / 2.);
        lp2.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, f);
    }
}

void CoolSnare::handleMidiEvent(const juce::MidiMessage& m)
{
    if (m.isNoteOn())
    {
        impulse_vol = m.getFloatVelocity();
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {

    }
}
