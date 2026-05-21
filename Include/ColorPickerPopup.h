#pragma once
//
#include "../JuceLibraryCode/JuceHeader.h"

class ColorPickerPopup : public juce::Component, public juce::TextEditor::Listener
{
public:
    ColorPickerPopup(juce::Colour initial, std::function<void(juce::Colour)> onPick);
    ~ColorPickerPopup() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent &e) override;
    void mouseDown(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &) override;

    void textEditorReturnKeyPressed(juce::TextEditor &) override;
    void textEditorFocusLost(juce::TextEditor &) override;

private:
    enum ColorSource
    {
        None,
        Square,
        HUE,
        Alpha,
        HEX,
        Swatch,
        Eyedropped,
    };

    void colourChanged(juce::Colour &c, ColorSource s);
    void applyHex();
    void updateFromSquare(juce::Point<int> p);
    void updateFromHue(juce::Point<int> p);
    void updateFromAlpha(juce::Point<int> p);
    void updateFromSwatch(juce::Point<int> p);

    void paintColourSquare(juce::Graphics &g);
    void paintHueSlider(juce::Graphics &g);
    void paintAlphaSlider(juce::Graphics &g);
    void paintEyedropper(juce::Graphics &g);
    void paintSwatchGrid(juce::Graphics &g);
    void initSwatches();

    float currentHUE = 0;
    float currentAlpha = 1;
    float currentSqX = 0;
    float currentSqY = 0;
    static char currentSwatch;
    juce::Colour currentColour;
    std::function<void(juce::Colour)> callback;
    static juce::Colour swatches[32];
    static bool isFirstRun;

    juce::Rectangle<float> sqBounds, hueBounds, alphaBounds, eyeBounds, hexBounds, swatchBounds;
    bool draggingSq = false, draggingHue = false, draggingAlpha = false;
    bool eyeHovered = false, eyePressed = false, eyePicking = false;

    juce::TextEditor hexEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorPickerPopup)
};