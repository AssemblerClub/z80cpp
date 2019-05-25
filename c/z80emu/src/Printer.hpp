#pragma once

#include <iostream>
#include <cstdint>

namespace Z80CPP {

class Printer {
   std::ostream& m_out;

public:   
   Printer(std::ostream& out) : m_out(out) {};

   std::ostream& printRegister(const char* name, uint16_t value);
};

}; // Namespace Z80CPP