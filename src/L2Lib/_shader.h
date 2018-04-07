#pragma once

#define ATTRIB_TEXINFO1 10
#define ATTRIB_TEXINFO2 11
//#define ATTRIB_TEXINFO3 2
#define ATTRIB_LIGHTVAL 12

struct DLLEXPORT Shader
{
	char *name;
	GLhandleARB program;
	GLhandleARB vertexShader;
	GLhandleARB fragmentShader;
};

class DLLEXPORT _shader
{
public:
	uint8 Add(char *vs, char *fs);
	uint8 Add(char *vs, char *fs, char *name);
	Shader *Get(int num) { return num >= num_shaders ? 0 : shaders[num]; };
	Shader *GetByName(char *name);
	void Enable(int num);
	void Enable(Shader *shader);
	void Disable();
	GLuint GetAttributeHandle(int num, LPCSTR name);
	void SetUniform1f(int num, LPCSTR name, float x);
	void SetUniform3f(int num, LPCSTR name, float x, float y, float z);
	void SetUniform4f(int num, LPCSTR name, float x, float y, float z, float w);
	void SetUniform1i(int num, LPCSTR name, int x);
	void SetUniform1i(Shader *shader, LPCSTR name, int x);
	void SetUniformMatrix3fv(int num, LPCSTR name, _matrix mt);
	void SetUniformMatrix4fv(int num, LPCSTR name, float *mt);
	void SetUniformMatrix4fv(Shader *shader, LPCSTR name, float *mt);
	void SetAttribute3f(GLuint hndl, float x, float y, float z);
	void SetAttribute4f(int num, LPCSTR name, float x, float y, float z, float w);
	void SetAttribute4f(GLuint hndl, float x, float y, float z, float w);
	void SetActiveTexture(int tnum);
	void SetMultiTexCoord2f(int tnum, float x, float y);
	void SetVertexAttrib1s(int index, int16 v);
	void SetVertexAttrib1f(int index, float v);
	void SetVertexAttrib4Nub(int index, uint8 v1, uint8 v2, uint8 v3, uint8 v4);
	void BindAttribLocation(int num, int index, char *attrib);
private:
	//char *getFile(LPCSTR fname);
private:
	Shader** shaders;
	//Shader *shaders;
	int num_shaders;
public:
	_shader(void);
	~_shader(void);
};
