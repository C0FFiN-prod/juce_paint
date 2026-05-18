#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/Enums.h"

class Separator : public juce::Component
{
public:
    Separator() {}

    void paint(juce::Graphics& g) override
    {
        g.fillAll(PEnums::Colours::Gray1);
    }
};