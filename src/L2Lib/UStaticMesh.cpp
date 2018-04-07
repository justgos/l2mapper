#include "UBase.h"
#include "UPackage.h"

#include "UStaticMesh.h"
#include "UPackageManager.h"

UStaticMesh::UStaticMesh(void)
	: UPrimitive()
{
	MaterialCount = 0;

	m_NumSurfs = 0;

	m_RenderLists = 0;
}

UStaticMesh::~UStaticMesh(void)
{
	glDeleteLists(m_RenderLists, Surfs.Size());

	Surfs.clear();
	Vertices.clear();
	TexCoords.clear();
	VIndices.clear();
}

bool UStaticMesh::SetVariable(const class UProperty& Prop)
{
	if(UPrimitive::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Materials") == 0)
	{
		Materials.realloc(Prop.ArrayLength);
		memcpy(Materials._elems, Prop.Data, Prop.ArrayLength * sizeof(UMaterialContainer*));

		return true;
	}

	return false;
}

void UStaticMesh::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;

	//Surfs.clear();
	/*Vertices.clear();
	VertColors.clear();
	VertColors2.clear();
	TexCoords.clear();
	VIndices.clear();
	VIndices2.clear();*/
}

UPackage& UStaticMesh::Serialize(UPackage& Ar)
{
	UPrimitive::Serialize(Ar);

	uint32 UnknownDW;

	/*for(uint32 u = 0; u < MaterialCount - 1; u++)
	{
		Ar >> Materials[u];
	}

	Ar.Seek(1, U_SEEK_CUR);*/

	Ar >> Surfs;
	if(Surfs.Size() > 500)
	{
		printf("\n\n=========================== TOO MANY SURFACES! (%s) ===========================\n\n", Surfs.Size());
	}
	Ar >> BBox;
	/*uint32 t = Ar.Tell();
	for(uint32 i = 0; i < 1000; i++)
	{
		int32 Index;
		Ar.Seek(t + i, U_SEEK_SET);
		Ar >> UIndex(Index);
		printf("%d\n", Index);
	}*/
	//Ar >> UnknownDW;
	Ar >> Vertices;
	/*for(uint32 i = 0; i < 1000; i++)
	{
		//printf("%f %f %f\n", Vertices[i].Normal.X, Vertices[i].Normal.Y, Vertices[i].Point.Z);
	}*/
	Ar >> UnknownDW;	// meitens 1
	Ar >> VertColors;
	Ar >> UnknownDW;	// meitens 1
	Ar >> VertColors2;	// ??
	Ar >> UnknownDW;	// meitens 1
	Ar >> TexCoords;	// TexCoords liest da das UnknownDW!
	Ar >> VIndices;
	//int32 Index;
	//Ar >> UIndex(Index);
	//Ar.TestDump(6 * 100, true);
	//Ar.Seek(Surfs[0].OffsetSize, U_SEEK_SET);
	/*for(uint32 i = 0; i < 1000; i++)
	{
		uint16 v1, v2, v3;
		Ar >> v1;
		Ar >> v2;
		Ar >> v3;
		//if(v1 != 24672 && v1 != -13108)
			printf("%d %d %d\n", v1, v2, v3);
	}*/
	Ar >> UnknownDW;	// meitens 1
	Ar >> VIndices2;
	Ar >> UnknownDW;	// meitens 1

	VertColors.clear();
	VertColors2.clear();
	VIndices2.clear();

	return Ar;
}

void UStaticMesh::finishLoad()
{
	if(_UIsInitialized)
		return;

	for(uint32 surf = 0; surf < Surfs.Size(); surf++)
	{
		UMaterialContainer *tc = Materials[surf];
		if(Materials[surf]->GetMaterial())
			Materials[surf]->GetMaterial()->finishLoad();
	}

	m_RenderLists = glGenLists(Surfs.Size());
	
	uint32 offset = 0;

	for(uint32 surf = 0; surf < Surfs.Size(); surf++)
	{
		UStaticMeshSurf &ts = Surfs[surf];
		UMaterialContainer *tc = Materials[surf];
		glNewList(m_RenderLists + surf, GL_COMPILE);

		glPushMatrix();
			glBegin(GL_TRIANGLES);
				if(offset * 3 + ts.TrisCount * 3 > VIndices.Size())
				{
					ts.TrisCount = (VIndices.Size() - offset * 3) / 3;
					if(ts.TrisCount < 0)
						ts.TrisCount = 0;
				}
				for(uint32 f = 0; f < Surfs[surf].TrisCount; f++)
				{
					uint32 v1 = VIndices[offset * 3 + f * 3];
					uint32 v2 = VIndices[offset * 3 + f * 3 + 1];
					uint32 v3 = VIndices[offset * 3 + f * 3 + 2];

					float nx1 = Vertices[v1].Normal.X;
					float ny1 = Vertices[v1].Normal.Y;
					float nz1 = Vertices[v1].Normal.Z;

					//glTexCoord2d(TexCoords[offset * 2 + f * 2]);
					glTexCoord2d(TexCoords[0].Elements[v1].U, TexCoords[0].Elements[v1].V);
					glNormal3f(Vertices[v1].Normal.X, Vertices[v1].Normal.Z, Vertices[v1].Normal.Y);
					glVertex3f(Vertices[v1].Point.X, Vertices[v1].Point.Z, Vertices[v1].Point.Y);
					glTexCoord2d(TexCoords[0].Elements[v2].U, TexCoords[0].Elements[v2].V);
					glNormal3f(Vertices[v2].Normal.X, Vertices[v2].Normal.Z, Vertices[v2].Normal.Y);
					glVertex3f(Vertices[v2].Point.X, Vertices[v2].Point.Z, Vertices[v2].Point.Y);
					glTexCoord2d(TexCoords[0].Elements[v3].U, TexCoords[0].Elements[v3].V);
					glNormal3f(Vertices[v3].Normal.X, Vertices[v3].Normal.Z, Vertices[v3].Normal.Y);
					glVertex3f(Vertices[v3].Point.X, Vertices[v3].Point.Z, Vertices[v3].Point.Y);
				}
				offset += Surfs[surf].TrisCount;
			glEnd();
		glPopMatrix();

		glEndList();
	}

	OnLoaded();

	_UIsInitialized = true;
}
