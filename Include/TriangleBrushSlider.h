#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class TriangleBrushSlider : public juce::Component
{
public:
    TriangleBrushSlider(float value = 50);

    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;

    std::function<void(float)> onValueChange;
    float getValue() const;
    void setValue(float newValue, bool notify = true);

    void setRange(float l, float u) { valueRange = { l, u }; };


private:
    void updateFromMouse(const juce::MouseEvent &e);
    juce::Path createRoundedTriangle(juce::Rectangle<float> bounds) const;

    juce::Range<float> valueRange = { 1, 10 };
    float currentValue;
};