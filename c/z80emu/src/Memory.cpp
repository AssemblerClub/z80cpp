#include <iomanip>
#include <cstring>
#include <Memory.hpp>

namespace Z80CPP {

Memory::Memory(uint16_t size) : m_size(size) {
   m_bytes = std::make_unique<uint8_t[]>(size);
}

void 
Memory::fill(uint8_t v) {
   std::memset(m_bytes.get(), v, m_size);
}

uint8_t&
Memory::operator[](uint16_t pos) {
   if (pos > m_size) throw std::out_of_range("[]: Requested memory location is out of range\n");
   
   return m_bytes[pos];
}

void
Memory::print(std::ostream& out, uint16_t pos, uint16_t blocks) const {
   if (pos > m_size || pos + 16*blocks > m_size) throw std::out_of_range("PRINT: Requested memory location is out of range\n");
   
   ++blocks;
   uint8_t* pmem = &m_bytes[pos];
   while(--blocks) {
      out << std::hex << std::setw(4) << std::setfill('0');
      out << pos << "|";
      uint8_t i = 17;
      while(--i) { 
         out << " " << std::setw(2) << std::setfill('0');
         out << static_cast<uint16_t>(*pmem);
         ++pmem;
      }
      pos += 16;
      out << " |\n";
   }
}

}; // Namespace Z80CPP