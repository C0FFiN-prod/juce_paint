#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/CustomLookAndFeel.h"

class MonospaceFontTextButtonLookAndFeel : public CustomLookAndFeel
{
public:
    juce::Font getTextButtonFont(juce::TextButton &, int buttonHeight) override
    {
        return juce::Font(juce::Font::getDefaultMonospacedFontName(), 16.0f, juce::Font::FontStyleFlags::bold);
    }
};
