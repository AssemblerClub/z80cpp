#pragma once

#include <iostream>
#include <cstdint>

namespace Z80CPP {

// Forward declares
class Z80;
class Memory;

//
// PRINTER
//
class Printer {
   std::ostream& m_out;

public:   
   Printer(std::ostream& out) : m_out(out) {};

   void  printRegister        (const char* name, uint16_t value);
   void  printCPUStatus       (const Z80& cpu);
   void  printMemoryContents  (const Memory& mem, uint16_t pos, uint16_t blocks);
};

} // Namespace Z80CPP