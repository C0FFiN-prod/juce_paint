#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/TriangleBrushSlider.h"

class BrushSizeComponent : public juce::Component
{
public:
    static void show(juce::Component *anchor, const juce::Rectangle<int> &anchorArea, std::function<void(float)> onChange, float value = 50);
    BrushSizeComponent(float value);

    TriangleBrushSlider slider;
};