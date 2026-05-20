#include "../Include/EditorMenuBar.h"
#include "../Include/CanvasSizeComponent.h"
#include "../Include/MainFrame.h"
#include "../Include/Application.h"

#define _(x) juce::String::fromUTF8(u8#x)

EditorMenuBar::EditorMenuBar() {}

juce::PopupMenu EditorMenuBar::getMenuForIndex(int topLevelIndex, const juce::String&)
{
	juce::PopupMenu menu;
	switch (topLevelIndex)
	{
	case 0:
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::FileNew);
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::FileOpen);
		menu.addSeparator();
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::FileSave);
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::FileSaveAs);
		menu.addSeparator();
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::AppQuit);
		break;
	case 1:
		menu.addItem(10, _(Отменить));
		menu.addItem(11, _(Повторить));
		menu.addSeparator();
		menu.addItem(12, _(Вырезать));
		menu.addItem(13, _(Копировать));
		menu.addItem(14, _(Вставить));
		break;
	case 2:
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::CanvasResize);
		menu.addSeparator();
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::CanvasFlipH);
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::CanvasFlipV);
		menu.addSeparator();
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::CanvasRotate90CW);
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::CanvasRotate90CC);
		menu.addSeparator();
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), PEnums::CommandIDs::CanvasClear);
		break;
	default:
		throw "Unknown MenuBar Menu Index";
	}
	return menu;
}
