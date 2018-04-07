#include "L2Skybox.h"

#include "main.h"

L2Skybox::L2Skybox(void)
{
	_size = 1000;
}

L2Skybox::~L2Skybox(void)
{
}

void L2Skybox::Init()
{
	tex[0] = g_texMgr.get("data/textures/skybox/default/back.jpg", true);
	tex[1] = g_texMgr.get("data/textures/skybox/default/front.jpg", true);
	tex[2] = g_texMgr.get("data/textures/skybox/default/bottom.jpg", true);
	tex[3] = g_texMgr.get("data/textures/skybox/default/top.jpg", true);
	tex[5] = g_texMgr.get("data/textures/skybox/default/left.jpg", true);
	tex[4] = g_texMgr.get("data/textures/skybox/default/right.jpg", true);
}

void L2Skybox::draw()
{
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
		
		glTranslatef(g_camera.getPos().x, g_camera.getPos().y, g_camera.getPos().z);
		glTranslatef(-_size / 2, -_size / 2, -_size / 2);

		glBindTexture(GL_TEXTURE_2D, tex[0]);

		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(_size, 0, 0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(_size, _size, 0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(0, _size, 0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex[1]);

		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(0, 0, _size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(0, _size, _size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(_size, _size, _size);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(_size, 0, _size);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex[2]);

		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(0, 0, 0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(0, 0, _size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(_size, 0, _size);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(_size, 0, 0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex[3]);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(_size, _size, 0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(_size, _size, _size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(0, _size, _size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(0, _size, 0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex[4]);

		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(0, _size, 0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(0, _size, _size);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0,_size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(0, 0, 0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex[5]);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(_size, 0, 0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(_size, 0,_size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(_size, _size,_size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(_size, _size, 0);
		glEnd();

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}
