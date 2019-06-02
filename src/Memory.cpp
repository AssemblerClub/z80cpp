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

const uint8_t&
Memory::get(uint16_t pos) const {
   if (pos > m_size) throw std::out_of_range("[]: Requested memory location is out of range\n");
   
   return m_bytes[pos];
}

}; // Namespace Z80CPP