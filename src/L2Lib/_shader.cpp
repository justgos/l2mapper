#include "UBase.h"

#include "_shader.h"


/*PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB  = NULL;
PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB         = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB     = NULL;
PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB   = NULL;
PFNGLSHADERSOURCEARBPROC         glShaderSourceARB         = NULL;
PFNGLCOMPILESHADERARBPROC        glCompileShaderARB        = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLATTACHOBJECTARBPROC         glAttachObjectARB         = NULL;
PFNGLGETINFOLOGARBPROC           glGetInfoLogARB           = NULL;
PFNGLLINKPROGRAMARBPROC          glLinkProgramARB          = NULL;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocationARB   = NULL;
PFNGLUNIFORM1FARBPROC            glUniform1fARB            = NULL;
PFNGLUNIFORM3FARBPROC            glUniform3fARB            = NULL;
PFNGLUNIFORM4FARBPROC            glUniform4fARB            = NULL;
PFNGLUNIFORM1IARBPROC            glUniform1iARB            = NULL;
PFNGLUNIFORMMATRIX3FVARBPROC     glUniformMatrix3fvARB     = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC     glUniformMatrix4fvARB     = NULL;
PFNGLVERTEXATTRIB3FVARBPROC      glVertexAttrib3fvARB      = NULL;
PFNGLVERTEXATTRIB4FVARBPROC      glVertexAttrib4fvARB      = NULL;
PFNGLACTIVETEXTUREARBPROC        glActiveTextureARB        = NULL;
PFNGLMULTITEXCOORD2FARBPROC      glMultiTexCoord2fARB      = NULL;

PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB = NULL;
PFNGLGETACTIVEATTRIBARBPROC    glGetActiveAttribARB    = NULL;
PFNGLGETATTRIBLOCATIONARBPROC  glGetAttribLocationARB    = NULL;

PFNGLVERTEXATTRIB1SARBPROC  glVertexAttrib1sARB    = NULL;
PFNGLVERTEXATTRIB1FARBPROC  glVertexAttrib1fARB    = NULL;
PFNGLVERTEXATTRIB4NUBARBPROC  glVertexAttrib4NubARB    = NULL;*/

GLuint _shader::GetAttributeHandle(int num, LPCSTR name)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	thndl = glGetAttribLocation(shaders[num]->program, name);
	return thndl;
#else
	return 0;
#endif
}

void _shader::SetUniform1f(int num, LPCSTR name, float x)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	thndl = glGetUniformLocation(shaders[num]->program, name);
	glUniform1f(thndl, x);
#endif
}

void _shader::SetUniform3f(int num, LPCSTR name, float x, float y, float z)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	thndl = glGetUniformLocation(shaders[num]->program, name);
	glUniform3f(thndl, x, y, z);
#endif
}
void _shader::SetUniform4f(int num, LPCSTR name, float x, float y, float z, float w)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	thndl = glGetUniformLocation(shaders[num]->program, name);
	glUniform4f(thndl, x, y, z, w);
#endif
}

void _shader::SetUniform1i(int num, LPCSTR name, int x)
{
	SetUniform1i(shaders[num], name, x);
}

void _shader::SetUniform1i(Shader *shader, LPCSTR name, int x)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	thndl = glGetUniformLocation(shader->program, name);
	glUniform1i(thndl, x);
#endif
}

void _shader::SetUniformMatrix3fv(int num, LPCSTR name, _matrix mt)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	thndl = glGetUniformLocationARB(shaders[num]->program, name);
	float tmat[9];
	memcpy(tmat, mt.data, sizeof(float) * 3);
	memcpy(tmat + 3, mt.data + 4, sizeof(float) * 3);
	memcpy(tmat + 6, mt.data + 8, sizeof(float) * 3);
	glUniformMatrix3fvARB(thndl, 1, false, tmat);
#endif
}

void _shader::SetUniformMatrix4fv(int num, LPCSTR name, float *mt)
{
	SetUniformMatrix4fv(shaders[num], name, mt);
}

void _shader::SetUniformMatrix4fv(Shader *shader, LPCSTR name, float *mt)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	thndl = glGetUniformLocation(shader->program, name);
	glUniformMatrix4fv(thndl, 1, false, mt);
#endif
}

void _shader::SetAttribute3f(GLuint hndl, float x, float y, float z)
{
#ifndef DISABLE_SHADERS
	float t[3];
	t[0] = x;
	t[1] = y;
	t[2] = z;
	glVertexAttrib3fv(hndl, t);
#endif
}

void _shader::SetAttribute4f(int num, LPCSTR name, float x, float y, float z, float w)
{
#ifndef DISABLE_SHADERS
	GLuint thndl = 0;
	float t[4];
	t[0] = x;
	t[1] = y;
	t[2] = z;
	t[3] = w;
	thndl = glGetAttribLocation(shaders[num]->program, name);
	glVertexAttrib4fv(thndl, t);
#endif
}

void _shader::SetAttribute4f(GLuint hndl, float x, float y, float z, float w)
{
#ifndef DISABLE_SHADERS
	float t[4];
	t[0] = x;
	t[1] = y;
	t[2] = z;
	t[3] = w;
	glVertexAttrib4fv(hndl, t);
#endif
}

void _shader::SetActiveTexture(int tnum)
{
#ifndef DISABLE_SHADERS
	glActiveTexture(tnum);
#endif
}

void _shader::SetMultiTexCoord2f(int tnum, float x, float y)
{
#ifndef DISABLE_SHADERS
	glMultiTexCoord2f(tnum, x, y);
#endif
}

void _shader::SetVertexAttrib1s(int index, int16 v)
{
	glVertexAttrib1s(index, v);
}

void _shader::SetVertexAttrib1f(int index, float v)
{
	glVertexAttrib1f(index, v);
}

void _shader::SetVertexAttrib4Nub(int index, uint8 v1, uint8 v2, uint8 v3, uint8 v4)
{
	glVertexAttrib4Nub(index, v1, v2, v3, v4);
}

void _shader::BindAttribLocation(int num, int index, char *attrib)
{
	glBindAttribLocation(shaders[num]->program, index, attrib);
}

uint8 _shader::Add(char *vs, char *fs)
{
	return Add(vs, fs, "");
}

uint8 _shader::Add(char *vs, char *fs, char *name)
{
#ifndef DISABLE_SHADERS
	GLint result;
	char info[2048];

	bool newShader = true;
	int8 shaderId = -1;

	for(int i = 0; i < num_shaders; i++)
	{
		if(strcmp(name, shaders[i]->name) == 0)
		{
			glDeleteProgram(shaders[i]->program);
			delete shaders[i];
			shaderId = i;
			newShader = false;
		}
	}

	if(newShader)
	{
		Shader **NewShaders = new Shader*[num_shaders + 1];
		memcpy(NewShaders, shaders, num_shaders * sizeof(Shader*));
		delete [] shaders;
		shaders = NewShaders;

		shaderId = num_shaders;
	}

	shaders[shaderId] = new Shader;

	shaders[shaderId]->name = name;

	/*glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
	glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
	glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
	glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
	glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
	glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
	glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
	glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
	glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocationARB");
	glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
	glUniform3fARB            = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
	glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
	glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	glUniformMatrix3fvARB     = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress("glUniformMatrix3fvARB");
	glUniformMatrix4fvARB     = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress("glUniformMatrix4fvARB");
	glVertexAttrib3fvARB      = (PFNGLVERTEXATTRIB3FVARBPROC)wglGetProcAddress("glVertexAttrib3fvARB");
	glVertexAttrib4fvARB      = (PFNGLVERTEXATTRIB4FVARBPROC)wglGetProcAddress("glVertexAttrib4fvARB");
	glActiveTextureARB        = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB      = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");

	glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)wglGetProcAddress("glBindAttribLocationARB");
    glGetActiveAttribARB    = (PFNGLGETACTIVEATTRIBARBPROC)wglGetProcAddress("glGetActiveAttribARB");
    glGetAttribLocationARB  = (PFNGLGETATTRIBLOCATIONARBPROC)wglGetProcAddress("glGetAttribLocationARB");

	glVertexAttrib1sARB  = (PFNGLVERTEXATTRIB1SARBPROC)wglGetProcAddress("glVertexAttrib1sARB");
	glVertexAttrib1fARB  = (PFNGLVERTEXATTRIB1FARBPROC)wglGetProcAddress("glVertexAttrib1fARB");
	glVertexAttrib4NubARB  = (PFNGLVERTEXATTRIB4NUBARBPROC)wglGetProcAddress("glVertexAttrib4NubARB");*/

	shaders[shaderId]->program = glCreateProgram();

	char *Data;
	const char *pData;

		pData = vs;
		shaders[shaderId]->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shaders[shaderId]->vertexShader, 1, &pData, NULL);
		glCompileShader(shaders[shaderId]->vertexShader);
		glAttachShader(shaders[shaderId]->program, shaders[shaderId]->vertexShader);

		pData = fs;
		shaders[shaderId]->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shaders[shaderId]->fragmentShader, 1, &pData, NULL);
		glCompileShader(shaders[shaderId]->fragmentShader);
		glAttachShader(shaders[shaderId]->program, shaders[shaderId]->fragmentShader);

	//glBindAttribLocationARB(shaders[shaderId]->program, ATTRIB_TEXINFO1, "TexInfo1");
	//glBindAttribLocationARB(shaders[shaderId]->program, ATTRIB_TEXINFO2, "TexInfo2");
	//glBindAttribLocationARB(shaders[shaderId]->program, ATTRIB_LIGHTVAL, "LightVal");

	glLinkProgram(shaders[shaderId]->program);

	//glGetObjectParameterivARB(shaders[shaderId]->vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
	glGetShaderiv(shaders[shaderId]->vertexShader, GL_COMPILE_STATUS, &result);
	if(result)
	{
		//glAttachObjectARB(shaders[shaderId]->program, shaders[shaderId]->vertexShader);
	}
	else
	{
		glGetShaderInfoLog(shaders[shaderId]->vertexShader, 2048, 0, info);
		MessageBoxA(0, info, 0, 0);
	}

	glGetShaderiv(shaders[shaderId]->fragmentShader, GL_COMPILE_STATUS, &result);
	if(result)
	{
		//glAttachObjectARB(shaders[shaderId]->program, shaders[shaderId]->vertexShader);
	}
	else
	{
		glGetShaderInfoLog(shaders[shaderId]->fragmentShader, 2048, 0, info);
		MessageBoxA(0, info, 0, 0);
	}

	if(newShader)
		num_shaders++;
	return shaderId;
#else
	return 0;
#endif
}

Shader *_shader::GetByName(char *name)
{
	for(int i = 0; i < num_shaders; i++)
	{
		if(strcmp(name, shaders[i]->name) == 0)
			return shaders[i];
	}

	return 0;
}

void _shader::Enable(int num)
{
#ifndef DISABLE_SHADERS
	glUseProgram(shaders[num]->program);
#endif
}

void _shader::Enable(Shader *shader)
{
	glUseProgram(shader->program);
}

void _shader::Disable()
{
#ifndef DISABLE_SHADERS
	glUseProgram(0);
#endif
}

_shader::_shader(void)
{
	num_shaders = 0;
}

_shader::~_shader(void)
{
}
