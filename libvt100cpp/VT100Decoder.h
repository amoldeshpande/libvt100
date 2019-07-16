#pragma once
#include <AnsiDecoder.h>

namespace libVT100
{
	enum class DeviceStatus
	{
		Unknown = -1,
		Ok = 0,
		Failure = 3,
	};
   class VT100Decoder;
   
   class IVT100DecoderClient : public IAnsiDecoderClient
   {
   public:
	   virtual String GetDeviceCode(VT100Decoder& _decoder) = 0;
	   virtual DeviceStatus GetDeviceStatus(VT100Decoder& _decoder) = 0;
	   /// <summary>Resize the terminal window to _size (given in characters).</summary>
	   virtual void ResizeWindow(VT100Decoder& _decoder, SIZE _size) = 0;
	   /// <summary>Move the terminal window to _size (given in pixels).</summary>
	   virtual void MoveWindow(VT100Decoder& _decoder, Point _position) = 0;
   };
   class VT100Decoder : public AnsiDecoder
   {
   protected :
	   std::vector<IVT100DecoderClient*> m_vt100Listeners;

   public:
      VT100Decoder() : AnsiDecoder()
      {
      }

   protected :
	   virtual bool IsValidParameterCharacter(char _c) override
	   {
		   return _c == '=' || _c == ' ' || AnsiDecoder::IsValidParameterCharacter(_c);
	   }

	   void ProcessCommand(byte _command, String& _parameter) override
	   {
         switch ( (char) _command )
         {
            case 'c':
			{
				String deviceCode = OnGetDeviceCode();
			}
               break;

            case 'n':
               if ( _parameter == L"5" )
               {
                  DeviceStatus status = OnGetDeviceStatus();
				  std::string stringStatus = std::to_string((int)status);
				  std::vector<byte> output;
                  output.push_back(EscapeCharacter);
                  output.push_back(LeftBracketCharacter);
                  for ( auto c : stringStatus )
                  {
                     output.push_back((byte) c);
                  }
                  output.push_back( (byte) 'n');
                  OnOutput( output );
               }
               else
               {
                  AnsiDecoder::ProcessCommand( _command, _parameter );
               }
               break;

            case '(':
               // Set normal font
               break;

            case ')':
            // Set alternative font

            case 'r':
               if ( _parameter == L"" )
               {
                  // Set scroll region to entire screen
               }
               else
               {
                  // Set scroll region, separated by ;
               }
               break;

            case 't':
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
					case L'3':
						if (parameters.size() >= 3)
						{
							int left = DecodeInt(parameters[1], -1);
							int top = DecodeInt(parameters[2], -1);
							if ((left != -1) && (top != -1))
							{
								OnMoveWindow(Point({ left, top }));
							}
						}
						break;

					case L'8':
						if (parameters.size() >= 3)
						{
							int rows = DecodeInt(parameters[1], -1);
							int columns = DecodeInt(parameters[2], -1);
							if ((rows != -1) && (columns != -1))
							{
								OnResizeWindow(SIZE({ columns, rows }));
							}
						}
						break;
				}
			}
               break;

            case '!':
               // Graphics Repeat Introducer
               break;

            default:
               AnsiDecoder::ProcessCommand( _command, _parameter );
               break;
         }
	  }
	protected:
		virtual String OnGetDeviceCode()
      {
         for( auto client : m_vt100Listeners )
         {
            String deviceCode = client->GetDeviceCode( *this );
            if ( deviceCode.length() != 0 )
            {
               return deviceCode;
            }
         }
         return String(L"UNKNOWN");
      }

      virtual DeviceStatus OnGetDeviceStatus()
      {
         for( auto client : m_vt100Listeners )
         {
            DeviceStatus status = client->GetDeviceStatus( *this );
            if ( status != DeviceStatus::Unknown )
            {
               return status;
            }
         }
         return DeviceStatus::Failure;
      }

      virtual void OnResizeWindow( SIZE _size )
      {
         for( auto client : m_vt100Listeners )
         {
            client->ResizeWindow(*this, _size );
         }
      }

      virtual void OnMoveWindow( Point _position )
      {
         for( auto client : m_vt100Listeners )
         {
            client->MoveWindow(*this, _position );
         }
      }

  public:
      void Subscribe( IVT100DecoderClient* _client )  
      {
         m_listeners.push_back( _client );
         m_vt100Listeners.push_back( _client );
      }

      void UnSubscribe( IVT100DecoderClient* _client ) 
      {
         m_vt100Listeners.push_back( _client );
         m_listeners.push_back( _client );
      }
   };
}
