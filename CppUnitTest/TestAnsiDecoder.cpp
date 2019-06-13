#include "pch.h"

#include "CppUnitTest.h"
#include "AnsiDecoder.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libVT100;

#define INPUT(literal) Input(literal,sizeof(literal)-1)
namespace CppUnitTest
{
	TEST_CLASS(TestAnsiDecoder),public IAnsiDecoderClient
	{
	private :
		std::vector<std::vector<WCHAR>> m_chars;
        std::vector<std::vector<byte> > m_output;
        std::shared_ptr<AnsiDecoder> m_vt100;
        
        Point m_cursorPosition;
        bool m_showCursor;
        bool m_hideCursor;
        int m_scrollPageDownwards;
        int m_scrollPageUpwards;
        ClearDirection m_clearLine;
        ClearDirection m_clearScreen;
        Point m_moveCursorTo;
        int m_moveCursorToColumn;
        int m_moveCursorToBeginningOfLineAbove;
        int m_moveCursorToBeginningOfLineBelow;
        Direction m_moveCursorDirection;
        int m_moveCursorAmount;
        SIZE m_size;
        bool m_restoreCursor;
        bool m_saveCursor;

		/*
        void vt100_Output ( IDecoder& _decoder, std::string&_output )
        {
            m_output.push_back( _output );
        }
		*/
	public:
		void Reset()
		{
			m_showCursor = false;
			m_hideCursor = false;
			m_scrollPageDownwards = -1;
			m_scrollPageUpwards = -1;
			m_clearLine = (ClearDirection)(-1);
			m_clearScreen = (ClearDirection)(-1);
			m_moveCursorTo = { 0,0 };// Point.Empty;
			m_moveCursorToColumn = -1;
			m_moveCursorToBeginningOfLineAbove = -1;
			m_moveCursorToBeginningOfLineBelow = -1;
			m_moveCursorDirection = (Direction)(-1);
			m_moveCursorAmount = -1;
			m_restoreCursor = false;
			m_saveCursor = false;
		}
		TEST_METHOD_INITIALIZE(SetUp)
		{
			Reset();

			m_cursorPosition = { 0,0 };
			m_size = { 10, 10 };

			m_vt100 = std::make_shared<AnsiDecoder>();

			m_vt100->Subscribe(this);
			m_vt100->Output = [this](const IDecoder& _decoder,const  std::vector<byte>& _output) { m_output.push_back(_output); };
		}
		TEST_METHOD_CLEANUP(TearDown)
		{
			m_vt100->Output = nullptr;
			m_vt100->UnSubscribe(this);
			m_chars.clear();
			m_output.clear();
			m_vt100 = nullptr;
			m_chars.clear();
		}
		TEST_METHOD(TestMoveCursorTo)
		{
			INPUT("AB\x001B[2;5fCDE" );
			AreEqualStrings(L"ABCDE", ReceivedCharacters());
			Assert::IsTrue( AreEqual({ 4, 1 }, m_moveCursorTo));

			Reset();

			INPUT("AB\x001B" "2;5fCDE" );
			AreEqualStrings(L"ABCDE", ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 4, 1 }, m_moveCursorTo));

			Reset();

			INPUT( "AB\x001B[;4fCDE" );
			AreEqualStrings(L"ABCDE", ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 3, 0 }, m_moveCursorTo));

			Reset();

			INPUT("AB\x001B[3;fCDE");
			AreEqualStrings(L"ABCDE", ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 0, 2 }, m_moveCursorTo));

			Reset();

			INPUT("AB\x001B[;fCDE");
			AreEqualStrings(L"ABCDE", ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 0, 0 }, m_moveCursorTo));

			Reset();

			INPUT("AB\x001B[fCDE");
			AreEqualStrings(L"ABCDE", ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 0, 0 }, m_moveCursorTo));
		}
        TEST_METHOD(TestGetCursorPosition)
        {
            INPUT ( "AB\x001B[6nCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters() );
            AreEqualStrings (L"\x001B[1;1R", Output());
            
            Reset();
            
			m_cursorPosition = { 7,2 };
            INPUT ( "AB\x001B[6nCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            AreEqualStrings (L"\x001B[3;8R", Output());
        }
        TEST_METHOD(TestShowCursor)
        {
            INPUT ( "AB\x001B[?25hCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( m_showCursor );
        }
        TEST_METHOD(TestHideCursor)
        {
            INPUT ( "AB\x001B[?25lCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( m_hideCursor );
        }
        TEST_METHOD(TestSaveCursorPosition )
        {
            INPUT ( "AB\x001B[sCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue ( m_saveCursor );
        }
        TEST_METHOD(TestRestoreCursorPosition)
        {
            INPUT ( "AB\x001B[uCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue ( m_restoreCursor );
        }
        TEST_METHOD(TestScrollPageDownwards)
        {
            INPUT ( "AB\x001B[TCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::AreEqual( 1, m_scrollPageDownwards );
            
            Reset();
            
            INPUT ( "AB\x001B[6TCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::AreEqual( 6, m_scrollPageDownwards );
            Reset();
        }
        TEST_METHOD(TestScrollPageUpwards)
        {
            INPUT ( "AB\x001B[SCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::AreEqual( 1, m_scrollPageUpwards );
            
            Reset();
            
            INPUT ( "AB\x001B[4SCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::AreEqual( 4, m_scrollPageUpwards );
            Reset();
        }
        TEST_METHOD(TestClearScreen)
        {
            INPUT ( "AB\x001B[JCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Forward == m_clearScreen );
            
            Reset();

            INPUT ( "AB\x001B[0JCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Forward == m_clearScreen );
            
            Reset();

            INPUT ( "AB\x001B[1JCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Backward == m_clearScreen );
            
            Reset();

            INPUT ( "AB\x001B[2JCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Both == m_clearScreen );
        }
        TEST_METHOD(TestClearLine)
        {
            INPUT ( "AB\x001B[KCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Forward == m_clearLine );
            
            Reset();

            INPUT ( "AB\x001B[0KCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Forward == m_clearLine );
            
            Reset();

            INPUT ( "AB\x001B[1KCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Backward == m_clearLine );
            
            Reset();

            INPUT ( "AB\x001B[2KCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue( ClearDirection::Both == m_clearLine );
        }
        TEST_METHOD(TestMoveCursor)
        {
            INPUT ( "AB\x001B[ACDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue ( Direction::Up == m_moveCursorDirection );
            Assert::IsTrue ( 1 == m_moveCursorAmount );
            
            Reset ();
            
            INPUT ( "AB\x001B[2ACDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue ( Direction::Up == m_moveCursorDirection );
            Assert::IsTrue ( 2 == m_moveCursorAmount );
            
            Reset ();

            INPUT ( "AB\x001B[BCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue ( Direction::Down == m_moveCursorDirection );
            Assert::IsTrue ( 1 == m_moveCursorAmount );
            
            Reset ();
            
            INPUT ( "AB\x001B[3BCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue ( Direction::Down == m_moveCursorDirection );
            Assert::IsTrue ( 3 == m_moveCursorAmount );
            
            Reset ();

            INPUT ( "AB\x001B[CCDE" );
            AreEqualStrings (L"ABCDE", ReceivedCharacters());
            Assert::IsTrue ( Direction::Forward == m_moveCursorDirection );
            Assert::IsTrue ( 1 == m_moveCursorAmount );
            
            Reset ();
            
            INPUT ( "AB\x001B[6CCDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( Direction::Forward == m_moveCursorDirection );
            Assert::IsTrue ( 6 == m_moveCursorAmount );
            
            Reset ();

            INPUT ( "AB\x001B[DCDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( Direction::Backward == m_moveCursorDirection );
            Assert::IsTrue ( 1 == m_moveCursorAmount );
            
            Reset ();
            
            INPUT ( "AB\x001B[4DCDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( Direction::Backward == m_moveCursorDirection );
            Assert::IsTrue ( 4 == m_moveCursorAmount );
        }

        TEST_METHOD(TestMoveCursorToBeginningOfLineAbove)
        {
            INPUT ("AB\x001B[FCDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( 1 == m_moveCursorToBeginningOfLineAbove );
            
            Reset ();

            INPUT ("AB\x001B[4FCDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( 4 == m_moveCursorToBeginningOfLineAbove );
        }
        TEST_METHOD(TestMoveCursorToBeginningOfLineBelow)
        {
            INPUT ("AB\x001B[ECDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( 1 == m_moveCursorToBeginningOfLineBelow );
            
            Reset ();

            INPUT ("AB\x001B[3ECDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( 3 == m_moveCursorToBeginningOfLineBelow );
        }
        TEST_METHOD(TestMoveCursorToColumn)
        {
            INPUT ("AB\x001B[1GCDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( 0 == m_moveCursorToColumn );
            
            Reset ();

            INPUT ("AB\x001B[7GCDE" );
            AreEqualStrings (L"ABCDE" , ReceivedCharacters());
            Assert::IsTrue ( 6 == m_moveCursorToColumn );
        }
	protected:
		void AreEqualStrings(const WCHAR* a, String& b)
		{
			Assert::AreEqual(std::wstring(a), b);
		}
		void Input(const char* _input,int len)
		{
			Input(std::vector<byte>(_input,_input + len));
		}
		void Input(std::vector<byte>& _input)
		{
			m_vt100->Input(_input);
		}
        String ReceivedCharacters()
		{
			std::wstringstream builder;
			for(auto strs : m_chars)
			{
				builder.write(&strs[0], strs.size());
			}
			m_chars.clear();
			return builder.str();
		}
		String Output()
		{
			{
				std::wstringstream builder;
				for (auto chars : m_output)
				{
					for (auto b : chars)
					{
						builder << (WCHAR)b;
					}
				}
				m_output.clear();
				return builder.str();
			}
		}
        
        virtual void Characters ( AnsiDecoder& _sender, std::vector<WCHAR>& _chars ) override
        {
            m_chars.push_back ( _chars );
        }
        virtual void OneCharacter ( AnsiDecoder& _sender, WCHAR _chars ) override
        {
			m_chars.push_back({ _chars });
        }
        
        virtual void SaveCursor ( AnsiDecoder& _sender ) override
        {
            m_saveCursor = true;
        }
        
        virtual void RestoreCursor ( AnsiDecoder& _sender ) override
        {
            m_restoreCursor = true;
        }
        
        SIZE GetSize ( AnsiDecoder& _sender ) override
        {
            return m_size;
        }
        
        virtual void MoveCursor ( AnsiDecoder& _sender, Direction _direction, int _amount ) override
        {
            m_moveCursorDirection = _direction;
            m_moveCursorAmount = _amount;
        }
        
        virtual void MoveCursorToBeginningOfLineBelow ( AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine ) override
        {
            m_moveCursorToBeginningOfLineBelow = _lineNumberRelativeToCurrentLine;
        }
        
        virtual void MoveCursorToBeginningOfLineAbove ( AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine ) override
        {
            m_moveCursorToBeginningOfLineAbove = _lineNumberRelativeToCurrentLine;
        }
        
        virtual void MoveCursorToColumn ( AnsiDecoder& _sender, int _columnNumber ) override
        {
            m_moveCursorToColumn = _columnNumber;
        }
        
        virtual void MoveCursorTo ( AnsiDecoder& _sender, Point _position ) override
        {
            m_moveCursorTo = _position;
        }
        
        virtual void ClearScreen ( AnsiDecoder& _sender, ClearDirection _direction ) override
        {
            m_clearScreen = _direction;
        }
        
        virtual void ClearLine ( AnsiDecoder& _sender, ClearDirection _direction ) override
        {
            m_clearLine = _direction;
        }
        
        virtual void ScrollPageUpwards ( AnsiDecoder& _sender, int _linesToScroll ) override
        {
            m_scrollPageUpwards = _linesToScroll;
        }
        
        virtual void ScrollPageDownwards ( AnsiDecoder& _sender, int _linesToScroll ) override
        {
            m_scrollPageDownwards = _linesToScroll;
        }

       virtual void ModeChanged( AnsiDecoder& _sender, AnsiMode _mode ) override
        {
           switch ( _mode )
           {
		   case AnsiMode::HideCursor:
                 m_hideCursor = true;
                 break;

              case AnsiMode::ShowCursor:
                 m_showCursor = true;
                 break;
           }
        }
        
        Point GetCursorPosition ( AnsiDecoder& _sender ) override
        {
            return m_cursorPosition;
        }

        virtual void SetGraphicRendition ( AnsiDecoder& _sender, std::vector<GraphicRendition>& _commands ) override
        {
            
        }
	};
}
