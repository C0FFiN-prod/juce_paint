#include "../Include/StatusBarComponent.h"
#include "../Include/Application.h"
#include "../Include/SliderLookAndFeel.h"

#define _(x) juce::String::fromUTF8(u8#x)

StatusBarComponent::StatusBarComponent(CanvasComponent& cnv) : canvas(cnv)
{
    setOpaque(false);

    cursorPosLabel.setJustificationType(juce::Justification::centredRight);
    cursorPosLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(0xFF191919));
    cursorPosLabel.setFont(juce::Font(11.0f));
    cursorPosLabel.setText(_(0×0), juce::dontSendNotification);
    addAndMakeVisible(cursorPosLabel);

    imageSizeLabel.setJustificationType(juce::Justification::centredRight);
    imageSizeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(0xFF191919));
    imageSizeLabel.setFont(juce::Font(11.0f));
    imageSizeLabel.setText(_(0×0 пикс.), juce::dontSendNotification);
    addAndMakeVisible(imageSizeLabel);

    auto zoomRange = canvas.getZoomRange();
    zoomSlider.setRange(zoomRange.getStart(), zoomRange.getEnd());
    zoomSlider.setSkewFactorFromMidPoint(1);
    zoomSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    zoomSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    static SliderLookAndFeel thinLAF;
    zoomSlider.setLookAndFeel(&thinLAF);
    zoomSlider.onValueChange = [this]() {
        canvas.setZoom(zoomSlider.getValue()); canvas.repaint(); updated();
        };
    zoomSlider.setDoubleClickReturnValue(true, 1);
    addAndMakeVisible(zoomSlider);

    zoomPercentLabel.setJustificationType(juce::Justification::centredLeft);
    zoomPercentLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(0xFF191919));
    zoomPercentLabel.setFont(juce::Font(11.0f));
    zoomPercentLabel.setText("100%", juce::dontSendNotification);

    addAndMakeVisible(zoomPercentLabel);

    resized();
}

void StatusBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void StatusBarComponent::resized()
{
    const int gap = 10;
    const int height = getHeight() - 2;

    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    fb.alignItems = juce::FlexBox::AlignItems::center;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    const int lISw = imageSizeLabel.getFont().getStringWidth(imageSizeLabel.getText());
    fb.items.add(juce::FlexItem(cursorPosLabel).withHeight(height).withMinWidth(60).withMargin({ 0, 0, 0, gap }));
    fb.items.add(juce::FlexItem(imageSizeLabel).withHeight(height).withWidth(lISw).withMargin({ 0, 0, 0, gap }));
    fb.items.add(juce::FlexItem(zoomSlider).withHeight(height).withWidth(100).withMargin({ 0, 0, 0, gap }));
    fb.items.add(juce::FlexItem(zoomPercentLabel).withHeight(height).withWidth(40));

    fb.performLayout(getLocalBounds().reduced(5, 0).toFloat());
}

void StatusBarComponent::updated()
{
    if (!canvas.getCanvasImage().isValid())
    {
        cursorPosLabel.setText(_(--×--), juce::dontSendNotification);
        imageSizeLabel.setText(_(--×--), juce::dontSendNotification);
        zoomSlider.setValue(1.0, juce::dontSendNotification);
        zoomPercentLabel.setText("100%", juce::dontSendNotification);
        return;
    }

    const int imgW = canvas.getCanvasImage().getWidth();
    const int imgH = canvas.getCanvasImage().getHeight();
    imageSizeLabel.setText(juce::String(imgW) + _(×) + juce::String(imgH) + _(пикс.),
        juce::dontSendNotification);

    if (canvas.isCursorOverImg()) {
        auto cursorImgPos = canvas.getCursorImgPos();
        cursorPosLabel.setText(
            juce::String(static_cast<int>(cursorImgPos.x)) + _(×) +
            juce::String(static_cast<int>(cursorImgPos.y)),
            juce::dontSendNotification);
        cursorPosLabel.setVisible(true);
    }
    else {
        cursorPosLabel.setVisible(false);
    }

    // === Зум ===
    const float zoom = canvas.getZoom();  // или camera.getZoom()
    zoomSlider.setValue(zoom, juce::dontSendNotification);
    zoomPercentLabel.setText(juce::String(static_cast<int>(zoom * 100)) + "%",
        juce::dontSendNotification);
}


