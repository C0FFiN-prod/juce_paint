#include "../Include/CanvasComponent.h"
#include "../Include/CanvasSizeComponent.h"
#include "../Include/Application.h"

#define _(x) juce::String::fromUTF8(u8#x)

CanvasComponent::CanvasComponent(): camera(*this)
{
	juce::Graphics g(canvasImage);
	g.fillAll(juce::Colours::transparentWhite);

	auto* commandManager = PracticeApplication::getInstance()->getCommandManager();
	commandManager->registerAllCommandsForTarget(this);

	setOpaque(true);
	setWantsKeyboardFocus(true);
	setMouseClickGrabsKeyboardFocus(true);
	// setRepaintsOnMouseActivity(true);

	addAndMakeVisible(horizontalScroll);
	addAndMakeVisible(verticalScroll);

	horizontalScroll.setColour(juce::ScrollBar::ColourIds::thumbColourId, juce::Colour(0xFFBEBEBE));
	horizontalScroll.setRangeLimits(0.0, 1.0);
	horizontalScroll.setCurrentRange(0.0, 1.0);

	verticalScroll.setColour(juce::ScrollBar::ColourIds::thumbColourId, juce::Colour(0xFFBEBEBE));
	verticalScroll.setRangeLimits(0.0, 1.0);
	verticalScroll.setCurrentRange(0.0, 1.0);

	horizontalScroll.addListener(this);
	verticalScroll.addListener(this);
}

void CanvasComponent::clearCanvas()
{
	if (canvasImage.isValid())
	{
		juce::Graphics g(canvasImage);
		g.fillAll(juce::Colours::transparentWhite);
		repaint();
	}
}

void CanvasComponent::paint(juce::Graphics& g)
{
	g.fillAll(PEnums::Colours::Gray1);


	g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::lowResamplingQuality);
	auto bImg = canvasImage.getBounds().toFloat();
	auto bImgCnv = juce::Rectangle<float>(camera.img2cnv(bImg.getTopLeft()), camera.img2cnv(bImg.getBottomRight()));

	g.reduceClipRegion(getLocalBounds());

	PEnums::Colours::Shadow.drawForRectangle(g, bImgCnv.toNearestInt());
	g.setOpacity(1);
	if (bgColour.getAlpha() != 255) g.fillCheckerBoard(bImgCnv, 5*camera.getZoom(), 5*camera.getZoom(), PEnums::Colours::White, PEnums::Colours::Gray2);
	g.setColour(bgColour);
	g.fillRect(bImgCnv);
	g.setOpacity(1);
	g.drawImage(canvasImage, bImgCnv, juce::RectanglePlacement::stretchToFit, false);
	if (isDrawing && !currentStroke.isEmpty())
	{
		auto transform = camera.getTransformImg2Cnv();
		g.saveState();
		g.addTransform(transform);
		drawCurrentPath(g);
		//g.strokePath(currentStroke, stroke);
		g.addTransform(juce::AffineTransform());
		g.restoreState();
	}
	if (needBrushDraw)
	{
		float r = brushSize * 0.5f * camera.getZoom();
		g.setColour(currentColour);
		switch (selectedTool) {
		case PEnums::CanvasTool::Pencil:
			g.fillRect(lastPos.x - r, lastPos.y - r, brushSize * camera.getZoom(), brushSize * camera.getZoom());
			break;
		case PEnums::CanvasTool::Brush:
			g.fillEllipse(lastPos.x - r, lastPos.y - r, brushSize * camera.getZoom(), brushSize * camera.getZoom());
			break;
		}
	}
}

void CanvasComponent::drawSigleClick(juce::Graphics& g) {
	float expand = brushSize + 5.0f;
	g.setColour(currentColour);
	auto p = camera.cnv2img(lastPos);
	switch (selectedTool) {
	case PEnums::CanvasTool::Pencil:
		g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::lowResamplingQuality);
		g.fillRect((int)(p.x - brushSize * 0.5f),
			(int)(p.y - brushSize * 0.5f),
			(int)brushSize, (int)brushSize);
		break;
	case PEnums::CanvasTool::Brush:
		g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::highResamplingQuality);
		g.fillEllipse(p.x - brushSize * 0.5f,
			p.y - brushSize * 0.5f,
			brushSize, brushSize);
		break;
	}
	repaint(p.x - expand, p.y - expand, expand * 2.0f, expand * 2.0f);
}

void CanvasComponent::drawCurrentPath(juce::Graphics& g) {
	juce::PathStrokeType stroke{0};
	g.reduceClipRegion(canvasImage.getBounds());
	g.setColour(currentColour);
	switch (selectedTool) {
	case PEnums::CanvasTool::Pencil:
		g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::lowResamplingQuality);
		stroke.setStrokeThickness((int)brushSize);
		stroke.setJointStyle(juce::PathStrokeType::mitered);
		stroke.setEndStyle(juce::PathStrokeType::square);
		break;
	default:
		g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::highResamplingQuality);
		stroke.setStrokeThickness(brushSize);
		stroke.setJointStyle(juce::PathStrokeType::curved);
		stroke.setEndStyle(juce::PathStrokeType::rounded);
		break;
	}

	g.strokePath(currentStroke, stroke);
}

void CanvasComponent::repaintUnderCursor(juce::Point<float> pos)
{
	float dirtyR = std::max(brushSize * camera.getZoom() + 5.0f, 20.0f);
	repaint(pos.x - dirtyR, pos.y - dirtyR, dirtyR * 2.0f, dirtyR * 2.0f);
	lastPos = pos;
}

void CanvasComponent::repaintToCursor(juce::Point<float> pos)
{
	float dirtyR = std::max(brushSize * camera.getZoom() + 5.0f, 20.0f);
	repaint(juce::Rectangle<float>(lastPos, pos)
		.expanded(dirtyR).toNearestInt());
	lastPos = pos;
}

void CanvasComponent::resized()
{
	camera.resized();
	if (onCameraChanged) onCameraChanged();
	updateScrollbars();
}

void CanvasComponent::mouseExit(const juce::MouseEvent& e)
{
	lastPos = juce::Point<float>(-1000, -1000);
	if (!isPanning)
		setMouseCursor(juce::MouseCursor::ParentCursor);
	repaint();
}

void CanvasComponent::mouseEnter(const juce::MouseEvent& e)
{
	setMouseCursor(juce::MouseCursor::CrosshairCursor);
	repaint();
}

void CanvasComponent::mouseDown(const juce::MouseEvent& e)
{
	auto currentPos = e.position.toFloat();
	if (e.mods.isRightButtonDown())
	{
		isDrawing = false;
		currentStroke.clear();
		repaint();
		
		return;
	}

	if (e.mods.isMiddleButtonDown()) 
	{
		startPanning(currentPos);
	}

	if (e.mods.isLeftButtonDown()) {
		isDrawing = true;
		startPos = currentPos;
		imgStartPos = camera.cnv2img(currentPos);
		currentStroke.clear();

		switch (selectedTool) {
		case PEnums::CanvasTool::Pencil:
			currentStroke.startNewSubPath(imgStartPos.roundToInt().toFloat());
			break;
		case PEnums::CanvasTool::Brush:
			currentStroke.startNewSubPath(imgStartPos);
			break;
		}

	}
	repaintUnderCursor(currentPos);
}

void CanvasComponent::startPanning(juce::Point<float> startPos)
{
	isPanning = true;
	needBrushDraw = false;
	lastPos = startPos;
	setMouseCursor(juce::MouseCursor::DraggingHandCursor);
}

void CanvasComponent::mouseMove(const juce::MouseEvent& e)
{
	if (isPanning || isDrawing) return;

	repaintToCursor(e.position);
	if (onCameraChanged) onCameraChanged();
}

void CanvasComponent::mouseDrag(const juce::MouseEvent& e)
{
	if (e.mods.currentModifiers.isRightButtonDown())
	{
		isDrawing = false;

		auto bImg = currentStroke.getBounds().toFloat().expanded(brushSize + 5);
		auto bImgCnv = juce::Rectangle<float>(camera.img2cnv(bImg.getTopLeft()), camera.img2cnv(bImg.getBottomRight()));
		currentStroke.clear();
		repaint(bImgCnv.toNearestInt());
		return;
	}

	auto currentPos = e.position.toFloat();
	if (isPanning)
	{
		auto delta = currentPos - lastPos;
		camera.moveBy(delta);
		updateScrollbars();
		auto bImg = canvasImage.getBounds().toFloat();
		auto bImgCnv = juce::Rectangle<float>(camera.img2cnv(bImg.getTopLeft()), camera.img2cnv(bImg.getBottomRight()));

		repaintToCursor(currentPos);

		repaint(bImgCnv.expanded(std::abs(delta.x) + camera.wiggle, std::abs(delta.y) + camera.wiggle).toNearestInt());
		if (onCameraChanged) onCameraChanged();
		return;
	}

	if (isDrawing) {




		auto newImgPos = camera.cnv2img(currentPos);
		if (selectedTool == PEnums::CanvasTool::Pencil) {
			float distance = 0;

			newImgPos = newImgPos.roundToInt().toFloat();
			if ((int)brushSize % 2 == 1) newImgPos.addXY(0.5f, 0.5f);

			distance = currentStroke.getCurrentPosition().getDistanceFrom(newImgPos);

			if (distance < 1.f) {
				lastPos = currentPos;
				if (onCameraChanged) onCameraChanged();
				return;
			}
			currentStroke.lineTo(newImgPos);
			repaintToCursor(currentPos);
		} else if (selectedTool == PEnums::CanvasTool::Brush) {
			currentStroke.lineTo(newImgPos);
			repaintToCursor(currentPos);
		}
		else {
			auto imgMidPos = (imgStartPos + newImgPos) / 2;
			//auto line = juce::Line(imgStartPos, imgLastPos);
			auto rect = juce::Rectangle(imgStartPos, newImgPos);
			float arrowSize = std::max(5.f, brushSize);
			float arrowStrokeSize = 0.5f;
			float expand = (brushSize + 5) * camera.getZoom();
			currentStroke.clear();
			switch (selectedTool) {
			case PEnums::CanvasTool::Pencil:
			case PEnums::CanvasTool::Brush:
				break;
			case PEnums::CanvasTool::Line:
				currentStroke.startNewSubPath(imgStartPos);
				currentStroke.lineTo(newImgPos);
				repaintLine(startPos, lastPos);
				break;
			case PEnums::CanvasTool::BiDirArrow:
				currentStroke.addArrow(juce::Line(imgMidPos, imgStartPos), arrowStrokeSize, arrowSize, arrowSize);
				currentStroke.addArrow(juce::Line(imgMidPos, newImgPos), arrowStrokeSize, arrowSize, arrowSize);
				repaintLine(startPos, lastPos);
				break;
			case PEnums::CanvasTool::OneDirArrow:
				currentStroke.addArrow(juce::Line(imgStartPos, newImgPos), arrowStrokeSize, arrowSize, arrowSize);
				repaintLine(startPos, lastPos);
				break;
			case PEnums::CanvasTool::Rect:
				currentStroke.addRectangle(rect);
				repaint(juce::Rectangle(currentPos.withX(startPos.x), lastPos).expanded(expand).toNearestInt());
				repaint(juce::Rectangle(lastPos, currentPos.withY(startPos.y)).expanded(expand).toNearestInt());
				break;
			case PEnums::CanvasTool::Ellipse:
				currentStroke.addEllipse(rect);
				repaintEllipseFromRect(juce::Rectangle(startPos, currentPos), expand);
				repaintEllipseFromRect(juce::Rectangle(startPos, lastPos), expand);
				//repaintEllipseFromRect(juce::Rectangle(startPos, lastPos), expand);
				//repaint(juce::Rectangle(startPos, currentPos).expanded(expand).toNearestInt());
				//repaint(juce::Rectangle(currentPos.withX(startPos.x), lastPos).expanded(expand).toNearestInt());
				//repaint(juce::Rectangle(lastPos, currentPos.withY(startPos.y)).expanded(expand).toNearestInt());
				break;
			case PEnums::CanvasTool::Triangle:
				currentStroke.addTriangle(
					rect.getX(), rect.getBottom(),
					rect.getCentreX(), rect.getY(),
					rect.getRight(), rect.getBottom()
				);
				rect = { startPos, currentPos };
				repaintLine({ rect.getX(), rect.getBottom() }, { rect.getCentreX(), rect.getY() });
				repaintLine({ rect.getRight(), rect.getBottom() }, { rect.getCentreX(), rect.getY() });
				repaint(juce::Rectangle<float>(
					{ rect.getX(), rect.getBottom() }, { rect.getRight(), rect.getBottom() }
				).expanded(expand).toNearestInt());
				//repaint(juce::Rectangle(startPos, currentPos).expanded(expand).toNearestInt());
				break;
			}
		}
		
	}
	lastPos = currentPos;
	if (onCameraChanged) onCameraChanged();
}


void CanvasComponent::repaintLine(juce::Point<float> start,
	juce::Point<float> end,
	float maxSegmentLength)
{
	const float dx = end.getX() - start.getX();
	const float dy = end.getY() - start.getY();
	const float length = std::hypot(dx, dy);

	const int numSegments = juce::jmax(1, static_cast<int>(length * 2 / maxSegmentLength));
	const float nx = dx / length / 2;
	const float ny = dy / length / 2;
	const float segmentLen = length * 2 / numSegments;
	const float pad = (brushSize + 5) * camera.getZoom();

	for (int i = 0; i <= numSegments; ++i)
	{
		const float t = i * segmentLen;
		const float x = start.getX() + nx * t;
		const float y = start.getY() + ny * t;

		repaint(juce::Rectangle<int>(
			static_cast<int>(x - pad),
			static_cast<int>(y - pad),
			static_cast<int>(pad * 2 + 1),
			static_cast<int>(pad * 2 + 1)
		));
	}
}

void CanvasComponent::repaintEllipseFromRect(const juce::Rectangle<float>& rect, float expand)
{
	constexpr float invSqrt2 = 0.7071067811865475f; // 1 / √2
	constexpr float aaPadding = 2.0f;               

	juce::Rectangle<float> outer = rect.expanded(expand);
	juce::Rectangle<float> inner = rect.reduced(expand);

	if (inner.getWidth() <= 100.0f || inner.getHeight() <= 100.0f)
	{
		repaint(outer.toNearestInt());
		return;
	}

	float rx = inner.getWidth() * 0.5f;
	float ry = inner.getHeight() * 0.5f;
	juce::Point<float> center = inner.getCentre();

	float skipHalfW = rx * invSqrt2;
	float skipHalfH = ry * invSqrt2;

	float skipLeft = center.x - skipHalfW;
	float skipRight = center.x + skipHalfW;
	float skipTop = center.y - skipHalfH;
	float skipBottom = center.y + skipHalfH;

	auto repaintPadded = [this, aaPadding](float x1, float y1, float x2, float y2)
		{
			repaint(
				static_cast<int>(std::floor(x1 - aaPadding)),
				static_cast<int>(std::floor(y1 - aaPadding)),
				static_cast<int>(std::ceil(x2 - x1 + aaPadding * 2.0f + 1.0f)),
				static_cast<int>(std::ceil(y2 - y1 + aaPadding * 2.0f + 1.0f))
			);
		};


	const float expandQ = expand * 0.5f;
	repaintPadded(outer.getX(), outer.getY(), outer.getRight(), skipTop);
	repaintPadded(outer.getX(), skipTop, skipLeft + expandQ, skipBottom);
	repaintPadded(skipRight - expandQ, skipTop, outer.getRight(), skipBottom);
	repaintPadded(outer.getX(), skipBottom, outer.getRight(), outer.getBottom());

	//repaintPadded(outer.getX(), skipTop, skipLeft, skipBottom);
	//repaintPadded(skipRight, skipTop, outer.getRight(), skipBottom);
	//repaintPadded(skipLeft, outer.getY(), skipRight, skipTop);
	//repaintPadded(skipLeft, skipBottom, skipRight, outer.getBottom());

	//const float expandH = expand * 2;
	//const float expandT = expand * 4.f;
	//repaint(skipLeft - expandH - expandQ, skipTop - expandH - expandQ, expandT, expandT);
	//repaint(skipRight - expandH + expandQ, skipTop - expandH - expandQ, expandT, expandT);
	//repaint(skipLeft - expandH - expandQ, skipBottom - expandH + expandQ, expandT, expandT);
	//repaint(skipRight - expandH + expandQ, skipBottom - expandH + expandQ, expandT, expandT);
}

void CanvasComponent::mouseUp(const juce::MouseEvent& e)
{
	repaintUnderCursor(e.position);
	if (isPanning)
	{
		isPanning = false;
		needBrushDraw = true;
		setMouseCursor(juce::MouseCursor::CrosshairCursor);
		return;
	}
	
	if (!isDrawing)
		return;
	isDrawing = false;

	if (!canvasImage.isValid())
	{
		currentStroke.clear();
		return;
	}

	juce::Graphics g(canvasImage);
	float expand = brushSize + 5.0f;
	if (!currentStroke.isEmpty())
	{
		drawCurrentPath(g);
		auto bStroke = currentStroke.getBounds().expanded(expand);
		currentStroke.clear();
		repaint(juce::Rectangle<float>(camera.img2cnv(bStroke.getTopLeft()), camera.img2cnv(bStroke.getBottomRight())).toNearestInt());
	} else if (selectedTool == PEnums::CanvasTool::Pencil || selectedTool == PEnums::CanvasTool::Brush)
		drawSigleClick(g);
	else
	{
		drawCurrentPath(g);
		repaint(juce::Rectangle<float>(imgStartPos, lastPos).expanded(brushSize + 5).toNearestInt());
	}
}

juce::ApplicationCommandTarget* CanvasComponent::getNextCommandTarget()
{
	return findFirstTargetParentComponent();
}

void CanvasComponent::getAllCommands(juce::Array<juce::CommandID>& c)
{
	c.add(PEnums::CommandIDs::CanvasResize);
}

void CanvasComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case PEnums::CommandIDs::CanvasResize:
		result.setInfo(_(Размер холста...), juce::String::fromUTF8(u8"Изменить размер рабочей области"), juce::String::fromUTF8(u8"Изображение"), 0);
		result.addDefaultKeypress('C', juce::ModifierKeys::commandModifier); // Cmd/Ctrl + C
		result.setActive(true);
		break;
	case PEnums::CommandIDs::CanvasClear:
		result.setInfo(juce::String::fromUTF8(u8"Размер холста..."), juce::String::fromUTF8(u8"Изменить размер рабочей области"), juce::String::fromUTF8(u8"Изображение"), 0);
		result.addDefaultKeypress('C', juce::ModifierKeys::commandModifier); // Cmd/Ctrl + C
		result.setActive(true);
		break;
	}
}

bool CanvasComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
	switch (info.commandID)
	{
	case PEnums::CommandIDs::CanvasResize:
		showCanvasSizeDialog();
		return true;
	default:
		return false;
	}
}

void CanvasComponent::showCanvasSizeDialog()
{
	auto bounds = getBounds();

	CanvasSizeComponent::show(getTopLevelComponent(), bounds,
		canvasImage.getWidth(),
		canvasImage.getHeight(),
		[this](int w, int h, int ox, int oy)
		{ applyCanvasResize(w, h, ox, oy); });
}

void CanvasComponent::applyCanvasResize(int newW, int newH, int offsetX, int offsetY)
{
	if (newW == canvasImage.getWidth() && newH == canvasImage.getHeight())
		return;

	juce::Image newImage(juce::Image::PixelFormat::ARGB, newW, newH, true);
	juce::Graphics g(newImage);
	g.drawImageAt(canvasImage, offsetX, offsetY);

	canvasImage = std::move(newImage);
	if (onCameraChanged) onCameraChanged();
	camera.resized();
	repaint();
}

void CanvasComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
	if (!canvasImage.isValid() || isDrawing) return;
	if (onCameraChanged) onCameraChanged();
	if (e.mods.isCommandDown())
	{
		camera.zoom(wheel.deltaY > 0, e.position);
		updateScrollbars();
		repaint();
		return;
	}

	scrollbarPan(e.mods.isShiftDown() || horizontalScroll.getBounds().contains(e.position.toInt()), wheel.deltaY > 0);
}

void CanvasComponent::scrollbarPan(bool isHorizontal, bool isPositive)
{
	const float step = 0.05f * camera.getZoom() * (isPositive ? 1.0f : -1.0f);

	if (isHorizontal)
	{
		float newStart = juce::jlimit(0.0, 1.0, horizontalScroll.getCurrentRangeStart() - step);
		horizontalScroll.setCurrentRangeStart(newStart, juce::sendNotification);
	}
	else
	{
		float newStart = juce::jlimit(0.0, 1.0, verticalScroll.getCurrentRangeStart() - step);
		verticalScroll.setCurrentRangeStart(newStart, juce::sendNotification);
	}
}

void CanvasComponent::updateScrollbars()
{
	if (!canvasImage.isValid())
	{
		horizontalScroll.setVisible(false);
		verticalScroll.setVisible(false);
		return;
	}

	const float minThumbSize = 0.05f;
	const bool needHScroll = camera.maxX() > 0.001f;
	const bool needVScroll = camera.maxY() > 0.001f;

	horizontalScroll.setVisible(needHScroll);
	verticalScroll.setVisible(needVScroll);

	if (needHScroll)
	{
		const float width = static_cast<float>(getWidth());
		const float thumbSize = juce::jlimit(minThumbSize, 1.0f,
			width / (width + 2 * camera.maxX()));

		const float start = juce::jlimit(0.0f, 1.0f,
			0.5f - camera.x() / (2.0f * camera.maxX()));

		horizontalScroll.setRangeLimits(0.0, 1.f + thumbSize);
		horizontalScroll.setCurrentRange(start, thumbSize, juce::dontSendNotification);
	}
	else
	{
		horizontalScroll.setRangeLimits(0.0, 1.0);
		horizontalScroll.setCurrentRange(0.0, 1.0, juce::dontSendNotification);
	}

	if (needVScroll)
	{
		const float height = static_cast<float>(getHeight());
		const float thumbSize = juce::jlimit(minThumbSize, 1.0f,
			height / (height + 2 * camera.maxY()));

		const float start = juce::jlimit(0.0f, 1.0f,
			0.5f - camera.y() / (2.0f * camera.maxY()));

		verticalScroll.setRangeLimits(0.0, 1.f + thumbSize);
		verticalScroll.setCurrentRange(start, thumbSize, juce::dontSendNotification);
	}
	else
	{
		verticalScroll.setRangeLimits(0.0, 1.0);
		verticalScroll.setCurrentRange(0.0, 1.0, juce::dontSendNotification);
	}

	updateScrollBarLayout();
}

void CanvasComponent::scrollBarMoved(juce::ScrollBar* scrollBar, double newStart)
{
	if (!canvasImage.isValid()) return;

	if (scrollBar == &horizontalScroll)
	{
		const float maxX = camera.maxX();
		const float newX = (0.5f - static_cast<float>(newStart)) * 2.0f * maxX;
		if (maxX > 0.001f) camera.setX(newX);
	}
	else if (scrollBar == &verticalScroll)
	{
		const float maxY = camera.maxY();
		const float newY = (0.5f - static_cast<float>(newStart)) * 2.0f * maxY;
		if (maxY > 0.001f) camera.setY(newY);
	}

	repaint();
}

void CanvasComponent::updateScrollBarLayout()
{
	const int scrollBarThickness = 8;
	const int padding = 5;
	auto bounds = getLocalBounds();
	bounds.reduce(padding, padding);
	bounds.removeFromLeft(scrollBarThickness);
	bounds.removeFromTop(scrollBarThickness);
	
	if (verticalScroll.isVisible())
	{
		auto bVS = bounds.removeFromRight(scrollBarThickness);
		bVS.removeFromBottom(scrollBarThickness);
		verticalScroll.setBounds(bVS);
	}

	if (horizontalScroll.isVisible())
	{
		horizontalScroll.setBounds(bounds.removeFromBottom(scrollBarThickness));
	}
}

