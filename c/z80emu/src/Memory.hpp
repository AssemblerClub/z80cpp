#pragma once

#include <memory>
#include <cstdint>
#include <cstring>

namespace Z80CPP {

class Memory {
   std::unique_ptr<uint8_t[]> m_bytes;    // Raw Memory Bytes
   uint32_t m_size = 0;                   // Size of the memory

public:
   Memory(uint32_t size) : m_size(size) {
      m_bytes = std::make_unique<uint8_t[]>(size);
   }

   void fill(uint8_t v) {
      std::memset(m_bytes.get(), v, m_size);
   }

   uint8_t& operator[](uint32_t pos) {
      if (pos > m_size) throw std::out_of_range("Requested memory location is out of range\n");
      return m_bytes[pos];
   }
};

}; // Namespace Z80CPP