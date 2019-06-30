#pragma once
#include <cmath>
#include <gdiplus.h>
#include <IAnsiDecoderClient.h>

//std::locale::id std::codecvt<char16_t, char, struct _Mbstatet>::id;

using namespace Gdiplus;

#define Rectangle Rect

namespace libVT100
{
    class Screen : public IAnsiDecoderClient
	{
	protected:
		enum class Blink
		{
			None,
			Slow,
			Rapid,
		};

		enum class Underline
		{
			None,
			Single,
			Double,
		};

		enum class TextColor
		{
			Black,
			Red,
			Green,
			Yellow,
			Blue,
			Magenta,
			Cyan,
			White,
			BrightBlack,
			BrightRed,
			BrightGreen,
			BrightYellow,
			BrightBlue,
			BrightMagenta,
			BrightCyan,
			BrightWhite,
		};

		struct GraphicAttributes
		{
		private:
			bool m_bold = false;
			bool m_faint = false;
			bool m_italic = false;
			Underline m_underline = Underline::None;
			Blink m_blink = Blink::None;
			bool m_conceal = false;
			TextColor m_foreground = TextColor::Black;
			TextColor m_background = TextColor::Black;

		public:
			bool Bold() { return m_bold; }
			void Bold(bool value) { m_bold = value; }
			bool Faint() { return m_faint; }
			void Faint(bool value) { m_faint = value; }
			bool Italic() { return m_italic; }
			void Italic(bool value) { m_italic = value; }
			Underline Underline() { return m_underline; }
			void Underline(Screen::Underline value) { m_underline = value; }
			Blink Blink() { return m_blink; }
			void Blink(Screen::Blink value) { m_blink = value; }
			bool Conceal() { return m_conceal; }
			void Conceal(bool value) { m_conceal = value; }
			TextColor Foreground() { return m_foreground; }
			TextColor Background() { return m_background; }
			void Background(TextColor value) { m_background = value; }
			void Foreground(TextColor value) { m_foreground = value; }
			Color ForegroundColor() { return TextColorToColor(m_foreground); }
			Color BackgroundColor() { return TextColorToColor(m_background); }

			//http://flounder.com/csharp_color_table.htm
			Color TextColorToColor(TextColor _textColor)
			{
				switch (_textColor)
				{
					case TextColor::Black:
						return Color();
					case TextColor::Red:
						return Color(139, 0, 0);
					case TextColor::Green:
						return Color(0, 128, 0);
					case TextColor::Yellow:
						return Color(255, 255, 0);
					case TextColor::Blue:
						return Color(0, 0, 255);
					case TextColor::Magenta:
						return Color(139, 0, 139);
					case TextColor::Cyan:
						return Color(0, 255, 255);
					case TextColor::White:
						return Color(255, 255, 255);
					case TextColor::BrightBlack:
						return Color(128, 128, 128);
					case TextColor::BrightRed:
						return Color(255, 0, 0);
					case TextColor::BrightGreen:
						return Color(144, 238, 144);
					case TextColor::BrightYellow:
						return Color(255, 255, 224);
					case TextColor::BrightBlue:
						return Color(173, 216, 230);
					case TextColor::BrightMagenta:
						return Color(255, 0, 255);
					case TextColor::BrightCyan:
						return Color(224, 255, 255);
					case TextColor::BrightWhite:
						return Color(128, 128, 128);
				}
				throw std::exception("_textColor :Unknown color value.");
			}

			void Reset()
			{
				m_bold = false;
				m_faint = false;
				m_italic = false;
				m_underline = Underline::None;
				m_blink = Blink::None;
				m_conceal = false;
				m_foreground = TextColor::White;
				m_background = TextColor::Black;
			}
		};

		class Character
		{
		public:
			WCHAR Char;
			GraphicAttributes GraphicAttributes;

			Character() : Character(' ')
			{
			}

			Character(char _char)
			{
				Char = _char;
				//this->GraphicAttributes = std::make_unique<Screen::GraphicAttributes>();
			}

		};

	protected:
		Point m_cursorPosition;
		Point m_savedCursorPosition;
		bool m_showCursor;
		std::vector<std::vector<Character>> m_screen;
		GraphicAttributes m_currentAttributes;

	public:
		SIZE Size() { return { Width(), Height() }; }
		void Size(SIZE value)
		{
			if (m_screen.size() == 0 || value.cx != Width() || value.cy != Height())
			{
				//m_screen = new Character[value.Width, value.Height];
				m_screen = std::vector<std::vector<Character>>(value.cx, std::vector<Character>(value.cy));
				for (int x = 0; x < Width(); ++x)
				{
					for (int y = 0; y < Height(); ++y)
					{
						m_screen[x][y] = Character();
					}
				}
				CursorPosition({ 0, 0 });
			}
		}

		int Width() { return (int)m_screen.size(); }

		int Height() { return (int)m_screen[0].size(); }

		Point CursorPosition() { return m_cursorPosition; }
		void CursorPosition(Point value)
		{
			if (!AreEqual(m_cursorPosition, value))
			{
				CheckColumnRow(value.x, value.y);

				m_cursorPosition = value;
			}
		}


	public:
		Screen(int _width, int _height)
		{
			Size({ _width, _height });
			m_showCursor = true;
			m_savedCursorPosition = PointEmpty;
			m_currentAttributes.Reset();
		}

	protected:
		void CheckColumnRow(int _column, int _row)
		{
			std::stringstream err;
			if (_column >= Width())
			{
				err << "The column number " << _column << " is larger than the screen width " << Width();
				throw std::exception(err.str().c_str());
			}
			if (_row >= Height())
			{
				std::stringstream err;
				err << "The row number (" << _row << " is larger than the screen height " << Height();
				throw std::exception(err.str().c_str());
			}
		}

	public:
		void CursorForward()
		{
			if (m_cursorPosition.x + 1 >= Width())
			{
				CursorPosition({ 0, m_cursorPosition.y + 1 });
			}
			else
			{
				CursorPosition({ m_cursorPosition.x + 1, m_cursorPosition.y });
			}
		}

		void CursorBackward()
		{
			if (m_cursorPosition.x - 1 < 0)
			{
				CursorPosition({ Width() - 1, m_cursorPosition.y - 1 });
			}
			else
			{
				CursorPosition({ m_cursorPosition.x - 1, m_cursorPosition.y });
			}
		}

		void CursorDown()
		{
			if (m_cursorPosition.y + 1 >= Height())
			{
				throw std::exception("Can not move further down!");
			}
			CursorPosition({ m_cursorPosition.x, m_cursorPosition.y + 1 });
		}

		void CursorUp()
		{
			if (m_cursorPosition.y - 1 < 0)
			{
				throw std::exception("Can not move further up!");
			}
			CursorPosition({ m_cursorPosition.x, m_cursorPosition.y - 1 });
		}

		std::string ToString()
		{
			std::stringstream builder;
			for (int y = 0; y < Height(); ++y)
			{
				for (int x = 0; x < Width(); ++x)
				{
					if (m_screen[x][y].Char > 127)
					{
						builder << '!';
					}
					else
					{
						builder << (m_screen[x][y].Char);
					}
				}
				builder << '\n';
			}
			return builder.str();
		}

	private:
		void toWideChar(char inChar, WCHAR* outChar, UINT cp)
		{
			int ret = MultiByteToWideChar(cp, MB_PRECOMPOSED, &inChar, 1, outChar, 1);

		}
	public:
		Bitmap* ToBitmap(Font& _font, REAL dpi, UINT cp)
		{
			Bitmap* bitmap = new Bitmap((int)std::ceil(_font.GetHeight(dpi)) * Width(), (int)std::ceil(_font.GetHeight(dpi)) * Height());
			Graphics* graphics = Graphics::FromImage(bitmap);
			for (int y = 0; y < Height(); ++y)
			{
				for (int x = 0; x < Width(); ++x)
				{
					Character character = m_screen[x][y];

					RectF rect(_font.GetHeight(dpi) * x, _font.GetHeight(dpi) * y, _font.GetHeight(dpi), _font.GetHeight(dpi));
					SolidBrush bgBrush(character.GraphicAttributes.BackgroundColor());
					SolidBrush fgBrush(character.GraphicAttributes.ForegroundColor());
					graphics->FillRectangle(&bgBrush, rect);

					FontFamily ff;
					_font.GetFamily(&ff);
					Font* font = new Font(&ff, _font.GetSize(), FontStyleRegular);
					if (character.GraphicAttributes.Bold())
					{
						if (character.GraphicAttributes.Italic())
						{
							font = new Font(&ff, _font.GetSize(), FontStyleBold | FontStyleItalic);
						}
						else
						{
							font = new Font(&ff, _font.GetSize(), FontStyleBold);
						}
					}
					else if (character.GraphicAttributes.Italic())
					{
						font = new Font(&ff, _font.GetSize(), FontStyleItalic);
					}

					WCHAR str[] = { character.Char,0 };
					PointF p(rect.X, rect.Y);
					graphics->DrawString(str, 1, font, p, &fgBrush);
					delete font;
				}
			}
			return bitmap;
		}

		/*
		IEnumerator<Screen.Character> IEnumerable<Screen.Character>.GetEnumerator ()
		{
			for ( int y = 0; y < Height; ++y )
			{
				for ( int x = 0; x < Width; ++x )
				{
					yield return this[x,y];
				}
			}
		}

		IEnumerator IEnumerable.GetEnumerator ()
		{
			return (this as IEnumerable<Screen.Character>).GetEnumerator();
		}
		*/

		virtual void Characters(AnsiDecoder& _sender, std::vector<WCHAR>& _chars) override
		{
			for (auto ch : _chars)
			{
				OneCharacter(_sender, ch);
			}
		}
		virtual void OneCharacter(AnsiDecoder& _sender, WCHAR ch) override
		{
			if (ch == '\n')
			{
				MoveCursorToBeginningOfLineBelow(_sender, 1);
			}
			else if (ch == '\r')
			{
				//(this as IVT100DecoderClient).MoveCursorToBeginningOfLineBelow ( _sender, 1 );
			}
			else
			{
				m_screen[CursorPosition().x][CursorPosition().y].Char = ch;
				m_screen[CursorPosition().x][CursorPosition().y].GraphicAttributes = m_currentAttributes;
				CursorForward();
			}
		}

		virtual void SaveCursor(AnsiDecoder& _sender) override
		{
			m_savedCursorPosition = m_cursorPosition;
		}

		virtual void RestoreCursor(AnsiDecoder& _sender) override
		{
			CursorPosition(m_savedCursorPosition);
		}

		virtual SIZE GetSize(AnsiDecoder& _sender) override
		{
			return Size();
		}

		virtual void MoveCursor(AnsiDecoder& _sender, Direction _direction, int _amount) override
		{
			switch (_direction)
			{
				case Direction::Up:
					while (_amount > 0)
					{
						CursorUp();
						_amount--;
					}
					break;

				case Direction::Down:
					while (_amount > 0)
					{
						CursorDown();
						_amount--;
					}
					break;

				case Direction::Forward:
					while (_amount > 0)
					{
						CursorForward();
						_amount--;
					}
					break;

				case Direction::Backward:
					while (_amount > 0)
					{
						CursorBackward();
						_amount--;
					}
					break;
			}
		}

		virtual void MoveCursorToBeginningOfLineBelow(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine)override
		{
			m_cursorPosition.x = 0;
			while (_lineNumberRelativeToCurrentLine > 0)
			{
				CursorDown();
				_lineNumberRelativeToCurrentLine--;
			}
		}

		virtual void MoveCursorToBeginningOfLineAbove(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine) override
		{
			m_cursorPosition.x = 0;
			while (_lineNumberRelativeToCurrentLine > 0)
			{
				CursorUp();
				_lineNumberRelativeToCurrentLine--;
			}
		}

		virtual void MoveCursorToColumn(AnsiDecoder& _sender, int _columnNumber) override
		{
			CheckColumnRow(_columnNumber, m_cursorPosition.y);

			CursorPosition({ _columnNumber, m_cursorPosition.y });
		}

		virtual void MoveCursorTo(AnsiDecoder& _sender, Point _position) override
		{
			CheckColumnRow(_position.x, _position.y);

			CursorPosition(_position);
		}

		virtual void ClearScreen(AnsiDecoder& _sender, ClearDirection _direction) override
		{
		}

		virtual void ClearLine(AnsiDecoder& _sender, ClearDirection _direction) override
		{
			switch (_direction)
			{
				case ClearDirection::Forward:
					for (int x = m_cursorPosition.x; x < Width(); ++x)
					{
						m_screen[x][m_cursorPosition.y].Char = ' ';
					}
					break;

				case ClearDirection::Backward:
					for (int x = m_cursorPosition.x; x >= 0; --x)
					{
						m_screen[x][m_cursorPosition.y].Char = ' ';
					}
					break;

				case ClearDirection::Both:
					for (int x = 0; x < Width(); ++x)
					{
						m_screen[x][m_cursorPosition.y].Char = ' ';
					}
					break;
			}
		}

		virtual void ScrollPageUpwards(AnsiDecoder& _sender, int _linesToScroll) override
		{
		}

		virtual void ScrollPageDownwards(AnsiDecoder& _sender, int _linesToScroll) override
		{
		}

		virtual void ModeChanged(AnsiDecoder& _sender, AnsiMode _mode) override
		{
			switch (_mode)
			{
				case AnsiMode::HideCursor:
					m_showCursor = false;
					break;

				case AnsiMode::ShowCursor:
					m_showCursor = true;
					break;
			}
		}

		virtual Point GetCursorPosition(AnsiDecoder& _sender) override
		{
			return { m_cursorPosition.x + 1, m_cursorPosition.y + 1 };
		}

		virtual void SetGraphicRendition(AnsiDecoder& _sender, std::vector<GraphicRendition>& _commands) override
		{
			for (auto command : _commands)
			{
				switch (command)
				{
					case GraphicRendition::Reset:
						m_currentAttributes.Reset();
						break;
					case GraphicRendition::Bold:
						m_currentAttributes.Bold(true);
						break;
					case GraphicRendition::Faint:
						m_currentAttributes.Faint(true);
						break;
					case GraphicRendition::Italic:
						m_currentAttributes.Italic(true);
						break;
					case GraphicRendition::Underline:
						m_currentAttributes.Underline(Underline::Single);
						break;
					case GraphicRendition::BlinkSlow:
						m_currentAttributes.Blink(Blink::Slow);
						break;
					case GraphicRendition::BlinkRapid:
						m_currentAttributes.Blink(Blink::Rapid);
						break;
					case GraphicRendition::Positive:
					case GraphicRendition::Inverse:
					{
						TextColor tmp = m_currentAttributes.Foreground();
						m_currentAttributes.Foreground(m_currentAttributes.Background());
						m_currentAttributes.Background(tmp);
					}
					break;
					case GraphicRendition::Conceal:
						m_currentAttributes.Conceal(true);
						break;
					case GraphicRendition::UnderlineDouble:
						m_currentAttributes.Underline(Underline::Double);
						break;
					case GraphicRendition::NormalIntensity:
						m_currentAttributes.Bold(false);
						m_currentAttributes.Faint(false);
						break;
					case GraphicRendition::NoUnderline:
						m_currentAttributes.Underline(Underline::None);
						break;
					case GraphicRendition::NoBlink:
						m_currentAttributes.Blink(Blink::None);
						break;
					case GraphicRendition::Reveal:
						m_currentAttributes.Conceal(false);
						break;
					case GraphicRendition::ForegroundNormalBlack:
						m_currentAttributes.Foreground(TextColor::Black);
						break;
					case GraphicRendition::ForegroundNormalRed:
						m_currentAttributes.Foreground(TextColor::Red);
						break;
					case GraphicRendition::ForegroundNormalGreen:
						m_currentAttributes.Foreground(TextColor::Green);
						break;
					case GraphicRendition::ForegroundNormalYellow:
						m_currentAttributes.Foreground(TextColor::Yellow);
						break;
					case GraphicRendition::ForegroundNormalBlue:
						m_currentAttributes.Foreground(TextColor::Blue);
						break;
					case GraphicRendition::ForegroundNormalMagenta:
						m_currentAttributes.Foreground(TextColor::Magenta);
						break;
					case GraphicRendition::ForegroundNormalCyan:
						m_currentAttributes.Foreground(TextColor::Cyan);
						break;
					case GraphicRendition::ForegroundNormalWhite:
						m_currentAttributes.Foreground(TextColor::White);
						break;
					case GraphicRendition::ForegroundNormalReset:
						m_currentAttributes.Foreground(TextColor::White);
						break;

					case GraphicRendition::BackgroundNormalBlack:
						m_currentAttributes.Background(TextColor::Black);
						break;
					case GraphicRendition::BackgroundNormalRed:
						m_currentAttributes.Background(TextColor::Red);
						break;
					case GraphicRendition::BackgroundNormalGreen:
						m_currentAttributes.Background(TextColor::Green);
						break;
					case GraphicRendition::BackgroundNormalYellow:
						m_currentAttributes.Background(TextColor::Yellow);
						break;
					case GraphicRendition::BackgroundNormalBlue:
						m_currentAttributes.Background(TextColor::Blue);
						break;
					case GraphicRendition::BackgroundNormalMagenta:
						m_currentAttributes.Background(TextColor::Magenta);
						break;
					case GraphicRendition::BackgroundNormalCyan:
						m_currentAttributes.Background(TextColor::Cyan);
						break;
					case GraphicRendition::BackgroundNormalWhite:
						m_currentAttributes.Background(TextColor::White);
						break;
					case GraphicRendition::BackgroundNormalReset:
						m_currentAttributes.Background(TextColor::Black);
						break;

					case GraphicRendition::ForegroundBrightBlack:
						m_currentAttributes.Foreground(TextColor::BrightBlack);
						break;
					case GraphicRendition::ForegroundBrightRed:
						m_currentAttributes.Foreground(TextColor::BrightRed);
						break;
					case GraphicRendition::ForegroundBrightGreen:
						m_currentAttributes.Foreground(TextColor::BrightGreen);
						break;
					case GraphicRendition::ForegroundBrightYellow:
						m_currentAttributes.Foreground(TextColor::BrightYellow);
						break;
					case GraphicRendition::ForegroundBrightBlue:
						m_currentAttributes.Foreground(TextColor::BrightBlue);
						break;
					case GraphicRendition::ForegroundBrightMagenta:
						m_currentAttributes.Foreground(TextColor::BrightMagenta);
						break;
					case GraphicRendition::ForegroundBrightCyan:
						m_currentAttributes.Foreground(TextColor::BrightCyan);
						break;
					case GraphicRendition::ForegroundBrightWhite:
						m_currentAttributes.Foreground(TextColor::BrightWhite);
						break;
					case GraphicRendition::ForegroundBrightReset:
						m_currentAttributes.Foreground(TextColor::White);
						break;

					case GraphicRendition::BackgroundBrightBlack:
						m_currentAttributes.Background(TextColor::BrightBlack);
						break;
					case GraphicRendition::BackgroundBrightRed:
						m_currentAttributes.Background(TextColor::BrightRed);
						break;
					case GraphicRendition::BackgroundBrightGreen:
						m_currentAttributes.Background(TextColor::BrightGreen);
						break;
					case GraphicRendition::BackgroundBrightYellow:
						m_currentAttributes.Background(TextColor::BrightYellow);
						break;
					case GraphicRendition::BackgroundBrightBlue:
						m_currentAttributes.Background(TextColor::BrightBlue);
						break;
					case GraphicRendition::BackgroundBrightMagenta:
						m_currentAttributes.Background(TextColor::BrightMagenta);
						break;
					case GraphicRendition::BackgroundBrightCyan:
						m_currentAttributes.Background(TextColor::BrightCyan);
						break;
					case GraphicRendition::BackgroundBrightWhite:
						m_currentAttributes.Background(TextColor::BrightWhite);
						break;
					case GraphicRendition::BackgroundBrightReset:
						m_currentAttributes.Background(TextColor::Black);
						break;

					case GraphicRendition::Font1:
						break;

					default:

						throw std::exception("Unknown rendition command");
				}
			}
		}
	};
}
