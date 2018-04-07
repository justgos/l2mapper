#include "L2LevelManager.h"

#include "../main.h"
#include "../GUI/L2UIMap.h"

L2LevelManager::L2LevelManager(void)
{
	loadingTilesCounter = 0;
}

L2LevelManager::~L2LevelManager(void)
{
}

void L2LevelManager::Init()
{
	//UPackageMgr.Init("G:/games/l2ct3_GoD/");
	//UPackageMgr.Init("E:/games/l2ct3_GoD/");
	//UPackageMgr.Init("E:/games/l2fr/");
	//UPackageMgr.Init("D:/downloads/l2c1/");
	//UPackageMgr.Init("../../l2god/");
	UPackageMgr.Init("../");
	//UPackageMgr.Init("D:/(WORK)/GoS/_l2_dev/_client/l2ct3_GoD/");

	int16 map_x = 23;
	int16 map_y = 22;

	// -------
	float MapPosX = (map_x - 20) * MAP_SIZE_X;
	float MapPosZ = (map_y - 18) * MAP_SIZE_Z;

	g_camera.setTarget(MapPosX + MAP_SIZE_X / 2,
						-4500,
						MapPosZ + MAP_SIZE_Z / 2);

	char buf[256];
	sprintf(buf, "%d_%d", map_x, map_y);
	// -------

	//m_Maps.add(UPackageMgr.GetULevel(buf));

	//g_geo.Load(map_x, map_y);

	//m_Maps.add(UPackageMgr.GetULevel("17_20"));
	//m_Maps.add(UPackageMgr.GetULevel("17_21"));
	//m_Maps.add(UPackageMgr.GetULevel("21_22"));
	//m_Maps.add(UPackageMgr.GetULevel("21_23"));
	//m_Maps.add(UPackageMgr.GetULevel("22_21"));
	//m_Maps.add(UPackageMgr.GetULevel("23_21"));

	varsHolder = new L2LibVarsHolder();

	GLuint dummyTex = g_texMgr.get("data/textures/dummy.png");
	
	varsHolder->dummyTex = dummyTex;

	char *default_vert_shader;
	char *default_frag_shader;

	uint32 t, fend;

	FILE *sh_file = fopen("data/shaders/default.vert", "rb");
	t = ftell(sh_file);
	fseek(sh_file, 0, SEEK_END);
	fend = ftell(sh_file);
	fseek(sh_file, 0, SEEK_SET);
	default_vert_shader = new char[fend - t + 1];
	fread(default_vert_shader, fend - t, 1, sh_file);
	default_vert_shader[fend - t] = 0;
	fclose(sh_file);

	sh_file = fopen("data/shaders/default.frag", "rb");
	t = ftell(sh_file);
	fseek(sh_file, 0, SEEK_END);
	fend = ftell(sh_file);
	fseek(sh_file, 0, SEEK_SET);
	default_frag_shader = new char[fend - t + 1];
	fread(default_frag_shader, fend - t, 1, sh_file);
	default_frag_shader[fend - t] = 0;
	fclose(sh_file);

	varsHolder->defaultObjectShader = g_shader.Add(default_vert_shader, default_frag_shader, "L2Default");

	g_shader.SetUniform1f(varsHolder->defaultObjectShader, "cAlpha", 1.0);

	/*for(uint32 i = 0; i < m_Maps.Size(); i++)
	{
		m_Maps[i]->setL2LibVars(varsHolder);
	}*/
}

void L2LevelManager::loadLevel(int map_x, int map_y)
{
	char buf[256];
	sprintf(buf, "%d_%d", map_x, map_y);

	// TODO: should fix this dumb check
	for(uint32 i = 0; i < m_Maps.Size(); i++)
	{
		char *mapName = UTIL_CopyString(m_Maps[i]->URL.Map.Txt);
		mapName[5] = 0;
		if(UTIL_strcmpi(mapName, buf) == 0)
		{
			if(!m_Maps[i]->_isVisible)
				m_Maps[i]->_isVisible = true;
			loadingTilesCounter--;
			//g_ui.getL2Map()->setTileState(map_x, map_y, g_ui.getL2Map()->getTileState(map_x, map_y) | L2UIMTS_ACTIVE);
			if(loadingTilesCounter == 0)
			{
				g_ui.getL2Map()->onTilesLoaded();
			}
			return;
		}
	}

	//sprintf(buf, "%d_%d", map_x, map_y);

	ULevel *level = UPackageMgr.GetULevel(buf);

	level->_mapLoc = _vector3((map_x - 20) * MAP_SIZE_X, 0, (map_y - 18) * MAP_SIZE_Z);

	//if(!level)
	//	return;

	level->setL2LibVars(varsHolder);

	m_Maps.add(level);

	loadingTilesCounter--;
	//g_ui.getL2Map()->setTileState(map_x, map_y, g_ui.getL2Map()->getTileState(map_x, map_y) | L2UIMTS_LOADED | L2UIMTS_ACTIVE);

	if(loadingTilesCounter == 0)
	{
		uint16 visibleTilesCounter = 0;
		for(uint32 i = 0; i < m_Maps.Size(); i++)
		{
			if(m_Maps[i]->_isVisible)
			{
				visibleTilesCounter++;
			}
		}

		if(visibleTilesCounter == 1 || (g_camera.getTarget().x == 0 && g_camera.getTarget().z == 0))
		{
			g_camera.setTarget(level->_mapLoc.x + MAP_SIZE_X / 2,
								-4500,
								level->_mapLoc.z + MAP_SIZE_Z / 2);
		}

		g_ui.getL2Map()->onTilesLoaded();
	}
}

void L2LevelManager::loadTile(void *arg)
{
	L2MapTileInfo *tileInfo = reinterpret_cast<L2MapTileInfo*>(arg);

	g_levelMgr.loadLevel(tileInfo->map_x, tileInfo->map_y);

	ExitThread(0);
}

void L2LevelManager::loadTiles(jfArray<L2MapTileInfo*, uint16> *tiles)
{
	for(uint16 i = 0; i < tiles->Size(); i++)
	{
		loadingTilesCounter++;
		DWORD tid;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&loadTile, (*tiles)[i], 0, &tid);
	}
}

void L2LevelManager::setLevelVisible(int map_x, int map_y, bool value)
{
	char buf[256];
	sprintf(buf, "%d_%d", map_x, map_y);

	// TODO: should fix this dumb check
	for(uint32 i = 0; i < m_Maps.Size(); i++)
	{
		char *mapName = UTIL_CopyString(m_Maps[i]->URL.Map.Txt);
		mapName[5] = 0;
		if(UTIL_strcmpi(mapName, buf) == 0)
		{
			if(m_Maps[i]->_isVisible != value)
				m_Maps[i]->_isVisible = value;
			return;
		}
	}
}

void L2LevelManager::update()
{
	g_shader.Enable(varsHolder->defaultObjectShader);

	for(uint32 l = 0; l < m_Maps.Size(); l++)
	{
		ULevel *level = m_Maps[l];

		if(!level->_isVisible)
			continue;

		level->finishLoad();

		level->_visibleDist = g_scene.getVisibleDist();
		level->_camPos = g_camera.getPos();

		/*if((g_camera.getPos().x < level->_mapLoc.x - g_scene.getVisibleDist() 
			|| g_camera.getPos().x > level->_mapLoc.x + MAP_SIZE_X - g_scene.getVisibleDist())
			&& (g_camera.getPos().z < level->_mapLoc.z - g_scene.getVisibleDist() 
			|| g_camera.getPos().z > level->_mapLoc.z + MAP_SIZE_Z - g_scene.getVisibleDist()))
		{
			continue;
		}*/

		for(uint32 i = 0; i < level->AllActors.Size(); i++)
		{
			AActor *a = level->AllActors[i];
			a->Tick();
		}

		glDisable(GL_CULL_FACE);

		glPushMatrix();
			for(uint32 i = 0; i < level->Model->Nodes.Size(); i++)
			{
				UBspNode &Node = level->Model->Nodes[i];
				UBspSurf &Surf = level->Model->Surfs[Node.iSurf];

				bool isWater = false;
				bool isSemisolidThing = false;

				if(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg || Surf.PolyFlags & PF_Unk3)
				{
					if(!(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg) && (Surf.PolyFlags & PF_NotSolid))
					{
						if(Surf.Material && UTIL_strcmpi(Surf.Material->ObjectName, "fx_e_t.WaterShader01") == 0)
						{
							isWater = true;
						}
						else
						{
							isSemisolidThing = true;
						}
					}
					else
					{
						if(!varsHolder->showBsp)
							continue;
					}
				}

				float TestY = level->Model->Points[level->Model->Verts[Node.iVertPool + 0].pVertex].Z;
				if(TestY <= -16000 || TestY >= 16000)
					continue;

				float TestX = level->Model->Points[level->Model->Verts[Node.iVertPool + 0].pVertex].X;
				if(TestX <= -327680.00 || TestX >= 327680.00)
					continue;

				float TestZ = level->Model->Points[level->Model->Verts[Node.iVertPool + 0].pVertex].Y;
				if(TestZ <= -262144.00 || TestZ >= 262144.00)
					continue;

				_vector3 aPos = _vector3(TestX, TestY, TestZ);

				if(!isWater && (aPos - g_camera.getPos()).Length() > g_scene.getVisibleDist())
					continue;

				if(Surf.Material)
					Surf.Material->StartPass();

				if(isWater)
				{
					g_shader.Disable();
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);
					glDisable(GL_ALPHA_TEST);
					glEnable(GL_BLEND);
					glColor4f(0.2, 0.35, 0.6, 0.4);
				}
				else if(isSemisolidThing)
				{
					glDisable(GL_ALPHA_TEST);
					glEnable(GL_BLEND);
					g_shader.SetUniform1f(varsHolder->defaultObjectShader, "cAlpha", 0.7);
				}

				glBegin(GL_TRIANGLE_FAN);
					for(uint32 u = 0; u < Node.NumVertices; u++)
					{
						int32 nIndex = Surf.vNormal;
						int32 vIndex = level->Model->Verts[Node.iVertPool + u].pVertex;

						UVector	TextureBase = level->Model->Points[Surf.pBase],
							TextureX = level->Model->Vectors[Surf.vTextureU],
							TextureY = level->Model->Vectors[Surf.vTextureV];

						float tu = ((level->Model->Points[vIndex] - TextureBase) | TextureX) / 128. / 4.;
						float tv = ((level->Model->Points[vIndex] - TextureBase) | TextureY) / 128. / 4.;

						glTexCoord2d(tu, tv);
						glNormal3f(level->Model->Vectors[nIndex].X, level->Model->Vectors[nIndex].Z, level->Model->Vectors[nIndex].Y);
						glVertex3f(level->Model->Points[vIndex].X, level->Model->Points[vIndex].Z, level->Model->Points[vIndex].Y);
					}
				glEnd();

				if(Surf.Material)
					Surf.Material->EndPass();

				if(isWater)
				{
					glColor4f(1, 1, 1, 1);
					glDisable(GL_BLEND);
					glEnable(GL_ALPHA_TEST);
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_LIGHTING);
					g_shader.Enable(varsHolder->defaultObjectShader);
				}
				else if(isSemisolidThing)
				{
					g_shader.SetUniform1f(varsHolder->defaultObjectShader, "cAlpha", 1.0);
					glDisable(GL_BLEND);
					glEnable(GL_ALPHA_TEST);
				}

				if(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg || Surf.PolyFlags & PF_Unk3)
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		glPopMatrix();

		glEnable(GL_CULL_FACE);
	}

	g_shader.Disable();
}

AActor *L2LevelManager::pickActor(_vector3 orig, _vector3 dir, float *dist)
{
	float minDist = 1000000000;
	UStaticMeshActor *nearestActor = 0;

	for(int i = 0; i < m_Maps.Size(); i++)
	{
		ULevel *level = m_Maps[i];

		if(!level->_isVisible)
			continue;

		for(int n = 0; n < m_Maps[i]->AllActors.Size(); n++)
		{
			UStaticMeshActor *a = dynamic_cast<UStaticMeshActor*>(m_Maps[i]->AllActors[n]);
			if(a == 0)
				continue;

			if(!a->m_Mesh)
				continue;

			if(a->m_Mesh->Surfs.Size() > 500 || a->m_Mesh->Materials.Size() > 500)
				continue;

			UVector loc = a->m_Location;

			_matrix mat, tmat;
			mat.Identity();
			tmat.Identity();
			tmat.ScalingMatrix(a->m_DrawScale3D.X * a->m_DrawScale, a->m_DrawScale3D.Z * a->m_DrawScale, a->m_DrawScale3D.Y * a->m_DrawScale);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationZ(-a->m_Rotation.Yaw / 32768. * _PI);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationX(2 * _PI - a->m_Rotation.Roll / 32768. * _PI);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationY(2 * _PI - a->m_Rotation.Pitch / 32768. * _PI);
			mat *= tmat;
			g_shader.SetUniformMatrix4fv(varsHolder->defaultObjectShader, "normalMat", mat.data);
			tmat.Identity();
			tmat.TranslationMatrix(loc.X, loc.Y, loc.Z);
			mat *= tmat;

			_vector3 bboxMin = _vector3(a->m_Mesh->BoundingBox.Min.X, a->m_Mesh->BoundingBox.Min.Z, a->m_Mesh->BoundingBox.Min.Y);
			_vector3 bboxMax = _vector3(a->m_Mesh->BoundingBox.Max.X, a->m_Mesh->BoundingBox.Max.Z, a->m_Mesh->BoundingBox.Max.Y);

			_vector3 bnl = g_math.TransformCoord(bboxMin, mat);
			_vector3 tfr = g_math.TransformCoord(bboxMax, mat);
			_vector3 bnr = g_math.TransformCoord(_vector3(bboxMax.x, bboxMin.y, bboxMin.z), mat);
			_vector3 bfr = g_math.TransformCoord(_vector3(bboxMax.x, bboxMin.y, bboxMax.z), mat);
			_vector3 bfl = g_math.TransformCoord(_vector3(bboxMin.x, bboxMin.y, bboxMax.z), mat);
			_vector3 tnr = g_math.TransformCoord(_vector3(bboxMax.x, bboxMax.y, bboxMin.z), mat);
			_vector3 tnl = g_math.TransformCoord(_vector3(bboxMin.x, bboxMax.y, bboxMin.z), mat);
			_vector3 tfl = g_math.TransformCoord(_vector3(bboxMin.x, bboxMax.y, bboxMax.z), mat);

			float t, u, v;

			if(//!a->m_Mesh->BoundingBox.IsValid ||
				(// near plane
				g_math.checkIntersectionRayTriangle(orig, dir, bnl, tnl, tnr, &t, &u, &v)
				|| g_math.checkIntersectionRayTriangle(orig, dir, bnl, tnr, bnr, &t, &u, &v)
				// right plane
				|| g_math.checkIntersectionRayTriangle(orig, dir, bnr, tnr, tfr, &t, &u, &v)
				|| g_math.checkIntersectionRayTriangle(orig, dir, bnr, tfr, bfr, &t, &u, &v)
				// far plane
				|| g_math.checkIntersectionRayTriangle(orig, dir, bfr, tfr, tfl, &t, &u, &v)
				|| g_math.checkIntersectionRayTriangle(orig, dir, bfr, tfl, bfl, &t, &u, &v)
				// left plane
				|| g_math.checkIntersectionRayTriangle(orig, dir, bfl, tfl, tnl, &t, &u, &v)
				|| g_math.checkIntersectionRayTriangle(orig, dir, bfl, tnl, bnl, &t, &u, &v)
				// bottom plane
				|| g_math.checkIntersectionRayTriangle(orig, dir, bnl, bnr, bfr, &t, &u, &v)
				|| g_math.checkIntersectionRayTriangle(orig, dir, bnl, bfr, bfl, &t, &u, &v)
				// top plane
				|| g_math.checkIntersectionRayTriangle(orig, dir, tnl, tfl, tfr, &t, &u, &v)
				|| g_math.checkIntersectionRayTriangle(orig, dir, tnl, bfr, tnr, &t, &u, &v)))
			{
				uint32 offset = 0;

				for(uint32 surf = 0; surf < a->m_Mesh->Surfs.Size(); surf++)
				{
					for(uint32 f = 0; f < a->m_Mesh->Surfs[surf].TrisCount; f++)
					{
						uint32 ind1 = a->m_Mesh->VIndices[offset * 3 + f * 3];
						uint32 ind2 = a->m_Mesh->VIndices[offset * 3 + f * 3 + 1];
						uint32 ind3 = a->m_Mesh->VIndices[offset * 3 + f * 3 + 2];
						_vector3 v1 = _vector3(a->m_Mesh->Vertices[ind1].Point.X, a->m_Mesh->Vertices[ind1].Point.Z, a->m_Mesh->Vertices[ind1].Point.Y);
						_vector3 v2 = _vector3(a->m_Mesh->Vertices[ind2].Point.X, a->m_Mesh->Vertices[ind2].Point.Z, a->m_Mesh->Vertices[ind2].Point.Y);
						_vector3 v3 = _vector3(a->m_Mesh->Vertices[ind3].Point.X, a->m_Mesh->Vertices[ind3].Point.Z, a->m_Mesh->Vertices[ind3].Point.Y);
						v1 = g_math.TransformCoord(v1, mat);
						v2 = g_math.TransformCoord(v2, mat);
						v3 = g_math.TransformCoord(v3, mat);

						t = 1000000000;
						u = 0;
						v = 0;

						if((g_math.checkIntersectionRayTriangle(orig, dir, v1, v2, v3, &t, &u, &v)
							//|| rayTri(orig, dir, v1, v3, v2, &t, &u, &v)
							))
						{
							if(t > 0 && t < minDist)
							{
								minDist = t;
								nearestActor = a;
							}
							break;
						}
					}

					offset += a->m_Mesh->Surfs[surf].TrisCount;
				}
			}
		}
	}

	*dist = minDist;
	return nearestActor;
}

UBspNode *L2LevelManager::pickBspNode(_vector3 orig, _vector3 dir, UModel **model, float *dist)
{
	float minDist = 1000000000;
	UBspNode *nearestNode = 0;

	for(uint32 l = 0; l < m_Maps.Size(); l++)
	{
		ULevel *level = m_Maps[l];

		if(!level->_isVisible)
			continue;

		for(uint32 i = 0; i < level->Model->Nodes.Size(); i++)
		{
			UBspNode &Node = level->Model->Nodes[i];
			UBspSurf &Surf = level->Model->Surfs[Node.iSurf];

			bool isWater = false;
			bool isSemisolidThing = false;

			if(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg || Surf.PolyFlags & PF_Unk3)
			{
				if(!(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg) && (Surf.PolyFlags & PF_NotSolid))
				{
					if(Surf.Material && UTIL_strcmpi(Surf.Material->ObjectName, "fx_e_t.WaterShader01") == 0)
					{
						isWater = true;
					}
					else
					{
						isSemisolidThing = true;
					}
				}
				else
				{
					if(!varsHolder->showBsp)
						continue;
				}
			}

			float TestY = level->Model->Points[level->Model->Verts[Node.iVertPool + 0].pVertex].Z;
			if(TestY <= -16000 || TestY >= 16000)
				continue;

			float TestX = level->Model->Points[level->Model->Verts[Node.iVertPool + 0].pVertex].X;
			if(TestX <= -327680.00 || TestX >= 327680.00)
				continue;

			float TestZ = level->Model->Points[level->Model->Verts[Node.iVertPool + 0].pVertex].Y;
			if(TestZ <= -262144.00 || TestZ >= 262144.00)
				continue;

			float t, u, v;

			int32 vIndex = level->Model->Verts[Node.iVertPool].pVertex;
			_vector3 v1 = _vector3(level->Model->Points[vIndex].X, level->Model->Points[vIndex].Z, level->Model->Points[vIndex].Y);

			for(uint32 n = 1; n < Node.NumVertices; n++)
			{
				int32 nIndex = Surf.vNormal;
				vIndex = level->Model->Verts[Node.iVertPool + n].pVertex;
				int32 vIndex2 = level->Model->Verts[Node.iVertPool + n + 1].pVertex;

				_vector3 v2 = _vector3(level->Model->Points[vIndex].X, level->Model->Points[vIndex].Z, level->Model->Points[vIndex].Y);
				_vector3 v3 = _vector3(level->Model->Points[vIndex2].X, level->Model->Points[vIndex2].Z, level->Model->Points[vIndex2].Y);

				if((g_math.checkIntersectionRayTriangle(orig, dir, v1, v2, v3, &t, &u, &v)
					|| g_math.checkIntersectionRayTriangle(orig, dir, v1, v3, v2, &t, &u, &v)
					))
				{
					if(t > 0 && t < minDist)
					{
						minDist = t;
						nearestNode = &Node;
						*model = level->Model;
					}
				}
				//glNormal3f(level->Model->Vectors[nIndex].X, level->Model->Vectors[nIndex].Z, level->Model->Vectors[nIndex].Y);
				//glVertex3f(level->Model->Points[vIndex].X, level->Model->Points[vIndex].Z, level->Model->Points[vIndex].Y);
			}
		}
	}

	*dist = minDist;
	return nearestNode;
}
