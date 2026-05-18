#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
// Меню-бар
class EditorMenuBar : public juce::MenuBarModel
{
public:
    EditorMenuBar();

    juce::StringArray getMenuBarNames() override
    {
        return {
            juce::String::fromUTF8(u8"Файл"),
            juce::String::fromUTF8(u8"Правка"),
            juce::String::fromUTF8(u8"Изображение"),
        };
    }

    juce::PopupMenu getMenuForIndex(int topLevelIndex, const juce::String &) override;

    void menuItemSelected(int itemId, int topLevelMenuIndex) override {};

private:
    juce::PopupMenu fileMenu, editMenu, imageMenu;
};
