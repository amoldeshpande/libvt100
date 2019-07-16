#pragma once
namespace libVT100
{
	class Keys;
	class IDecoder;
	class TextDecoder;

	typedef std::function<void(IDecoder& _decoder, std::vector<byte>& _output)> DecoderOutputDelegate;
	class IDecoder
	{
	public:
		IDecoder() {}
		virtual ~IDecoder() throw(){}

		std::shared_ptr<TextDecoder> Encoding;
		/// <summary>
		/// Tell decoder to process the given data.
		/// 
		/// If an invalid byte is passed InvalidByteException or one
		/// of the its sub-classes is thrown. The decoder will try its
		/// best to survive any invalid data and should still be able
		/// to process data after an exception is thrown.
		/// </summary>
		virtual void Input(std::vector<byte>& _data) = 0;
		virtual void Input(byte* _data, int len) = 0;

		DecoderOutputDelegate Output;

		virtual bool KeyPressed(Keys& _modifiers, Keys& _key) = 0;
		virtual void CharacterTyped(WCHAR _character) = 0;
	};
}

