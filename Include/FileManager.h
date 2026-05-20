#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class FileManager
{
public:
    using LoadImageCallback = std::function<void(const juce::Image&)>;
    using GetImageCallback  = std::function<juce::Image()>;

    FileManager(juce::Component& windowToRename,
                juce::String programName,
                LoadImageCallback onLoad,
                GetImageCallback onGetImage);

    // Основные операции
    void newFile();
    void openFile();
    void saveFile(std::function<void()> onSave = nullptr);
    void saveAsFile(std::function<void()> onSave = nullptr);

    // Управление состоянием
    void markAsChanged(bool changed);
    bool isChanged() const { return isModified; }

    // Вызывать из MainFrame при попытке закрытия окна
    // Возвращает true, если окно можно закрыть, false - если пользователь отменил действие
    bool handleWindowClose();

private:
    void updateWindowTitle();
    void showSavePrompt(std::function<void(bool)> onDecision);
    bool performSave(const juce::File& file);
    void promptForOpen();

    juce::Component& window;
    juce::String programName;
    juce::File currentFile;
    std::unique_ptr<juce::FileChooser> chooser;
    bool isModified = false;
    LoadImageCallback loadCallback;
    GetImageCallback getImageCallback;
};