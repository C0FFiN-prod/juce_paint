#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Generated/IconsEmbedded.h"

namespace PEnums {
	namespace Colours {
		const juce::Colour Yellow{ 0xFFFFBB00 };
		const juce::Colour Violet{ 0xFF8821f7 };
		const juce::Colour White{ 0xFFFFFFFF };
		const juce::Colour Gray1{ 0xFFe5e5e5 };
		const juce::Colour Gray2{ 0xFFd1d1d1 };
		const juce::Colour Gray3{ 0xFFbebebe };
		const juce::Colour Text{ 0xFF191919 };
		const juce::DropShadow Shadow{ juce::Colours::black.withAlpha(0.25f), 10, { 0, 0 } };
	};

	namespace ToolBtnStyles {
		struct ToolBtnStyle {
			juce::Colour bgColor;
			juce::Colour strokeColor;
			juce::Colour cornerColor = Colours::Gray1;
			juce::Colour iconColor = Colours::Text;
		};

		const ToolBtnStyle Default{ Colours::White, Colours::White };
		const ToolBtnStyle Hover{ Colours::White, Colours::Gray3 };
		const ToolBtnStyle Pressed{ Colours::Gray2, Colours::Gray3 };
		const ToolBtnStyle Active{ Colours::Yellow, Colours::Yellow, Colours::White };
		const ToolBtnStyle Disabled{ Colours::Gray1, Colours::Gray2, Colours::Gray3, Colours::Gray3 };
	};

	static const enum CanvasTool {
		Cursor,
		Pencil,
		Brush,
		Eraser,
		Line,
		OneDirArrow,
		BiDirArrow,
		Rect,
		Ellipse,
		Triangle,
		Text,
		Fill,
	};

	static const enum CommandIDs {
		CanvasResize = 30001,
		CanvasFlipH,
		CanvasFlipV,
		CanvasRotate90CW,
		CanvasRotate90CC,
		CanvasClear,
		FileNew,
		FileOpen,
		FileSave,
		FileSaveAs,
		AppQuit,
	};

	namespace Icons {
		const EmbeddedIcon Triangle = IconsData::Triangle;
		const EmbeddedIcon BiDirArrow = IconsData::BiDirArrow;
		const EmbeddedIcon Brush = IconsData::Brush;
		const EmbeddedIcon Cursor = IconsData::Cursor;
		const EmbeddedIcon Ellipse = IconsData::Ellipse;
		const EmbeddedIcon Erase = IconsData::Erase;
		const EmbeddedIcon Fill = IconsData::Fill;
		const EmbeddedIcon Line = IconsData::Line;
		const EmbeddedIcon OneDirArrow = IconsData::OneDirArrow;
		const EmbeddedIcon Pencil = IconsData::Pencil;
		const EmbeddedIcon Rect = IconsData::Rect;
		const EmbeddedIcon Size = IconsData::Size;
		const EmbeddedIcon SizeF = IconsData::SizeF;
		const EmbeddedIcon Text = IconsData::Text;

		static std::unique_ptr<juce::Drawable> create(const EmbeddedIcon& icon) {
			return juce::Drawable::createFromImageData(icon.data, icon.size);
		}

		static void draw(juce::Graphics& g,
			const EmbeddedIcon* icon,
			juce::Rectangle<float> targetBounds, 
			juce::Colour& color,
			float opacity = 1.0f)
		{
			auto drawable = create(*icon);
			if (!drawable) return;

			const float srcX = icon->hasViewBox ? icon->viewBoxX : 0.0f;
			const float srcY = icon->hasViewBox ? icon->viewBoxY : 0.0f;
			const float srcW = icon->hasViewBox ? icon->viewBoxW : drawable->getDrawableBounds().getWidth();
			const float srcH = icon->hasViewBox ? icon->viewBoxH : drawable->getDrawableBounds().getHeight();

			if (srcW <= 0 || srcH <= 0) return;

			const float scaleX = targetBounds.getWidth() / srcW;
			const float scaleY = targetBounds.getHeight() / srcH;
			const float scale = juce::jmin(scaleX, scaleY);

			const float scaledW = srcW * scale;
			const float scaledH = srcH * scale;
			const float dx = targetBounds.getX() + (targetBounds.getWidth() - scaledW) * 0.5f - srcX * scale;
			const float dy = targetBounds.getY() + (targetBounds.getHeight() - scaledH) * 0.5f - srcY * scale;

			drawable->replaceColour(juce::Colour(0xFF222222), color);
			g.saveState();
			g.addTransform(juce::AffineTransform::translation(dx, dy).scaled(scale));
			drawable->draw(g, opacity);
			g.restoreState();
		}
	}
}