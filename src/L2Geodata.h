#pragma once

#include "_afx.h"

class L2Geodata
{
public:
	void Init();
	bool Load(uint8 map_x, uint8 map_y);
	void Tick();
private:
	void TexCoordsForNSWE(float *Coords, int8 nswe);
private:
	GLuint nsweTex;

	uint32 *Indexes;
	int8 *Data;

	uint8 _map_x;
	uint8 _map_y;

	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
	GLuint m_TexCoordBuffer;

	jfArray<float, uint32> m_VertexArray;
	jfArray<GLuint, uint32> m_IndexArray;
	jfArray<float, uint32> m_TexCoordArray;
public:
	L2Geodata(void);
	~L2Geodata(void);
};

