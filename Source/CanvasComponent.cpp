#include <stack>
#include "../Include/CanvasComponent.h"
#include "../Include/CanvasSizeComponent.h"
#include "../Include/Application.h"

CanvasComponent::CanvasComponent() : camera(*this)
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

	horizontalScroll.setRangeLimits(0.0, 1.0);
	horizontalScroll.setCurrentRange(0.0, 1.0);

	verticalScroll.setRangeLimits(0.0, 1.0);
	verticalScroll.setCurrentRange(0.0, 1.0);

	horizontalScroll.addListener(this);
	verticalScroll.addListener(this);

	resizeImageRect();
}

void CanvasComponent::clearCanvas()
{
	if (canvasImage.isValid())
	{
		canvasImage.clear(canvasImage.getBounds(), juce::Colours::transparentBlack);
		repaint();
	}
}

void CanvasComponent::setBgColour(juce::Colour col) { 
	bgColour = col; 
	repaint(canvasImageRect.expanded(50).toNearestInt());
}

void CanvasComponent::setTool(PEnums::CanvasTool tool) { 
	selectedTool = tool; 
	resetBrushDraw();
}

void CanvasComponent::resetBrushDraw() {
	setFlag(CanvasFlags::NeedBrush, true);
	switch (selectedTool) {
	case PEnums::CanvasTool::Cursor:
	case PEnums::CanvasTool::Fill:
	case PEnums::CanvasTool::Text:
		setFlag(CanvasFlags::NeedBrush, false);
		break;
	}
}

void CanvasComponent::paint(juce::Graphics& g)
{
	g.fillAll(PEnums::Colours::Gray1);

	g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::lowResamplingQuality);

	g.reduceClipRegion(getLocalBounds());

	PEnums::Colours::Shadow.drawForRectangle(g, canvasImageRect.toNearestInt());
	g.setOpacity(1);
	if (bgColour.getAlpha() != 255) g.fillCheckerBoard(canvasImageRect, 5 * camera.getZoom(), 5 * camera.getZoom(), PEnums::Colours::White, PEnums::Colours::Gray2);
	g.setColour(bgColour);
	g.fillRect(canvasImageRect);
	g.setOpacity(1);
	g.drawImage(canvasImage, canvasImageRect, juce::RectanglePlacement::stretchToFit, false);
	if (hasFlag(CanvasFlags::Drawing) && !currentStroke.isEmpty())
	{
		auto transform = camera.getTransformImg2Cnv();
		g.saveState();
		g.addTransform(transform);
		drawCurrentPath(g);
		//g.strokePath(currentStroke, stroke);
		g.addTransform(juce::AffineTransform());
		g.restoreState();
	}
	if (hasFlag(CanvasFlags::NeedBrush))
	{
		const float s = brushSize * camera.getZoom();
		const float r = s * 0.5f;
		switch (selectedTool) {
		case PEnums::CanvasTool::Pencil:
			g.setColour(currentColour);
			g.fillRect(lastPos.x - r, lastPos.y - r, s, s);
			break;
		case PEnums::CanvasTool::Eraser:
			g.setColour(juce::Colours::white);
			g.fillRect(lastPos.x - r, lastPos.y - r, s, s);
			g.setColour(juce::Colours::black);
			g.drawRect(lastPos.x - r, lastPos.y - r, s, s);
			break;
		case PEnums::CanvasTool::Brush:
			g.setColour(currentColour);
			g.fillEllipse(lastPos.x - r, lastPos.y - r, s, s);
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
	
}

void CanvasComponent::drawCurrentPath(juce::Graphics& g) {
	juce::PathStrokeType stroke{ 0 };
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

void CanvasComponent::eraseLine(const juce::Point<float>& to)
{
	if (!canvasImage.isValid()) return;
	if (onImageChanged) onImageChanged();
	// Рисуем линию как последовательность кругов/квадратов для жёстких краёв
	const auto from = imgLastPos;
	auto dx = to.getX() - from.getX();
	auto dy = to.getY() - from.getY();
	const auto dist = std::hypot(dx, dy);
	const auto steps = std::max(1, static_cast<int>(dist));
	dx /= steps;
	dy /= steps;
	const float padding = brushSize * 0.5f;
	for (int i = 0; i <= steps; ++i)
	{
		float x = from.getX() + dx * i - padding;
		float y = from.getY() + dy * i - padding;

		canvasImage.clear(juce::Rectangle<float>(x, y, brushSize, brushSize).toNearestInt(), juce::Colours::transparentBlack);
	}
}
void CanvasComponent::repaintUnderCursor(juce::Point<float> pos)
{
	float dirtyR = std::max(brushSize * camera.getZoom() + 5.0f, 20.0f) / 2;
	repaint(pos.x - dirtyR, pos.y - dirtyR, dirtyR * 2.0f, dirtyR * 2.0f);
	lastPos = pos;
}

void CanvasComponent::repaintToCursor(juce::Point<float> pos)
{
	float dirtyR = std::max(brushSize * camera.getZoom() + 5.0f, 20.0f) / 2;
	repaint(juce::Rectangle<float>(lastPos, pos)
		.expanded(dirtyR).toNearestInt());
	lastPos = pos;
}

void CanvasComponent::resized()
{
	camera.resized();
	resizeImageRect();
	if (onCameraChanged) onCameraChanged();
	updateScrollbars();
}

void CanvasComponent::mouseExit(const juce::MouseEvent& e)
{
	lastPos = juce::Point<float>(-1000, -1000);
	if (hasFlag(CanvasFlags::Panning))
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
		setFlag(CanvasFlags::Drawing, false);
		currentStroke.clear();
		repaint();

		return;
	}

	if (e.mods.isMiddleButtonDown())
	{
		startPanning(currentPos);
	}

	if (e.mods.isLeftButtonDown()) {
		setFlag(CanvasFlags::Drawing, true);
		startPos = currentPos;
		imgStartPos = camera.cnv2img(currentPos);
		imgLastPos = imgStartPos;
		currentStroke.clear();

		auto bImg = canvasImage.getBounds().toFloat().expanded(brushSize + 5);
		resetBrushDraw();
		switch (selectedTool) {
		case PEnums::CanvasTool::Pencil:
			currentStroke.startNewSubPath(imgStartPos.roundToInt().toFloat());
			break;
		case PEnums::CanvasTool::Brush:
			currentStroke.startNewSubPath(imgStartPos);
			break;
		case PEnums::CanvasTool::Fill:
			floodFill(imgStartPos.toInt(), 30);
			
			setFlag(CanvasFlags::Drawing, false);
			repaint(juce::Rectangle<float>(
				camera.img2cnv(bImg.getTopLeft()), 
				camera.img2cnv(bImg.getBottomRight())).toNearestInt());
			break;
		case PEnums::CanvasTool::Eraser:
			eraseLine(imgStartPos);
			break;
		}

	}
	repaintUnderCursor(currentPos);
}

void CanvasComponent::startPanning(juce::Point<float> startPos)
{
	setFlag(CanvasFlags::Panning, true);
	setFlag(CanvasFlags::NeedBrush, false);
	lastPos = startPos;
	setMouseCursor(juce::MouseCursor::DraggingHandCursor);
}

void CanvasComponent::mouseMove(const juce::MouseEvent& e)
{
	if (hasFlag(CanvasFlags::Panning | CanvasFlags::Drawing)) return;

	repaintToCursor(e.position);
	if (onCameraChanged) onCameraChanged();
}

void CanvasComponent::mouseDrag(const juce::MouseEvent& e)
{
	if (e.mods.currentModifiers.isRightButtonDown())
	{
		setFlag(CanvasFlags::Drawing, false);
		currentStroke.clear();
		repaint(canvasImageRect.expanded(50).toNearestInt());
		return;
	}

	auto currentPos = e.position.toFloat();
	if (hasFlag(CanvasFlags::Panning))
	{
		auto delta = currentPos - lastPos;
		camera.moveBy(delta);
		updateScrollbars();
		//repaintToCursor(currentPos);
		resizeImageRect();
		repaint(canvasImageRect.expanded(50).expanded(std::abs(delta.x) + camera.wiggle, std::abs(delta.y) + camera.wiggle).toNearestInt());
		if (onCameraChanged) onCameraChanged();
		return;
	}

	if (hasFlag(CanvasFlags::Drawing)) {
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
		}
		else if (selectedTool == PEnums::CanvasTool::Brush) {
			currentStroke.lineTo(newImgPos);
			repaintToCursor(currentPos);
		}
		else if (selectedTool == PEnums::CanvasTool::Eraser) {
			eraseLine(newImgPos);
			imgLastPos = newImgPos;
			repaintToCursor(currentPos);
		}
		else {
			auto imgMidPos = (imgStartPos + newImgPos) / 2;
			auto rect = juce::Rectangle(imgStartPos, newImgPos);
			float arrowSize = std::max(5.f, brushSize);
			float arrowStrokeSize = 0.5f;
			float expand = (brushSize)*camera.getZoom() / 2 + 10;
			currentStroke.clear();
			switch (selectedTool) {
			case PEnums::CanvasTool::Pencil:
			case PEnums::CanvasTool::Brush:
				break;
			case PEnums::CanvasTool::Line:
				currentStroke.startNewSubPath(imgStartPos);
				currentStroke.lineTo(newImgPos);
				repaintLine(startPos, lastPos, expand);
				break;
			case PEnums::CanvasTool::BiDirArrow:
				currentStroke.addArrow(juce::Line(imgMidPos, imgStartPos), arrowStrokeSize, arrowSize, arrowSize);
				currentStroke.addArrow(juce::Line(imgMidPos, newImgPos), arrowStrokeSize, arrowSize, arrowSize);
				repaintLine(startPos, lastPos, expand, true, true);
				break;
			case PEnums::CanvasTool::OneDirArrow:
				currentStroke.addArrow(juce::Line(imgStartPos, newImgPos), arrowStrokeSize, arrowSize, arrowSize);
				repaintLine(startPos, lastPos, expand, false, true);
				break;
			case PEnums::CanvasTool::Rect:
				currentStroke.addRectangle(rect);
				repaint(juce::Rectangle(currentPos.withX(startPos.x), lastPos).expanded(expand).toNearestInt());
				repaint(juce::Rectangle(lastPos, currentPos.withY(startPos.y)).expanded(expand).toNearestInt());
				break;
			case PEnums::CanvasTool::Ellipse:
				currentStroke.addEllipse(rect);
				repaintEllipse({ startPos, currentPos }, expand);
				repaintEllipse({ startPos, lastPos }, expand);
				break;
			case PEnums::CanvasTool::Triangle:
				currentStroke.addTriangle(
					rect.getX(), rect.getBottom(),
					rect.getCentreX(), rect.getY(),
					rect.getRight(), rect.getBottom()
				);
				rect = { startPos, currentPos };
				repaintLine({ rect.getX(), rect.getBottom() }, { rect.getCentreX(), rect.getY() }, expand * 1.5);
				repaintLine({ rect.getRight(), rect.getBottom() }, { rect.getCentreX(), rect.getY() }, expand * 1.5);
				repaint(juce::Rectangle<float>(
					{ rect.getX(), rect.getBottom() }, { rect.getRight(), rect.getBottom() }
				).expanded(expand).toNearestInt());
				break;
			}
		}

	}
	lastPos = currentPos;
	if (onCameraChanged) onCameraChanged();
	if (onImageChanged) onImageChanged();
}

void CanvasComponent::redrawImageWithTransform(int w, int h, const juce::AffineTransform& t){
	juce::Image newImg{juce::Image::ARGB, w, h, true};
	juce::Graphics g(newImg);
	g.drawImageTransformed(canvasImage, t);
	canvasImage = std::move(newImg);
	if (onCameraChanged) onCameraChanged();
	repaint(canvasImageRect.expanded(50).toNearestInt());
	camera.resized();
	resizeImageRect();
	if (onImageChanged) onImageChanged();
	repaint(canvasImageRect.expanded(50).toNearestInt());
}

void CanvasComponent::repaintLine(juce::Point<float> start,
	juce::Point<float> end, float expand, bool needBigStart, bool needBigEnd)
{
	const float dx = end.getX() - start.getX();
	const float dy = end.getY() - start.getY();
	const float length = std::hypot(dx, dy);
	const int N = juce::jmax(1, juce::roundFloatToInt(length / 144));
	const float pad = expand / 2;

	if (needBigStart || needBigEnd) {
		const auto d = juce::Point<float>(dx, dy) / length * expand;
		auto r = juce::Rectangle<float>{ 0, 0, expand * 5, expand * 5 };
		if (needBigStart)
			repaint(r.withCentre(start + d).toNearestInt());
		if (needBigEnd)
			repaint(r.withCentre(end - d).toNearestInt());
	}

	if (N == 0 ||
		std::abs(dx) <= 0.01 ||
		std::abs(dy) <= 0.01 ||
		std::abs(dy / dx) <= 0.1 ||
		std::abs(dx / dy) <= 0.1
		) {
		repaint(juce::Rectangle(start, end).expanded(expand).toNearestInt());
		return;
	}

	const float nx = dx / N / 2;
	const float ny = dy / N / 2;

	float x1 = start.getX(), y1 = start.getY();

	for (int i = 0; i <= N * 2; ++i)
	{
		const float x2 = start.getX() + nx * i;
		const float y2 = start.getY() + ny * i;

		const float minX = juce::jmin(x1, x2) - expand;
		const float maxX = juce::jmax(x1, x2) + expand;
		const float minY = juce::jmin(y1, y2) - expand;
		const float maxY = juce::jmax(y1, y2) + expand;

		x1 = x2, y1 = y2;

		repaint(
			static_cast<int>(std::floor(minX)),
			static_cast<int>(std::floor(minY)),
			static_cast<int>(std::ceil(maxX - minX)),
			static_cast<int>(std::ceil(maxY - minY))
		);
	}
}

void CanvasComponent::repaintEllipse(juce::Rectangle<float> rect, float expand)
{
	const float cx = rect.getCentreX();
	const float cy = rect.getCentreY();
	const float rx = rect.getWidth() * 0.5f;
	const float ry = rect.getHeight() * 0.5f;

	const float L = (juce::MathConstants<float>::pi * rx * ry + std::pow(rx - ry, 2)) / (rx + ry);

	const int N = juce::roundFloatToInt(L / 144) * 4;
	if (N == 0) {
		repaint(rect.expanded(expand).toNearestInt());
		repaint(0, 0, 10, 10);
		return;
	}

	const float ANGLE = juce::MathConstants<float>::twoPi / N;

	float a1 = 0, x1 = rect.getRight(), y1 = cy;
	for (int i = 1; i < N + 1; ++i)
	{
		const float a2 = i * ANGLE;

		const float x2 = cx + rx * std::cos(a2);
		const float y2 = cy + ry * std::sin(a2);

		const float minX = juce::jmin(x1, x2) - expand;
		const float maxX = juce::jmax(x1, x2) + expand;
		const float minY = juce::jmin(y1, y2) - expand;
		const float maxY = juce::jmax(y1, y2) + expand;

		a1 = a2, x1 = x2, y1 = y2;

		repaint(
			static_cast<int>(std::floor(minX)),
			static_cast<int>(std::floor(minY)),
			static_cast<int>(std::ceil(maxX - minX)),
			static_cast<int>(std::ceil(maxY - minY))
		);
	}

}

void CanvasComponent::mouseUp(const juce::MouseEvent& e)
{
	repaintUnderCursor(e.position);
	if (hasFlag(CanvasFlags::Panning))
	{
		setFlag(CanvasFlags::Panning, false);
		resetBrushDraw();
		setMouseCursor(juce::MouseCursor::CrosshairCursor);
		return;
	}

	if (!hasFlag(CanvasFlags::Drawing))
		return;

	setFlag(CanvasFlags::Drawing, false);
	if (!canvasImage.isValid())
	{
		currentStroke.clear();
		return;
	}

	if (selectedTool == PEnums::CanvasTool::Eraser) return;

	juce::Graphics g(canvasImage);
	const float expand = brushSize * camera.getZoom() + 5.0f;
	if (!currentStroke.isEmpty())
	{
		drawCurrentPath(g);
		auto bStroke = currentStroke.getBounds().expanded(brushSize + 5);
		currentStroke.clear();
		repaint(juce::Rectangle<float>(camera.img2cnv(bStroke.getTopLeft()), camera.img2cnv(bStroke.getBottomRight())).toNearestInt());
	}
	else if (selectedTool == PEnums::CanvasTool::Pencil || selectedTool == PEnums::CanvasTool::Brush) {
		drawSigleClick(g);
		repaint(lastPos.x - expand / 2, lastPos.y - expand / 2, expand, expand);
	}
	else
	{
		drawCurrentPath(g);
		repaint(juce::Rectangle<float>(startPos, lastPos).expanded(expand).toNearestInt());
	}
}

juce::ApplicationCommandTarget* CanvasComponent::getNextCommandTarget()
{
	return findFirstTargetParentComponent();
}

void CanvasComponent::getAllCommands(juce::Array<juce::CommandID>& c)
{
	c.add(PEnums::CommandIDs::CanvasResize);
	c.add(PEnums::CommandIDs::CanvasClear);
	c.add(PEnums::CommandIDs::CanvasFlipH);
	c.add(PEnums::CommandIDs::CanvasFlipV);
	c.add(PEnums::CommandIDs::CanvasRotate90CW);
	c.add(PEnums::CommandIDs::CanvasRotate90CC);
}

void CanvasComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case PEnums::CommandIDs::CanvasResize:
		result.setInfo(_("Размер холста..."), _("Изменить размер рабочей области"), _("Изображение"), 0);
		//result.addDefaultKeypress('C', juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::CanvasClear:
		result.setInfo(_("Очистить холст"), _("Очистить холст"), _("Изображение"), 0);
		//result.addDefaultKeypress('D', juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::CanvasFlipH:
		result.setInfo(_("Отразить горизонтально"), _("Отражение слева направо"), _("Изображение"), 0);
		result.addDefaultKeypress('F', juce::ModifierKeys::shiftModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::CanvasFlipV:
		result.setInfo(_("Отразить вертикально"), _("Отражение сверху вниз"), _("Изображение"), 0);
		result.addDefaultKeypress('F', juce::ModifierKeys::commandModifier | juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::CanvasRotate90CW:
		result.setInfo(_("Повернуть на 90°"), _("Поворот по часовой стрелке на 90°"), _("Изображение"), 0);
		result.addDefaultKeypress('R', juce::ModifierKeys::commandModifier);
		result.setActive(true);
		break;
	case PEnums::CommandIDs::CanvasRotate90CC:
		result.setInfo(_("Повернуть на -90°"), _("Поворот против часовой стрелки на 90°"), _("Изображение"), 0);
		result.addDefaultKeypress('R', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
		result.setActive(true);
		break;
	}
}

static AffineTransform horizontalFlip(float width)
{
	return { -1.f, 0.f, width,
			  0.f, 1.f, 0.f };
}

bool CanvasComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
	switch (info.commandID)
	{
	case PEnums::CommandIDs::CanvasResize:
		showCanvasSizeDialog();
		return true;
	case PEnums::CommandIDs::CanvasClear:
		clearCanvas();
		return true;
	case PEnums::CommandIDs::CanvasFlipH:
		redrawImageWithTransform(canvasImage.getWidth(), canvasImage.getHeight(), 
			horizontalFlip(canvasImage.getHeight()));
		return true;
	case PEnums::CommandIDs::CanvasFlipV:
		redrawImageWithTransform(canvasImage.getWidth(), canvasImage.getHeight(), 
			juce::AffineTransform::verticalFlip(canvasImage.getHeight()));
		return true;
	case PEnums::CommandIDs::CanvasRotate90CW:
		redrawImageWithTransform(canvasImage.getHeight(), canvasImage.getWidth(), 
			juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi).translated(0, canvasImage.getWidth()));
		return true;
	case PEnums::CommandIDs::CanvasRotate90CC:
		redrawImageWithTransform(canvasImage.getHeight(), canvasImage.getWidth(), 
			juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi).translated(canvasImage.getHeight(), 0));
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
	if (onImageChanged) onImageChanged();
	repaint(canvasImageRect.expanded(50).toNearestInt());
	camera.resized();
	resizeImageRect();
	repaint(canvasImageRect.expanded(50).toNearestInt());
}

void CanvasComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
	if (!canvasImage.isValid() || hasFlag(CanvasFlags::Drawing)) return;
	if (onCameraChanged) onCameraChanged();
	if (e.mods.isCommandDown())
	{
		auto bImgCnvOld = canvasImageRect;
		camera.zoom(wheel.deltaY > 0, e.position);
		resizeImageRect();
		updateScrollbars();

		if (wheel.deltaY > 0)
			repaint(canvasImageRect.expanded(50).toNearestInt());
		else
			repaint(bImgCnvOld.expanded(50).toNearestInt());
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

	resizeImageRect();
	repaint(canvasImageRect.expanded(50).toNearestInt());
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

void CanvasComponent::floodFill(juce::Point<int> p, juce::uint8 tolerance)
{
	int x = p.getX(), y = p.getY();
	if (!canvasImage.getBounds().contains(x, y))
		return;

	// Получаем доступ к пикселям изображения
	juce::Image::BitmapData bitmapData(canvasImage,
		juce::Image::BitmapData::readWrite);

	// Цвет стартовой точки
	juce::Colour targetColor = bitmapData.getPixelColour(x, y);

	// Если целевой и новый цвета совпадают - выходим
	if (targetColor == currentColour)
		return;

	if (onImageChanged) onImageChanged();

	// Используем стек для scanline алгоритма
	struct ScanLine { int x1, x2, y; };
	std::stack<ScanLine> stack;

	// Добавляем начальную строку
	stack.push({ x, x, y });

	while (!stack.empty())
	{
		ScanLine line = stack.top();
		stack.pop();

		int x1 = line.x1;
		int x2 = line.x2;
		int y = line.y;

		// Идем влево от начальной точки
		int left = x1;
		while (left > 0 && isColorMatch(bitmapData.getPixelColour(left - 1, y),
			targetColor, tolerance))
			left--;

		// Идем вправо от начальной точки
		int right = x1;
		while (right < canvasImage.getWidth() - 1 &&
			isColorMatch(bitmapData.getPixelColour(right + 1, y),
				targetColor, tolerance))
			right++;

		// Заливаем текущую строку
		for (int i = left; i <= right; i++)
			bitmapData.setPixelColour(i, y, currentColour);

		// Проверяем строку выше
		if (y > 0)
		{
			bool inSpan = false;

			for (int i = left; i <= right; i++)
			{
				if (!inSpan && i < canvasImage.getWidth() &&
					isColorMatch(bitmapData.getPixelColour(i, y - 1),
						targetColor, tolerance))
				{
					inSpan = true;
					stack.push({ i, i, y - 1 });
				}
				else if (inSpan && (i == right || i == canvasImage.getWidth() - 1 ||
					!isColorMatch(bitmapData.getPixelColour(i, y - 1),
						targetColor, tolerance)))
				{
					inSpan = false;
					stack.top().x2 = i - 1;
				}
			}
		}

		// Проверяем строку ниже
		if (y < canvasImage.getHeight() - 1)
		{
			bool inSpan = false;

			for (int i = left; i <= right; i++)
			{
				if (!inSpan && i < canvasImage.getWidth() &&
					isColorMatch(bitmapData.getPixelColour(i, y + 1),
						targetColor, tolerance))
				{
					inSpan = true;
					stack.push({ i, i - 1, y + 1 });
				}
				else if (inSpan && (i == right || i == canvasImage.getWidth() - 1 ||
					!isColorMatch(bitmapData.getPixelColour(i, y + 1),
						targetColor, tolerance)))
				{
					inSpan = false;
					stack.top().x2 = i - 1;
				}
			}
		}
	}
}

bool CanvasComponent::isColorMatch(juce::Colour c1, juce::Colour c2, juce::uint8 tolerance)
{
	if (tolerance == 0)
		return c1 == c2;

	const juce::uint8 dr = std::abs(c1.getRed() - c2.getRed());
	const juce::uint8 dg = std::abs(c1.getGreen() - c2.getGreen());
	const juce::uint8 db = std::abs(c1.getBlue() - c2.getBlue());
	const juce::uint8 da = std::abs(c1.getAlpha() - c2.getAlpha());

	return dr <= tolerance && dg <= tolerance && db <= tolerance && da <= tolerance;
}
