#include "TriangleBrushSlider.h"

TriangleBrushSlider::TriangleBrushSlider(float value) : currentValue(value)
{
    setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    
}

void TriangleBrushSlider::paint(juce::Graphics &g)
{
    auto area = getLocalBounds().toFloat();

    juce::Path trianglePath = createRoundedTriangle(area);
    g.setColour(juce::Colour(0xFFE0E0E0)); // ‘ветло-серый фон трека
    g.fillPath(trianglePath);
    g.setColour(juce::Colour(0xFFBEBEBE)); // ѓраница
    g.strokePath(trianglePath, juce::PathStrokeType(1.0f));

    float normalised = juce::jmap(currentValue, valueRange.getStart(), valueRange.getEnd(), 0.0f, 1.0f);
    float handleMaxRadius = valueRange.getEnd() / 2;
    float handleMinRadius = valueRange.getStart() / 2;

    float trackLeft = handleMinRadius;
    float trackRight = area.getWidth() - handleMaxRadius;
    float handleX = trackLeft + normalised * (trackRight - trackLeft);
    float handleY = area.getCentreY();

    float handleRadius = currentValue / 2;

    g.setColour(juce::Colour(0xFF4A4A4A));
    g.fillEllipse(handleX - handleRadius, handleY - handleRadius, handleRadius * 2.0f, handleRadius * 2.0f);

    g.setColour(juce::Colour(0xFF1A1A1A));
    g.drawEllipse(handleX - handleRadius, handleY - handleRadius, handleRadius * 2.0f, handleRadius * 2.0f, 1.0f);
}

void TriangleBrushSlider::mouseDown(const juce::MouseEvent &e)
{
    updateFromMouse(e);
}

void TriangleBrushSlider::mouseDrag(const juce::MouseEvent &e)
{
    updateFromMouse(e);
}

float TriangleBrushSlider::getValue() const
{
    return currentValue;
}

void TriangleBrushSlider::setValue(float newValue, bool notify)
{
    currentValue = juce::jlimit(valueRange.getStart(), valueRange.getEnd(), newValue);
    if (notify && onValueChange)
        onValueChange(currentValue);
    repaint();
}

void TriangleBrushSlider::updateFromMouse(const juce::MouseEvent &e)
{
    float handleMaxRadius = valueRange.getEnd() / 2;
    float handleMinRadius = valueRange.getStart() / 2;
    float trackLeft = handleMinRadius;
    float trackRight = getWidth() - handleMaxRadius;
    float normalised = juce::jlimit(0.0f, 1.0f, (e.x - trackLeft) / (trackRight - trackLeft));
    float newValue = juce::jmap(normalised, valueRange.getStart(), valueRange.getEnd());
    setValue(newValue);
}

juce::Path TriangleBrushSlider::createRoundedTriangle(juce::Rectangle<float> bounds) const
{
    juce::Path path;
    const float r = valueRange.getEnd() / 2;

    juce::Point<float> pL(bounds.getX(), bounds.getCentreY());
    juce::Point<float> pTRr(bounds.getRight() - r, bounds.getCentreY() - r);
    juce::Point<float> pTLr(bounds.getRight() - 2 * r, bounds.getCentreY() - r);
    juce::Point<float> pTCr(bounds.getRight() - r, bounds.getCentreY() - r);

    path.startNewSubPath(pTCr);
    path.addArc(pTLr.x, pTLr.y, r * 2.0f, r * 2.0f, 0, juce::MathConstants<float>::pi);
    path.lineTo(pL);
    path.lineTo(pTRr);
    path.closeSubPath();
    return path;
}