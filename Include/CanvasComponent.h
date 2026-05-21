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
    void setBgColour(juce::Colour col);
    void setTool(PEnums::CanvasTool tool);
    void resetBrushDraw();
    void setBrushSize(float size) { brushSize = size; }

    void clearCanvas();

    juce::Point<float> getCursorImgPos() const { return camera.cnv2img(lastPos); };
    bool isCursorOverImg() const { return canvasImage.getBounds().contains(camera.cnv2img(lastPos).toInt()); };
    float getZoom() const { return camera.getZoom(); };
    void setZoom(float v) {
        auto oldRect = canvasImageRect;
        auto oldZ = camera.getZoom();
        camera.setZoom(v); 
        resizeImageRect();
        updateScrollbars();
        if (oldZ > v) repaint(oldRect.expanded(50).toNearestInt());
        else repaint(canvasImageRect.expanded(50).toNearestInt());
    };
    juce::Range<float> getZoomRange() const { return camera.getZoomRange(); }
    juce::Image getCanvasImage() const { return canvasImage; }
    juce::Image getImage() const { 
        juce::Image newImg{ juce::Image::ARGB, canvasImage.getWidth(), canvasImage.getHeight(), false };
        newImg.clear(canvasImage.getBounds(), bgColour);
        juce::Graphics g(newImg);
        g.drawImage(canvasImage, newImg.getBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
        return newImg;
    }
    void setCanvasImage(const juce::Image& img) { canvasImage = std::move(img); resized(); }
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
    void redrawImageWithTransform(int w, int h, const juce::AffineTransform& t);
    void repaintLine(juce::Point<float> start, juce::Point<float> end, float expand, bool needBigStart = false, bool needBigEnd = false);
    void repaintEllipse(juce::Rectangle<float> rect, float expand);
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    juce::ApplicationCommandTarget *getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID> &c) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo &info) override;

    void scrollBarMoved(juce::ScrollBar* scrollBar, double newStart) override;
    void updateScrollbars();
    void updateScrollBarLayout();

    void floodFill(juce::Point<int> p, juce::uint8 tolerance = 0);

    static bool isColorMatch(juce::Colour c1, juce::Colour c2, juce::uint8 tolerance);

    std::function<void()> onCameraChanged;
    std::function<void()> onImageChanged;

private:

    static const enum CanvasFlags {
        Drawing     = 0x01,
        Panning     = 0x02,
        NeedBrush   = 0x04,
        Dragging    = 0x08,
        Selecting   = 0x10,
    };

    juce::uint32 flags = 0;

    bool hasFlag(const juce::uint32 f) { return (bool)(flags & f); }
    void setFlag(const juce::uint32 f, bool b = true) { 
        if (b) flags |= f;
        else flags &= ~f;
    }

    PEnums::CanvasTool selectedTool = PEnums::CanvasTool::Brush;
    Camera camera;

    juce::ScrollBar horizontalScroll{ false };
    juce::ScrollBar verticalScroll{ true };
    void scrollbarPan(bool isHorizontal, bool isPositive);

    void drawSigleClick(juce::Graphics& g);
    void drawCurrentPath(juce::Graphics& g);
    void eraseLine(const juce::Point<float>& to);
    void repaintUnderCursor(juce::Point<float> pos);

    void repaintToCursor(juce::Point<float> pos);

    juce::Image canvasImage{juce::Image::PixelFormat::ARGB, 200, 200, true};
    juce::Rectangle<float> canvasImageRect{0,0,0,0};
    juce::Rectangle<int> selection{0,0,0,0};

    void resizeImageRect() {
        auto bImg = canvasImage.getBounds().toFloat();
        canvasImageRect = juce::Rectangle<float>(camera.img2cnv(bImg.getTopLeft()), camera.img2cnv(bImg.getBottomRight()));
    }

    juce::Colour currentColour{juce::Colours::black};
    juce::Colour bgColour{juce::Colours::white};
    float brushSize{2.0f};

    juce::Point<float> imgStartPos;
    juce::Point<float> imgLastPos;
    juce::Point<float> startPos;
    juce::Point<float> lastPos;

    juce::Path currentStroke;

    void showCanvasSizeDialog();
    void applyCanvasResize(int newW, int newH, int offsetX, int offsetY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CanvasComponent)
};