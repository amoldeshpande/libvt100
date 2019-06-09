#pragma once

namespace libVT100
{
	enum class Direction
	{
		Up,
		Down,
		Forward,
		Backward
	};

	enum class ClearDirection
	{
		Forward = 0,
		Backward = 1,
		Both = 2
	};

	enum class GraphicRendition
	{
		/// all attributes off
		Reset = 0,
		/// Intensity: Bold
		Bold = 1,
		/// Intensity: Faint     not widely supported
		Faint = 2,
		/// Italic: on     not widely supported. Sometimes treated as inverse.
		Italic = 3,
		/// Underline: Single     not widely supported
		Underline = 4,
		/// Blink: Slow     less than 150 per minute
		BlinkSlow = 5,
		/// Blink: Rapid     MS-DOS ANSI.SYS; 150 per minute or more
		BlinkRapid = 6,
		/// Image: Negative     inverse or reverse; swap foreground and background
		Inverse = 7,
		/// Conceal     not widely supported
		Conceal = 8,
		/// Font selection (not sure which)
		Font1 = 10,
		/// Underline: Double
		UnderlineDouble = 21,
		/// Intensity: Normal     not bold and not faint
		NormalIntensity = 22,
		/// Underline: None     
		NoUnderline = 24,
		/// Blink: off     
		NoBlink = 25,
		/// Image: Positive
		///
		/// Not sure what this is supposed to be, the opposite of inverse???
		Positive = 27,
		/// Reveal,     conceal off
		Reveal = 28,
		/// Set foreground color, normal intensity
		ForegroundNormalBlack = 30,
		ForegroundNormalRed = 31,
		ForegroundNormalGreen = 32,
		ForegroundNormalYellow = 33,
		ForegroundNormalBlue = 34,
		ForegroundNormalMagenta = 35,
		ForegroundNormalCyan = 36,
		ForegroundNormalWhite = 37,
		ForegroundNormalReset = 39,
		/// Set background color, normal intensity
		BackgroundNormalBlack = 40,
		BackgroundNormalRed = 41,
		BackgroundNormalGreen = 42,
		BackgroundNormalYellow = 43,
		BackgroundNormalBlue = 44,
		BackgroundNormalMagenta = 45,
		BackgroundNormalCyan = 46,
		BackgroundNormalWhite = 47,
		BackgroundNormalReset = 49,
		/// Set foreground color, high intensity (aixtem)
		ForegroundBrightBlack = 90,
		ForegroundBrightRed = 91,
		ForegroundBrightGreen = 92,
		ForegroundBrightYellow = 93,
		ForegroundBrightBlue = 94,
		ForegroundBrightMagenta = 95,
		ForegroundBrightCyan = 96,
		ForegroundBrightWhite = 97,
		ForegroundBrightReset = 99,
		/// Set background color, high intensity (aixterm)
		BackgroundBrightBlack = 100,
		BackgroundBrightRed = 101,
		BackgroundBrightGreen = 102,
		BackgroundBrightYellow = 103,
		BackgroundBrightBlue = 104,
		BackgroundBrightMagenta = 105,
		BackgroundBrightCyan = 106,
		BackgroundBrightWhite = 107,
		BackgroundBrightReset = 109,
	};

	enum class AnsiMode
	{
		ShowCursor,
		HideCursor,
		LineFeed,
		NewLine,
		CursorKeyToCursor,
		CursorKeyToApplication,
		ANSI,
		VT52,
		Columns80,
		Columns132,
		JumpScrolling,
		SmoothScrolling,
		NormalVideo,
		ReverseVideo,
		OriginIsAbsolute,
		OriginIsRelative,
		LineWrap,
		DisableLineWrap,
		AutoRepeat,
		DisableAutoRepeat,
		Interlacing,
		DisableInterlacing,
		NumericKeypad,
		AlternateKeypad,
	};

	class AnsiDecoder;
	class IAnsiDecoderClient
	{
	public:
		virtual void Characters(AnsiDecoder& _sender, std::vector<WCHAR>& _chars) = 0;
		virtual void SaveCursor(AnsiDecoder& _sender) = 0;
		virtual void RestoreCursor(AnsiDecoder& _sender) = 0;
		virtual SIZE GetSize(AnsiDecoder& _sender) = 0;
		virtual void MoveCursor(AnsiDecoder& _sender, Direction _direction, int _amount) = 0;
		virtual void MoveCursorToBeginningOfLineBelow(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine) = 0;
		virtual void MoveCursorToBeginningOfLineAbove(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine) = 0;
		virtual void MoveCursorToColumn(AnsiDecoder& _sender, int _columnNumber) = 0;
		virtual void MoveCursorTo(AnsiDecoder& _sender, Point _position) = 0;
		virtual void ClearScreen(AnsiDecoder& _sender, ClearDirection _direction) = 0;
		virtual void ClearLine(AnsiDecoder& _sender, ClearDirection _direction) = 0;
		virtual void ScrollPageUpwards(AnsiDecoder& _sender, int _linesToScroll) = 0;
		virtual void ScrollPageDownwards(AnsiDecoder& _sender, int _linesToScroll) = 0;
		virtual Point GetCursorPosition(AnsiDecoder& _sender) = 0;
		virtual void SetGraphicRendition(AnsiDecoder& _sender, std::vector<GraphicRendition>& _commands) = 0;
		virtual void ModeChanged(AnsiDecoder& _sender, AnsiMode _mode) = 0;
	};
}

