#pragma once
//
#include "../JuceLibraryCode/JuceHeader.h"
//#include "../Include/CanvasComponent.h"

class CanvasComponent;

class Camera {
public:
	Camera(const CanvasComponent& cnv) : _cnv(cnv) {}

	juce::Point<float> getShiftedC() { return _shiftedC + pan; }

	template<class T>
	juce::Point<float> img2cnv(juce::Point<T> p) const { return (p.toFloat() * _zoom + _shiftedC + pan); };

	template<class T>
	juce::Point<float> cnv2img(juce::Point<T> p) const { return (p.toFloat() - _shiftedC - pan) / _zoom; };

	template<class T>
	void moveTo(juce::Point<T> p) { pan = p.toFloat(); clampPan(); }

	template<class T>
	void moveBy(juce::Point<T> p) { pan += p.toFloat(); clampPan();}

	float getZoom() const { return _zoom; }
	float x() { return pan.x; }
	float y() { return pan.y; }
	float maxX() { return _maxPan.x; }
	float maxY() { return _maxPan.y; }

	void setX(float v) { pan.setX(v); clampPan(); }
	void setY(float v) { pan.setY(v); clampPan(); }

	juce::Point<float> getPan() { return pan; }

	void setZoom(float newZoom) { setZoom(newZoom, cnvC); }
	void setZoom(float newZoom, juce::Point<float> p) {
		const float oldZoom = _zoom;
		const auto imgPivot = (p - _shiftedC - pan) / oldZoom;
		_zoom = juce::jlimit(zoomRange.getStart(), zoomRange.getEnd(), newZoom);
		zoomed();
		pan = p - _shiftedC - imgPivot * _zoom;
		clampPan();
	}

	template<class T>
	void zoom(bool isZoomIn, juce::Point<T> p) { 
		float scale = isZoomIn ? zoomFactor : 1 / zoomFactor;
		setZoom(scale * _zoom, p.toFloat());
	}
	
	void zoom(bool isZoomIn) { zoom(isZoomIn, cnvC); }

	void setZoomRange(float l, float u) { zoomRange = { l,u }; zoom(true); zoom(false); }
	juce::Range<float> getZoomRange() const { return zoomRange; }

	juce::AffineTransform getTransformImg2Cnv() { return juce::AffineTransform().scaled(_zoom).translated(pan + _shiftedC); }

	void resized();

	const float wiggle = 20;
	const float zoomFactor = 1.1;

private:
	void zoomed();
	float _zoom = 1;

	void clampPan();

	const CanvasComponent& _cnv;
	juce::Point<float> _maxPan{0,0};
	juce::Point<float> _shiftedC{0,0};
	juce::Range<float> zoomRange{ 0.01, 50 };
	juce::Point<float> pan{ 0,0 };
	juce::Point<float> imgC{ 0,0 };
	juce::Point<float> cnvC{ 0,0 };

};