#include "../Include/Geometry.h"
#include "../Include/CanvasComponent.h"

static float dd(float v, float w) {
	if (v < -w) return 0;
	if (v < 0) return w;
	return v + w;
}

void Camera::resized() {
	auto bCnv = _cnv.getLocalBounds().toFloat();
	auto bImg = _cnv.getCanvasImage().getBounds().toFloat();
	cnvC = bCnv.getCentre();
	imgC = bImg.getCentre();
	zoomed();
	clampPan();
}

void Camera::zoomed() {
	auto bCnv = _cnv.getLocalBounds().toFloat();
	auto bImg = _cnv.getCanvasImage().getBounds().toFloat();
	_shiftedC = cnvC - imgC * _zoom;

	const float dx = (bImg.getWidth() * _zoom - bCnv.getWidth()) / 2;
	const float dy = (bImg.getHeight() * _zoom - bCnv.getHeight()) / 2;

	_maxPan.setXY(
		dd(dx, wiggle),
		dd(dy, wiggle)
	);
	
}

void Camera::clampPan() {
	pan.setXY(
		juce::jlimit(-_maxPan.x, _maxPan.x, pan.x),
		juce::jlimit(-_maxPan.y, _maxPan.y, pan.y)
	);
}
