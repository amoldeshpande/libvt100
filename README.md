
C++ port of original at https://github.com/rasmus-toftdahl-olesen/libvt100

Passes all relevant  Unit Tests duplicated from the original.


Requires the ICU library. Easiest way is to get it via vcpkg:

	- Set VCPKG_DEFAULT_TRIPLET=x64-windows-static in the environment 
	- vcpkg install icu

Note that linking with C-runtime DLLs is not supported so you must compile everything static. You could presumably change the project settings to link with the runtimes, but please do not ask me to support that configuration.




