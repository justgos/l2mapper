#include "L2Geodata.h"

#include "main.h"

#define COLOR_FLAT 0.4, 0.4, 0.4
#define COLOR_COMPLEX 0.9, 0.9, 0.0
#define COLOR_MULTILEVEL 0.18, 0.55, 0.37
#define COLOR_NODE 1.0, 0.0, 0.0

L2Geodata::L2Geodata(void)
{
	Data = 0;
	Indexes = new uint32[65536];
}

L2Geodata::~L2Geodata(void)
{
}

void L2Geodata::TexCoordsForNSWE(float *Coords, int8 nswe)
{
	*(Coords + 0) = (float)(nswe % 4) * 16. / 64.;
	*(Coords + 1) = 64. - (float)(nswe / 4) * 16. / 64. + 0.01;

	*(Coords + 2) = (float)(nswe % 4) * 16. / 64.;
	*(Coords + 3) = 64. - (float)(nswe / 4 + 1) * 16. / 64.;

	*(Coords + 4) = (float)(nswe % 4 + 1) * 16. / 64. - 0.01;
	*(Coords + 5) = 64. - (float)(nswe / 4 + 1) * 16. / 64.;

	*(Coords + 6) = (float)(nswe % 4 + 1) * 16. / 64. - 0.01;
	*(Coords + 7) = 64. - (float)(nswe / 4) * 16. / 64. + 0.01;
}

void L2Geodata::Init()
{
	nsweTex = g_texMgr.get("geo_nswe.png");
}

bool L2Geodata::Load(uint8 map_x, uint8 map_y)
{
	_map_x = map_x;
	_map_y = map_y;

	char buf[256];
	sprintf(buf, "E:/l2/freya/server/game/data/geodata/%d_%d.l2j", _map_x, _map_y);

	std::ifstream _f;
	_f.open(buf, std::ios_base::binary);

	if(!_f.is_open())
		return false;

	_f.seekg(0, std::ios_base::end);
	uint32 size = _f.tellg();
	if(Data)
		delete Data;
	Data = new int8[size];
	
	_f.seekg(0);
	
	_f.close();
	FILE *_fl = fopen(buf, "rb");

	uint32 rs = fread(Data, 1, size, _fl);
	if(size != rs)
		MessageBoxA(0, "file read error", 0, 0);
	fclose(_fl);

	uint32 block = 0, index = 0, flor = 0;

	while(block < 65536)
	{
		int8 type = Data[index];
		Indexes[block] = index;
		block++;
		index++;
		if(type == 0)
        	index += 2; // 1x short
        else if(type == 1)
        	index += 128; // 64 x short
        else
        {
            int b;
            for(b = 0; b < 64; b++)
            {
                int8 layers = Data[index];
                index += (layers << 1) + 1;
                if (layers > flor)
                     flor = layers;
            }
        }
	}

	glGenBuffers(1, &m_VertexBuffer);

	//m_VertexArray = new float[17 * 17 * 3];
	//m_VertexArray.alloc(255 * 255 * 64 * 1 * 12);

	glGenBuffers(1, &m_IndexBuffer);

	//m_IndexArray.alloc(255 * 255 * 64 * 1 * 6);

	glGenBuffers(1, &m_TexCoordBuffer);

	uint32 VArrayOffset = 0;
	uint32 IArrayOffset = 0;

	float GeoScale = 16.0;
	float GeoHeightScale = 1.0;

	uint32 NumQuads = 0;

	for(uint16 i = 0; i < 256; i++)
	{
		for(uint16 j = 0; j < 256; j++)
		{
			// GeoData
			uint32 offset = Indexes[i * 256 + j];
			uint8 type = Data[offset];
			if(type == 0)
			{
				//NumQuads += 1;
				NumQuads += 64;
			}
			else if(type == 1)
			{
				NumQuads += 64;
			}
			else if(type == 2)
			{
				for(uint8 u = 0; u < 64; u++)
				{
					uint8 layers = (Data + offset + 1)[0];

					NumQuads += layers;

					offset += (layers << 1) + 1;
				}
			}
		}
	}

	m_VertexArray.alloc(NumQuads * 12);
	m_IndexArray.alloc(NumQuads * 6);
	m_TexCoordArray.alloc(NumQuads * 8);

	for(uint16 i = 0; i < 256; i++)
	{
		for(uint16 j = 0; j < 256; j++)
		{
			// GeoData
			uint32 offset = Indexes[i * 256 + j];
			uint8 type = Data[offset];
			if(type == 0)
			{
				int16 height = *((int16*)(Data + offset + 1));

				//glColor3f(COLOR_FLAT);

				/*m_VertexArray[VArrayOffset + 0] = i * 8 * GeoScale;
				m_VertexArray[VArrayOffset + 1] = height * GeoHeightScale;
				m_VertexArray[VArrayOffset + 2] = j * 8 * GeoScale;

				m_VertexArray[VArrayOffset + 3] = i * 8 * GeoScale;
				m_VertexArray[VArrayOffset + 4] = height * GeoHeightScale;
				m_VertexArray[VArrayOffset + 5] = (j + 1) * 8 * GeoScale;

				m_VertexArray[VArrayOffset + 6] = (i + 1) * 8 * GeoScale;
				m_VertexArray[VArrayOffset + 7] = height * GeoHeightScale;
				m_VertexArray[VArrayOffset + 8] = (j + 1) * 8 * GeoScale;

				m_VertexArray[VArrayOffset + 9] = (i + 1) * 8 * GeoScale;
				m_VertexArray[VArrayOffset + 10] = height * GeoHeightScale;
				m_VertexArray[VArrayOffset + 11] = j * 8 * GeoScale;

				m_IndexArray[IArrayOffset + 0] = (uint32)(VArrayOffset / 12) * 4 + 0;
				m_IndexArray[IArrayOffset + 1] = (uint32)(VArrayOffset / 12) * 4 + 3;
				m_IndexArray[IArrayOffset + 2] = (uint32)(VArrayOffset / 12) * 4 + 1;

				m_IndexArray[IArrayOffset + 3] = (uint32)(VArrayOffset / 12) * 4 + 3;
				m_IndexArray[IArrayOffset + 4] = (uint32)(VArrayOffset / 12) * 4 + 2;
				m_IndexArray[IArrayOffset + 5] = (uint32)(VArrayOffset / 12) * 4 + 1;

				TexCoordsForNSWE(m_TexCoordArray._elems + (uint32)(VArrayOffset / 12) * 8, 0x0f);

				VArrayOffset += 12;
				IArrayOffset += 6;*/

				for(uint8 u = 0; u < 64; u++)
				{
					m_VertexArray[VArrayOffset + 0] = (i * 8 + (int)(u / 8)) * GeoScale;
					m_VertexArray[VArrayOffset + 1] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 2] = (j * 8 + u % 8) * GeoScale;

					m_VertexArray[VArrayOffset + 3] = (i * 8 + (int)(u / 8)) * GeoScale;
					m_VertexArray[VArrayOffset + 4] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 5] = (j * 8 + u % 8 + 1) * GeoScale;

					m_VertexArray[VArrayOffset + 6] = (i * 8 + (int)(u / 8) + 1) * GeoScale;
					m_VertexArray[VArrayOffset + 7] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 8] = (j * 8 + u % 8 + 1) * GeoScale;

					m_VertexArray[VArrayOffset + 9] = (i * 8 + (int)(u / 8) + 1) * GeoScale;
					m_VertexArray[VArrayOffset + 10] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 11] = (j * 8 + u % 8) * GeoScale;

					m_IndexArray[IArrayOffset + 0] = (uint32)(VArrayOffset / 12) * 4 + 0;
					m_IndexArray[IArrayOffset + 1] = (uint32)(VArrayOffset / 12) * 4 + 3;
					m_IndexArray[IArrayOffset + 2] = (uint32)(VArrayOffset / 12) * 4 + 1;

					m_IndexArray[IArrayOffset + 3] = (uint32)(VArrayOffset / 12) * 4 + 3;
					m_IndexArray[IArrayOffset + 4] = (uint32)(VArrayOffset / 12) * 4 + 2;
					m_IndexArray[IArrayOffset + 5] = (uint32)(VArrayOffset / 12) * 4 + 1;

					TexCoordsForNSWE(m_TexCoordArray._elems + (uint32)(VArrayOffset / 12) * 8, 0x0f);

					VArrayOffset += 12;
					IArrayOffset += 6;
				}
			}
			else if(type == 1)
			{
				//glColor3f(COLOR_COMPLEX);

				//m_VertexArray.realloc(m_VertexArray.Size() + 63 * 12);
				//m_IndexArray.realloc(m_VertexArray.Size() + 63 * 6);

				for(uint8 u = 0; u < 64; u++)
				{
					int16 height = *((int16*)(Data + offset + 1));
					int8 NSWE = (int8)(height & 0x0f);
					height = (int16)(height & 0x0fff0);
					height = (int16)(height >> 1); //height / 2

					m_VertexArray[VArrayOffset + 0] = (i * 8 + (int)(u / 8)) * GeoScale;
					m_VertexArray[VArrayOffset + 1] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 2] = (j * 8 + u % 8) * GeoScale;

					m_VertexArray[VArrayOffset + 3] = (i * 8 + (int)(u / 8)) * GeoScale;
					m_VertexArray[VArrayOffset + 4] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 5] = (j * 8 + u % 8 + 1) * GeoScale;

					m_VertexArray[VArrayOffset + 6] = (i * 8 + (int)(u / 8) + 1) * GeoScale;
					m_VertexArray[VArrayOffset + 7] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 8] = (j * 8 + u % 8 + 1) * GeoScale;

					m_VertexArray[VArrayOffset + 9] = (i * 8 + (int)(u / 8) + 1) * GeoScale;
					m_VertexArray[VArrayOffset + 10] = height * GeoHeightScale;
					m_VertexArray[VArrayOffset + 11] = (j * 8 + u % 8) * GeoScale;

					m_IndexArray[IArrayOffset + 0] = (uint32)(VArrayOffset / 12) * 4 + 0;
					m_IndexArray[IArrayOffset + 1] = (uint32)(VArrayOffset / 12) * 4 + 3;
					m_IndexArray[IArrayOffset + 2] = (uint32)(VArrayOffset / 12) * 4 + 1;

					m_IndexArray[IArrayOffset + 3] = (uint32)(VArrayOffset / 12) * 4 + 3;
					m_IndexArray[IArrayOffset + 4] = (uint32)(VArrayOffset / 12) * 4 + 2;
					m_IndexArray[IArrayOffset + 5] = (uint32)(VArrayOffset / 12) * 4 + 1;

					TexCoordsForNSWE(m_TexCoordArray._elems + (uint32)(VArrayOffset / 12) * 8, NSWE);

					VArrayOffset += 12;
					IArrayOffset += 6;

					offset += 2;
				}
			}
			else if(type == 2)
			{
				//glColor3f(COLOR_MULTILEVEL);

				//m_VertexArray.realloc(m_VertexArray.Size() + 63 * 12);
				//m_IndexArray.realloc(m_VertexArray.Size() + 63 * 6);

				for(uint8 u = 0; u < 64; u++)
				{
					uint8 layers = (Data + offset + 1)[0];

					//m_VertexArray.realloc(m_VertexArray.Size() + (layers - 1) * 12);
					//m_IndexArray.realloc(m_VertexArray.Size() + (layers - 1) * 6);

					for(uint8 l = 0; l < layers; l++)
					{
						int16 height = *((int16*)(Data + offset + 2 + l * 2));
						int8 NSWE = (int8)(height & 0x0f);
						height = (int16)(height & 0x0fff0);
						height = (int16)(height >> 1); //height / 2

						m_VertexArray[VArrayOffset + 0] = (i * 8 + (int)(u / 8)) * GeoScale;
						m_VertexArray[VArrayOffset + 1] = height * GeoHeightScale;
						m_VertexArray[VArrayOffset + 2] = (j * 8 + u % 8) * GeoScale;

						m_VertexArray[VArrayOffset + 3] = (i * 8 + (int)(u / 8)) * GeoScale;
						m_VertexArray[VArrayOffset + 4] = height * GeoHeightScale;
						m_VertexArray[VArrayOffset + 5] = (j * 8 + u % 8 + 1) * GeoScale;

						m_VertexArray[VArrayOffset + 6] = (i * 8 + (int)(u / 8) + 1) * GeoScale;
						m_VertexArray[VArrayOffset + 7] = height * GeoHeightScale;
						m_VertexArray[VArrayOffset + 8] = (j * 8 + u % 8 + 1) * GeoScale;

						m_VertexArray[VArrayOffset + 9] = (i * 8 + (int)(u / 8) + 1) * GeoScale;
						m_VertexArray[VArrayOffset + 10] = height * GeoHeightScale;
						m_VertexArray[VArrayOffset + 11] = (j * 8 + u % 8) * GeoScale;

						m_IndexArray[IArrayOffset + 0] = (uint32)(VArrayOffset / 12) * 4 + 0;
						m_IndexArray[IArrayOffset + 1] = (uint32)(VArrayOffset / 12) * 4 + 3;
						m_IndexArray[IArrayOffset + 2] = (uint32)(VArrayOffset / 12) * 4 + 1;

						m_IndexArray[IArrayOffset + 3] = (uint32)(VArrayOffset / 12) * 4 + 3;
						m_IndexArray[IArrayOffset + 4] = (uint32)(VArrayOffset / 12) * 4 + 2;
						m_IndexArray[IArrayOffset + 5] = (uint32)(VArrayOffset / 12) * 4 + 1;

						TexCoordsForNSWE(m_TexCoordArray._elems + (uint32)(VArrayOffset / 12) * 8, NSWE);

						VArrayOffset += 12;
						IArrayOffset += 6;
					}
					offset += (layers << 1) + 1;
				}
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_VertexArray.Size() * sizeof(float), m_VertexArray._elems, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexArray.Size() * sizeof(uint32), m_IndexArray._elems, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_TexCoordArray.Size() * sizeof(float), m_TexCoordArray._elems, GL_STATIC_DRAW);

	return true;
}

void L2Geodata::Tick()
{
	glBindTexture(GL_TEXTURE_2D, nsweTex);

	glDisable(GL_LIGHTING);
	//glDisable(GL_TEXTURE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float MapPosX = (_map_x - 20) * MAP_SIZE_X;
	float MapPosZ = (_map_y - 18) * MAP_SIZE_Z;

	glPushMatrix();
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(MapPosX, 0, MapPosZ);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glDrawElements(GL_TRIANGLES, m_IndexArray.Size(), GL_UNSIGNED_INT, 0);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();

	glDisable(GL_BLEND);

	//glEnable(GL_TEXTURE);
	glEnable(GL_LIGHTING);
}
