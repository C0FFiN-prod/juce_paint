#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class CanvasSizeComponent : public juce::Component
{
public:
    CanvasSizeComponent(int w, int h,
                        std::function<void(int newW, int newH, int offsetX, int offsetY)> onApply);

    void resized() override;

    static void show(juce::Component *anchor, juce::Rectangle<int> anchorArea,
                     int currentW, int currentH,
                     std::function<void(int newW, int newH, int offsetX, int offsetY)> onApply);

private:
    int currentW, currentH;
    juce::Label titleLabel, xLabel;
    juce::TextEditor widthInput, heightInput;
    juce::TextButton applyBtn, cancelBtn;
    juce::OwnedArray<juce::TextButton> anchorBtns;
    void applyResize(std::function<void(int, int, int, int)> onApply, int oldW, int oldH);
};