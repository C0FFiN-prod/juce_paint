#include "../Include/MainFrame.h"
#include "../Include/Application.h"
//
MainFrame::MainFrame(const String& name)
	: DocumentWindow(name, Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
		DocumentWindow::allButtons)
{
	setUsingNativeTitleBar(true);
	setResizable(true, false);
	setResizeLimits(400, 600, 10000, 10000);

	customLNF = std::make_unique<CustomLookAndFeel>();
	juce::LookAndFeel::setDefaultLookAndFeel(customLNF.get());
	auto* commandManager = PracticeApplication::getInstance()->getCommandManager();
	commandManager->registerAllCommandsForTarget(JUCEApplication::getInstance());

	setContentOwned(new MainComponent(), false);
	setVisible(true);

	menuBarModel.reset(new EditorMenuBar());
	setMenuBar(menuBarModel.get());
	menuBarModel->setApplicationCommandManagerToWatch(commandManager);
	addKeyListener(commandManager->getKeyMappings());
	commandManager->commandStatusChanged();
}
