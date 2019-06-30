#pragma once
#include <IAnsiDecoderClient.h>
#include <EscapeCharacterDecoder.h>

namespace libVT100
{

   class AnsiDecoder : public EscapeCharacterDecoder
   {
   protected:
	   std::vector<IAnsiDecoderClient*> m_listeners;


   private:
	   static const char* FUNCTIONKEY_MAP[];

   public:
	   AnsiDecoder() :EscapeCharacterDecoder() {}

   protected:
	   int DecodeInt(String& _value, int _default)
	   {
		   if (_value.size() == 0)
		   {
			   return _default;
		   }
		   int ret;
		   WCHAR* end = nullptr;
		   ret = wcstol(_value.c_str(), &end, 10);
		   if ((end != nullptr) && (end != _value.c_str()))
		   {
			   return ret;
		   }
		   else
		   {
			   return _default;
		   }
	   }

   public:
	   virtual void Input(std::vector<byte>& _data) override
	   {
		   EscapeCharacterDecoder::Input(_data);
	   }
	   virtual void CharacterTyped(WCHAR _character) override
	   {
		   EscapeCharacterDecoder::CharacterTyped(_character);
	   }
	   virtual void ProcessCommand(byte _command, String& _parameter)override
	   {
		   //System.Console.WriteLine ( "ProcessCommand: {0} {1}", (char) _command, _parameter );
		   switch ((char)_command)
		   {
		   case 'A':
			   OnMoveCursor(Direction::Up, DecodeInt(_parameter, 1));
			   break;

		   case 'B':
			   OnMoveCursor(Direction::Down, DecodeInt(_parameter, 1));
			   break;

		   case 'C':
			   OnMoveCursor(Direction::Forward, DecodeInt(_parameter, 1));
			   break;

		   case 'D':
			   OnMoveCursor(Direction::Backward, DecodeInt(_parameter, 1));
			   break;

		   case 'E':
			   OnMoveCursorToBeginningOfLineBelow(DecodeInt(_parameter, 1));
			   break;

		   case 'F':
			   OnMoveCursorToBeginningOfLineAbove(DecodeInt(_parameter, 1));
			   break;

		   case 'G':
			   OnMoveCursorToColumn(DecodeInt(_parameter, 1) - 1);
			   break;

		   case 'H':
		   case 'f':
		   {
			   size_t separator = _parameter.find(';');
			   if (separator == std::string::npos)
			   {
				   OnMoveCursorTo({ 0,0 });
			   }
			   else
			   {
				   String row = _parameter.substr(0, separator);
				   String column = _parameter.substr(separator + 1, _parameter.length() - separator - 1);
				   OnMoveCursorTo({ DecodeInt(column, 1) - 1, DecodeInt(row, 1) - 1 });
			   }
		   }
		   break;

		   case 'J':
			   OnClearScreen((ClearDirection)DecodeInt(_parameter, 0));
			   break;

		   case 'K':
			   OnClearLine((ClearDirection)DecodeInt(_parameter, 0));
			   break;

		   case 'S':
			   OnScrollPageUpwards(DecodeInt(_parameter, 1));
			   break;

		   case 'T':
			   OnScrollPageDownwards(DecodeInt(_parameter, 1));
			   break;

		   case 'm':
		   {
			   std::wistringstream commands(_parameter);
			   String s;
			   std::vector<GraphicRendition> renditionCommands;
			   while (std::getline(commands, s, L';'))
			   {
				   renditionCommands.push_back((GraphicRendition)DecodeInt(s, 0));
			   }
			   OnSetGraphicRendition(renditionCommands);
		   }
		   break;

		   case 'n':
			   if (_parameter == L"6")
			   {
				   Point cursorPosition = OnGetCursorPosition();
				   cursorPosition.x++;
				   cursorPosition.y++;
				   std::string row = std::to_string(cursorPosition.y);
				   std::string column = std::to_string(cursorPosition.x);
				   std::vector<byte> output;
				   output.push_back( EscapeCharacter);
				   output.push_back( LeftBracketCharacter);
				   for (auto c : row)
				   {
					   output.push_back( (byte)c);
				   }
				   output.push_back( (byte) ';');
				   for (auto c : column)
				   {
					   output.push_back( (byte)c);
				   }
				   output.push_back( (byte) 'R');
				   OnOutput(output);
			   }
			   break;

		   case 's':
			   OnSaveCursor();
			   break;

		   case 'u':
			   OnRestoreCursor();
			   break;

		   case 'l':
		   {
			   if (_parameter == L"20")
			   {
				   // Set line feed mode
				   OnModeChanged(AnsiMode::LineFeed);
			   }
			   else if (_parameter == L"?1")
			   {
				   // Set cursor key to cursor  DECCKM 
				   OnModeChanged(AnsiMode::CursorKeyToCursor);
			   }
			   else if (_parameter == L"?2") {
				   // Set ANSI (versus VT52)  DECANM
				   OnModeChanged(AnsiMode::VT52);
			   }

			   else if (_parameter == L"?3") {
				   // Set number of columns to 80  DECCOLM 
				   OnModeChanged(AnsiMode::Columns80);
			   }

			   else if (_parameter == L"?4") {
				   // Set jump scrolling  DECSCLM 
				   OnModeChanged(AnsiMode::JumpScrolling);
			   }

			   else if (_parameter == L"?5") {
				   // Set normal video on screen  DECSCNM 
				   OnModeChanged(AnsiMode::NormalVideo);
			   }

			   else if (_parameter == L"?6") {
				   // Set origin to absolute  DECOM 
				   OnModeChanged(AnsiMode::OriginIsAbsolute);
			   }

			   else if (_parameter == L"?7") {
				   // Reset auto-wrap mode  DECAWM 
				   // Disable line wrap
				   OnModeChanged(AnsiMode::DisableLineWrap);
			   }

			   else if (_parameter == L"?8") {
				   // Reset auto-repeat mode  DECARM 
				   OnModeChanged(AnsiMode::DisableAutoRepeat);
			   }

			   else if (_parameter == L"?9") {
				   // Reset interlacing mode  DECINLM 
				   OnModeChanged(AnsiMode::DisableInterlacing);
			   }

			   else if (_parameter == L"?25") {
				   OnModeChanged(AnsiMode::HideCursor);
			   }

			   else
				   throw InvalidParameterException(_command, _parameter);
		   }
		   break;

		   case 'h':
			   if (_parameter == L"") {
				   //Set ANSI (versus VT52)  DECANM
				   OnModeChanged(AnsiMode::ANSI);
			   }

			   else if (_parameter == L"20") {
				   // Set new line mode
				   OnModeChanged(AnsiMode::NewLine);
			   }

			   else if (_parameter == L"?1") {
				   // Set cursor key to application  DECCKM
				   OnModeChanged(AnsiMode::CursorKeyToApplication);
			   }

			   else if (_parameter == L"?3") {
				   // Set number of columns to 132  DECCOLM
				   OnModeChanged(AnsiMode::Columns132);
			   }

			   else if (_parameter == L"?4") {
				   // Set smooth scrolling  DECSCLM
				   OnModeChanged(AnsiMode::SmoothScrolling);
			   }

			   else if (_parameter == L"?5") {
				   // Set reverse video on screen  DECSCNM
				   OnModeChanged(AnsiMode::ReverseVideo);
			   }

			   else if (_parameter == L"?6") {
				   // Set origin to relative  DECOM
				   OnModeChanged(AnsiMode::OriginIsRelative);
			   }

			   else if (_parameter == L"?7") {
				   //  Set auto-wrap mode  DECAWM
				   // Enable line wrap
				   OnModeChanged(AnsiMode::LineWrap);
			   }

			   else if (_parameter == L"?8") {
				   // Set auto-repeat mode  DECARM
				   OnModeChanged(AnsiMode::AutoRepeat);
			   }

			   else if (_parameter == L"?9") {
				   /// Set interlacing mode 
				   OnModeChanged(AnsiMode::Interlacing);
			   }

			   else if (_parameter == L"?25") {
				   OnModeChanged(AnsiMode::ShowCursor);
			   }
			   else {
				   throw InvalidParameterException(_command, _parameter);
			   }
			   break;

		   case '>':
			   // Set numeric keypad mode
			   OnModeChanged(AnsiMode::NumericKeypad);
			   break;

		   case '=':
			   OnModeChanged(AnsiMode::AlternateKeypad);
			   // Set alternate keypad mode (rto: non-numeric, presumably)
			   break;

		   default:
			   throw InvalidCommandException(_command, _parameter);
		   }
	   }

   protected:
	   virtual bool IsValidOneCharacterCommand(char _command)override
	   {
		   return _command == '=' || _command == '>';
	   }

	   virtual void OnSetGraphicRendition(std::vector<GraphicRendition>& _commands)
	   {
		   for (auto client : m_listeners)
		   {
			   client->SetGraphicRendition(*this, _commands);
		   }
	   }

	   virtual void OnScrollPageUpwards(int _linesToScroll)
	   {
		   for (auto client : m_listeners)
		   {
			   client->ScrollPageUpwards(*this, _linesToScroll);
		   }
	   }

	   virtual void OnScrollPageDownwards(int _linesToScroll)
	   {
		   for (auto client : m_listeners)
		   {
			   client->ScrollPageDownwards(*this, _linesToScroll);
		   }
	   }

	   virtual void OnModeChanged(AnsiMode _mode)
	   {
		   for (auto client : m_listeners)
		   {
			   client->ModeChanged(*this, _mode);
		   }
	   }

	   virtual void OnSaveCursor()
	   {
		   for (auto client : m_listeners)
		   {
			   client->SaveCursor(*this);
		   }
	   }

	   virtual void OnRestoreCursor()
	   {
		   for (auto client : m_listeners)
		   {
			   client->RestoreCursor(*this);
		   }
	   }

	   virtual Point OnGetCursorPosition()
	   {
		   Point ret;
		   for (auto client : m_listeners)
		   {
			   ret = client->GetCursorPosition(*this);
			   if (!IsEmpty(ret))
			   {
				   return ret;
			   }
		   }
		   return { 0, 0 };
	   }

	   virtual void OnClearScreen(ClearDirection _direction)
	   {
		   for (auto client : m_listeners)
		   {
			   client->ClearScreen(*this, _direction);
		   }
	   }

	   virtual void OnClearLine(ClearDirection _direction)
	   {
		   for (auto client : m_listeners)
		   {
			   client->ClearLine(*this, _direction);
		   }
	   }

	   virtual void OnMoveCursorTo(Point _position)
	   {
		   for (auto client : m_listeners)
		   {
			   client->MoveCursorTo(*this, _position);
		   }
	   }

	   virtual void OnMoveCursorToColumn(int _columnNumber)
	   {
		   for (auto client : m_listeners)
		   {
			   client->MoveCursorToColumn(*this, _columnNumber);
		   }
	   }

	   virtual void OnMoveCursor(Direction _direction, int _amount)
	   {
		   for (auto client : m_listeners)
		   {
			   client->MoveCursor(*this, _direction, _amount);
		   }
	   }

	   virtual void OnMoveCursorToBeginningOfLineBelow(int _lineNumberRelativeToCurrentLine)
	   {
		   for (auto client : m_listeners)
		   {
			   client->MoveCursorToBeginningOfLineBelow(*this, _lineNumberRelativeToCurrentLine);
		   }
	   }

	   virtual void OnMoveCursorToBeginningOfLineAbove(int _lineNumberRelativeToCurrentLine)
	   {
		   for (auto client : m_listeners)
		   {
			   client->MoveCursorToBeginningOfLineAbove(*this, _lineNumberRelativeToCurrentLine);
		   }
	   }

	   virtual void OnCharacters(std::vector<WCHAR>& _characters) override
	   {
		   for (auto client : m_listeners)
		   {
			   client->Characters(*this, _characters);
		   }
	   }
	   virtual void OnCharacter(WCHAR _character) override
	   {
		   for (auto client : m_listeners)
		   {
			   client->OneCharacter(*this, _character);
		   }
	   }

   private:

   protected:
	   virtual bool KeyPressed(Keys& /*_modifiers*/, Keys& /*_key*/) override
	   {
#if 0
		   if ((int)VK_F1 <= (int)_key && (int)_key <= (int)VK_F12)
		   {
			   byte[] r = new byte[5];
			   r[0] = 0x1B;
			   r[1] = (byte) '[';
			   int n = (int)_key - (int)VK_F1;
			   if ((_modifiers & VK_Shift) != VK_None)
				   n += 10;
			   char tail;
			   if (n >= 20)
				   tail = (_modifiers & VK_Control) != VK_None ? '@' : '$';
			   else
				   tail = (_modifiers & VK_Control) != VK_None ? '^' : '~';
			   string f = FUNCTIONKEY_MAP[n];
			   r[2] = (byte)f[0];
			   r[3] = (byte)f[1];
			   r[4] = (byte)tail;
			   OnOutput(r);
			   return true;
		   }
		   else if (_key == VK_Left || _key == VK_Right || _key == VK_Up || _key == VK_Down)
		   {
			   byte[] r = new byte[3];
			   r[0] = 0x1B;
			   //if ( _cursorKeyMode == TerminalMode.Normal )
			   r[1] = (byte) '[';
			   //else
			   //    r[1] = (byte) 'O';

			   switch (_key)
			   {
			   case VK_Up:
				   r[2] = (byte) 'A';
				   break;
			   case VK_Down:
				   r[2] = (byte) 'B';
				   break;
			   case VK_Right:
				   r[2] = (byte) 'C';
				   break;
			   case VK_Left:
				   r[2] = (byte) 'D';
				   break;
			   default:
				   throw std::exception("unknown cursor key code: key");
			   }
			   OnOutput(r);
			   return true;
		   }
		   else
		   {
			   byte[] r = new byte[4];
			   r[0] = 0x1B;
			   r[1] = (byte) '[';
			   r[3] = (byte) '~';
			   if (_key == VK_Insert)
			   {
				   r[2] = (byte) '1';
			   }
			   else if (_key == VK_Home)
			   {
				   r[2] = (byte) '2';
			   }
			   else if (_key == VK_PageUp)
			   {
				   r[2] = (byte) '3';
			   }
			   else if (_key == VK_Delete)
			   {
				   r[2] = (byte) '4';
			   }
			   else if (_key == VK_End)
			   {
				   r[2] = (byte) '5';
			   }
			   else if (_key == VK_PageDown)
			   {
				   r[2] = (byte) '6';
			   }
			   else if (_key == VK_Enter)
			   {
				   //return new byte[] { 0x1B, (byte) 'M', (byte) '~' };
				   //r[1] = (byte) 'O';
				   //r[2] = (byte) 'M';
				   //return new byte[] { (byte) '\r', (byte) '\n' };
				   r = new byte[]{ 13 };
			   }
			   else if (_key == VK_Escape)
			   {
				   r = new byte[]{ 0x1B };
			   }
			   else if (_key == VK_Tab)
			   {
				   r = new byte[]{ (byte) '\t' };
			   }
			   else
			   {
				   return false;
			   }
			   OnOutput(r);
			   return true;
		   }
#else
		   return false;
#endif 0
	   }

   public:
	   virtual void Subscribe(IAnsiDecoderClient* _client) 
	   {
		   m_listeners.push_back(_client);
	   }

	   virtual void UnSubscribe(IAnsiDecoderClient* /*_client*/)
	   {
		   //m_listeners.erase(m_listeners.find( _client) );
	   }

   };
}
