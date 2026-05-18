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
		menu.addItem(1, _(Новый));
		menu.addItem(2, _(Открыть...));
		menu.addSeparator();
		menu.addItem(3, _(Сохранить));
		menu.addItem(4, _(Сохранить как...));
		menu.addSeparator();
		//menu.addItem(5, _(Выход));
		menu.addCommandItem(PracticeApplication::getInstance()->getCommandManager(), MainComponent::cmdQuit);
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
		menu.addItem(21, _(Масштаб...));
		menu.addSeparator();
		menu.addItem(22, _(Отразить горизонтально));
		menu.addItem(23, _(Отразить вертикально));
		break;
	default:
		throw "Unknown MenuBar Menu Index";
	}
	return menu;
}
