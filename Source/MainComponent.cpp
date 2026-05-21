#include "../Include/MainComponent.h"
#include "../Include/Application.h"
#include "../Include/Enums.h"

//
MainComponent::MainComponent(juce::DocumentWindow& w)
	: fileManager{ w, JUCEApplication::getInstance()->getApplicationName(),
	[this](const juce::Image& img) { canvas.setCanvasImage(img); },
	[this]() { return canvas.getImage(); }
	}
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

	toolbar.onBrushSizeChanged = [this](float size) { canvas.setBrushSize(size); };
	toolbar.onToolChanged = [this](PEnums::CanvasTool tool) { canvas.setTool(tool); };
	canvas.onCameraChanged = [this]() { statusbar.updated(); };

	canvas.onImageChanged = [this]() { fileManager.markAsChanged(true); };

	centreWithSize(getWidth(), getHeight());
}
//
MainComponent::~MainComponent(void)
{
}
//
void MainComponent::paint(Graphics& g)
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
	c.add(PEnums::CommandIDs::AppQuit);
	c.add(PEnums::CommandIDs::FileNew);
	c.add(PEnums::CommandIDs::FileOpen);
	c.add(PEnums::CommandIDs::FileSave);
	c.add(PEnums::CommandIDs::FileSaveAs);
}

inline void MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case PEnums::CommandIDs::FileNew:
		result.setInfo(_("Новый"), _("Создать новый файл"), _("Файл"), 0);
		result.addDefaultKeypress('N', juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::FileOpen:
		result.setInfo(_("Открыть..."), _("Открыть файл"), _("Файл"), 0);
		result.addDefaultKeypress('O', juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::FileSave:
		result.setInfo(_("Сохранить"), _("Сохранить текущий файл"), _("Файл"), 0);
		result.addDefaultKeypress('S', juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::FileSaveAs:
		result.setInfo(_("Сохранить как..."), _("Сохранить в выбранный файл"), _("Файл"), 0);
		result.addDefaultKeypress('S', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::AppQuit:
		result.setInfo(_("Выйти"), _("Закрыть приложение"), _("Файл"), 0);
		result.addDefaultKeypress(juce::KeyPress::F4Key, juce::ModifierKeys::altModifier);
		result.setActive(true);
		break;
	default: break;
	}
}

inline bool MainComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
	switch (info.commandID)
	{
	case PEnums::CommandIDs::AppQuit:
		fileManager.handleWindowClose();
		return true;
	case PEnums::CommandIDs::FileNew:
		fileManager.newFile();
		return true;
	case PEnums::CommandIDs::FileOpen:
		fileManager.openFile();
		return true;
	case PEnums::CommandIDs::FileSave:
		fileManager.saveFile();
		return true;
	case PEnums::CommandIDs::FileSaveAs:
		fileManager.saveAsFile();
		return true;

	default:
		return false;
	}
}
