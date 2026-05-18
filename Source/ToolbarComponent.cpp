#include "../Include/ToolbarComponent.h"
#include "../Include/BrushSizeComponent.h"
#include "../Include/Enums.h"

ToolbarComponent::ToolbarComponent()
{
	setOpaque(true);

	separatorLine = std::make_unique<Separator>();
	btnCursor.reset(addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Cursor));
	btnBrushSize.reset(addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Size));
	btnBrushSize->callback = [this]()
		{
			BrushSizeComponent::show(getTopLevelComponent(), btnBrushSize->getBounds().toNearestInt(),
				[this](float s)
				{
					brushSize = s;
					if (onBrushSizeChanged) onBrushSizeChanged(s); }, brushSize);
		};
	btnBrush.reset(addToolButton(ToolButtonComponent::Type::Multiple, PEnums::Icons::Brush));
	btnBrush->data = PEnums::CanvasTool::Brush;
	btnBrush->callback = [this]()
		{
			if (btnBrush->getState() == ToolButtonComponent::State::Disabled) return;
			if (btnBrush->getState() == ToolButtonComponent::State::Active) {
				if (btnBrush->data == PEnums::CanvasTool::Brush) {
					btnBrush->setIcon(PEnums::Icons::Pencil);
					btnBrush->data = PEnums::CanvasTool::Pencil;
				}
				else {
					btnBrush->setIcon(PEnums::Icons::Brush);
					btnBrush->data = PEnums::CanvasTool::Brush;
				}
				btnBrush->repaint();
			}
			else {
				btnBrush->setState(ToolButtonComponent::State::Active);
			}
			selectedTool = (PEnums::CanvasTool)btnBrush->data;
			if (onBrushSizeChanged) onToolChanged(selectedTool);
		};
	btnEraser.reset(addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Erase));
	btnFill.reset(addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Fill));
	btnShapes.reset(addToolButton(ToolButtonComponent::Type::Multiple, PEnums::Icons::Rect));
	btnLines.reset(addToolButton(ToolButtonComponent::Type::Multiple, PEnums::Icons::Line));
	btnText.reset(addToolButton(ToolButtonComponent::Type::Single, PEnums::Icons::Text));

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

	addBtn(*btnCursor);
	addBtn(*btnBrushSize);
	addBtn(*btnBrush);
	addBtn(*btnEraser);
	addBtn(*btnFill);
	addBtn(*btnShapes);
	addBtn(*btnLines);
	addBtn(*btnText);

	fb.items.add(juce::FlexItem(*separatorLine)
		.withWidth(buttonSize)
		.withHeight(2.0f)
		.withMargin({ 3.0f, 0.0f, 8.0f, 0.0f }));

	addBtn(*colorSelector);
	auto bounds = getLocalBounds().toFloat();
	bounds.removeFromTop(padding);
	fb.performLayout(bounds);
}
