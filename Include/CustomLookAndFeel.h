#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/Enums.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawCallOutBoxBackground(juce::CallOutBox &box, juce::Graphics &g,
                                  const juce::Path &, juce::Image &) override {
        juce::Path p;
        p.addRoundedRectangle(box.getLocalBounds().toFloat().reduced(20.5f), 10.0f);

        PEnums::Colours::Shadow.drawForPath(g, p);

        g.setColour(PEnums::Colours::White);
        g.fillPath(p);
        g.setColour(PEnums::Colours::Gray3);
        g.strokePath(p, juce::PathStrokeType(1));
    }

    CustomLookAndFeel()
    {
        setColour(juce::PopupMenu::backgroundColourId, PEnums::Colours::White);
        setColour(juce::PopupMenu::textColourId, PEnums::Colours::Text);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, PEnums::Colours::Text.withAlpha(0.2f));
        setColour(juce::PopupMenu::highlightedTextColourId, PEnums::Colours::Text);
    }

    void drawMenuBarBackground(Graphics& g, int width, int height, bool isMouseOverBar, MenuBarComponent& menuBar) override
    {
        g.fillAll(PEnums::Colours::White);
        g.setColour(PEnums::Colours::Gray1);
        g.drawHorizontalLine(height - 1, 0, width);
    }

    void drawMenuBarItem(Graphics& g, int width, int height, int itemIndex, const String& itemText,
        bool isMouseOverItem, bool isMenuOpen, bool isMouseOverBar, MenuBarComponent&) override
    {
        juce::Colour fillColor = PEnums::Colours::White;
        if (isMouseOverItem) fillColor = PEnums::Colours::Gray1;
        if (isMenuOpen) fillColor = PEnums::Colours::Gray2;
        g.fillAll(fillColor);
        g.setColour(PEnums::Colours::Gray1);
        g.drawHorizontalLine(height - 1, 0, width);
        g.setColour(PEnums::Colours::Text);
        g.setFont(16);
        g.drawText(itemText, 0, 0, width, height, juce::Justification::centred);
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int w, int h,
        float sliderPos, float, float, juce::Slider::SliderStyle style,
        juce::Slider& slider) override
    {
        static const juce::Colour bg(0xFFd1d1d1);
        static const juce::Colour active(0xFF8821f7);
        static const int trackSize = 2, thumbSize = 8;

        static const int trackSizeH = trackSize / 2, thumbSizeH = thumbSize / 2;

        if (style == juce::Slider::SliderStyle::LinearHorizontal) {
            // Фон трека
            g.setColour(bg);
            g.fillRect(x, y + h / 2 - trackSizeH, w, trackSize);

            // Активная часть
            g.setColour(active);
            g.fillRect(x, y + h / 2 - trackSizeH, (int)sliderPos - x, trackSize);

            const int centerPx = x + w / 2;
            g.setColour(bg);
            g.fillRect(centerPx - 1, y + h / 2 - thumbSizeH, 2, thumbSize);

            // Ручка
            g.setColour(active);
            g.fillEllipse(sliderPos - thumbSizeH, y + h / 2 - thumbSizeH, thumbSize, thumbSize);
        }
        else if (style == juce::Slider::SliderStyle::LinearVertical) {
            // Фон трека
            g.setColour(bg);
            g.fillRect(x + w / 2 - trackSizeH, y, trackSize, h);

            // Активная часть
            g.setColour(active);
            g.fillRect(x + w / 2 - trackSizeH, y, trackSize, (int)sliderPos - y);

            const int centerPy = y + h / 2;
            g.setColour(bg);
            g.fillRect(centerPy + h / 2 - thumbSizeH, y - 1, thumbSize, 2);

            // Ручка
            g.setColour(active);
            g.fillEllipse(x + w / 2 - thumbSizeH, sliderPos - thumbSizeH, thumbSize, thumbSize);
        }
    }

};