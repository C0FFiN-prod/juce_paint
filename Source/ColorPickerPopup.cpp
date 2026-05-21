#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/ColorPickerPopup.h"
#include "../Include/Enums.h"

char ColorPickerPopup::currentSwatch = 0;
bool ColorPickerPopup::isFirstRun = true;
juce::Colour ColorPickerPopup::swatches[32];

ColorPickerPopup::ColorPickerPopup(juce::Colour initial, std::function<void(juce::Colour)> onPick)
	: callback(std::move(onPick))
{
	hexEditor.setMultiLine(false);
	hexEditor.setReturnKeyStartsNewLine(false);
	hexEditor.setReadOnly(false);
	hexEditor.setScrollbarsShown(false);
	hexEditor.setCaretVisible(true);
	hexEditor.setIndents(2, 1);
	hexEditor.setFont(juce::Font(10.0f));
	hexEditor.setJustification(juce::Justification::centredLeft);
	hexEditor.addListener(this);
	addAndMakeVisible(hexEditor);

	setWantsKeyboardFocus(true);
	//setMouseClickGrabsKeyboardFocus(true);

	if (isFirstRun)
		initSwatches();

	colourChanged(initial, ColorSource::None);
	setSize(185, 350);
}

ColorPickerPopup::~ColorPickerPopup()
{
	hexEditor.removeListener(this);
}

void ColorPickerPopup::paint(juce::Graphics &g)
{
	paintColourSquare(g);

	paintHueSlider(g);

	paintAlphaSlider(g);

	paintEyedropper(g);

	paintSwatchGrid(g);
}

void ColorPickerPopup::resized()
{
	auto r = getLocalBounds().reduced(15);

	sqBounds = r.removeFromTop(155).toFloat();
	r.removeFromTop(10);
	hueBounds = r.removeFromTop(15).toFloat();
	r.removeFromTop(10);
	alphaBounds = r.removeFromTop(15).toFloat();
	r.removeFromTop(10);

	auto row4 = r.removeFromTop(20);
	eyeBounds = row4.removeFromLeft(20).toFloat();
	row4.removeFromLeft(5);
	hexBounds = row4.toFloat();
	hexEditor.setBounds(hexBounds.toNearestInt());

	r.removeFromTop(10);
	swatchBounds = r.toFloat();
}
void ColorPickerPopup::mouseMove(const juce::MouseEvent& e)
{
	if (eyeHovered != eyeBounds.contains(e.position)) {
		eyeHovered = eyeBounds.contains(e.position);
		repaint(eyeBounds.toNearestInt());
	}
}


void ColorPickerPopup::mouseDown(const juce::MouseEvent &e)
{
	if (sqBounds.contains(e.position))
	{
		draggingSq = true;
		updateFromSquare(e.position.toInt());
	}
	else if (hueBounds.contains(e.position))
	{
		draggingHue = true;
		updateFromHue(e.position.toInt());
	}
	else if (alphaBounds.contains(e.position))
	{
		draggingAlpha = true;
		updateFromAlpha(e.position.toInt());
	}
	else if (eyeBounds.contains(e.position)) 
	{
		eyePressed = true;
		repaint(eyeBounds.toNearestInt());
	}
	else if (swatchBounds.contains(e.position))
	{
		updateFromSwatch(e.position.toInt());
	}
}

void ColorPickerPopup::mouseDrag(const juce::MouseEvent &e)
{
	if (draggingSq)
		updateFromSquare(e.position.toInt());
	else if (draggingHue)
		updateFromHue(e.position.toInt());
	else if (draggingAlpha)
		updateFromAlpha(e.position.toInt());
}

void ColorPickerPopup::mouseUp(const juce::MouseEvent &e)
{
	if (eyeBounds.contains(e.position)) {
		eyePicking = true;
	}
	else if (eyePicking) eyePicking = false;

	draggingSq = draggingHue = draggingAlpha = eyePressed = false;
	repaint(eyeBounds.toNearestInt());
}

void ColorPickerPopup::textEditorReturnKeyPressed(juce::TextEditor &)
{
	hexEditor.giveAwayKeyboardFocus();
}

void ColorPickerPopup::textEditorFocusLost(juce::TextEditor &)
{
	applyHex();
}

void ColorPickerPopup::colourChanged(juce::Colour &c, ColorSource s)
{
	currentColour = c;

	if (s == ColorSource::None || 
		s == ColorSource::HEX || 
		s == ColorSource::Swatch ||
		s == ColorSource::Eyedropped
		)
	{
		currentSqX = currentColour.getSaturation();
		currentSqY = 1.0f - currentColour.getBrightness();
		if (currentSqX > 0)
			currentHUE = currentColour.getHue();
		currentAlpha = currentColour.getFloatAlpha();
	}
	if (s != ColorSource::Swatch)
		swatches[currentSwatch] = currentColour;

	hexEditor.setText(c.toDisplayString(false).toUpperCase(), juce::dontSendNotification);
	repaint();
	if (callback)
		callback(c);
}

void ColorPickerPopup::applyHex()
{
	auto txt = hexEditor.getText().trim();
	if (txt.startsWithChar('#'))
		txt = txt.substring(1);

	if (txt.isEmpty())
	{
		hexEditor.setText(currentColour.toDisplayString(false).toUpperCase(), juce::sendNotification);
		return;
	}
	auto alpha = false;
	// Нормализация длины
	switch (txt.length())
	{
	case 1:
		txt = juce::String::repeatedString(txt, 6);
		alpha = true;
		break; // F -> FFFFFF
	case 2:
		txt = juce::String::repeatedString(txt, 3);
		alpha = true;
		break; // A0 -> A0A0A0
	case 3:
		txt = juce::String::charToString(txt[0]) + txt[0] +
			  txt[1] + txt[1] +
			  txt[2] + txt[2];
		alpha = true;
		break; // RGB -> RRGGBB
	case 4:
		txt = juce::String::charToString(txt[3]) + txt[3] +
			  txt[0] + txt[0] +
			  txt[1] + txt[1] +
			  txt[2] + txt[2];
		break; // RGBA -> AARRGGBB
	case 5:
		txt = txt.substring(3) +
			  txt[0] + txt[0] +
			  txt[1] + txt[1] +
			  txt[2] + txt[2];
		break; // RGBAA -> AARRGGBB
	case 7:
		txt = juce::String::charToString(txt[6]) + txt[6] + txt.substring(0, 6);
		break; // RRGGBBAA -> AARRGGBB
	case 8: // RRGGBBAA -> AARRGGBB
		txt = txt.substring(6) + txt.substring(0, 6);
		break;
	default:
		alpha = true;
		break;
	}
	auto color = juce::Colour::fromString(txt);
	if (alpha)
		color = color.withAlpha(currentAlpha);
	colourChanged(color, ColorSource::HEX);
}

void ColorPickerPopup::updateFromSquare(juce::Point<int> p)
{
	currentSqX = juce::jlimit(0.0f, 1.0f, (p.getX() - sqBounds.getX()) / sqBounds.getWidth());
	currentSqY = juce::jlimit(0.0f, 1.0f, (p.getY() - sqBounds.getY()) / sqBounds.getHeight());
	colourChanged(currentColour.withHue(currentHUE).withAlpha(currentAlpha).withSaturation(currentSqX).withBrightness(1.0f - currentSqY), ColorSource::Square);
}

void ColorPickerPopup::updateFromHue(juce::Point<int> p)
{
	auto reduced = hueBounds.reduced(hueBounds.getHeight() / 2);
	currentHUE = juce::jlimit(0.0f, 1.0f, (p.getX() - reduced.getX()) / reduced.getWidth());
	colourChanged(currentColour.withHue(currentHUE), ColorSource::HUE);
}

void ColorPickerPopup::updateFromAlpha(juce::Point<int> p)
{
	auto reduced = alphaBounds.reduced(alphaBounds.getHeight() / 2);
	currentAlpha = juce::jlimit(0.0f, 1.0f, (p.getX() - reduced.getX()) / reduced.getWidth());
	colourChanged(currentColour.withAlpha(currentAlpha), ColorSource::Alpha);
}

void ColorPickerPopup::updateFromSwatch(juce::Point<int> p)
{
	float cellW = 20, cellH = 20;
	int col = static_cast<int>((p.getX() - swatchBounds.getX()) / cellW);
	int row = static_cast<int>((p.getY() - swatchBounds.getY()) / cellH);
	if (col >= 0 && col < 8 && row >= 0 && row < 4)
	{
		currentSwatch = row * 8 + col;
		colourChanged(swatches[currentSwatch], ColorSource::Swatch);
	}
}

void ColorPickerPopup::paintColourSquare(juce::Graphics &g)
{
	// Слой 1: Белый -> Текущий HUE (горизонталь, ось насыщенности)
	juce::ColourGradient hueGrad;
	hueGrad.addColour(0.0f, juce::Colours::white);
	hueGrad.addColour(1.0f, juce::Colours::red.withHue(currentHUE));
	hueGrad.point1 = {sqBounds.getX(), sqBounds.getY()};	 // левый верх
	hueGrad.point2 = {sqBounds.getRight(), sqBounds.getY()}; // правый верх
	g.setGradientFill(hueGrad);
	g.fillRoundedRectangle(sqBounds, 5);

	// Слой 2: Прозрачный -> Чёрный (вертикаль, ось яркости)
	juce::ColourGradient blackGrad;
	blackGrad.addColour(0.0f, juce::Colours::transparentBlack); // верх = прозрачный
	blackGrad.addColour(1.0f, juce::Colours::black);			// низ = чёрный
	blackGrad.point1 = {sqBounds.getX(), sqBounds.getY()};		// верх
	blackGrad.point2 = {sqBounds.getX(), sqBounds.getBottom()}; // низ
	g.setGradientFill(blackGrad);
	g.fillRoundedRectangle(sqBounds, 5);

	// Курсор: x = насыщенность, y = яркость
	float cx = sqBounds.getX() + currentSqX * sqBounds.getWidth();
	float cy = sqBounds.getY() + currentSqY * sqBounds.getHeight();

	float h = alphaBounds.getHeight();
	g.setColour(juce::Colours::white);
	g.fillEllipse(cx - h / 2, cy - h / 2, h, h);
	g.setColour(currentColour.withAlpha(1.f));
	g.fillEllipse(cx - h / 4, cy - h / 4, h / 2, h / 2);
	g.setColour(juce::Colours::grey);
	g.drawEllipse(cx - h / 2, cy - h / 2, h, h, 0.5f);
}

void ColorPickerPopup::paintHueSlider(juce::Graphics &g)
{

	float h = hueBounds.getHeight();
	auto reduced = hueBounds.reduced(h / 2);
	juce::ColourGradient grad;
	grad.addColour(0.00f, juce::Colours::red);
	grad.addColour(0.16f, juce::Colours::yellow);
	grad.addColour(0.33f, juce::Colours::green);
	grad.addColour(0.50f, juce::Colours::cyan);
	grad.addColour(0.66f, juce::Colours::blue);
	grad.addColour(0.83f, juce::Colours::magenta);
	grad.addColour(1.00f, juce::Colours::red);
	grad.point1 = {hueBounds.getX(), hueBounds.getY()};
	grad.point2 = {hueBounds.getRight(), hueBounds.getY()};
	g.setGradientFill(grad);

	g.fillRoundedRectangle(hueBounds, h / 2);

	float tx = reduced.getX() + currentHUE * reduced.getWidth();

	g.setColour(juce::Colours::white);
	g.fillEllipse(tx - h / 2, hueBounds.getY(), h, h);
	g.setColour(juce::Colours::red.withHue(currentHUE));
	g.fillEllipse(tx - h / 4, hueBounds.getY() + h / 4, h / 2, h / 2);
	g.setColour(juce::Colours::grey);
	g.drawEllipse(tx - h / 2, hueBounds.getY(), h, h, 0.5f);
}

void ColorPickerPopup::paintAlphaSlider(juce::Graphics &g)
{
	float h = alphaBounds.getHeight();
	auto reduced = alphaBounds.reduced(h / 2);
	float tile = 4.0f;
	g.fillCheckerBoard(alphaBounds, tile, tile, juce::Colour(0xFFE5E5E5), juce::Colour(0xFFFFFFFF));

	g.setColour(juce::Colours::white);
	g.drawRoundedRectangle(alphaBounds.expanded(tile / 2), (h + tile) / 2, tile);

	juce::ColourGradient grad;
	grad.addColour(0.0f, currentColour.withAlpha(0.0f));
	grad.addColour(1.0f, currentColour.withAlpha(1.0f));
	grad.point1 = {alphaBounds.getX() + h / 2, alphaBounds.getY()};
	grad.point2 = {alphaBounds.getRight() - h / 2, alphaBounds.getY()};
	g.setGradientFill(grad);

	g.fillRoundedRectangle(alphaBounds, h / 2);

	float tx = reduced.getX() + currentAlpha * reduced.getWidth();

	g.setColour(juce::Colours::white);
	g.fillEllipse(tx - h / 2, alphaBounds.getY(), h, h);
	g.setColour(currentColour);
	g.fillEllipse(tx - h / 4, alphaBounds.getY() + h / 4, h / 2, h / 2);
	g.setColour(juce::Colours::grey);
	g.drawEllipse(tx - h / 2, alphaBounds.getY(), h, h, 0.5f);
}

void ColorPickerPopup::paintEyedropper(juce::Graphics &g)
{
	auto style = PEnums::ToolBtnStyles::Default;
	if (eyePressed || eyePicking) style = PEnums::ToolBtnStyles::Pressed;
	else if (eyeHovered) style = PEnums::ToolBtnStyles::Hover;
	style = PEnums::ToolBtnStyles::Disabled;
	g.setColour(style.bgColor);
	g.fillRoundedRectangle(eyeBounds, 2.0f);
	g.setColour(style.strokeColor);
	g.drawRoundedRectangle(eyeBounds.reduced(0.5f), 2.0f, 1.0f);

	PEnums::Icons::draw(g, &PEnums::Icons::Eyedropper, eyeBounds.reduced(2), style.iconColor);
}

void ColorPickerPopup::paintSwatchGrid(juce::Graphics &g)
{
	float cellS = 16, padding = 4, tile = 4;
	float cellB = cellS + padding, cellH = cellS / 2;

	static auto ac = juce::Colour(0xFF8821F7), bc = juce::Colour(0xFFe5e5e5);
	static auto c0 = juce::Colour(0xFFE5E5E5), c1 = juce::Colour(0xFFFFFFFF);

	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 8; ++col)
		{
			int i = row * 8 + col;
			float x = swatchBounds.getX() + col * cellB;
			float y = swatchBounds.getY() + row * cellB;
			auto &c = swatches[i];
			if (c.getAlpha() == 255)
			{
				g.setColour(c);
				g.fillRect(x, y, cellS, cellS);
			}
			else
			{
				juce::Rectangle r(x + cellH, y, cellH, cellS);
				g.setColour(c.withAlpha(1.f));
				g.fillRect(x, y, cellH, cellS);
				g.setColour(c);
				g.fillCheckerBoard(r, tile, tile, c0, c1);
				g.fillRect(r);
			}
			g.setColour(i == currentSwatch ? ac : bc);
			g.drawRoundedRectangle(x, y, cellS, cellS, 2.0f, 1.0f);
		}
	}
}

void ColorPickerPopup::initSwatches()
{
	const int colors[] = {
		0xFFFF0000, 0xFFFF7F00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FFFF, 0xFF0000FF, 0xFF8B00FF, 0xFFFF00FF,
		0xFF400000, 0xFF402000, 0xFF404000, 0xFF004000, 0xFF004040, 0xFF000040, 0xFF200040, 0xFF400040,
		0xFFFFC0CB, 0xFFADD8E6, 0xFF90EE90, 0xFFFFFACD, 0xFFD3D3D3, 0xFF808080, 0xFFA9A9A9, 0xFF696969,
		0xFF2F4F4F, 0xFF8B4513, 0xFF556B2F, 0xFF483D8B, 0xFF800080, 0xFFB22222, 0xFFFFFFFF, 0xFF000000};
	for (int i = 0; i < 32; ++i)
		swatches[i] = juce::Colour(colors[i]);

	ColorPickerPopup::isFirstRun = false;
}