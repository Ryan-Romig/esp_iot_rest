#include "smart-home-wrapper.h"
#include "smart-home.hpp"

extern "C" void init_smart_home(void)
{
    // Call the C++ function
    ::init_matter();
}