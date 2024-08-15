/*
  ==============================================================================

    EQComponent.cpp
    Created: 12 Aug 2024 2:52:20pm
    Author:  noone

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EQComponent.h"

//==============================================================================
EQComponent::EQComponent(juce::AudioProcessorValueTreeState& a,
    juce::RangedAudioParameter& hpf, juce::RangedAudioParameter& hpr, juce::RangedAudioParameter& hpq,
    juce::RangedAudioParameter& pf, juce::RangedAudioParameter& pr, juce::RangedAudioParameter& pq,
    juce::RangedAudioParameter& lpf, juce::RangedAudioParameter& lpr, juce::RangedAudioParameter& lpq) :
    apvts(a),
    hp(EQNode::NodeType::HP, hpf, hpr, hpq),
    peak(EQNode::NodeType::PEAK, pf, pr, pq),
    lp(EQNode::NodeType::LP, lpf, lpr, lpq)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    //addAndMakeVisible(hp);
    //addAndMakeVisible(peak);
    //addAndMakeVisible(lp);
    setInterceptsMouseClicks(true, false);
    setRepaintsOnMouseActivity(true);
    
    sampleRate = a.processor.getSampleRate();
    juce::dsp::ProcessSpec Monospec;
    Monospec.maximumBlockSize = 32;
    Monospec.numChannels = 1;
    Monospec.sampleRate = sampleRate;
    
    f_peak.prepare(Monospec);
    f_lp.prepare(Monospec);
    f_lp.setType(HigherOrderFilter::LP);
    f_hp.prepare(Monospec);
    f_hp.setType(HigherOrderFilter::HP);

    init();
}

EQComponent::~EQComponent()
{
}

void EQComponent::paint (juce::Graphics& g)
{
    auto lb = getLocalBounds();
    auto w = lb.getWidth();
    auto h = lb.getHeight();
    
    // paint response curves
    updateFilters();
    juce::Path responsePath;
    
    std::vector<float> plot_freqs = plotting_freqs;

    plot_freqs.push_back(hp.freq.convertFrom0to1(hp.freq.getValue()));
    plot_freqs.push_back(lp.freq.convertFrom0to1(lp.freq.getValue()));
    plot_freqs.push_back(peak.freq.convertFrom0to1(peak.freq.getValue()));

    std::sort(plot_freqs.begin(), plot_freqs.end());

    float starty = magnitudeToScreen(magnitude(20));
    responsePath.startNewSubPath(0, starty);
    for (auto freq = plot_freqs.begin(); freq != plot_freqs.end(); freq++)
    {
        float x = juce::mapFromLog10(*freq, GRAPH_MIN, GRAPH_MAX)*w; //  juce::jmap(float(i), 0.f, pathpoints, 0.f, float(w));
        float y = magnitudeToScreen(magnitude(*freq));
        responsePath.lineTo(x, y);
    }

    g.setColour(juce::Colours::white);
    g.strokePath(responsePath, juce::PathStrokeType(2));

    hp.paint(g);
    peak.paint(g);
    lp.paint(g);
}

void EQComponent::resized()
{
    auto lb = getLocalBounds();
    hp.setBounds(lb);
    peak.setBounds(lb);
    lp.setBounds(lb);
}

void EQComponent::mouseDown(const juce::MouseEvent& e)
{
    // which node is closest
    auto p = e.getPosition();
    EQNode* closest = &hp;
    if (p.getDistanceFrom(juce::Point<int>(lp.screenx, lp.screeny)) < p.getDistanceFrom(juce::Point<int>(closest->screenx, closest->screeny)))
    {
        closest = &lp;
    }

    if (p.getDistanceFrom(juce::Point<int>(peak.screenx, peak.screeny)) < p.getDistanceFrom(juce::Point<int>(closest->screenx, closest->screeny)))
    {
        closest = &peak;
    }

    closest->mouseDown(e);
    if(closest->dragging)
    {
        setMouseCursor(juce::MouseCursor::NoCursor);
        updateMouseCursor();
    }

}

void EQComponent::mouseUp(const juce::MouseEvent& e)
{
    setMouseCursor(juce::MouseCursor::NormalCursor);
    hp.mouseUp(e);
    peak.mouseUp(e);
    lp.mouseUp(e);
}

void EQComponent::mouseDrag(const juce::MouseEvent& e)
{
    hp.mouseDrag(e);
    peak.mouseDrag(e);
    lp.mouseDrag(e);

    repaint();
}

void EQComponent::mouseDoubleClick(const juce::MouseEvent& e)
{
    hp.mouseDoubleClick(e);
    peak.mouseDoubleClick(e);
    lp.mouseDoubleClick(e);

    repaint();
}

void EQComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
{
    hp.mouseWheelMove(e, mw);
    peak.mouseWheelMove(e, mw);
    lp.mouseWheelMove(e, mw);

    repaint();
}

void EQComponent::updateFilters()
{
    f_peak.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        peak.freq.convertFrom0to1(peak.freq.getValue()),
        peak.q.convertFrom0to1(peak.q.getValue()),
        juce::Decibels::decibelsToGain(peak.gainOrOrder.convertFrom0to1(peak.gainOrOrder.getValue())));

    f_lp.setCutoffFrequency(lp.freq.convertFrom0to1(lp.freq.getValue()));
    f_lp.setResonance(lp.q.convertFrom0to1(lp.q.getValue()));
    f_lp.setOrder(lp.gainOrOrder.convertFrom0to1(lp.gainOrOrder.getValue()));
    f_hp.setCutoffFrequency(hp.freq.convertFrom0to1(hp.freq.getValue()));
    f_hp.setResonance(lp.q.convertFrom0to1(hp.q.getValue()));
    f_hp.setOrder(lp.gainOrOrder.convertFrom0to1(hp.gainOrOrder.getValue()));
}

void EQComponent::init()
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        float freq = juce::mapToLog10(i / NUM_POINTS, GRAPH_MIN, GRAPH_MAX);
        plotting_freqs.push_back(freq);
    }
}

float EQComponent::magnitude(float freq)
{
    return f_hp.magnitude(freq) * f_peak.coefficients.get()->getMagnitudeForFrequency(freq, sampleRate) * f_lp.magnitude(freq);
}

float EQComponent::magnitudeToScreen(float m)
{
    auto h = getLocalBounds().getHeight();
    float y = juce::Decibels::gainToDecibels(m);
    y = juce::jmap(y, -24.f, 24.f, float(h), 0.f);
    return y;
}

EQNode::EQNode(NodeType t, juce::RangedAudioParameter& f, juce::RangedAudioParameter& g, juce::RangedAudioParameter& inq) :
    freq(f),
    gainOrOrder(g),
    q(inq),
    type(t)
{
    screenx = 0;
    screeny = 0;
    dragging = false;
    setOpaque(false);
    setInterceptsMouseClicks(false, false);
}

void EQNode::paint(juce::Graphics& g)
{
    auto lb = getLocalBounds();
    auto w = lb.getWidth();
    auto h = lb.getHeight();
    float freqx = juce::jmap(freq.getValue(), GRAPH_MIN, GRAPH_MAX);
    freqx = juce::mapFromLog10(freqx, GRAPH_MIN, GRAPH_MAX);
    screenx = juce::jmap(freqx, 0.f, float(w));
    auto point = juce::Point<float>(screenx, screeny);

    screeny = h / 2.;
    if (type == NodeType::PEAK)
    {
        screeny = juce::jmap(gainOrOrder.getValue(), float(h), 0.f);
    }
    else
    {
        screeny = juce::jmap(pow(q.getValue(), 1.f/3.5f), 0.f, 1.f, float(h), 0.f);
    }

    if (dragging)
    {
        g.setColour(juce::Colours::white.withAlpha(.3f));
        juce::Rectangle<float> outline(20, 20);
        g.fillEllipse(outline.withCentre(point));
        g.setColour(juce::Colours::aqua);
    }
    else
    {
        g.setColour(juce::Colours::white);
    }

    juce::Rectangle<float> circ(10, 10);
    g.fillEllipse(circ.withCentre(point));
}

void EQNode::mouseDown(const juce::MouseEvent& e)
{
    auto p = e.getPosition();
    // detect if the mouse was over this when clicked
    if (p.getDistanceFrom(juce::Point<int>(screenx, screeny)) < range && !dragging)
    {
        dragging = true;
        setMouseCursor(juce::MouseCursor::NoCursor);
        updateMouseCursor();
        mouseDrag(e);
    }
}

void EQNode::mouseUp(const juce::MouseEvent& e)
{
    dragging = false;
    repaint();
}

void EQNode::mouseDrag(const juce::MouseEvent& e)
{
    if (dragging)
    {
        auto lb = getLocalBounds();
        auto w = lb.getWidth();
        auto h = lb.getHeight();
        auto p = e.getPosition();

        float s = juce::jmap(float(p.getX()), 0.f, float(w), 0.f, 1.f);
        float hertz = juce::mapToLog10(s, GRAPH_MIN, GRAPH_MAX);
        float newv = juce::jmap(hertz, GRAPH_MIN, GRAPH_MAX, 0.f, 1.f);
        
        freq.setValueNotifyingHost(newv);

        if (type == NodeType::PEAK)
        {
            float newg = juce::jmap(float(p.getY()), float(h), 0.f, 0.f, 1.f);
            gainOrOrder.setValueNotifyingHost(newg);
        }
        else
        {
            float newq = juce::jmap(float(p.getY()), float(h), 0.f, 0.f, 1.f);
            // adjust q so that .707 is near the middle
            newq = juce::jlimit(0.f, 1.f, newq);
            newq = pow(newq, 3.5f);
            q.setValueNotifyingHost(newq);
        }

        repaint();
    }
}

void EQNode::mouseDoubleClick(const juce::MouseEvent& e)
{
    auto p = e.getPosition();
    // detect if the mouse was over this when clicked
    if (p.getDistanceFrom(juce::Point<int>(screenx, screeny)) < range)
    {
        freq.setValueNotifyingHost(freq.getDefaultValue());
        q.setValueNotifyingHost(q.getDefaultValue());
        gainOrOrder.setValueNotifyingHost(gainOrOrder.getDefaultValue());

        repaint();
    }
}

void EQNode::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
{
    auto p = e.getPosition();
    // detect if the mouse was over this when clicked
    if (p.getDistanceFrom(juce::Point<int>(screenx, screeny)) < range)
    {
        float cv;
        juce::RangedAudioParameter* p = nullptr;
        if (type == PEAK)
            p = &q;
        else
            p = &gainOrOrder;

        cv = p->getValue();
        if (type == PEAK)
            cv += .1 * mw.deltaY;
        else
            cv += copysign(.25, mw.deltaY);

        cv = juce::jlimit(0.f, 1.f, cv);
        p->setValueNotifyingHost(cv);

        repaint();
    }
}
