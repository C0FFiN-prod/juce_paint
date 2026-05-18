#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class SliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
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