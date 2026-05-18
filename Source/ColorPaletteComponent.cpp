#pragma once
//
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Include/ColorPaletteComponent.h"

ColorPaletteComponent::ColorPaletteComponent()
{
	// �������������� ������� (����� ������� � ��������� ������ ������)
	for (int i = 0; i < 32; ++i)
		colors[i] = juce::Colour::fromFloatRGBA(
			juce::Random::getSystemRandom().nextFloat(),
			juce::Random::getSystemRandom().nextFloat(),
			juce::Random::getSystemRandom().nextFloat(),
			1.0f
		);
}

void ColorPaletteComponent::paint(juce::Graphics& g)
{
	float size = 12.0f;
	float gap = 2.0f;
	int cols = 8;

	for (int i = 0; i < 32; ++i)
	{
		int row = i / cols;
		int col = i % cols;

		juce::Rectangle<float> rect(
			col * (size + gap),
			row * (size + gap),
			size, size
		);

		g.setColour(colors[i]);
		g.fillRect(rect);
		g.setColour(juce::Colours::grey);
		g.drawRect(rect, 0.5f);
	}
}

void ColorPaletteComponent::mouseDown(const juce::MouseEvent& e)
{
	float size = 12.0f;
	float gap = 2.0f;
	int cols = 8;

	int col = static_cast<int>(e.x / (size + gap));
	int row = static_cast<int>(e.y / (size + gap));

	if (col >= 0 && col < cols && row >= 0 && row < 4)
	{
		int index = row * cols + col;
		
		if (onColorSelected) onColorSelected(colors[index]);
	}
}