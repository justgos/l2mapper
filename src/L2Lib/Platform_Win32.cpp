#include "UBase.h"
#include "Platform_Win32.h"
#include "UPackage.h"

jfArray<char *, uint32> *UTIL_FindFiles(char *dir, char *pattern)
{
	jfArray<char *, uint32> *Result = new jfArray<char *, uint32>();

	char curdir[CM_SYSTEM_MAXNAME];
	intptr_t handle;
	_finddata_t	fd;

	getcwd(curdir, sizeof(curdir));

	if(chdir(dir) == -1)
		return Result;

	handle = _findfirst(pattern, &fd);

	if(handle == -1)
		return Result;

	do
	{
		if(!(fd.attrib & _A_SUBDIR))
			Result->add(UTIL_CopyString(fd.name));
	}
	while(_findnext(handle, &fd) != -1);

	_findclose(handle);

	chdir(curdir);

	return Result;
}
