#include "implementation/misc/Logger.hpp"

#include <iostream>

namespace orbit
{
    
    std::ostream& Logger::infoStream = std::cout;
    std::ostream& Logger::errorStream = std::cerr;
    std::mutex    Logger::sPrintingMutex;

}