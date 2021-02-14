
#include "color.h"
#include <ctime>
#include <cstdlib>

namespace ts3::math
{

	RGBAColorU8 generateRandomColor()
	{
		return RGBAColorU8{
			static_cast<uint8>( rand() % 256 ),
			static_cast<uint8>( rand() % 256 ),
			static_cast<uint8>( rand() % 256 )
		};
	}

}
