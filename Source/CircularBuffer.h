/*
  ==============================================================================

    CircularBuffer.h
    Created: 13 Jun 2024 12:19:16pm
    Author:  noone

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CircularBuffer
{
public:
    CircularBuffer(int s = default_size):
        data(nullptr),
        size(juce::nextPowerOfTwo(s)),
        mask(juce::nextPowerOfTwo(s) - 1),
        ptr(mask)
    {
        init_data();
    };

    ~CircularBuffer() {
        delete_data();
    }

    void setSize(float s) {
        size = juce::nextPowerOfTwo(s);
        mask = size - 1;
        ptr = mask;
        delete_data();
        init_data();
    }

    void put(float d) {
        jassert(mask > -1);
        data[mask & --ptr] = d;
    }

    float get(int delay)
    {
        jassert(mask > -1);
        auto t = ptr + delay & mask;
        jassert(t >= 0 && t < size);
        return data[t];
    };

private:
    const static int default_size = 48000;
    float* data;
    int size;
    int mask;
    int ptr;

    void init_data()
    {
        data = new float[size];
        for (int i = 0; i < size; i++)
            data[i] = 0;
    }

    void delete_data()
    {
        if(data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }
    }
};

template <typename T>
class T_CircularBuffer
{
public:
    T_CircularBuffer(int s = default_size) :
        data(nullptr),
        size(juce::nextPowerOfTwo(s)),
        mask(juce::nextPowerOfTwo(s) - 1),
        ptr(mask)
    {
        init_data();
    };

    ~T_CircularBuffer() {
        delete_data();
    }

    void setSize(float s) {
        size = juce::nextPowerOfTwo(s);
        mask = size - 1;
        ptr = mask;
        delete_data();
        init_data();
    }

    void put(float d) {
        jassert(mask > -1);
        data[mask & --ptr] = d;
    }

    float get(int delay)
    {
        jassert(mask > -1);
        auto t = ptr + delay & mask;
        jassert(t >= 0 && t < size);
        return data[t];
    };

private:
    const static int default_size = 48000;
    T* data;
    int size;
    int mask;
    int ptr;

    void init_data()
    {
        data = new T[size];
    }

    void delete_data()
    {
        if (data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }
    }
};