#include "BrushSizeComponent.h"

BrushSizeComponent::BrushSizeComponent(float value) : slider{value}
{
    setSize(150, 40);
    
    slider.setBounds(getLocalBounds().reduced(8, 3));
    slider.setRange(1, slider.getHeight());
    addAndMakeVisible(slider);
}

void BrushSizeComponent::show(juce::Component *anchor, const juce::Rectangle<int> &anchorArea, std::function<void(float)> onChange, float value)
{
    auto popup = std::make_unique<BrushSizeComponent>(value);
    popup->slider.onValueChange = std::move(onChange);

    juce::CallOutBox::launchAsynchronously(std::move(popup), anchorArea, anchor);
}