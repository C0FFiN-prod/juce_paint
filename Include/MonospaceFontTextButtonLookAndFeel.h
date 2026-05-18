#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MonospaceFontTextButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Font getTextButtonFont(juce::TextButton &, int buttonHeight) override
    {
        return juce::Font(juce::Font::getDefaultMonospacedFontName(), 16.0f, juce::Font::FontStyleFlags::bold);
    }
};
