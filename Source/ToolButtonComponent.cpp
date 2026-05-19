#include "../Include/ToolButtonComponent.h"
#include "../Include/Enums.h"

ToolButtonComponent::ToolButtonComponent(Type buttonType)
	: type(buttonType), icon(nullptr)
{
}

void ToolButtonComponent::setState(State newState)
{
	if (state != newState)
	{
		state = newState;
		repaint();
	}
}

ToolButtonComponent::State ToolButtonComponent::getState() const { return state; }

void ToolButtonComponent::setType(Type newType)
{
	type = newType;
	repaint();
}

void ToolButtonComponent::paint(juce::Graphics &g)
{

	auto bounds = getLocalBounds().toFloat();
	float cornerSize = 8.0f;

	juce::Path bgPath;
	bgPath.addRoundedRectangle(bounds.reduced(0.5f), cornerSize);

	juce::Colour bgColor, strokeColor, iconColor, cornerColor;
	switch (state)
	{
	case State::Active:
		bgColor = PEnums::ToolBtnStyles::Active.bgColor;
		strokeColor = PEnums::ToolBtnStyles::Active.strokeColor;
		iconColor = PEnums::ToolBtnStyles::Active.iconColor;
		cornerColor = PEnums::ToolBtnStyles::Active.cornerColor;
		break;
	case State::Pressed:
		bgColor = PEnums::ToolBtnStyles::Pressed.bgColor;
		strokeColor = PEnums::ToolBtnStyles::Pressed.strokeColor;
		iconColor = PEnums::ToolBtnStyles::Pressed.iconColor;
		cornerColor = PEnums::ToolBtnStyles::Pressed.cornerColor;
		break;
	case State::Hover:
		bgColor = PEnums::ToolBtnStyles::Hover.bgColor;
		strokeColor = PEnums::ToolBtnStyles::Hover.strokeColor;
		iconColor = PEnums::ToolBtnStyles::Hover.iconColor;
		cornerColor = PEnums::ToolBtnStyles::Hover.cornerColor;
		break;
	case State::Disabled:
		bgColor = PEnums::ToolBtnStyles::Disabled.bgColor;
		strokeColor = PEnums::ToolBtnStyles::Disabled.strokeColor;
		iconColor = PEnums::ToolBtnStyles::Disabled.iconColor;
		cornerColor = PEnums::ToolBtnStyles::Disabled.cornerColor;
		break;
	default:
		bgColor = PEnums::ToolBtnStyles::Default.bgColor;
		strokeColor = PEnums::ToolBtnStyles::Default.strokeColor;
		iconColor = PEnums::ToolBtnStyles::Default.iconColor;
		cornerColor = PEnums::ToolBtnStyles::Default.cornerColor;
		break;
	}

	g.setColour(bgColor);
	g.fillPath(bgPath);
	g.setColour(strokeColor);
	g.strokePath(bgPath, juce::PathStrokeType(1.0f));

	drawIcon(g, iconColor);

	if (type == Type::Multiple)
	{
		drawCorner(g, cornerColor);
	}
}

void ToolButtonComponent::mouseEnter(const juce::MouseEvent &)
{
	if (state == State::Default)
		setState(State::Hover);
}

void ToolButtonComponent::mouseExit(const juce::MouseEvent &)
{
	if (state == State::Hover)
		setState(State::Default);
}

void ToolButtonComponent::mouseDown(const juce::MouseEvent &)
{
	if (getState() == State::Disabled) return;
	if (getState() != State::Active) setState(State::Pressed);
}

void ToolButtonComponent::mouseUp(const juce::MouseEvent &)
{
	switch (getState()) {
	case State::Disabled: return;
	case State::Pressed:
		setState(State::Default);
		if (callback)
			callback(this);
		break;
	case State::Active:
		if (callback)
			callback(this);
		break;
	}
}

void ToolButtonComponent::drawIcon(juce::Graphics &g, juce::Colour col)
{
	g.setColour(col);
	if (icon == nullptr) {
		g.setFont(juce::Font(14.0f, juce::Font::plain));
		g.drawText("T", getLocalBounds(), juce::Justification::centred);
	}
	else {
		PEnums::Icons::draw(g, icon, getLocalBounds().toFloat(), col);
	}
}

void ToolButtonComponent::drawCorner(juce::Graphics &g, juce::Colour col)
{
	auto b = getLocalBounds().toFloat();
	float s = 15.0f;
	float p = 3.0f;
	float x = b.getRight() - s - p;
	float y = b.getBottom() - p;

	juce::Path corner;
	corner.startNewSubPath(x, y);
	corner.lineTo(x + s, y);
	corner.lineTo(x + s, y - s);
	corner.lineTo(x + s - 3, y - 3);
	corner.closeSubPath();
	g.setColour(col);
	//g.strokePath(corner.createPathWithRoundedCorners(3.f), juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
	g.fillPath(corner.createPathWithRoundedCorners(5.f));
}
