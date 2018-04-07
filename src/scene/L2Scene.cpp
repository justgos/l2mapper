#include "L2Scene.h"

#include "../main.h"

L2Scene::L2Scene(void)
{
	_visibleDist = 3000000;
	targetActor = 0;
}

L2Scene::~L2Scene(void)
{
}

void L2Scene::Init()
{
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glDepthFunc(GL_NEVER);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glDisable(GL_LIGHTING);

	/*base = glGenLists(500);								// Storage For 96 Characters

	font = CreateFontA(	-56,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_NORMAL,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Courier New");					// Font Name
	oldfont = (HFONT)SelectObject(g_dc, font);           // Selects The Font We Want
	wglUseFontBitmaps(g_dc, 0, 500, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(g_dc, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font*/

	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glDepthFunc(GL_LEQUAL);

	GLfloat fogColor[4]= {0.5f, 0.5f, 0.55f, 1.0f};
	glFogi(GL_FOG_MODE, GL_LINEAR);        // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
	glFogf(GL_FOG_DENSITY, 1.0f);              // How Dense Will The Fog Be
	//glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
	glHint(GL_FOG_HINT, GL_NICEST);          // Fog Hint Value
	glFogf(GL_FOG_START, 7000.0f);             // Fog Start Depth
	glFogf(GL_FOG_END, 20000.0f);               // Fog End Depth
	//glEnable(GL_FOG);                   // Enables GL_FOG

	glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	GLfloat LightAmbient[]= { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glEnable(GL_LIGHT0);
}

void L2Scene::preUpdate()
{
	glPushMatrix();
		//glLoadIdentity();
		//GLfloat LightPosition[]= {0, 0, 7, 1.};
		//glTranslatef(0, 0, 0);
		/*_matrix m;
		m.Identity();
		m.TranslationMatrix(MapPosX + MAP_SIZE_X * 17.0, 10000, MapPosZ - MAP_SIZE_Z * 10.0);
		glMultMatrixf(m.data);*/
		//GLfloat LightPosition[]= {MapPosX + MAP_SIZE_X * 2, 100000, MapPosZ + MAP_SIZE_Z, 1.};
		GLfloat LightPosition[]= {g_camera.getTarget().x + MAP_SIZE_X * 1.6, 100000, g_camera.getTarget().z + MAP_SIZE_Z * 1.0, 0.};
		//GLfloat LightPosition[]= {0, 100000, 0};
		//GLfloat LightPosition[]= {0, 0, 0, 1.};
		glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glPopMatrix();
}

void L2Scene::update()
{
	glEnable(GL_CULL_FACE);

	g_skybox.draw();

	glPushMatrix();
		//glEnable(GL_FOG);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);

		g_levelMgr.update();

		glDisable(GL_BLEND);

		glDisable(GL_ALPHA_TEST);

		glDisable(GL_FOG);

		g_geo.Tick();

		//glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();

	if(targetActor)
	{
		UStaticMeshActor *targetMesh = dynamic_cast<UStaticMeshActor*>(targetActor);
		if(targetMesh)
		{
			targetMesh->Tick();

			UVector loc = targetMesh->m_Location;

			_matrix mat, tmat;
			mat.Identity();
			tmat.Identity();
			tmat.ScalingMatrix(targetMesh->m_DrawScale3D.X * targetMesh->m_DrawScale, targetMesh->m_DrawScale3D.Z * targetMesh->m_DrawScale, targetMesh->m_DrawScale3D.Y * targetMesh->m_DrawScale);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationZ(-targetMesh->m_Rotation.Yaw / 32768. * _PI);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationX(2 * _PI - targetMesh->m_Rotation.Roll / 32768. * _PI);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationY(2 * _PI - targetMesh->m_Rotation.Pitch / 32768. * _PI);
			mat *= tmat;
			tmat.Identity();
			tmat.TranslationMatrix(loc.X, loc.Y, loc.Z);
			mat *= tmat;

			_vector3 bboxMin = _vector3(targetMesh->m_Mesh->BoundingBox.Min.X, targetMesh->m_Mesh->BoundingBox.Min.Z, targetMesh->m_Mesh->BoundingBox.Min.Y);
			_vector3 bboxMax = _vector3(targetMesh->m_Mesh->BoundingBox.Max.X, targetMesh->m_Mesh->BoundingBox.Max.Z, targetMesh->m_Mesh->BoundingBox.Max.Y);

			_vector3 bnl = g_math.TransformCoord(bboxMin, mat);
			_vector3 tfr = g_math.TransformCoord(bboxMax, mat);
			_vector3 bnr = g_math.TransformCoord(_vector3(bboxMax.x, bboxMin.y, bboxMin.z), mat);
			_vector3 bfr = g_math.TransformCoord(_vector3(bboxMax.x, bboxMin.y, bboxMax.z), mat);
			_vector3 bfl = g_math.TransformCoord(_vector3(bboxMin.x, bboxMin.y, bboxMax.z), mat);
			_vector3 tnr = g_math.TransformCoord(_vector3(bboxMax.x, bboxMax.y, bboxMin.z), mat);
			_vector3 tnl = g_math.TransformCoord(_vector3(bboxMin.x, bboxMax.y, bboxMin.z), mat);
			_vector3 tfl = g_math.TransformCoord(_vector3(bboxMin.x, bboxMax.y, bboxMax.z), mat);

			glPushMatrix();
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_LIGHTING);
				//glDisable(GL_DEPTH_TEST);

				glBegin(GL_LINES);
					glVertex3f(bnl.x, bnl.y, bnl.z);
					glVertex3f(bnr.x, bnr.y, bnr.z);

					glVertex3f(bnr.x, bnr.y, bnr.z);
					glVertex3f(bfr.x, bfr.y, bfr.z);

					glVertex3f(bfr.x, bfr.y, bfr.z);
					glVertex3f(bfl.x, bfl.y, bfl.z);

					glVertex3f(bnl.x, bnl.y, bnl.z);
					glVertex3f(bfl.x, bfl.y, bfl.z);

					glVertex3f(tnl.x, tnl.y, tnl.z);
					glVertex3f(tnr.x, tnr.y, tnr.z);

					glVertex3f(tnr.x, tnr.y, tnr.z);
					glVertex3f(tfr.x, tfr.y, tfr.z);

					glVertex3f(tfr.x, tfr.y, tfr.z);
					glVertex3f(tfl.x, tfl.y, tfl.z);

					glVertex3f(tnl.x, tnl.y, tnl.z);
					glVertex3f(tfl.x, tfl.y, tfl.z);

					glVertex3f(bnl.x, bnl.y, bnl.z);
					glVertex3f(tnl.x, tnl.y, tnl.z);

					glVertex3f(bnr.x, bnr.y, bnr.z);
					glVertex3f(tnr.x, tnr.y, tnr.z);

					glVertex3f(bfr.x, bfr.y, bfr.z);
					glVertex3f(tfr.x, tfr.y, tfr.z);

					glVertex3f(bfl.x, bfl.y, bfl.z);
					glVertex3f(tfl.x, tfl.y, tfl.z);
				glEnd();

				//glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
				glEnable(GL_TEXTURE_2D);
			glPopMatrix();

			glDisable(GL_CULL_FACE);
		}
	}
	else if(targetNode)
	{
		UBspSurf &Surf = targetModel->Surfs[targetNode->iSurf];
		int32 vIndex = targetModel->Verts[targetNode->iVertPool].pVertex;
		_vector3 v1 = _vector3(targetModel->Points[vIndex].X - 100, targetModel->Points[vIndex].Z - 100, targetModel->Points[vIndex].Y - 100);

		_vector3 bboxMin = v1;
		_vector3 bboxMax = v1;

		for(uint32 u = 0; u < targetNode->NumVertices; u++)
		{
			int32 nIndex = Surf.vNormal;
			int32 vIndex = targetModel->Verts[targetNode->iVertPool + u].pVertex;

			_vector3 v = _vector3(targetModel->Points[vIndex].X, targetModel->Points[vIndex].Z, targetModel->Points[vIndex].Y);

			if(v.x < bboxMin.x)
				bboxMin.x = v.x - 5;
			if(v.y < bboxMin.y)
				bboxMin.y = v.y - 5;
			if(v.z < bboxMin.z)
				bboxMin.z = v.z - 5;

			if(v.x > bboxMax.x)
				bboxMax.x = v.x + 5;
			if(v.y > bboxMax.y)
				bboxMax.y = v.y + 5;
			if(v.z > bboxMax.z)
				bboxMax.z = v.z + 5;
		}

		_vector3 bnl = bboxMin;
		_vector3 tfr = bboxMax;
		_vector3 bnr = _vector3(bboxMax.x, bboxMin.y, bboxMin.z);
		_vector3 bfr = _vector3(bboxMax.x, bboxMin.y, bboxMax.z);
		_vector3 bfl = _vector3(bboxMin.x, bboxMin.y, bboxMax.z);
		_vector3 tnr = _vector3(bboxMax.x, bboxMax.y, bboxMin.z);
		_vector3 tnl = _vector3(bboxMin.x, bboxMax.y, bboxMin.z);
		_vector3 tfl = _vector3(bboxMin.x, bboxMax.y, bboxMax.z);

		glPushMatrix();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			//glDisable(GL_DEPTH_TEST);

			glBegin(GL_LINES);
				glVertex3f(bnl.x, bnl.y, bnl.z);
				glVertex3f(bnr.x, bnr.y, bnr.z);

				glVertex3f(bnr.x, bnr.y, bnr.z);
				glVertex3f(bfr.x, bfr.y, bfr.z);

				glVertex3f(bfr.x, bfr.y, bfr.z);
				glVertex3f(bfl.x, bfl.y, bfl.z);

				glVertex3f(bnl.x, bnl.y, bnl.z);
				glVertex3f(bfl.x, bfl.y, bfl.z);

				glVertex3f(tnl.x, tnl.y, tnl.z);
				glVertex3f(tnr.x, tnr.y, tnr.z);

				glVertex3f(tnr.x, tnr.y, tnr.z);
				glVertex3f(tfr.x, tfr.y, tfr.z);

				glVertex3f(tfr.x, tfr.y, tfr.z);
				glVertex3f(tfl.x, tfl.y, tfl.z);

				glVertex3f(tnl.x, tnl.y, tnl.z);
				glVertex3f(tfl.x, tfl.y, tfl.z);

				glVertex3f(bnl.x, bnl.y, bnl.z);
				glVertex3f(tnl.x, tnl.y, tnl.z);

				glVertex3f(bnr.x, bnr.y, bnr.z);
				glVertex3f(tnr.x, tnr.y, tnr.z);

				glVertex3f(bfr.x, bfr.y, bfr.z);
				glVertex3f(tfr.x, tfr.y, tfr.z);

				glVertex3f(bfl.x, bfl.y, bfl.z);
				glVertex3f(tfl.x, tfl.y, tfl.z);
			glEnd();

			//glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	glDisable(GL_CULL_FACE);
}

bool L2Scene::injectMouseDown(MyGUI::MouseButton btn)
{
	_vector3 orig, dir;

	g_camera.projectRay(g_input.getMousePos().x, g_input.getMousePos().y, &orig, &dir);
	float actorDist;
	targetActor = g_levelMgr.pickActor(orig, dir, &actorDist);
	float nodeDist;
	targetNode = g_levelMgr.pickBspNode(orig, dir, &targetModel, &nodeDist);

	if(nodeDist <= actorDist)
		targetActor = 0;
	else
		targetNode = 0;

	return false;
}

bool L2Scene::injectMouseUp(MyGUI::MouseButton btn)
{
	return false;
}
