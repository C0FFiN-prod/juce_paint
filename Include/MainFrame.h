#pragma once
//
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/MainComponent.h"
#include "../Include/EditorMenuBar.h"
#include "../Include/CustomLookAndFeel.h"
//
class MainFrame : public DocumentWindow
{
public:
	//
	MainFrame(const String &name);
	~MainFrame() override
	{
		setMenuBar(nullptr);
		juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
	};
	//
	void closeButtonPressed(void) override { JUCEApplication::getInstance()->systemRequestedQuit(); }
	//
	MainComponent &getMainComponent(void) { return *dynamic_cast<MainComponent *>(getContentComponent()); }
	//
private:
	std::unique_ptr<EditorMenuBar> menuBarModel;
	std::unique_ptr<CustomLookAndFeel> customLNF;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainFrame)
};