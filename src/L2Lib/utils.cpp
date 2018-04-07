#include "UBase.h"

#include "utils.h"

/*
=============
UTIL_CopyString
=============
*/
char *UTIL_CopyString( const char *s )
{
	char *b;
	b = new char[ strlen(s)+1 ];
	strcpy( b, s );
	return b;
}

/*
===============
UTIL_Match

Try to match string with pattern. Pattern can include the wildcards '*' and '?'.
Returns true if a match is possible.
===============
*/
bool UTIL_Match(const char* string, const char* pattern)
{
	for(;;)
	{
		// Reached the end of the pattern.
		// Match if end of string has been reached, fail otherwise
		if (!*pattern)
		//	return !*string;
			return true;	// @Prefect: wenn ich "ic_cvarlist rv" eingebe, dann will ich Ergebnisse!

		if (*pattern == '*')
		{
		//	const char *asterisk; - Unreferenzierte lokale Variable
			int fraglen;
			int minlen;

			// Skip the pattern, get the minimum number of characters it must match
			minlen = 0;
			
			pattern++;
			while(*pattern == '*' || *pattern == '?')
			{
				if (*pattern == '?')
					minlen++;
				pattern++;
			}
			
			// Pattern goes up to the end of string, we're done
			if (!*pattern)
				return (signed)strlen(string) >= minlen;
			
			// Find the non-wildcard fragment after the pattern
			fraglen = strcspn(pattern, "*?");

			string += minlen;
			while(*string)
			{
				if (!strncmp(string, pattern, fraglen))
				{
					string += fraglen;
					pattern += fraglen;
					break;
				}
				string++;
			}

			continue;
		}
		
		if (*pattern == '?')
		{
			if (!*string)
				return false;
			
			string++;
			pattern++;
			continue;
		}

		// Match normal characters
		if (*string != *pattern)
			return false;
		
		string++;
		pattern++;
		continue;
	}
}


/*
=============
UTIL_StripExtension
=============
*/
void UTIL_StripExtension( char *path )
{
	int             length;

	length = strlen(path)-1;

	while( length > 0 && path[length] != '.' )
	{
		length--;
		if( PATHSEPARATOR( path[length] ) )
			return;		// no extension
	}
	if( length )
		path[length] = 0;
}

/*
=============
UTIL_FixSlashes
=============
*/
void UTIL_FixSlashes( char *path )
{
	int i, len = strlen( path ) - 1;

	for( i=0 ; i<len ; i++ )
	{
		if( PATHSEPARATOR( path[i] ) )
			path[i] = '/';	// platform independent
	}
}


/*
=============
UTIL_FileName
=============
*/
char *UTIL_FileName( char *path )
{
	int pos;
	char *ret;

	pos = strlen( path );
	ret = path + pos - 1;

	for( ; !PATHSEPARATOR( *ret ) && pos-- > 0 ; ret-- )
		;

	return ++ret;
}


/*
=============
UTIL_FileExt
=============
*/
char *UTIL_FileExt( char *path )
{
	char *src;

	src = path + strlen( path ) - 1;

	// den '.' suchen
	while( src != path && *( src - 1 ) != '.' )
		src--;

	if( src == path )	// keine Extension?
		return UTIL_FileName( path );

	return src;
}


/*
=============
UTIL_FilePath
=============
*/
char *UTIL_FilePath( const char *fname )
{
	static char buffer[1024];
	int pos;

	pos = strlen( fname ) - 1;
	strncpy( buffer, fname, sizeof(buffer) );

	for( ; !PATHSEPARATOR( fname[pos] ) && pos >=0 ; pos-- )
		;

	buffer[pos] = 0;

	return buffer;
}

/*
==============
UTIL_32BitChecksum
creates a 32-bit additive checksum from the given data
==============
*/
uint32 UTIL_32BitChecksum( const void *ptr, int len )
{
	int nativelen, bytelen;
	uint32 checksum;
	const byte *data = (byte*)ptr;
	int i;

	checksum = 0;

	nativelen = (len & ~(sizeof(int)-1)) >> 2;
	for( i=0 ; i<nativelen ; i++ )
	{
		checksum += *((int*)data);
		data += sizeof(int);
	}

	bytelen = len % sizeof(int);	// die restlichen bytes...
	for( i=0 ; i<bytelen ; i++ )
	{
		checksum += *((byte*)data);
		data++;
	}

	return checksum;
}

int8 UTIL_strcmpi(const char *str1, const char *str2)
{
	uint16 len1 = strlen(str1);
	uint16 len2 = strlen(str2);

	if(len1 != len2)
		return 1;

	for(uint16 i = 0; i < len1; i++)
	{
		if(tolower(str1[i]) != tolower(str2[i]))
			return 1;
	}

	return 0;
}
