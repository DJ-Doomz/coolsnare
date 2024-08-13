/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout CoolsnareAudioProcessor::createParams()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;

    params.add(std::make_unique<juce::AudioParameterFloat>("delay1", "delay1", juce::NormalisableRange<float> { 0.0f, 600.0f, 1.0f, .7 }, 210.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("FB1", "FB1", -1.1, 1.1, 0.93));
    params.add(std::make_unique<juce::AudioParameterFloat>("headMix", "headMix", 0., 10., 4.21));
    params.add(std::make_unique<juce::AudioParameterFloat>("noiseMix", "noiseMix", 0., 2., 0.17));
    params.add(std::make_unique<juce::AudioParameterFloat>("impulseMix", "impulseMix", 0., 2., 0.));
    params.add(std::make_unique<juce::AudioParameterFloat>("noiseRelease", "noiseRelease", 0., 1., .43));
    params.add(std::make_unique<juce::AudioParameterFloat>("hpFreq", "hpFreq", GRAPH_MIN, GRAPH_MAX, 190.));
    params.add(std::make_unique<juce::AudioParameterInt>("hpOrder", "hpOrder", 1, 4, 1));
    params.add(std::make_unique<juce::AudioParameterFloat>("lpFreq", "lpFreq", 0., 1., .06));
    params.add(std::make_unique<juce::AudioParameterFloat>("peakFreq", "peakFreq", 0., 1., 1.));
    params.add(std::make_unique<juce::AudioParameterFloat>("peakQ", "peakQ", 0., 1., 1.));
    params.add(std::make_unique<juce::AudioParameterFloat>("peakGain", "peakGain", 0., 1., 1.));
    params.add(std::make_unique<juce::AudioParameterFloat>("accent", "accent", 0., 1., 0.));
    params.add(std::make_unique<juce::AudioParameterChoice>("impulseType", "impulseType", juce::StringArray({ "sample", "noise", "click"}), 0));
    
    return params;
}

//==============================================================================
CoolsnareAudioProcessor::CoolsnareAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParams()),
    cs(apvts)
#endif
{
}

CoolsnareAudioProcessor::~CoolsnareAudioProcessor()
{
}

//==============================================================================
const juce::String CoolsnareAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CoolsnareAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CoolsnareAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CoolsnareAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CoolsnareAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CoolsnareAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CoolsnareAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CoolsnareAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CoolsnareAudioProcessor::getProgramName (int index)
{
    return {};
}

void CoolsnareAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CoolsnareAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    cs.prepareToPlay(sampleRate, samplesPerBlock);

    juce::dsp::ProcessSpec spec_stereo;

    spec_stereo.maximumBlockSize = samplesPerBlock;
    spec_stereo.numChannels = getTotalNumOutputChannels();
    spec_stereo.sampleRate = sampleRate;

    limiter.prepare(spec_stereo);
    limiter.setThreshold(0);
    limiter.setRelease(10);
}

void CoolsnareAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CoolsnareAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CoolsnareAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // spent 2 or so hours debugging why everything is broken only to find out this line is actually important
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    cs.processBlock(buffer, midiMessages);

    // apply limiting
    //juce::dsp::AudioBlock<float> block(buffer);

    //juce::dsp::ProcessContextReplacing<float> ctx(block);
    //limiter.process(ctx);
}

//==============================================================================
bool CoolsnareAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CoolsnareAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor(*this);
    return new CoolsnareAudioProcessorEditor (*this, apvts);
}

//==============================================================================
void CoolsnareAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CoolsnareAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CoolsnareAudioProcessor();
}
