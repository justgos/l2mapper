#include "L2Camera.h"

#include "../main.h"

L2Camera::L2Camera(void)
{
	_dist = 7000;
	_angle_x = 0;
	_angle_y = 0;
	_isRotating = false;
}

L2Camera::~L2Camera(void)
{
}

void L2Camera::update()
{
	if(_isRotating)
	{
		_angle_x -= (g_input.getMousePos().y - rotationStartMousePos.y) * 0.005f;
		_angle_y += (g_input.getMousePos().x - rotationStartMousePos.x) * 0.005f;

		g_input.setMousePos(rotationStartMousePosAbs.x, rotationStartMousePosAbs.y);
	}

	if(_angle_x > _PI / 2 - 0.01f) 
		_angle_x = _PI / 2 - 0.01f;
	if(_angle_x < -_PI / 2 + 0.01f) 
		_angle_x = -_PI / 2 + 0.01f;

	_offset.x = _dist * cos(_angle_y) * cos(-_angle_x) + 0.;
	_offset.y = _dist * sin(-_angle_x) + 0.;
	_offset.z = _dist * sin(_angle_y) * cos(-_angle_x) + 0.;

	float speed = g_time.getTicksDelta() / 1000.0 * 2500 * (_dist / 10000);

	if(g_input.getKeyState(VK_SPACE))
		_target.y += speed;
	if(g_input.getKeyState(VK_CONTROL))
		_target.y -= speed;

	if(g_input.getKeyState('W'))
	{
		_target.x -= (cos(_angle_y)) * speed;
		_target.z -= (sin(_angle_y)) * speed;
	}
	if(g_input.getKeyState('S'))
	{
		_target.x += (cos(_angle_y)) * speed;
		_target.z += (sin(_angle_y)) * speed;
	}
	if(g_input.getKeyState('D'))
	{
		_target.x += (cos(_angle_y - _PI / 2)) * speed;
		_target.z += (sin(_angle_y - _PI / 2)) * speed;
	}
	if(g_input.getKeyState('A'))
	{
		_target.x += (cos(_angle_y + _PI / 2)) * speed;
		_target.z += (sin(_angle_y + _PI / 2)) * speed;
	}

	_pos = _target + _offset;

	gluLookAt(_pos.x, _pos.y, _pos.z,
					_target.x, _target.y, _target.z,
					0.0f, 1.0f, 0.0f);
}

void L2Camera::onResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 100.0f, 1000000.0f);

	glMatrixMode(GL_MODELVIEW);
}

void L2Camera::projectRay(uint16 x, uint16 y, _vector3 *orig, _vector3 *dir)
{
	double modelview[16], projection[16];
	GLint viewport[4];
	_vector3 point;
	double tx, ty, tz;

	y = g_window.getInnerSize().y - y;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(x, y, 0,
				modelview, projection, viewport,
				&tx, &ty, &tz);

	orig->x = tx;
	orig->y = ty;
	orig->z = tz;

	gluUnProject(x, y, 1,
				modelview, projection, viewport,
				&tx, &ty, &tz);

	dir->x = tx - orig->x;
	dir->y = ty - orig->y;
	dir->z = tz - orig->z;
}

void L2Camera::setTarget(float x, float y, float z)
{
	_target = _vector3(x, y, z);
}

bool L2Camera::injectMouseDown(MyGUI::MouseButton btn)
{
	if(btn == MyGUI::MouseButton::Right)
	{
		if(!_isRotating)
		{
			rotationStartMousePos = g_input.getMousePos();
			rotationStartMousePosAbs = g_input.getMousePosAbs();
			_isRotating = true;
			g_input.setCursorVisible(false);

			return true;
		}
	}

	return false;
}

bool L2Camera::injectMouseUp(MyGUI::MouseButton btn)
{
	if(btn == MyGUI::MouseButton::Right)
	{
		if(_isRotating)
		{
			_isRotating = false;
			g_input.setCursorVisible(true);

			return true;
		}
	}

	return false;
}

bool L2Camera::injectMouseScroll(int16 scroll)
{
	_dist *= (scroll < 0 ? 0.9 : 1.1);

	return true;
}
