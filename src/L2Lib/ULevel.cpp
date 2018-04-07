#include "UBase.h"
#include "UPackage.h"

#include "ULevel.h"
#include "UBrush.h"
#include "UShader.h"
#include "math.h"

ULevel::ULevel(void)
{
	_isVisible = true;
}

ULevel::~ULevel(void)
{
	for(uint32 i = 0; i < AllObjects.Size(); i++)
	{
		delete AllObjects[i];
	}
}

bool ULevel::SetVariable(const UProperty& Prop)
{
	return true;
}

void ULevel::setL2LibVars(L2LibVarsHolder *vars)
{
	for(uint32 i = 0; i < AllActors.Size(); i++)
	{
		AActor *a = AllActors[i];
		a->setL2LibVars(vars);
	}

	for(uint32 i = 0; i < Model->Nodes.Size(); i++)
	{
		UBspNode &Node = Model->Nodes[i];
		UBspSurf &Surf = Model->Surfs[Node.iSurf];

		Surf.setL2LibVars(vars);
	}
}

void ULevel::Tick()
{
	//char buf[256];

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//double modelview[16];
	//glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	//_vector3 camPos = _vector3(modelview[12], modelview[13], modelview[14]);

	// ------------------------------------
	for(uint32 i = 0; i < AllActors.Size(); i++)
	{
		/*FILE *fl = fopen("log.txt", "a");
		sprintf(buf, "%d", i);
		fputs(buf, fl);
		fputs("\r\n", fl);
		fclose(fl);*/

		AActor *a = AllActors[i];
		a->Tick();
	}
	// ------------------------------------
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	///*float MinX, MinZ, MaxX, MaxZ;
	//MinX = 1000000;
	//MinZ = 1000000;
	//MaxX = -1000000;
	//MaxZ = -1000000;*/

	glPushMatrix();
		for(uint32 i = 0; i < Model->Nodes.Size(); i++)
		{
			UBspNode &Node = Model->Nodes[i];
			UBspSurf &Surf = Model->Surfs[Node.iSurf];

			//if(!Surf.Actor || !(((UBrush*)Surf.Actor)->m_CsgOper & CSG_Intersect))
			//	continue;

			//if(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg) // || Surf.PolyFlags & 0x0000a000
			//	continue;

			bool isWater = false;

			if(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg)
			{
				if(Surf.Material && strcmpi(Surf.Material->ObjectName, "fx_e_t.WaterShader01") == 0)
				{
					isWater = true;
				}
				else
				{
					continue;
				}
			}

			float TestY = Model->Points[Model->Verts[Node.iVertPool + 0].pVertex].Z;
			if(TestY <= -16000 || TestY >= 16000)
				continue;

			float TestX = Model->Points[Model->Verts[Node.iVertPool + 0].pVertex].X;
			if(TestX <= -327680.00 || TestX >= 327680.00)
				continue;

			float TestZ = Model->Points[Model->Verts[Node.iVertPool + 0].pVertex].Y;
			if(TestZ <= -262144.00 || TestZ >= 262144.00)
				continue;

			_vector3 aPos = _vector3(TestX, TestY, TestZ);

			if(!isWater && (aPos - _camPos).Length() > _visibleDist)
				continue;

			//if(Surf.PolyFlags & PF_Invisible || Surf.PolyFlags & PF_NotSolid)
			//	continue;
			//  || Node.NodeFlags & EBspNodeFlags::NF_NotCsg
			//if((Surf.PolyFlags & 0x0000a000 || Surf.PolyFlags & PF_Invisible) && (Surf.Actor ? ((UBrush*)Surf.Actor)->m_CsgOper == CSG_Add : true))
			//if((Surf.Actor ? ((UBrush*)Surf.Actor)->m_CsgOper != CSG_Add : true))
			//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			//if(!(Surf.PolyFlags & PF_Portal))
			//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			
			//if(Surf.Material && strcmpi(Surf.Material->ObjectName, "fx_e_t.WaterShader01") == 0)
			//if(Surf.Material && (strstr(Surf.Material->ObjectName, "Water") != 0 || strstr(Surf.Material->ObjectName, "water") != 0))
			//	isWater = true;

			if(Surf.Material)
				Surf.Material->StartPass();

			if(isWater)
			{
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_LIGHTING);
				glDisable(GL_ALPHA_TEST);
				glEnable(GL_BLEND);
				glColor4f(0.2, 0.35, 0.6, 0.4);
			}

			/*if(dynamic_cast<UShader*>(Surf.Material) != NULL)
			{
				if(!Surf.Actor)
				{
					char *nm = Surf.Actor->Tag;
					int g = 0;
					g = 1;
				}
			}*/
			glBegin(GL_TRIANGLE_FAN);
				for(uint32 u = 0; u < Node.NumVertices; u++)
				{
					int32 nIndex = Surf.vNormal;
					int32 vIndex = Model->Verts[Node.iVertPool + u].pVertex;

					UVector	TextureBase = Model->Points[Surf.pBase],
						TextureX = Model->Vectors[Surf.vTextureU],
						TextureY = Model->Vectors[Surf.vTextureV];

					float tu = ((Model->Points[vIndex] - TextureBase) | TextureX) / 128. / 4.;
					float tv = ((Model->Points[vIndex] - TextureBase) | TextureY) / 128. / 4.;

					glTexCoord2d(tu, tv);
					glNormal3f(Model->Vectors[nIndex].X, Model->Vectors[nIndex].Z, Model->Vectors[nIndex].Y);
					glVertex3f(Model->Points[vIndex].X, Model->Points[vIndex].Z, Model->Points[vIndex].Y);

					/*if(Model->Points[vIndex].X < MinX)
						MinX = Model->Points[vIndex].X;
					if(Model->Points[vIndex].Y < MinZ)
						MinZ = Model->Points[vIndex].Y;

					if(Model->Points[vIndex].X > MaxX)
						MaxX = Model->Points[vIndex].X;
					if(Model->Points[vIndex].Y > MaxZ)
						MaxZ = Model->Points[vIndex].Y;*/
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
			}

			if(Surf.PolyFlags & 0x0000a000 || Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	glPopMatrix();
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*glEnable(GL_BLEND);

	for(uint32 i = 0; i < AllActors.Size(); i++)
	{
		AActor *a = AllActors[i];

		AllActors[i]->Tick();
	}*/
}

UPackage& ULevel::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	UObject *TObj;

	int32 Index;

	int32 DbNum, DbMax;

	Ar >> DbNum >> DbMax;

	AllObjects.realloc(DbMax);

	/*tbb::parallel_for( tbb::blocked_range<size_t>(0,DbMax),
		[=](const tbb::blocked_range<size_t>& r) {
			//for(size_t i=r.begin(); i!=r.end(); ++i)
			//	Foo(a[i]);
			UObject *TObj;

			for(size_t i = r.begin(); i!=r.end(); ++i)
			{
				(UPackage&)Ar >> (UObject*)TObj;
				AllObjects[i] = (UObject*)TObj;
			}
		}
	);*/
	
	for(uint32 i = 0; i < DbMax; i++)
	{
		Ar >> (UObject*)TObj;
		AllObjects[i] = (UObject*)TObj;
		//Ar >> UIndex(Index);
		//printf("Loaded: %s\n", TObj->ObjectName);
	}

	Ar >> DbNum >> DbMax;

	AllObjects.realloc(DbMax);
	for(uint32 i = 0; i < DbMax; i++)
	{
		Ar >> (UObject*)TObj;
		AllObjects[i] = (UObject*)TObj;
		//printf("Loaded: %s\n", TObj->ObjectName);
	}

	/*uint32 t = Ar.Tell();
	for(uint32 u = 0; u < 200; u++)
	{
		Ar.Seek(t + u, U_SEEK_SET);
		int32 Index;
		Ar >> UIndex(Index);
		//if(Index < 0 && -Index - 1 < Ar.Imports.Size())
		//	printf("Import: %d, %s (%d)\n", u, Ar.Imports[-Index - 1].Name, Index);
		if(Index > 0 && Index - 1 < Ar.Exports.Size())
			printf("Export: %d, %s (%d)\n", u, Ar.Exports[Index - 1].Name, Index);
		//if(Index >= 0 && Index < Ar.Names.Size())
		//	printf("Name: %d, %s (%d, 0x%02X)\n", u, Ar.Names[Index].Name, Index, (Index > 0 && Index < 256 ? (uint8)Index : 0));
	}*/

	Ar >> URL; // FURL
	Ar.Seek(7); // ich weiß mir gerade nicht besser zu helfen!

	Ar >> ReachSpecs;
	Ar >> *(UObject**)&Model; // main model

	int32 BaseIndex = 0;
	for(uint32 i = 0; i < Model->Nodes.Size(); i++)
	{
		Model->Nodes[i].BaseIndex = BaseIndex;
		BaseIndex += Model->Nodes[i].NumVertices;
	}

	AActor *TActor;
	UBrush *TBrush;

	for(uint32 i = 0; i < AllObjects.Size(); i++)
	{
		TBrush = dynamic_cast<UBrush*>(AllObjects[i]);
		if(TBrush)
			continue;
		TActor = dynamic_cast<AActor*>(AllObjects[i]);
		if(TActor)
		{
			TActor->setLevel(this);
			AllActors.add(TActor);
		}
	}

	/*for(uint32 i = 0; i < Model->Nodes.Size(); i++)
	{
		UBspNode &Node = Model->Nodes[i];
		UBspSurf &Surf = Model->Surfs[Node.iSurf];

		bool isWater = false;

		if(Surf.PolyFlags & PF_Invisible || Node.NodeFlags & EBspNodeFlags::NF_NotCsg)
		{
			if(Surf.Material && strcmpi(Surf.Material->ObjectName, "fx_e_t.WaterShader01") == 0)
			{
				isWater = true;
			}
			else
			{
				continue;
			}
		}
	}*/

	return Ar;
}

void ULevel::finishLoad()
{
	if(_UIsInitialized)
		return;

	for(uint32 i = 0; i < AllObjects.Size(); i++)
	{
		AllObjects[i]->finishLoad();
	}

	Model->finishLoad();

	_UIsInitialized = true;
}
