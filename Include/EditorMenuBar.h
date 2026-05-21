#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
// Меню-бар
class EditorMenuBar : public juce::MenuBarModel
{
public:
    EditorMenuBar();

    juce::StringArray getMenuBarNames() override;

    juce::PopupMenu getMenuForIndex(int topLevelIndex, const juce::String &) override;

    void menuItemSelected(int itemId, int topLevelMenuIndex) override {};

private:
    juce::PopupMenu fileMenu, editMenu, imageMenu;
};
