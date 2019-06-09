#include "pch.h"
#include <iostream>
#include <fstream>
#include "CppUnitTest.h"
#include <AnsiDecoder.h>
#include <Screen.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libVT100;

namespace CppUnitTest
{
	TEST_CLASS(TestAnsiArt)
	{
		static ULONG_PTR GdiPlusToken;
		TEST_CLASS_INITIALIZE(Init)
		{
			GdiplusStartupInput input = { 0 };
			input.GdiplusVersion = 1;
			GdiplusStartup(&GdiPlusToken,&input,NULL);
		}
		TEST_CLASS_CLEANUP(Cleanup)
		{
			GdiplusShutdown(GdiPlusToken);
		}
		TEST_METHOD(TestWendy)
		{
			ReadAndRenderFile("../../tests/70-twilight.ans",437, { 80, 80 });
			ReadAndRenderFile("../../tests/n4-wendy.ans",437,{ 80, 80 });
			ReadAndRenderFile("../../tests/zv-v01d.ans",437, { 80, 180 });
		}

		TEST_METHOD(TestSimpleTxt)
		{
			std::cout << (ReadAndRenderFile("../../tests/simple.txt",CP_UTF8, { 50, 6 }).ToString());
		}

		TEST_METHOD(TestUnixProgramOutput)
		{
			ReadAndRenderFile("../../tests/mc.output",CP_UTF8, { 180, 65 });
			ReadAndRenderFile("../../tests/ls.output",CP_UTF8, { 65, 10 });
		}

		Screen ReadAndRenderFile(std::string _filename,UINT codePage, SIZE _size)
		{
			AnsiDecoder vt100;
			Screen screen(_size.cx, _size.cy);
			vt100.Encoding = std::make_shared<TextDecoder>(codePage);
			vt100.Subscribe(&screen);
			{
				std::ifstream stream;
				stream.open(_filename, std::ifstream::in | std::ifstream::binary);
				Assert::IsTrue(stream.good());
				char read;
				while (true)
				{
					stream.read(&read, 1);
					if (stream.eof())
					{
						break;
					}
					vt100.Input(std::vector<byte>({ (byte)read }));
				}
			}
			//System.Console.Write ( screen.ToString() );
			Font font(L"Courier New", 6);
			Bitmap* bitmap = screen.ToBitmap(font,96,codePage);
			SaveBitmap(*bitmap,_filename);
			delete bitmap;
			return screen;
		}
		void SaveBitmap(Bitmap& bitmap,std::string& _filename)
		{
			CLSID pngClsid;
			GetEncoderClsid(L"image/png", &pngClsid);
			WCHAR fname[MAX_PATH + 1];

			_snwprintf_s(fname, MAX_PATH + 1,_TRUNCATE, L"..\\..\\build\\cpp\\%S_utf8.png", strrchr(_filename.c_str(), '/') + 1);
			Status s = bitmap.Save(fname, &pngClsid, NULL);

		}
		int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
		{
			UINT  num = 0;          // number of image encoders
			UINT  size = 0;         // size of the image encoder array in bytes

			ImageCodecInfo* pImageCodecInfo = NULL;

			GetImageEncodersSize(&num, &size);
			if (size == 0)
				return -1;  // Failure

			pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
			if (pImageCodecInfo == NULL)
				return -1;  // Failure

			GetImageEncoders(num, size, pImageCodecInfo);

			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
				{
					*pClsid = pImageCodecInfo[j].Clsid;
					free(pImageCodecInfo);
					return j;  // Success
				}
			}

			free(pImageCodecInfo);
			return -1;  // Failure
		}
	};
	ULONG_PTR TestAnsiArt::GdiPlusToken;
}