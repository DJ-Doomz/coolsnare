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
    impulse_phase = 0;
    noise_vol = 0;
    note_in_hertz = 440;
    vel = 0;
    init_params();
    c_hpFreq = c_lpFreq = -1;
    updateFilters();

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatReader> reader(wavFormat.createReaderFor(new juce::MemoryInputStream(BinaryData::Hitbeads2_wav, BinaryData::Hitbeads2_wavSize, false), true));
    if (reader != nullptr)
    {
        impulseBuffer.setSize(1, reader->lengthInSamples);
        reader->read(&impulseBuffer, 0, reader->lengthInSamples, 0, false, true);
    }
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
    synthBuffer.setSize(1, numSamples, false, false, true);
    synthBuffer.clear();
    updateFilters();

    for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
    {
        // actual per-sample processing goes here
        updateEnvelopes();
        impulse = get_impulse();
        do_resonance();
        float o = *impulseMix * impulse + *headMix * head.get(0) + *noiseMix * get_noise();
        if (isnan(o) || isinf(o)) 
            o = 0.;

        o = juce::jlimit(-1.f, 1.f, o);

        for (int ch = 0; ch < synthBuffer.getNumChannels(); ch++)
        {
            synthBuffer.addSample(ch, s, o);
        }
    }

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, 0, 0, numSamples);
    }
}

void CoolSnare::prepareToPlay(double newRate, int samplePerBlock)
{
    sampleRate = newRate;
    juce::dsp::ProcessSpec spec_mono;
    spec_mono.maximumBlockSize = samplePerBlock;
    spec_mono.numChannels = 1;
    spec_mono.sampleRate = sampleRate;
    lp.prepare(spec_mono);
    peak.prepare(spec_mono);
    hp.prepare(spec_mono);
    noisepeak.prepare(spec_mono);

    noisepeak.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3000, 2., juce::Decibels::decibelsToGain(5));

    c_hpFreq = c_lpFreq = -1;
    updateFilters();
}

void CoolSnare::updateEnvelopes()
{
    impulse_vol = smoothit(impulse_vol, 0., 0.999);

    float nr = juce::jmap(noiseRelease->load(), .999f, .99999f);
    noise_vol = smoothit(noise_vol, 0., nr);
}

float CoolSnare::get_impulse()
{
    float s = 0;
    
    int it = floor(*impulseType);
    switch (it)
    {
    case 0:
        if (impulse_phase < impulseBuffer.getNumSamples())
        {
            s = impulseBuffer.getSample(0, impulse_phase);
            impulse_phase += (note_in_hertz * 44100.f / 440.f) / sampleRate;
        }
        break;
    case 1:
        s = 2. * random.nextFloat() - 1.;
        break;
    case 2:
        s = impulse_phase <= 1;
        impulse_phase++;
        break;
    default:
        break;
    }


    return s * impulse_vol;
}

float CoolSnare::get_noise()
{
    float n = noisepeak.processSample(2. * random.nextFloat() - 1.);

    return n * noise_vol;
}

void CoolSnare::do_resonance()
{
    float hs = 0.;
    /*
    h1s += impulse + *head1FB * lp1.processSample(hp1.processSample(head1.get(*head1Delay))) + *sympathy * head2.get(*depth);
    h2s += *head2FB * lp1.processSample(hp1.processSample(head2.get(*head2Delay))) + *sympathy * head1.get(*depth);
    */
    hs += impulse + *FB1 * head.get(*delay1);
    if (isnan(hs) || isinf(hs))
    {
        hs = 0;
        hp.reset();
        lp.reset();
    }

    hs = juce::jlimit(-1.f, 1.f, hs);

    hs = lp.processSample(hp.processSample(hs));

    head.put(hs);
}

void CoolSnare::updateFilters()
{

    if (*hpFreq != c_hpFreq)
    {
        c_hpFreq = *hpFreq;
        float f = juce::jlimit(10.f, (sampleRate / 2.f) - 10.f, hpFreq->load());
        hp.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, f);
    }

    // lowpass opens up when velocity goes up
    float a = vel* vel* vel* vel * *accent;
    float f =  (*lpFreq + a) * sampleRate / 2.;
    c_lpFreq = juce::jlimit(10.f, (sampleRate / 2.f) - 10.f, smoothit(c_lpFreq, f, .9));

    lp.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, c_lpFreq);
    
    //if (*lpFreq != c_lpFreq)
    //{
    //    c_lpFreq = *lpFreq;
    //    float f = juce::jlimit(10., (sampleRate / 2.) - 10., *lpFreq * sampleRate / 2.);
    //    lp.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, f);
    //}

}

void CoolSnare::handleMidiEvent(const juce::MidiMessage& m)
{
    if (m.isNoteOn())
    {
        note_in_hertz = m.getMidiNoteInHertz(m.getNoteNumber());
        vel = m.getFloatVelocity();
        impulse_vol = vel;
        if (vel > noise_vol)
            noise_vol = vel;
        
        impulse_phase = 0;
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {

    }
}
