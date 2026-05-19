#include "../Include/CanvasSizeComponent.h"
#include "../Include/MonospaceFontTextButtonLookAndFeel.h"

#define _(x) juce::String::fromUTF8(u8#x)

inline CanvasSizeComponent::CanvasSizeComponent(int w, int h, std::function<void(int newW, int newH, int offsetX, int offsetY)> onApply) : currentW(w), currentH(h)
{
	setWantsKeyboardFocus(true);

	titleLabel.setText(_(Размер холста), juce::dontSendNotification);
	titleLabel.setJustificationType(juce::Justification::centred);
	titleLabel.setFont(juce::Font().withHeight(14.0f).withStyle(juce::Font::bold));
	titleLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(0xFF191919));
	
	addAndMakeVisible(titleLabel);

	widthInput.setMultiLine(false);
	widthInput.setReturnKeyStartsNewLine(false);
	widthInput.setReadOnly(false);
	widthInput.setScrollbarsShown(false);
	widthInput.setCaretVisible(true);

	widthInput.setIndents(2, 1);
	widthInput.setFont(juce::Font(14.0f));
	widthInput.setJustification(juce::Justification::centredRight);
	widthInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
	widthInput.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFFE5E5E5));
	widthInput.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xFFBEBEBE));
	widthInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);
	widthInput.setInputRestrictions(0, "0123456789");
	widthInput.setText(juce::String(currentW));


	heightInput.setMultiLine(false);
	heightInput.setReturnKeyStartsNewLine(false);
	heightInput.setReadOnly(false);
	heightInput.setScrollbarsShown(false);
	heightInput.setCaretVisible(true);

	heightInput.setIndents(2, 2);
	heightInput.setFont(juce::Font(14.0f));
	heightInput.setJustification(juce::Justification::centredRight);
	heightInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
	heightInput.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFFE5E5E5));
	heightInput.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xFFBEBEBE));
	heightInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);
	heightInput.setInputRestrictions(0, "0123456789");
	heightInput.setText(juce::String(currentH));

	xLabel.setText(_(×), juce::dontSendNotification);
	xLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(0xFF191919));
	xLabel.setJustificationType(juce::Justification::centred);
	xLabel.setFont(juce::Font().withHeight(14.0f));

	addAndMakeVisible(widthInput);
	addAndMakeVisible(heightInput);
	addAndMakeVisible(xLabel);

	static MonospaceFontTextButtonLookAndFeel msFont;

	const auto boxChars = _(┌┬┐├┼┤└┴┘);

	for (int i = 0; i < 9; ++i)
	{
		anchorBtns.add(new juce::TextButton());
		anchorBtns[i]->setLookAndFeel(&msFont);
		anchorBtns[i]->setButtonText(juce::String::charToString(boxChars[i]));
		anchorBtns[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::white);
		anchorBtns[i]->setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
		anchorBtns[i]->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour(0xFF191919));
		anchorBtns[i]->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour(0xFF8821f7));
		anchorBtns[i]->setToggleState(i == 4, juce::dontSendNotification);
		anchorBtns[i]->setClickingTogglesState(true);
		anchorBtns[i]->onClick = [this, i]()
		{
			for (int j = 0; j < 9; ++j)
				anchorBtns[j]->setToggleState(j == i, juce::dontSendNotification);
		};
		addAndMakeVisible(anchorBtns[i]);
	}

	applyBtn.setButtonText(_(Применить));
	applyBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFFFBB00));
	applyBtn.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
	applyBtn.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour(0xFF191919));
	applyBtn.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour(0xFF191919));

	cancelBtn.setButtonText(_(Отменить));
	cancelBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFFFFFFF));
	cancelBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFFD1D1D1));
	cancelBtn.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour(0xFF191919));
	cancelBtn.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour(0xFF191919));

	applyBtn.onClick = [this, onApply]()
	{ applyResize(onApply, currentW, currentH); };
	cancelBtn.onClick = [this]()
	{ getParentComponent()->setVisible(false); };
	addAndMakeVisible(applyBtn);
	addAndMakeVisible(cancelBtn);
	
	setSize(220, 250);
	resized();
}

inline void CanvasSizeComponent::resized()
{
	auto content = getLocalBounds().reduced(10);
	int y = content.getY();

	// titleLabel
	titleLabel.setBounds(content.withY(y).withHeight(20));
	y += 20 + 5;

	// inputRow (ручная расстановка)
	int inputRowWidth = 70 + 24 + 70;
	int inputRowX = content.getX() + (content.getWidth() - inputRowWidth) / 2;
	widthInput.setBounds(inputRowX, y, 70, 26);
	xLabel.setBounds(inputRowX + 70, y, 24, 26);
	heightInput.setBounds(inputRowX + 70 + 24, y, 70, 26);
	y += 26 + 10;

	// anchorGrid (сетка 3×3)
	const int btnSize = 26, margin = 2;
	const int gridSize = btnSize * 3 + margin * 4;
	int gridX = content.getX() + (content.getWidth() - gridSize) / 2;
	for (int i = 0; i < 9; ++i)
	{
		int col = i % 3, row = i / 3;
		int x = gridX + margin + col * (btnSize + margin);
		int btnY = y + margin + row * (btnSize + margin);
		anchorBtns[i]->setBounds(x, btnY, btnSize, btnSize);
	}
	y += gridSize + 10;

	// Кнопки
	int btnRowWidth = 90 + 2 + 90;
	int btnRowX = content.getX() + (content.getWidth() - btnRowWidth) / 2;
	applyBtn.setBounds(btnRowX, y, 90, 28);
	cancelBtn.setBounds(btnRowX + 90 + 2, y, 90, 28);
	setSize(std::max(inputRowWidth, btnRowWidth)+20, y + 28 + 10);
}

void CanvasSizeComponent::show(juce::Component *anchor, juce::Rectangle<int> anchorArea, int currentW, int currentH, std::function<void(int newW, int newH, int offsetX, int offsetY)> onApply)
{
	auto popup = std::make_unique<CanvasSizeComponent>(currentW, currentH, onApply);
	juce::CallOutBox::launchAsynchronously(std::move(popup), anchorArea, anchor);
}

inline void CanvasSizeComponent::applyResize(std::function<void(int, int, int, int)> onApply, int oldW, int oldH)
{
	int newW = juce::jmax(1, widthInput.getText().getIntValue());
	int newH = juce::jmax(1, heightInput.getText().getIntValue());

	int selAnchor = 4; // default center
	for (int i = 0; i < 9; ++i)
		if (anchorBtns[i]->getToggleState())
		{
			selAnchor = i;
			break;
		}

	int col = selAnchor % 3; // 0=left, 1=center, 2=right
	int row = selAnchor / 3; // 0=top,  1=center, 2=bottom

	// Смещение старого изображения на новом холсте
	int offsetX = juce::roundToInt(static_cast<float>(newW - oldW) * col / 2.0f);
	int offsetY = juce::roundToInt(static_cast<float>(newH - oldH) * row / 2.0f);

	onApply(newW, newH, offsetX, offsetY);
	currentW = newW;
	currentH = newH;
}
