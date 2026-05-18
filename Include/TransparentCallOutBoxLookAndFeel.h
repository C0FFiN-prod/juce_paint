#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class TransparentCallOutBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawCallOutBoxBackground(juce::CallOutBox &, juce::Graphics &,
                                  const juce::Path &, juce::Image &) override {}
};