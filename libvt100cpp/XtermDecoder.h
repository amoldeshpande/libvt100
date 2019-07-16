#pragma once
#include <VT100Decoder.h>
namespace libVT100
{
	// Reference https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
	//
	class XtermDecoder;
	class IXtermDecoderClient : public IVT100DecoderClient
	{
	public:
		virtual void SetTitle(XtermDecoder&,String& title) = 0;
	};
	class XtermDecoder : public VT100Decoder
	{
	public:
		XtermDecoder() 
			: m_commandState(CommandState::Idle)
			, VT100Decoder()
		{

		}
	protected:
		enum class CommandState
		{
			Idle,
			ProcessingOSC
		};
		std::vector<IXtermDecoderClient*> m_xtermListeners;
		CommandState m_commandState;
		bool IsValidControlSequenceCharacter(char _c)override
		{
			if (m_commandState == CommandState::Idle)
			{
				if (_c == RightBracketCharacter) // only OSC for now
				{
					m_commandState = CommandState::ProcessingOSC;
					return true;
				}
				else
				{
					return EscapeCharacterDecoder::IsValidControlSequenceCharacter(_c);
				}
			}
			return false;
		}
		bool IsValidParameterCharacter(char _c) override
		{
			switch (m_commandState)
			{
				case CommandState::Idle:
					return VT100Decoder::IsValidParameterCharacter(_c);
				case CommandState::ProcessingOSC:
					if (_c == '\007') 
					{
						m_commandState = CommandState::Idle;
						return false;
					}
					break;
			}
			return (isprint(_c) || _c == ';');
		}
		void ProcessCommand(byte _command, String& _parameter) override
		{
			switch ((char)_command)
			{
				case '\007': //OSC
				{
					std::wistringstream commands(_parameter);
					std::vector<String> parameters;
					String param;
					while (std::getline(commands, param, L';'))
					{
						parameters.push_back(param);
					}
					wchar_t c = parameters[0][0];
					switch (c)
					{
						case L'0':
						case L'2': // Set Title
							if (parameters.size() > 1)
							{
								OnSetTitle(String(parameters[1],0, parameters[1].length()));
							}
							break;
					}
				}
				break;
				default:
					VT100Decoder::ProcessCommand(_command, _parameter);
					break;
			}
		}
		virtual void OnSetTitle(String& title)
		{
			for (auto client : m_xtermListeners)
			{
				client->SetTitle(*this, title);
			}
		}

	public:
		void Subscribe(IXtermDecoderClient* _client)
		{
			m_listeners.push_back(_client);
			m_vt100Listeners.push_back(_client);
			m_xtermListeners.push_back(_client);
		}

		void UnSubscribe(IXtermDecoderClient* _client)
		{
			m_listeners.push_back(_client);
			m_vt100Listeners.push_back(_client);
			m_xtermListeners.push_back(_client);
		}
	};
}//namespace
