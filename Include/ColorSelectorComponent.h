#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class ColorSelectorComponent : public juce::Component
{
public:
    ColorSelectorComponent() {}

    void setForegroundColor(juce::Colour col);
    void setBackgroundColor(juce::Colour col);
    juce::Colour getForegroundColor() { return fgColor; };
    juce::Colour getBackgroundColor() { return bgColor; };

    void paint(juce::Graphics &g) override;

    void mouseDown(const juce::MouseEvent &e) override;

    std::function<void(juce::Colour)> onForegroundChanged;
    std::function<void(juce::Colour)> onBackgroundChanged;
    std::function<void()> onSwapColors;

private:
    juce::ComponentBoundsConstrainer colorPickerConstrainer{};
    juce::Colour fgColor{juce::Colours::black};
    juce::Colour bgColor{juce::Colours::white};
};