#pragma once
namespace libVT100
{

	typedef unsigned char byte;
	typedef std::wstring String;

	class IAnsiDecoderClient;
	typedef std::shared_ptr<IAnsiDecoderClient> IAnsiDecoderClientPtr;
	class IAnsiDecoder;
	typedef std::shared_ptr<IAnsiDecoder> IAnsiDecoderPtr;

	typedef POINT Point ;
	const POINT PointEmpty = { 0,0 };
	inline bool IsEmpty(Point p) { return p.x == 0 && p.y == 0; }
	inline bool AreEqual(Point a, Point b) { return (a.x == b.x) && (a.y == b.y); }

	extern void dprintf(const char* fmt, ...);
}
