#include "../Include/MainComponent.h"
#include "../Include/Application.h"

#define _(x) juce::String::fromUTF8(u8#x)

//
MainComponent::MainComponent(void)
{
	setOpaque(true);
	setSize(900, 600);

	auto* commandManager = PracticeApplication::getInstance()->getCommandManager();
	commandManager->registerAllCommandsForTarget(this);

	addAndMakeVisible(&toolbar);
	addAndMakeVisible(&canvas);
	addAndMakeVisible(&statusbar);

	toolbar.onColorChanged = [this](juce::Colour col, bool isBg)
	{
		if (isBg) canvas.setBgColour(col);
		else canvas.setCurrentColour(col);
	};

	toolbar.onBrushSizeChanged = [this](float size)
	{
		canvas.setBrushSize(size);
	};
	toolbar.onToolChanged = [this](PEnums::CanvasTool tool)
	{
		canvas.setTool(tool);
	};
	canvas.onCameraChanged = [this]()
	{
		statusbar.updated();
	};

	centreWithSize(getWidth(), getHeight());
}
//
MainComponent::~MainComponent(void)
{
}
//
void MainComponent::paint(Graphics &g)
{
	g.fillAll(Colours::powderblue);
}
//
void MainComponent::resized(void)
{
	const int toolbarWidth = 50;
	const int statusbarHeight = 30;
	auto bounds = getLocalBounds();

	toolbar.setBounds(bounds.removeFromLeft(toolbarWidth));
	statusbar.setBounds(bounds.removeFromBottom(statusbarHeight));
	canvas.setBounds(bounds);
}

inline juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
	return findFirstTargetParentComponent();
}

inline void MainComponent::getAllCommands(juce::Array<juce::CommandID>& c)
{
	c.add(cmdQuit);
}

inline void MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case cmdQuit:
		result.setInfo(_(Выйти), _(Закрыть приложение), _(Файл), 0);
		result.setActive(true);
		break;
	default: break;
	}
}

inline bool MainComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
	switch (info.commandID)
	{
	case cmdQuit:
		JUCEApplication::getInstance()->quit();
		return true;
	default:
		return false;
	}
}
