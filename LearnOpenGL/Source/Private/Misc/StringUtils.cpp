
#include "StringUtils.h"

namespace NStringUtils
{
	std::vector<uint8> HexToBytes(const char* Hex)
	{
		if(Hex[0] == '\0') return {};
		
		std::vector<uint8> resultBytes;
		
		const char* curPtr = Hex;
		if(Hex[0] == '#') // "#" prefix
			curPtr += 1;
		else if(Hex[0] == '0' && Hex[1] == 'x') // "0x" prefix
			curPtr += 2;
		
		while(curPtr[0] != '\0' && curPtr[1] != '\0')
		{
			const char subString[] = { curPtr[0], curPtr[1] };
			resultBytes.push_back((uint8)strtol(subString, nullptr, 16));
			
			curPtr += 2;
		}
		
		return resultBytes;
	}
}