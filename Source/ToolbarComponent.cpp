#include "../Include/ToolbarComponent.h"
#include "../Include/BrushSizeComponent.h"
#include "../Include/Enums.h"

ToolbarComponent::ToolbarComponent()
{
	setOpaque(true);

	separatorLine = std::make_unique<Separator>();
	btnCursor = addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Cursor);
	btns.add(btnCursor);
	btnCursor->data = PEnums::CanvasTool::Cursor;
	btnCursor->callback = [this](ToolButtonComponent* btn)
		{
			if (btn->getState() == ToolButtonComponent::State::Disabled) return;
			resetBtnsExcept(btn);
			btn->setState(ToolButtonComponent::State::Active);
			selectedTool = (PEnums::CanvasTool)btn->data;
			if (onToolChanged) onToolChanged(selectedTool);
		};

	btnBrushSize = addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Size);
	btns.add(btnBrushSize);
	btnBrushSize->callback = [this](ToolButtonComponent*)
		{
			BrushSizeComponent::show(getTopLevelComponent(), btnBrushSize->getBounds().toNearestInt(),
				[this](float s)
				{
					brushSize = s;
					if (onBrushSizeChanged) onBrushSizeChanged(s); }, brushSize);
		};
	btnBrush = addToolButton(ToolButtonComponent::Type::Multiple, PEnums::Icons::Brush);
	btns.add(btnBrush);
	btnBrush->data = PEnums::CanvasTool::Brush;
	btnBrush->callback = [this](ToolButtonComponent* btn)
		{
			if (btn->getState() == ToolButtonComponent::State::Disabled) return;
			if (btn->getState() == ToolButtonComponent::State::Active) {
				switch (btn->data) {
				case PEnums::CanvasTool::Brush:
					btn->setIcon(PEnums::Icons::Pencil);
					btn->data = PEnums::CanvasTool::Pencil;
					break;
				case PEnums::CanvasTool::Pencil:
					btn->setIcon(PEnums::Icons::Brush);
					btn->data = PEnums::CanvasTool::Brush;
					break;
				}
				btn->repaint();
			}
			else {
				resetBtnsExcept(btn);
				btn->setState(ToolButtonComponent::State::Active);
			}
			selectedTool = (PEnums::CanvasTool)btn->data;
			if (onToolChanged) onToolChanged(selectedTool);
		};
	
	btnEraser = addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Erase);
	btns.add(btnEraser);
	btnEraser->data = PEnums::CanvasTool::Eraser;
	btnEraser->callback = [this](ToolButtonComponent* btn)
		{
			if (btn->getState() == ToolButtonComponent::State::Disabled) return;
			resetBtnsExcept(btn);
			btn->setState(ToolButtonComponent::State::Active);
			selectedTool = (PEnums::CanvasTool)btn->data;
			if (onToolChanged) onToolChanged(selectedTool);
		};

	btnFill = addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Fill);
	btns.add(btnFill);
	btnFill->data = PEnums::CanvasTool::Fill;
	btnFill->callback = [this](ToolButtonComponent* btn)
		{
			if (btn->getState() == ToolButtonComponent::State::Disabled) return;
			resetBtnsExcept(btn);
			btn->setState(ToolButtonComponent::State::Active);
			selectedTool = (PEnums::CanvasTool)btn->data;
			if (onToolChanged) onToolChanged(selectedTool);
		};

	btnShapes = addToolButton(ToolButtonComponent::Type::Multiple, PEnums::Icons::Rect);
	btns.add(btnShapes);
	btnShapes->data = PEnums::CanvasTool::Rect;
	btnShapes->callback = [this](ToolButtonComponent* btn)
		{
			if (btn->getState() == ToolButtonComponent::State::Disabled) return;
			if (btn->getState() == ToolButtonComponent::State::Active) {
				switch (btn->data) {
				case PEnums::CanvasTool::Rect:
					btn->setIcon(PEnums::Icons::Ellipse);
					btn->data = PEnums::CanvasTool::Ellipse;
					break;
				case PEnums::CanvasTool::Ellipse:
					btn->setIcon(PEnums::Icons::Triangle);
					btn->data = PEnums::CanvasTool::Triangle;
					break;
				case PEnums::CanvasTool::Triangle:
					btn->setIcon(PEnums::Icons::Rect);
					btn->data = PEnums::CanvasTool::Rect;
					break;
				}
				btn->repaint();
			}
			else {
				resetBtnsExcept(btn);
				btn->setState(ToolButtonComponent::State::Active);
			}
			selectedTool = (PEnums::CanvasTool)btn->data;
			if (onToolChanged) onToolChanged(selectedTool);
		};
	btnLines = addToolButton(ToolButtonComponent::Type::Multiple, PEnums::Icons::Line);
	btns.add(btnLines);
	btnLines->data = PEnums::CanvasTool::Line;
	btnLines->callback = [this](ToolButtonComponent* btn)
		{
			if (btn->getState() == ToolButtonComponent::State::Disabled) return;
			if (btn->getState() == ToolButtonComponent::State::Active) {
				switch (btn->data) {
				case PEnums::CanvasTool::Line:
					btn->setIcon(PEnums::Icons::OneDirArrow);
					btn->data = PEnums::CanvasTool::OneDirArrow;
					break;
				case PEnums::CanvasTool::OneDirArrow:
					btn->setIcon(PEnums::Icons::BiDirArrow);
					btn->data = PEnums::CanvasTool::BiDirArrow;
					break;
				case PEnums::CanvasTool::BiDirArrow:
					btn->setIcon(PEnums::Icons::Line);
					btn->data = PEnums::CanvasTool::Line;
					break;
				}
				btn->repaint();
			}
			else {
				resetBtnsExcept(btn);
				btn->setState(ToolButtonComponent::State::Active);
			}
			selectedTool = (PEnums::CanvasTool)btn->data;
			if (onToolChanged) onToolChanged(selectedTool);
		};

	btnText = addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Text);
	btns.add(btnText);
	btnText->data = PEnums::CanvasTool::Text;
	btnText->callback = [this](ToolButtonComponent* btn)
		{
			if (btn->getState() == ToolButtonComponent::State::Disabled) return;
			resetBtnsExcept(btn);
			btn->setState(ToolButtonComponent::State::Active);
			selectedTool = (PEnums::CanvasTool)btn->data;
			if (onToolChanged) onToolChanged(selectedTool);
		};


	btnBrush->setState(ToolButtonComponent::State::Active);

	colorSelector = std::make_unique<ColorSelectorComponent>();
	addAndMakeVisible(*colorSelector);
	colorSelector->setForegroundColor(juce::Colours::black);
	colorSelector->setBackgroundColor(juce::Colours::white);

	colorSelector = std::make_unique<ColorSelectorComponent>();
	addAndMakeVisible(*colorSelector);

	colorSelector->onForegroundChanged = [this](juce::Colour c)
		{
			fgColor = c;
			if (onColorChanged)
				onColorChanged(c, false);
		};
	colorSelector->onBackgroundChanged = [this](juce::Colour c)
		{
			bgColor = c;
			if (onColorChanged) onColorChanged(c, true);
		};

	

	//colorSelector->onSwapColors = [this]()
	//{
	//    std::swap(fgColor, bgColor);
	//    colorSelector->repaint();
	//    if (onColorChanged)
	//        onColorChanged(fgColor);
	//    if (onSwapColors)
	//        onSwapColors();
	//};
}

ToolButtonComponent* ToolbarComponent::addToolButton(ToolButtonComponent::Type type, const EmbeddedIcon& icon)
{
	auto btn = std::make_unique<ToolButtonComponent>(type);
	btn->setIcon(icon);
	addAndMakeVisible(*btn);
	return btn.release();
}

void ToolbarComponent::resetBtnsExcept(ToolButtonComponent* theBtn)
{
	for (auto* btn : btns) {
		if (btn == theBtn) continue;
		btn->setState(ToolButtonComponent::State::Default);
	}
}

void ToolbarComponent::paint(juce::Graphics& g)
{
	g.fillAll(PEnums::Colours::White);
	g.setColour(PEnums::Colours::Gray1);
	g.drawVerticalLine(getWidth() - 1, 0.0f, static_cast<float>(getHeight()));
}

void ToolbarComponent::resized()
{
	const float padding = 5.0f;
	const float spacing = 5.0f;
	const float buttonSize = static_cast<float>(getWidth()) - 2.0f * padding;

	juce::FlexBox fb;
	fb.flexDirection = juce::FlexBox::Direction::column;
	fb.alignItems = juce::FlexBox::AlignItems::center;
	fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	fb.flexWrap = juce::FlexBox::Wrap::noWrap;

	const auto btnMargin = juce::FlexItem::Margin{ 0.0f, 0.0f, spacing, 0.0f };

	auto addBtn = [&](juce::Component& btn) {
		fb.items.add(juce::FlexItem(btn)
			.withWidth(buttonSize)
			.withHeight(buttonSize)
			.withMargin(btnMargin));
		};

	for (auto* btn : btns)
		addBtn(*btn);
	//addBtn(*btnCursor);
	//addBtn(*btnBrushSize);
	//addBtn(*btnBrush);
	//addBtn(*btnEraser);
	//addBtn(*btnFill);
	//addBtn(*btnShapes);
	//addBtn(*btnLines);
	//addBtn(*btnText);

	fb.items.add(juce::FlexItem(*separatorLine)
		.withWidth(buttonSize)
		.withHeight(2.0f)
		.withMargin({ 3.0f, 0.0f, 8.0f, 0.0f }));

	addBtn(*colorSelector);
	auto bounds = getLocalBounds().toFloat();
	bounds.removeFromTop(padding);
	fb.performLayout(bounds);
}
