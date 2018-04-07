#pragma once

#include "../_afx.h"

class L2Camera
{
public:
	void update();

	void onResize(int width, int height);

	void projectRay(uint16 x, uint16 y, _vector3 *orig, _vector3 *dir);

	void setTarget(float x, float y, float z);

	_vector3 getPos() { return _pos; }
	_vector3 getOffset() { return _offset; }
	_vector3 getTarget() { return _target; }

	bool injectMouseDown(MyGUI::MouseButton btn);
	bool injectMouseUp(MyGUI::MouseButton btn);
	bool injectMouseScroll(int16 scroll);
protected:
	_vector3 _pos;
	_vector3 _offset;
	_vector3 _target;
	float _dist;
	float _angle_x, _angle_y;
	bool _isRotating;

	_vector2 rotationStartMousePos;
	_vector2 rotationStartMousePosAbs;
public:
	L2Camera(void);
	~L2Camera(void);
};

