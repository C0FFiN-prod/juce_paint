#pragma once
//
#include "../JuceLibraryCode/JuceHeader.h"

class ColorPaletteComponent : public juce::Component
{
public:
    ColorPaletteComponent();

    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& e) override;

    std::function<void(juce::Colour)> onColorSelected;

private:
    juce::Colour colors[32];
};