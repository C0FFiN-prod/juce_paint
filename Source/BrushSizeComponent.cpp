#include "BrushSizeComponent.h"

BrushSizeComponent::BrushSizeComponent(float value) : slider{value}
{
    setSize(150, 40);
    
    slider.setBounds(getLocalBounds().reduced(8, 3));
    slider.setRange(1, slider.getHeight());
    addAndMakeVisible(slider);

    setPaintingIsUnclipped(true);
}

void BrushSizeComponent::paint(juce::Graphics &g)
{

    juce::Path p;
    p.addRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 10.0f);

    DropShadow(Colours::black.withAlpha(0.5f), 5, { 0, 0 })
        .drawForPath(g, p);

    g.setColour(juce::Colour(0xFFFFFFFF));
    g.fillPath(p);
    g.setColour(juce::Colour(0xFFBEBEBE));
    g.strokePath(p, juce::PathStrokeType(1));
}

void BrushSizeComponent::show(juce::Component *anchor, const juce::Rectangle<int> &anchorArea, std::function<void(float)> onChange, float value)
{
    auto popup = std::make_unique<BrushSizeComponent>(value);
    popup->slider.onValueChange = std::move(onChange);

    juce::CallOutBox::launchAsynchronously(std::move(popup), anchorArea, anchor);
}