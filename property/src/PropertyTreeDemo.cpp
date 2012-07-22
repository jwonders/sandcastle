// STD Includes
#include <iostream>

// Library Includes

// Project Includes
#include "Property.h"

REGISTER_PROPERTY_TYPE(std::string, 2)

int main(int argc, char** argv)
{
    typedef Property<std::string> PropertyT;

    PropertyT property = PropertyT("myProperty", 10);
    std::cout << "Value = " << property.get<int>() << std::endl;

    property.set(std::string("WTF?"));
    std::cout << "Value = " << property.get<std::string>() << std::endl;
    
    return 0;
}
