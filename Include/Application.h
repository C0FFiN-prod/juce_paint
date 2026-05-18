#pragma once
//
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/MainFrame.h"
//
class PracticeApplication : public JUCEApplication
{
public:
	PracticeApplication(void) {}
	~PracticeApplication(void) override {}
	//
	const String getApplicationName(void) override { return ProjectInfo::projectName; }
	const String getApplicationVersion(void) override { return ProjectInfo::versionString; }
	bool moreThanOneInstanceAllowed(void) override { return true; }
	//

	static PracticeApplication* getInstance()
	{
		return static_cast<PracticeApplication*>(juce::JUCEApplication::getInstance());
	}

	juce::ApplicationCommandManager* getCommandManager() { return commandManager.get(); }

	void initialise(const String&) override
	{
		commandManager = std::make_unique<juce::ApplicationCommandManager>();
		mainFrame.reset(new MainFrame(getApplicationName()));
	}
	//
	void shutdown(void) override { mainFrame = nullptr; commandManager = nullptr; }
	//
	void systemRequestedQuit(void) override { quit(); }
	void anotherInstanceStarted(const String&) override {}
	//
private:
	std::unique_ptr<juce::ApplicationCommandManager> commandManager;
	std::unique_ptr<MainFrame> mainFrame;
};
//


