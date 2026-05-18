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

	transparentLNF = std::make_unique<TransparentCallOutBoxLookAndFeel>();
	juce::LookAndFeel::setDefaultLookAndFeel(transparentLNF.get());
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
