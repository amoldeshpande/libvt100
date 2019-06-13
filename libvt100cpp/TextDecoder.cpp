#include "pch.h"
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include "TextDecoder.h"

namespace libVT100
{
	TextDecoder::TextDecoder(UINT codePage) 
		: m_converter(nullptr)
		, m_codePage(codePage)
	{
		UErrorCode err = U_ZERO_ERROR;
		switch (m_codePage) //http://demo.icu-project.org/icu-bin/convexp?s=WINDOWS
		{
			case 437:
				m_converter = ucnv_open("IBM437", &err);
				break;
			case CP_UTF8:
				m_converter = ucnv_open("UTF-8", &err);
				break;
			case 1252:
			case CP_ACP:
			default:
				m_converter = ucnv_open(ucnv_getDefaultName(), &err);
		}
		if (m_converter == nullptr)
		{
			dprintf("ucnv_open for code page %d got error %d\n",m_codePage,err);
		}

	}
	TextDecoder::~TextDecoder()
	{
		if (m_converter != nullptr)
		{
			ucnv_close(m_converter);
		}
	}
	int TextDecoder::GetChars(const char* inStr, int count, std::vector<WCHAR>& chars)
	{
		UErrorCode err = U_ZERO_ERROR;
		WCHAR* target = new WCHAR[count];
		WCHAR* origPtr = target;
		WCHAR* startPtr = target;
		int i = 0;
		ucnv_toUnicode(m_converter, (UChar**)&target, (UChar*)target + count, &inStr, inStr + count, nullptr, FALSE, &err);
		while (startPtr < target)
		{
			chars.push_back(*startPtr);
			startPtr++;
			i++;
		}
		delete origPtr;
		return i;
	}
	int TextDecoder::GetChar(const char inStr, WCHAR& outChar)
	{
		UErrorCode err = U_ZERO_ERROR;
		WCHAR out;
		const char* src = &inStr;
		WCHAR* target = &out;
		ucnv_toUnicode(m_converter, (UChar**)&target, (UChar*)target + 1, &src, src + 1, nullptr, FALSE, &err);
		if(target > &out)
		{
			outChar = out;
			return 1;
		}
		return 0;
	}
	std::vector<byte> TextDecoder::GetBytes(WCHAR* chars, int count)
	{
		UErrorCode err = U_ZERO_ERROR;
		std::vector<byte> ret;
		icu::UnicodeString ustr((UChar*)chars, count);

		int targetsize = ucnv_fromUChars(m_converter, nullptr, 0,(UChar*) chars, count, &err);
		if (err == U_BUFFER_OVERFLOW_ERROR)
		{
			ret.reserve(targetsize);
			ustr.extract((char*)&ret[0], targetsize, m_converter, err);
		}
		return ret;
	}
	
	void dprintf(const char* fmt, ...)
	{
		char buf[4096];
		va_list args;
		va_start(args, fmt);
		vsnprintf_s(buf, ARRAYSIZE(buf), _TRUNCATE, fmt, args);
		va_end(args);
		OutputDebugStringA(buf);

	}

}
