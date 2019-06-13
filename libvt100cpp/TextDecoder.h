#pragma once

struct UConverter;
namespace libVT100
{
	class TextDecoder
	{
		UConverter* m_converter;
		UINT m_codePage;
	public:
		TextDecoder(UINT codePage);
		~TextDecoder();
		TextDecoder() = delete;
		int GetChars(const char* bytes,  int count, std::vector<WCHAR>& chars);
		int GetChar(const char byte,   WCHAR& outChar);
		std::vector<byte> GetBytes(WCHAR* chars,int count);
	};

}
