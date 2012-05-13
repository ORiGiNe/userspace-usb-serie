#ifndef DEBUG_H
#define DEBUG_H

#ifdef ORIGINE_ARDUINO
#define debug(args)
#else
#include <iostream>
#define debug(args) \
	if (ORIGINE_DEBUG) \
	{ \
		std::cout << args; \
	}
#endif

#endif // DEBUG_H
