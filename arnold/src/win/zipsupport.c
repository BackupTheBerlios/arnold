#include <windows.h>
#include "zipsupport.h"
#include "zlib\contrib\minizip\unzip.h"
#include <stdio.h>
#include <stdlib.h>

// add unzip.c/unzip.h from contrib\minizip\
// to build.
// add ioapi.c/ioapi.h from contrib\minizip too
//
// ZLIB_DLL is defined
// try and define function and link to it.

typedef int  (WINAPI *INFLATE_FUNC) OF((z_streamp strm, int flush));
typedef int (WINAPI *INFLATE_END_FUNC) OF((z_streamp strm));
typedef uLong (WINAPI *CRC32_FUNC) OF((uLong crc, const Bytef *buf, uInt len));
typedef uLongf  *(WINAPI *GET_CRC_TABLE_FUNC) OF(());
typedef int (WINAPI *INFLATEINIT2_FUNC) OF((z_streamp strm, int windowBits, const char *version, int stream_size));	

INFLATE_FUNC inflate_func = NULL;
INFLATE_END_FUNC inflateEnd_func = NULL;
CRC32_FUNC crc32_func = NULL;
GET_CRC_TABLE_FUNC get_crc_table_func = NULL;
INFLATEINIT2_FUNC inflateInit2_func = NULL;

int WINAPI inflate(z_streamp strm, int flush)
{
	return inflate_func(strm, flush);
}

int WINAPI inflateEnd(z_streamp strm)
{
	return inflateEnd_func(strm);
}

uLong WINAPI crc32(uLong crc, const Bytef *buf, uInt len)
{
	return crc32_func(crc, buf, len);
}

const uLongf * WINAPI get_crc_table()
{
	return get_crc_table_func();
}
int WINAPI inflateInit2_(z_streamp strm, int  windowBits,const char *version, int stream_size)
{
	return inflateInit2_func(strm, windowBits, version, stream_size);
}

//INFLATE_FUNC inflate = NULL;
void ZIP_Init()
{
	HMODULE hModule = LoadLibrary("zlib.dll");
	if (hModule!=NULL)
	{
		inflate_func = (INFLATE_FUNC)GetProcAddress(hModule,"inflate");
		inflateEnd_func = (INFLATE_END_FUNC)GetProcAddress(hModule,"inflateEnd");
		crc32_func = (CRC32_FUNC)GetProcAddress(hModule,"crc32");
		get_crc_table_func = (GET_CRC_TABLE_FUNC)GetProcAddress(hModule,"get_crc_table");
		inflateInit2_func = (INFLATEINIT2_FUNC)GetProcAddress(hModule,"inflateInit2_");
	}
}

// make sure zlib is in path.


#ifdef _UNICODE
zlib_filefunc_def file_api;


/* TROELS K addition for UNICODE support */
static voidpf ZCALLBACK fopen_file_func_UNICODE(voidpf opaque, const char *filename, int mode)
{
   FILE* file = NULL;
   const wchar_t* mode_fopen = NULL;
   if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER)==ZLIB_FILEFUNC_MODE_READ)
      mode_fopen = L"rb";
   else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
      mode_fopen = L"r+b";
   else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
      mode_fopen = L"wb";
   if (!((filename == NULL) || (mode_fopen == NULL)))
      file = _wfopen((const wchar_t *)filename, mode_fopen);
   return file;
}
#endif

int ZipSupport_IsZipArchive(const TCHAR *pZipFilename)
{
	unzFile unz;
#ifdef _UNICODE
	/* TODO: Find out why this crashes when using UNICODE and non-UNICODE method.
	It tries to read data into a NULL buffer!!! tested with ZLIB DLL 1.1.4 */
	fill_fopen_filefunc(&file_api);

	file_api.zopen_file = fopen_file_func_UNICODE;

	/* filename is passed onto API which will recast it back to a wide character
	filename and use it appropiatly */
	unz = unzOpen2((const char*)pZipFilename, &file_api);
#else
	unz = unzOpen(pZipFilename);
#endif

	if (unz!=NULL)
	{
		/* open succeeded */
		int Code = ZIP_SUPPORT_OK;
		int result;

		/* get first file */
		result = unzGoToFirstFile(unz);

		/* if not end of file list and not ok, then assume error */
		if ((result!=UNZ_END_OF_LIST_OF_FILE) && (result!=UNZ_OK))
			Code = ZIP_SUPPORT_ERROR_IN_ARCHIVE;

		if (Code==ZIP_SUPPORT_OK)
		{
			/* continue traversing files */
			while (result==UNZ_OK)
			{
				/* get next file */
				result = unzGoToNextFile(unz);
			}

			/* if not end of file list, assume that there is an error */
			if (result!=UNZ_END_OF_LIST_OF_FILE)
				Code = ZIP_SUPPORT_ERROR_IN_ARCHIVE;
		}

		/* close the archive */
		unzClose(unz);

		/* return status */
		return Code;
	}

	/* not a supported archive */
	return ZIP_SUPPORT_NOT_ARCHIVE;
}


/* this assumes zip is ok */
void	ZipSupport_GenerateFileList(const TCHAR *pZipFilename, file_list *pList)
{
	unzFile unz;
#ifdef _UNICODE
	/* TODO: Find out why this crashes when using UNICODE and non-UNICODE method.
	It tries to read data into a NULL buffer!!! tested with ZLIB DLL 1.1.4 */
	fill_fopen_filefunc(&file_api);

	file_api.zopen_file = fopen_file_func_UNICODE;

	/* filename is passed onto API which will recast it back to a wide character
	filename and use it appropiatly */
	unz = unzOpen2((const char*)pZipFilename, &file_api);
#else
	unz = unzOpen(pZipFilename);
#endif


	if (unz!=NULL)
	{
		int result;

		/* get first file */
		result = unzGoToFirstFile(unz);

		if (result==UNZ_OK)
		{
			while (result!=UNZ_END_OF_LIST_OF_FILE)
			{
				char filename_buffer[MAX_PATH+1];

				unz_file_info file_info;

				if (unzGetCurrentFileInfo(unz, &file_info,
					filename_buffer, sizeof(filename_buffer),
					NULL, 0, NULL, 0)==UNZ_OK)
				{
					file_list_linked_item *item;

					/* fill in file details */
					item = file_list_allocate_item(filename_buffer, file_info.uncompressed_size);
					if (item)
						file_list_add_item(pList, item);
				}

				/* get next file */
				result = unzGoToNextFile(unz);
			}
		}
		/* close archive */
		unzClose(unz);
	}
}

BOOL	ZipSupport_ExtractFile(const TCHAR *pZipFilename, const char *pFilename, const TCHAR *pOutputFilename)
{
	BOOL bOK = FALSE;
	unzFile unz;

#ifdef _UNICODE
	/* TODO: Find out why this crashes when using UNICODE and non-UNICODE method.
	It tries to read data into a NULL buffer!!! tested with ZLIB DLL 1.1.4 */
	fill_fopen_filefunc(&file_api);

	file_api.zopen_file = fopen_file_func_UNICODE;

	/* filename is passed onto API which will recast it back to a wide character
	filename and use it appropiatly */
	unz = unzOpen2((char*)pZipFilename, &file_api);
#else
	unz = unzOpen(pZipFilename);
#endif

	if (unz!=NULL)
	{
		int result;

		/* get first file */
		result = unzGoToFirstFile(unz);

		if (result==UNZ_OK)
		{
			/* locate file by name - sensitive case match */
			if (unzLocateFile(unz, pFilename, 1)==UNZ_OK)
			{
				/* open the current file */
				if (unzOpenCurrentFile(unz)==UNZ_OK)
				{
					unsigned char *buffer = (unsigned char *)malloc(4096);

					if (buffer!=NULL)
					{
						int result;
						FILE *fh;

						/* open temporary file */
						fh = _tfopen(pOutputFilename, _T("w+b"));

						if (fh!=NULL)
						{

							do
							{
								/* read as much data as possible */
								result = unzReadCurrentFile(unz, buffer, 4096);

								if (result>0)
								{
									/* write to output temporary file */
									fwrite(buffer, result, 1,fh);
								}
							}
							while (result>0);

							/* result < or == 0 */
							if (result==0)
							{
								bOK = TRUE;
							}

							/* close temporary file */
							fclose(fh);
						}

						free(buffer);
					}

					/* close the current file */
					unzCloseCurrentFile(unz);
				}
			}
		}
		/* close archive */
		unzClose(unz);
	}

	return bOK;
}
