#pragma once
#include <exception>

namespace libVT100
{
	class InvalidByteException : public std::exception
	{
	protected:
		std::string formatMsg(const char* thing, byte _command, String _parameter)
		{
			char buffer[256] = { 0 };
			sprintf_s(buffer, 256, "Invalid %s for command %x '%c',\"%S\"",thing, _command, (char)_command, _parameter.c_str());
			return std::string(buffer);
		}
	protected:
		byte m_byte;

	public:
		byte Byte() { return m_byte; }


		InvalidByteException(byte _byte, std::string _message)
			: std::exception(_message.c_str())
		{
			m_byte = _byte;
		}
	};
	class InvalidParameterException : public InvalidByteException
	{

	public:
		InvalidParameterException(byte _command, String _parameter) : InvalidByteException(_command,formatMsg("parameter",_command, _parameter)) {}

	};
	class InvalidCommandException : public InvalidByteException
	{
	public:
		InvalidCommandException(byte _command, String _parameter) : InvalidByteException(_command, formatMsg("command", _command, _parameter))
		{
		}
	};
}
