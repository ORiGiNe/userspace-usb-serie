#ifndef DEBUG_H
#define DEBUG_H

#ifdef ORIGINE_ARDUINO
#  define debug(args)
#else
#  ifdef ORIGINE_DEBUG
#    include <iostream>
#    define debug(args) std::cout << args;
#  else
#    define debug(args)
#  endif
#endif

#endif // DEBUG_H
