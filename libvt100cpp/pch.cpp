// pch.cpp: source file corresponding to the pre-compiled header
#include "pch.h"
#include <AnsiDecoder.h>

namespace libVT100
{
	// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
	const char* AnsiDecoder::FUNCTIONKEY_MAP[] = {
		//      F1     F2     F3     F4     F5     F6     F7     F8     F9     F10    F11  F12
			"11", "12", "13", "14", "15", "17", "18", "19", "20", "21", "23", "24",
			//      F13    F14    F15    F16    F17  F18    F19    F20    F21    F22
				"25", "26", "28", "29", "31", "32", "33", "34", "23", "24" };
}
