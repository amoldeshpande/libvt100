#include "pch.h"

#include <functional>

#include "AnsiDecoder.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libVT100;

namespace TestAnsiDecoder
{
	TEST_CLASS(TestAnsiDecoder),public IAnsiDecoderClient
	{
	private :
		std::vector<std::string> m_chars;
        std::vector<std::string > m_output;
        std::shared_ptr<IAnsiDecoder> m_vt100;
        
        POINT m_cursorPosition;
        bool m_showCursor;
        bool m_hideCursor;
        int m_scrollPageDownwards;
        int m_scrollPageUpwards;
        ClearDirection m_clearLine;
        ClearDirection m_clearScreen;
        POINT m_moveCursorTo;
        int m_moveCursorToColumn;
        int m_moveCursorToBeginningOfLineAbove;
        int m_moveCursorToBeginningOfLineBelow;
        Direction m_moveCursorDirection;
        int m_moveCursorAmount;
        RECT m_size;
        bool m_restoreCursor;
        bool m_saveCursor;

        void vt100_Output ( IDecoder& _decoder, std::string&_output )
        {
            m_output.push_back( _output );
        }
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
			m_vt100->Output = [this](IDecoder& _decoder, std::string& _output) {m_output.push_back(_output); };
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
			Input(std::string("AB\x001B[2;5fCDE" ));
			Assert::AreEqual(std::string("ABCDE"), ReceivedCharacters());
			Assert::IsTrue( AreEqual({ 4, 1 }, m_moveCursorTo));

			Reset();

			Input(std::string("AB\x001B" "2;5fCDE") );
			Assert::AreEqual(std::string("ABCDE"), ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 4, 1 }, m_moveCursorTo));

			Reset();

			Input(std::string( "AB\x001B[;4fCDE") );
			Assert::AreEqual(std::string("ABCDE"), ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 3, 0 }, m_moveCursorTo));

			Reset();

			Input(std::string("AB\x001B[3;fCDE"));
			Assert::AreEqual(std::string("ABCDE"), ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 0, 2 }, m_moveCursorTo));

			Reset();

			Input(std::string("AB\x001B[;fCDE"));
			Assert::AreEqual(std::string("ABCDE"), ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 0, 0 }, m_moveCursorTo));

			Reset();

			Input(std::string("AB\x001B[fCDE"));
			Assert::AreEqual(std::string("ABCDE"), ReceivedCharacters());
			Assert::IsTrue(AreEqual({ 0, 0 }, m_moveCursorTo));
		}
	protected:
		void Input(std::string& _input)
		{
			m_vt100->Input(_input);
		}
        std::string ReceivedCharacters()
		{
			std::stringstream builder;
			for(auto strs : m_chars)
			{
				builder << strs;
			}
			m_chars.clear();
			return builder.str();
		}
        
        virtual void Characters ( IAnsiDecoder& _sender, std::string& _chars ) override
        {
            m_chars.push_back ( _chars );
        }
        
        virtual void SaveCursor ( IAnsiDecoder& _sender ) override
        {
            m_saveCursor = true;
        }
        
        virtual void RestoreCursor ( IAnsiDecoder& _sender ) override
        {
            m_restoreCursor = true;
        }
        
        RECT GetSize ( IAnsiDecoder& _sender ) override
        {
            return m_size;
        }
        
        virtual void MoveCursor ( IAnsiDecoder& _sender, Direction _direction, int _amount ) override
        {
            m_moveCursorDirection = _direction;
            m_moveCursorAmount = _amount;
        }
        
        virtual void MoveCursorToBeginningOfLineBelow ( IAnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine ) override
        {
            m_moveCursorToBeginningOfLineBelow = _lineNumberRelativeToCurrentLine;
        }
        
        virtual void MoveCursorToBeginningOfLineAbove ( IAnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine ) override
        {
            m_moveCursorToBeginningOfLineAbove = _lineNumberRelativeToCurrentLine;
        }
        
        virtual void MoveCursorToColumn ( IAnsiDecoder& _sender, int _columnNumber ) override
        {
            m_moveCursorToColumn = _columnNumber;
        }
        
        virtual void MoveCursorTo ( IAnsiDecoder& _sender, POINT _position ) override
        {
            m_moveCursorTo = _position;
        }
        
        virtual void ClearScreen ( IAnsiDecoder& _sender, ClearDirection _direction ) override
        {
            m_clearScreen = _direction;
        }
        
        virtual void ClearLine ( IAnsiDecoder& _sender, ClearDirection _direction ) override
        {
            m_clearLine = _direction;
        }
        
        virtual void ScrollPageUpwards ( IAnsiDecoder& _sender, int _linesToScroll ) override
        {
            m_scrollPageUpwards = _linesToScroll;
        }
        
        virtual void ScrollPageDownwards ( IAnsiDecoder& _sender, int _linesToScroll ) override
        {
            m_scrollPageDownwards = _linesToScroll;
        }

       virtual void ModeChanged( IAnsiDecoder& _sender, AnsiMode _mode ) override
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
        
        POINT GetCursorPosition ( IAnsiDecoder& _sender ) override
        {
            return m_cursorPosition;
        }

        virtual void SetGraphicRendition ( IAnsiDecoder& _sender, std::vector<GraphicRendition>& _commands ) override
        {
            
        }
	};
}
