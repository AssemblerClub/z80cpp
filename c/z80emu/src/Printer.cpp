#include <Printer.hpp>
#include <iomanip>

namespace Z80CPP {

std::ostream& 
Printer::printRegister(const char* name, uint16_t value) {
   m_out << name << "|";
   m_out << std::hex << std::setw(4) << std::setfill('0');
   m_out << value << "|";
   return m_out;
}

}; // Namespace Z80CPP