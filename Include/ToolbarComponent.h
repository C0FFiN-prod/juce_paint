#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/ToolButtonComponent.h"
#include "../Include/ColorSelectorComponent.h"
#include "../Include/ColorPaletteComponent.h"
#include "../Include/Separator.h"

class ToolbarComponent : public juce::Component
{
public:
    ToolbarComponent();

    void paint(juce::Graphics &g) override;
    void resized() override;

    void swapColors() { std::swap(fgColor, bgColor); }

    std::function<void(juce::Colour, bool)> onColorChanged;
    std::function<void()> onSwapColors;
    std::function<void(float)> onBrushSizeChanged;
    std::function<void(PEnums::CanvasTool)> onToolChanged;

private:
    PEnums::CanvasTool selectedTool = PEnums::CanvasTool::Brush;
    juce::Colour fgColor;
    juce::Colour bgColor;
    float brushSize = 2;

    ToolButtonComponent* btnCursor;
    ToolButtonComponent* btnBrushSize;
    ToolButtonComponent* btnBrush;
    ToolButtonComponent* btnEraser;
    ToolButtonComponent* btnFill;
    ToolButtonComponent* btnShapes;
    ToolButtonComponent* btnLines;
    ToolButtonComponent* btnText;
    OwnedArray<ToolButtonComponent> btns;


    std::unique_ptr<Separator> separatorLine;
    std::unique_ptr<ColorSelectorComponent> colorSelector;
    ToolButtonComponent* addToolButton(ToolButtonComponent::Type type, const EmbeddedIcon& icon);
    void resetBtnsExcept(ToolButtonComponent* btn);
};