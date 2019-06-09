#pragma once
#include <AnsiDecoder.h>

namespace libVT100
{
   enum class DeviceStatus
   {
      Unknown = -1,
      Ok = 0,
      Failure = 3,
   }
   class VT100Decoder;
   
   class IVT100DecoderClient : public IAnsiDecoderClient
   {
	   std::string GetDeviceCode(VT100Decoder& _decoder);
	   DeviceStatus GetDeviceStatus(VT100Decoder& _decoder);
	   /// <summary>Resize the terminal window to _size (given in characters).</summary>
	   void ResizeWindow(VT100Decoder& _decoder, SIZE _size);
	   /// <summary>Move the terminal window to _size (given in pixels).</summary>
	   void MoveWindow(VT100Decoder& _decoder, Point _position);
   };
   class VT100Decoder : public AnsiDecoder
   {
   protected :
	   std::vector<IVT100DecoderClient> m_vt100Listeners;

   public:
      VT100Decoder() : AnsiDecoder()
      {
      }
	virtual void Subscribe ( IVT100DecoderClient _client ) = 0;
	virtual void UnSubscribe ( IVT100DecoderClient _client ) = 0;

   protected :
	   virtual bool IsValidParameterCharacter(char _c) override
	   {
		   return _c == '=' || _c == ' ' || AnsiDecoder::IsValidParameterCharacter(_c);
	   }

	   void ProcessCommand(byte _command, std::string& _parameter) override
	   {
         switch ( (char) _command )
         {
            case 'c':
			{
				std::string deviceCode = OnGetDeviceCode();
			}
               break;

            case 'n':
               if ( _parameter == "5" )
               {
                  DeviceStatus status = OnGetDeviceStatus();
                  string stringStatus = ((int) status).ToString();
                  byte[] output = new byte[2 + stringStatus.Length + 1];
                  int i = 0;
                  output[i++] = EscapeCharacter;
                  output[i++] = LeftBracketCharacter;
                  foreach ( char c in stringStatus )
                  {
                     output[i++] = (byte) c;
                  }
                  output[i++] = (byte) 'n';
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
               if ( _parameter == "" )
               {
                  // Set scroll region to entire screen
               }
               else
               {
                  // Set scroll region, separated by ;
               }
               break;

            case 't':
               string[] parameters = _parameter.Split( ';' );
               switch ( parameters[0] )
               {
                  case "3":
                     if ( parameters.Length >= 3 )
                     {
                        int left, top;
                        if ( Int32.TryParse( parameters[1], out left ) && Int32.TryParse( parameters[2], out top ) )
                        {
                           OnMoveWindow( new Point( left, top ) );
                        }
                     }
                     break;

                  case "8":
                     if ( parameters.Length >= 3 )
                     {
                        int rows, columns;
                        if ( Int32.TryParse( parameters[1], out rows ) && Int32.TryParse( parameters[2], out columns ) )
                        {
                           OnResizeWindow( new Size( columns, rows ) );
                        }
                     }
                     break;
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
		virtual string OnGetDeviceCode()
      {
         for( auto client : m_vt100Listeners )
         {
            string deviceCode = client.GetDeviceCode( this );
            if ( deviceCode != null )
            {
               return deviceCode;
            }
         }
         return "UNKNOWN";
      }

      virtual DeviceStatus OnGetDeviceStatus()
      {
         for( auto client : m_vt100Listeners )
         {
            DeviceStatus status = client.GetDeviceStatus( this );
            if ( status != DeviceStatus.Unknown )
            {
               return status;
            }
         }
         return DeviceStatus.Failure;
      }

      virtual void OnResizeWindow( Size _size )
      {
         for( auto client : m_vt100Listeners )
         {
            client.ResizeWindow( this, _size );
         }
      }

      virtual void OnMoveWindow( Point _position )
      {
         for( auto client : m_vt100Listeners )
         {
            client.MoveWindow( this, _position );
         }
      }

      void IVT100Decoder.Subscribe( IVT100DecoderClient _client )
      {
         m_listeners.Add( _client );
         m_vt100Listeners.Add( _client );
      }

      void IVT100Decoder.UnSubscribe( IVT100DecoderClient _client )
      {
         m_vt100Listeners.Remove( _client );
         m_listeners.Remove( _client );
      }
   }
}
