#pragma once
#include <vector>
#include <cctype>
#include <string>
#include <TextDecoder.h>
#include <IDecoder.h>

namespace libVT100
{
	class EscapeCharacterDecoder : public IDecoder
	{
	public:
		const byte EscapeCharacter = 0x1B;
		const byte LeftBracketCharacter = 0x5B;
		const byte XonCharacter = 17;
		const byte XoffCharacter = 19;

	protected:
		enum class State
		{
			Normal,
			Command,
		};
		State m_state;
		bool m_supportXonXoff;
		bool m_xOffReceived;
		std::vector<std::vector<byte>> m_outBuffer;

	private:
		std::vector<byte> m_commandBuffer;

	public:
		EscapeCharacterDecoder()
		{
			m_state = State::Normal;
			m_supportXonXoff = true;
			m_xOffReceived = false;
			Encoding = std::make_shared<TextDecoder>(CP_ACP);
		}
		virtual ~EscapeCharacterDecoder() throw() {}

	protected:
			virtual bool IsValidParameterCharacter(char _c)
			{
				return (std::isdigit(_c) || _c == ';' || _c == '"' || _c == '?');
			}

			void AddToCommandBuffer(byte _byte)
			{
				if (m_supportXonXoff)
				{
					if (_byte == XonCharacter || _byte == XoffCharacter)
					{
						return;
					}
				}

				m_commandBuffer.push_back(_byte);
			}

			void AddToCommandBuffer(std::vector<byte>& _bytes)
			{
				if (m_supportXonXoff)
				{
					for(auto b : _bytes)
					{
						if (!(b == XonCharacter || b == XoffCharacter))
						{
							m_commandBuffer.push_back(b);
						}
					}
				}
				else
				{
					m_commandBuffer.insert(m_commandBuffer.end(),_bytes.begin(),_bytes.end());
				}
			}

			virtual bool IsValidOneCharacterCommand(char /*_command*/)
			{
				return false;
			}

			void ProcessCommandBuffer()
			{
				m_state = State::Command;

				if (m_commandBuffer.size() > 1)
				{
					if (m_commandBuffer[0] != EscapeCharacter)
					{
						throw std::exception("Internal error, first command character _MUST_ be the escape character, please report this bug to the author.");
					}

					int start = 1;
					// Is this a one or two byte escape code?
					if (m_commandBuffer[start] == LeftBracketCharacter)
					{
						start++;

						// It is a two byte escape code, but we still need more data
						if (m_commandBuffer.size() < 3)
						{
							return;
						}
					}

					bool insideQuotes = false;
					size_t end = start;
					while (end < m_commandBuffer.size() && (IsValidParameterCharacter((char)m_commandBuffer[end]) || insideQuotes))
					{
						if (m_commandBuffer[end] == '"')
						{
							insideQuotes = !insideQuotes;
						}
						end++;
					}

					if (m_commandBuffer.size() == 2 && IsValidOneCharacterCommand((char)m_commandBuffer[start]))
					{
						end = m_commandBuffer.size() - 1;
					}
					if (end == m_commandBuffer.size())
					{
						// More data needed
						return;
					}

					
					std::vector<byte> parameterData;
					for ( int i = 0; i < end-start; i++ )
					{
						parameterData.push_back(m_commandBuffer[start + i]);
					}
					std::vector<WCHAR>  decoderTarget;
					if (parameterData.size() > 0)
					{
						int count = Encoding->GetChars((const char*)& parameterData[0], (int)parameterData.size(), decoderTarget);
						if (count != parameterData.size())
						{
							dprintf("Decoding failed %d bytes instead of %d\n", count, (int)parameterData.size());
						}
					}
					String parameter(decoderTarget.begin(),decoderTarget.end());

					byte command = m_commandBuffer[end];

					try
					{
						ProcessCommand(command, parameter);
					}
					catch(std::exception& ){}
					//finally
					{
						//System.Console.WriteLine ( "Remove the processed commands" );

						// Remove the processed commands
						if (m_commandBuffer.size() == end - 1)
						{
							// All command bytes processed, we can go back to normal handling
							m_commandBuffer.clear();
							m_state = State::Normal;
						}
						else
						{
							bool returnToNormalState = true;
							for (size_t i = end + 1; i < m_commandBuffer.size(); i++)
							{
								if (m_commandBuffer[i] == EscapeCharacter)
								{
									m_commandBuffer.erase(m_commandBuffer.begin(), m_commandBuffer.begin() + i);
									ProcessCommandBuffer();
									returnToNormalState = false;
								}
								else
								{
									ProcessNormalInput(m_commandBuffer[i]);
								}
							}
							if (returnToNormalState)
							{
								m_commandBuffer.clear();

								m_state = State::Normal;
							}
						}
					}
				}
			}
			void ProcessNormalInput(byte _data)
			{
				if (_data == EscapeCharacter)
				{
					throw std::exception("Internal error, ProcessNormalInput was passed an escape character, please report this bug to the author.");
				}
				if (m_supportXonXoff)
				{
					if (_data == XonCharacter || _data == XoffCharacter)
					{
						return;
					}
				}
				std::vector<byte> data({ _data });
				std::vector<WCHAR> str;
				int count = Encoding->GetChars((const char*)&data[0], 1, str);
				if (count != 1)
				{
					dprintf("Decoding failed for ProcessNormalInput %d instead of 1\n", count);
				}
				if (str.size() > 0)
				{
					OnCharacters(str);
				}
				else
				{
					//System.Console.WriteLine ( "char count was zero" );
				}

			}

			virtual void Input(std::vector<byte>& _data) override
			{

				if (_data.size() == 0)
				{
					throw std::exception("Input can not process an empty array.");
				}

				if (m_supportXonXoff)
				{
					for(auto b : _data)
					{
						if (b == XoffCharacter)
						{
							m_xOffReceived = true;
						}
						else if (b == XonCharacter)
						{
							m_xOffReceived = false;
							if (m_outBuffer.size() > 0)
							{
								for(auto output : m_outBuffer)
								{
									OnOutput(output);
								}
							}
						}
					}
				}

				switch (m_state)
				{
				case State::Normal:
					if (_data[0] == EscapeCharacter)
					{
						AddToCommandBuffer(_data);
						ProcessCommandBuffer();
					}
					else
					{
						size_t i = 0;
						while (i < _data.size() && _data[i] != EscapeCharacter)
						{
							ProcessNormalInput(_data[i]);
							i++;
						}
						if (i != _data.size())
						{
							while (i < _data.size())
							{
								AddToCommandBuffer(_data[i]);
								i++;
							}
							ProcessCommandBuffer();
						}
					}
					break;

				case State::Command:
					AddToCommandBuffer(_data);
					ProcessCommandBuffer();
					break;
				}
			}

			virtual void CharacterTyped(WCHAR _character) override
			{
				WCHAR arr[2] = { _character,0 };
				std::vector<byte> vec = Encoding->GetBytes(arr,1);
				OnOutput(vec);
			}

			virtual bool KeyPressed(Keys& /*_modifiers*/, Keys& /*_key*/) override
			{
				return false;
			}

			protected:
				virtual void OnCharacters(std::vector<WCHAR>& _characters) = 0;
				virtual void ProcessCommand(byte _command, String& _parameter) = 0;

			protected:
				virtual void OnOutput(std::vector<byte>& _output)
				{
					if (Output != nullptr)
					{
						if (m_supportXonXoff && m_xOffReceived)
						{
							m_outBuffer.push_back(_output);
						}
						else
						{
							Output(*this, _output);
						}
					}
				}
	};
}
