#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/Geometry.h"
#include "../Include/Enums.h"

class CanvasComponent : public juce::Component, public juce::ApplicationCommandTarget, public juce::ScrollBar::Listener
{
public:
    CanvasComponent();
    ~CanvasComponent() override = default;

    void setCurrentColour(juce::Colour col) { currentColour = col; }
    void setBgColour(juce::Colour col) { bgColour = col; repaint(); }
    void setTool(PEnums::CanvasTool tool) { selectedTool = tool; }
    void setBrushSize(float size) { brushSize = size; }

    void clearCanvas();

    juce::Point<float> getCursorImgPos() const { return camera.cnv2img(lastPos); };
    bool isCursorOverImg() const { return canvasImage.getBounds().contains(camera.cnv2img(lastPos).toInt()); };
    float getZoom() const { return camera.getZoom(); };
    void setZoom(float v) { camera.setZoom(v); updateScrollbars(); };
    juce::Range<float> getZoomRange() const { return camera.getZoomRange(); }
    juce::Image getCanvasImage() const { return canvasImage; }
    juce::Rectangle<int> getImageRect() const { return canvasImage.getBounds(); }
    juce::Rectangle<int> getZoomedImageRect() const { return canvasImage.getBounds().withCentre(getLocalBounds().getCentre()) * camera.getZoom(); }

    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseExit(const juce::MouseEvent &e) override;
    void mouseEnter(const juce::MouseEvent &e) override;
    void mouseDown(const juce::MouseEvent &e) override;
    void startPanning(juce::Point<float> startPos);
    void mouseMove(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void repaintLine(juce::Point<float> start, juce::Point<float> end, float maxSegmentLength = 50.f);
    void repaintEllipseFromRect(const juce::Rectangle<float>& rect, float expand);
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    juce::ApplicationCommandTarget *getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID> &c) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo &info) override;

    void scrollBarMoved(juce::ScrollBar* scrollBar, double newStart) override;
    void updateScrollbars();
    void updateScrollBarLayout();

    std::function<void()> onCameraChanged;

private:
    PEnums::CanvasTool selectedTool = PEnums::CanvasTool::Brush;
    Camera camera;

    juce::ScrollBar horizontalScroll{ false };
    juce::ScrollBar verticalScroll{ true };
    void scrollbarPan(bool isHorizontal, bool isPositive);

    void drawSigleClick(juce::Graphics& g);
    void drawCurrentPath(juce::Graphics& g);
    void repaintUnderCursor(juce::Point<float> pos);

    void repaintToCursor(juce::Point<float> pos);

    bool isPanning{ false };

    juce::Image canvasImage{juce::Image::PixelFormat::ARGB, 200, 200, true};
    juce::Rectangle<int> canvasImageBounds{canvasImage.getBounds().toNearestInt()};

    juce::Colour currentColour{juce::Colours::black};
    juce::Colour bgColour{juce::Colours::white};
    float brushSize{2.0f};

    bool needBrushDraw{false};
    bool isDrawing{false};
    juce::Point<float> imgStartPos;
    juce::Point<float> startPos;
    juce::Point<float> lastPos;

    juce::Path currentStroke;

    void showCanvasSizeDialog();
    void applyCanvasResize(int newW, int newH, int offsetX, int offsetY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CanvasComponent)
};