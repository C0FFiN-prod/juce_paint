#include "../Include/ColorSelectorComponent.h"
#include "../Include/ToolbarComponent.h"
#include "../Include/ColorPickerPopup.h"

void ColorSelectorComponent::setForegroundColor(juce::Colour col)
{
    fgColor = col;
    repaint();
}
void ColorSelectorComponent::setBackgroundColor(juce::Colour col)
{
    bgColor = col;
    repaint();
}

void ColorSelectorComponent::paint(juce::Graphics &g)
{
    static float tile = 4, cornerSize = 5.0f;
    static auto c0 = juce::Colour(0xFFE5E5E5), c1 = juce::Colour(0xFFFFFFFF), cb = juce::Colour(0xFFBEBEBE);

    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colours::transparentWhite);
    g.fillAll();

    float size = bounds.getWidth() * 0.65f;
    float sizeH = size * 0.5f;
    float bgX = bounds.getX();
    float bgY = bounds.getY();

    juce::Path bg;
    juce::Rectangle bgRect(bgX, bgY, size, size);
    g.saveState();
    g.reduceClipRegion(bgRect.toNearestInt());
    g.fillCheckerBoard(bgRect.reduced(1.f), tile, tile, c0, c1);
    g.setColour(bgColor.withAlpha(1.f));
    bg.addRoundedRectangle(bgX, bgY, sizeH, size, cornerSize, cornerSize, true, false, true, false);
    g.fillPath(bg);
    bg.clear();
    g.setColour(bgColor);
    bg.addRoundedRectangle(bgX + sizeH, bgY, sizeH, size, cornerSize, cornerSize, false, true, false, true);
    g.fillPath(bg);
    bg.clear();
    g.setColour(cb);
    bg.addRoundedRectangle(bgX, bgY, size, size, cornerSize);
    g.strokePath(bg, juce::PathStrokeType(1.f));
    g.restoreState();

    float fgX = bounds.getRight() - size;
    float fgY = bounds.getBottom() - size;

    juce::Path fg;
    juce::Rectangle fgRect(fgX, fgY, size, size);
    g.saveState();
    g.reduceClipRegion(fgRect.toNearestInt());
    g.fillCheckerBoard(fgRect.reduced(1.f), tile, tile, c0, c1);
    g.setColour(fgColor.withAlpha(1.f));
    fg.addRoundedRectangle(fgX, fgY, sizeH, size, cornerSize, cornerSize, true, false, true, false);
    g.fillPath(fg);
    fg.clear();
    g.setColour(fgColor);
    fg.addRoundedRectangle(fgX + sizeH, fgY, sizeH, size, cornerSize, cornerSize, false, true, false, true);
    g.fillPath(fg);
    fg.clear();
    g.setColour(cb);
    fg.addRoundedRectangle(fgX, fgY, size, size, cornerSize);
    g.strokePath(fg, juce::PathStrokeType(1.f));
    g.restoreState();

    // float swapSize = bounds.getWidth() * 0.35f;
    // float swapX = bounds.getX() + 1.0f;
    // float swapY = bounds.getBottom() - swapSize - 1.0f;

    // g.setColour(juce::Colour(0xFF222222));
    // g.setFont(juce::Font(11.0f, juce::Font::plain));
    // g.drawText("+", swapX, swapY, swapSize, swapSize, juce::Justification::centred);
}

void ColorSelectorComponent::mouseDown(const juce::MouseEvent &e)
{
    colorPickerConstrainer.setMinimumSize(200, 250);
    colorPickerConstrainer.setFixedAspectRatio(200.0 / 250.0);
    auto bI = getBounds();
    auto b = getLocalBounds().toFloat();
    float sz = b.getWidth() * 0.65f;

    juce::Rectangle<float> fgRect(b.getRight() - sz, b.getBottom() - sz, sz, sz);
    juce::Rectangle<float> bgRect(b.getX(), b.getY(), sz, sz);
    juce::Rectangle<float> swapRect(b.getX(), b.getBottom() - sz * 0.5f, sz * 0.5f, sz * 0.5f);

    if (fgRect.contains(e.position))
    {
        auto popup = std::make_unique<ColorPickerPopup>(fgColor, [this](juce::Colour c)
                                                        {
                                                            fgColor = c;
                                                            repaint();
                                                            if (onForegroundChanged)
                                                                onForegroundChanged(c);
                                                            // juce::LookAndFeel::setDefaultLookAndFeel(&oldLNF);
                                                        });

        juce::CallOutBox::launchAsynchronously(std::move(popup), bI, getTopLevelComponent());
    }
    else if (bgRect.contains(e.position))
    {
        auto popup = std::make_unique<ColorPickerPopup>(bgColor, [this](juce::Colour c)
                                                        {
                                                            bgColor = c;
                                                            repaint();
                                                            if (onBackgroundChanged)
                                                                onBackgroundChanged(c);
                                                            // juce::LookAndFeel::setDefaultLookAndFeel(&oldLNF);
                                                        });

        juce::CallOutBox::launchAsynchronously(std::move(popup), bI, getTopLevelComponent());
    }
    /*else if (swapRect.contains(e.position))
    {
        std::swap(fgColor, bgColor);
        repaint();
        if (onSwapColors)
            onSwapColors();
    }*/
}