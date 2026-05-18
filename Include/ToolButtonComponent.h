#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/Enums.h"

class ToolButtonComponent : public juce::Component
{
public:
    enum class State
    {
        Default,
        Hover,
        Pressed,
        Active,
        Disabled
    };
    enum class Type
    {
        Single,
        Multiple
    };

    ToolButtonComponent(Type buttonType = Type::Single);

    void setState(State newState);
    State getState() const;
    void setType(Type newType);
    void setIcon(const EmbeddedIcon& newIcon) { icon = &newIcon; }

    void paint(juce::Graphics &g) override;
    void mouseEnter(const juce::MouseEvent &) override;
    void mouseExit(const juce::MouseEvent &) override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseUp(const juce::MouseEvent &) override;
    std::function<void()> callback;
    int data = 0;
private:
    void drawIcon(juce::Graphics &g, juce::Colour col);
    void drawCorner(juce::Graphics &g, juce::Colour col);

    const EmbeddedIcon* icon = nullptr;
    State state = State::Default;
    Type type = Type::Single;
};