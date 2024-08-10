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

    params.add(std::make_unique<juce::AudioParameterFloat>("head1Delay", "head1Delay", juce::NormalisableRange<float> { 0.0f, 480.0f, 1.0f, .7 }, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("head1FB", "head1FB", -1.1, 1.1, 0.));
    params.add(std::make_unique<juce::AudioParameterFloat>("head1Mix", "head1Mix", 0., 10., 1.));
    params.add(std::make_unique<juce::AudioParameterFloat>("hp1Freq", "hp1Freq", 0., .1, 0.));
    params.add(std::make_unique<juce::AudioParameterFloat>("lp1Freq", "lp1Freq", 0., 1., 1.));
    //params.add(std::make_unique<juce::AudioParameterFloat>("peak1Freq", "peak1Freq", 0., 1., 1.));
    //params.add(std::make_unique<juce::AudioParameterFloat>("peak1Q", "peak1Q", 0., 1., 1.));
    //params.add(std::make_unique<juce::AudioParameterFloat>("peak1Gain", "peak1Gain", 0., 1., 1.));

    params.add(std::make_unique<juce::AudioParameterFloat>("head2Delay", "head2Delay", juce::NormalisableRange<float> { 0.0f, 480.0f, 1.0f, .7 }, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("head2FB", "head2FB", -1.1, 1.1, 0.));
    params.add(std::make_unique<juce::AudioParameterFloat>("head2Mix", "head2Mix", 0., 10., 1.));
    params.add(std::make_unique<juce::AudioParameterFloat>("hp2Freq", "hp2Freq", 0., .1, 0.));
    params.add(std::make_unique<juce::AudioParameterFloat>("lp2Freq", "lp2Freq", 0., 1., 1.));
    //params.add(std::make_unique<juce::AudioParameterFloat>("peak2Freq", "peak2Freq", 0., 1., 1.));
    //params.add(std::make_unique<juce::AudioParameterFloat>("peak2Q", "peak2Q", 0., 1., 1.));
    //params.add(std::make_unique<juce::AudioParameterFloat>("peak2Gain", "peak2Gain", 0., 1., 1.));
    params.add(std::make_unique<juce::AudioParameterFloat>("depth", "depth", juce::NormalisableRange<float> { 0.0f, 280.0f, 1.0f, .7 }, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("sympathy", "sympathy", -1., 1., 0.));
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
    cs.processBlock(buffer, midiMessages);

    // apply limiting
    juce::dsp::AudioBlock<float> block(buffer);

    juce::dsp::ProcessContextReplacing<float> ctx(block);
    limiter.process(ctx);
}

//==============================================================================
bool CoolsnareAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CoolsnareAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
    //return new CoolsnareAudioProcessorEditor (*this);
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
