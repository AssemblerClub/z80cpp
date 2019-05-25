#include <Printer.hpp>
#include <iomanip>

void printHex(std::ostream& out, uint16_t r) {
   out << std::hex << std::setw(4) << std::setfill('0') << r;
}
