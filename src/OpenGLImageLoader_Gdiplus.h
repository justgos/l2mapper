#pragma once

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include <windows.h>
#include <gdiplus.h>

#pragma comment (lib, "gdiplus.lib")
 
#include <MyGUI_DataManager.h>
#include <MyGUI_OpenGLImageLoader.h>
#include <MyGUI_OpenGLDataManager.h>
#include <MyGUI_UString.h>

class OpenGLImageLoader_Gdiplus : public MyGUI::OpenGLImageLoader
{
public:
    OpenGLImageLoader_Gdiplus() :
        MyGUI::OpenGLImageLoader()
    {
        // Take this out of here if you initialise DevIL elsewhere
        //ilInit();

		ULONG_PTR gdiplusToken;

		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }
 
	virtual ~OpenGLImageLoader_Gdiplus()
    {
    }
	
	void convertRawData(Gdiplus::BitmapData* _out_data, void* _result, size_t _size, MyGUI::PixelFormat _format)
	{
		size_t num = 0;

		if (_format == MyGUI::PixelFormat::L8)
		{
			num = 1;
		}
		if (_format == MyGUI::PixelFormat::L8A8)
		{
			num = 2;
		}
		if (_format == MyGUI::PixelFormat::R8G8B8)
		{
			num = 3;
		}
		else if (_format == MyGUI::PixelFormat::R8G8B8A8)
		{
			num = 4;
		}
		else
		{
			return;
		}

		unsigned char* ptr_source = (unsigned char*)_out_data->Scan0;
		unsigned char* ptr_dest = (unsigned char*)_result;

		size_t stride_source = _out_data->Stride;
		size_t stride_dest = _out_data->Width * num;

		if (stride_dest == stride_source)
		{
			memcpy(_result, _out_data->Scan0, _size);
		}
		else
		{
			for (unsigned int y = 0; y < _out_data->Height; ++y)
			{
				memcpy(ptr_dest, ptr_source, stride_dest);
				ptr_dest += stride_dest;
				ptr_source += stride_source;
			}
		}
	}

	void* loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename)
	{
		std::string fullname = MyGUI::OpenGLDataManager::getInstance().getDataPath(_filename);

		void* result = 0;

		Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromFile(MyGUI::UString(fullname).asWStr_c_str());
		if (image)
		{
			_width = image->GetWidth();
			_height = image->GetHeight();
			Gdiplus::PixelFormat format = image->GetPixelFormat();

			if (format == PixelFormat24bppRGB)
				_format = MyGUI::PixelFormat::R8G8B8;
			else if (format == PixelFormat32bppARGB)
				_format = MyGUI::PixelFormat::R8G8B8A8;
			else
				_format = MyGUI::PixelFormat::Unknow;

			if (_format != MyGUI::PixelFormat::Unknow)
			{
				Gdiplus::Rect rect(0, 0, _width, _height);
				Gdiplus::BitmapData out_data;
				image->LockBits(&rect, Gdiplus::ImageLockModeRead, format, &out_data);

				size_t size = out_data.Height * out_data.Stride;
				result = new unsigned char[size];

				convertRawData(&out_data, result, size, _format);

				image->UnlockBits(&out_data);
			}

			delete image;
		}

		return result;
	}

	void saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename)
	{
		Gdiplus::PixelFormat format;
		int bpp;

		if (_format == MyGUI::PixelFormat::R8G8B8A8)
		{
			bpp = 4;
			format = PixelFormat32bppARGB;
		}
		else if (_format == MyGUI::PixelFormat::R8G8B8)
		{
			bpp = 3;
			format = PixelFormat24bppRGB;
		}
		else if (_format == MyGUI::PixelFormat::L8A8)
		{
			bpp = 2;
			format = PixelFormat16bppGrayScale;
		}
		else
		{
			MYGUI_LOG(Error, "Unsuitable texture format for saving.");
			return;
		}

		Gdiplus::Bitmap image(_width, _height, bpp * _width, format, (BYTE*)_texture);

		UINT num, size;
		Gdiplus::GetImageEncodersSize(&num, &size);

		Gdiplus::ImageCodecInfo* imageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
		GetImageEncoders(num, size, imageCodecInfo);

		CLSID* pngClsid = NULL;
		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(imageCodecInfo[j].MimeType, L"image/png") == 0)
			{
				pngClsid = &imageCodecInfo[j].Clsid;
				break;
			}
		}

		if (pngClsid == NULL)
		{
			MYGUI_LOG(Error, "png codec not found");
			return;
		}

		HRESULT res = image.Save(MyGUI::UString(_filename).asWStr_c_str(), pngClsid, NULL);
		if (res != S_OK)
			MYGUI_LOG(Error, "Texture saving error. result =" << res);
	}
};
