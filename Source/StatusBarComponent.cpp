#include "../Include/StatusBarComponent.h"
#include "../Include/Enums.h"

StatusBarComponent::StatusBarComponent(CanvasComponent& cnv) : canvas(cnv)
{
    setOpaque(false);

    const juce::Font font(16.0f);

    cursorPosLabel.setJustificationType(juce::Justification::centredRight);
    cursorPosLabel.setFont(font);
    cursorPosLabel.setText(_("0×0"), juce::dontSendNotification);
    addAndMakeVisible(cursorPosLabel);

    imageSizeLabel.setJustificationType(juce::Justification::centredRight);
    imageSizeLabel.setFont(font);
    imageSizeLabel.setText(_("0×0 px"), juce::dontSendNotification);
    addAndMakeVisible(imageSizeLabel);

    auto zoomRange = canvas.getZoomRange();
    zoomSlider.setRange(zoomRange.getStart(), zoomRange.getEnd());
    zoomSlider.setSkewFactorFromMidPoint(1);
    zoomSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    zoomSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    zoomSlider.onValueChange = [this]() {
        canvas.setZoom(zoomSlider.getValue()); canvas.repaint(); updated();
        };
    zoomSlider.setDoubleClickReturnValue(true, 1);
    addAndMakeVisible(zoomSlider);

    zoomPercentLabel.setJustificationType(juce::Justification::centredLeft);
    zoomPercentLabel.setFont(font);
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
    const int height = getHeight();

    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    fb.alignItems = juce::FlexBox::AlignItems::center;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    const int lISw = imageSizeLabel.getFont().getStringWidth("M") * 13;
    const int lZPw = zoomPercentLabel.getFont().getStringWidth("M") * 5;
    fb.items.add(juce::FlexItem(cursorPosLabel).withHeight(height).withWidth(lISw));
    fb.items.add(juce::FlexItem(imageSizeLabel).withHeight(height).withWidth(lISw));
    fb.items.add(juce::FlexItem(zoomSlider).withHeight(height).withWidth(100));
    fb.items.add(juce::FlexItem(zoomPercentLabel).withHeight(height).withMinWidth(lZPw));

    fb.performLayout(getLocalBounds().reduced(3, 0).toFloat());
}

void StatusBarComponent::updated()
{
    if (!canvas.getCanvasImage().isValid())
    {
        cursorPosLabel.setText(_("--×--"), juce::dontSendNotification);
        imageSizeLabel.setText(_("--×--"), juce::dontSendNotification);
        zoomSlider.setValue(1.0, juce::dontSendNotification);
        zoomPercentLabel.setText("100%", juce::dontSendNotification);
        return;
    }

    const int imgW = canvas.getCanvasImage().getWidth();
    const int imgH = canvas.getCanvasImage().getHeight();
    imageSizeLabel.setText(juce::String(imgW) + _("×") + juce::String(imgH) + "px",
        juce::dontSendNotification);

    if (canvas.isCursorOverImg()) {
        auto cursorImgPos = canvas.getCursorImgPos();
        cursorPosLabel.setText(
            juce::String(static_cast<int>(cursorImgPos.x + 1)) + _("×") +
            juce::String(static_cast<int>(cursorImgPos.y + 1)),
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


