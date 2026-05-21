#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/Enums.h"

class FileManager
{
public:
    using LoadImageCallback = std::function<void(const juce::Image&)>;
    using GetImageCallback  = std::function<juce::Image()>;

    FileManager(juce::Component& windowToRename,
                juce::String programName,
                LoadImageCallback onLoad,
                GetImageCallback onGetImage);

    void newFile();
    void openFile();
    void saveFile(std::function<void()> onSave = nullptr);
    PEnums::Formats extensionToFormat(const juce::File& file);
    void saveAsFile(std::function<void()> onSave = nullptr);

    void markAsChanged(bool changed);
    bool isChanged() const { return isModified; }

    bool handleWindowClose();

private:
    void updateWindowTitle();
    void showSavePrompt(std::function<void(bool)> onDecision);
    bool performSave(const juce::File& file, const PEnums::Formats targetFormat = PEnums::Formats::PNG);
    void promptForOpen();

    juce::Component& window;
    juce::String programName;
    juce::File currentFile;
    std::unique_ptr<juce::FileChooser> chooser;
    bool isModified = false;
    LoadImageCallback loadCallback;
    GetImageCallback getImageCallback;
};