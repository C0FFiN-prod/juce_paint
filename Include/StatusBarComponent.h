#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/CanvasComponent.h"

//class CanvasComponent;

class StatusBarComponent : public juce::Component
{
public:
    StatusBarComponent(CanvasComponent& cnv);

    void paint(juce::Graphics &g) override;
    void resized() override;
    void updated();

private:
    juce::Label cursorPosLabel{};
    juce::Label imageSizeLabel{};
    juce::Slider zoomSlider{};
    juce::Label zoomPercentLabel{};

    CanvasComponent& canvas;
};