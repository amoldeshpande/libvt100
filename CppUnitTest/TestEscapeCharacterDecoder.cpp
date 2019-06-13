#include "pch.h"
#include <functional>

#include "CppUnitTest.h"
#include "EscapeCharacterDecoder.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libVT100;

namespace CppUnitTest
{
    TEST_CLASS(TestEscapeCharacterDecoder) , public EscapeCharacterDecoder
    {
	private :
		struct Command
		{
			byte m_command;
			String m_parameter;

			Command(byte _command, String _parameter)
			{
				m_command = _command;
				m_parameter = _parameter;
			}
		};
        
        std::vector<std::vector<WCHAR>> m_chars;
        std::vector<Command> m_commands;
        
		TEST_METHOD_INITIALIZE(SetUp)
        {
        }
        
		TEST_METHOD_CLEANUP(TearDown)
        {
			m_chars.clear();
            m_commands .clear();
        }
        
        TEST_METHOD(TestNormalCharactersAreJustPassedThrough)
        {
			std::string s({ 'A',  'B',  'C',  'D',  'E' });
            EscapeCharacterDecoder::Input(std::vector<byte>(s.begin(),s.end()));
            
            Assert::AreEqual ( String(L"ABCDE"), ReceivedCharacters() );
        }
        TEST_METHOD(TestCommandsAreNotInterpretedAsNormalCharacters)
        {
			std::string s({ (byte) 'A', (byte) 'B', 0x1B, (byte) '1', (byte) '2', (byte) '3', (byte) 'm', (byte) 'C', (byte) 'D', (byte) 'E' });
            EscapeCharacterDecoder::Input(std::vector<byte>(s.begin(),s.end()));
            Assert::AreEqual ( String(L"ABCDE"), ReceivedCharacters() );
            
            Input (std::string("\x001B" "123mA") );
            Assert::AreEqual ( String(L"A"), ReceivedCharacters() );
            
            Input ( std::string("\x001B" "123m\x001B" "123mA") );
            Input ( std::string("A" ));
            Assert::AreEqual ( String(L"AA"), ReceivedCharacters() );
            
            Input ( std::string("AB\x001B" "123mCDE") );
            Assert::AreEqual ( String(L"ABCDE"), ReceivedCharacters() );
            
            Input ( std::string("AB\x001B" "123m") );
            Assert::AreEqual ( String(L"AB"), ReceivedCharacters() );
            
            Input ( std::string("A") );
            Input ( std::string("AB\x001B" "123mCDE\x001B" "123m\x001B" "123mCDE") );
            Assert::AreEqual ( String(L"AABCDECDE"), ReceivedCharacters() );

            Input ( std::string("A\x001B" "[123m\x001B[123mA") );
            Input (std::string( "A" ));
            Assert::AreEqual ( String(L"AAA"), ReceivedCharacters() );
            
            Input ( std::string("A\x001B" "123m\x001B" "[123mA") );
            Assert::AreEqual ( String(L"AA"), ReceivedCharacters() );

            Input ( std::string("A\x001B" "[123;321;456a\x001B" "[\"This string is part of the command\"123bA") );
            Assert::AreEqual ( String(L"AA"), ReceivedCharacters() );
        }
        TEST_METHOD(TestCommands)
        {
            Input ( std::string("A\x001B" "123m\x001B" "[123mA") );
            AssertCommand ( 'm', String(L"123") );
            AssertCommand ( 'm', String(L"123") );
            
            Input ( std::string("A\x001B" "[123;321;456a\x001B" "[\"This string is part of the command\"123bA") );
            AssertCommand ( 'a', String(L"123;321;456") );
            AssertCommand ( 'b', String(L"\"This string is part of the command\"123") );
        }
        
        void AssertCommand ( char _command, String& _parameter )
        {
            Assert::IsFalse( m_commands.size() == 0 );
            Assert::AreEqual ( (byte) _command, m_commands[0].m_command );
            Assert::AreEqual ( _parameter, m_commands[0].m_parameter );
            m_commands.erase( m_commands.begin() );
        }
        
		void Input(std::string& _input)
		{
			EscapeCharacterDecoder::Input(std::vector<byte>(_input.begin(),_input.end()));
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
        virtual void ProcessCommand ( byte _command, String& _parameter ) override
        {
            m_commands.push_back(Command(_command, _parameter) );
        }
        
        virtual void OnCharacters ( std::vector<WCHAR>& _chars ) override
        {
            m_chars.push_back ( _chars );
        }
        virtual void OnCharacter ( WCHAR _char ) override
        {
			m_chars.push_back({ _char });
        }

       virtual void OnOutput( std::vector<byte>& _data ) override
       {
          if ( Output != nullptr )
          {
             Output(*this, _data );
          }
       }
		virtual TestEscapeCharacterDecoder::~TestEscapeCharacterDecoder() throw() {}
	};
}
