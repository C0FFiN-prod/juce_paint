#pragma once
//
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/ToolbarComponent.h"
#include "../Include/CanvasComponent.h"
#include "../Include/StatusBarComponent.h"
#include "../Include/FileManager.h"

//
class MainComponent : public Component, public juce::ApplicationCommandTarget
{
public:
	MainComponent(juce::DocumentWindow& w);
	~MainComponent(void) override;
	//
	void paint(Graphics &) override;
	void resized(void) override;

	juce::ApplicationCommandTarget* getNextCommandTarget() override;
	void getAllCommands(juce::Array<juce::CommandID>& c) override;
	void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
	bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;
	FileManager fileManager;
	//
private:
	ToolbarComponent toolbar{};
	CanvasComponent canvas{};
	StatusBarComponent statusbar{canvas};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};