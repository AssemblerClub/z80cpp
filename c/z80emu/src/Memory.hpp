#pragma once

#include <memory>
#include <cstdint>
#include <iostream>

namespace Z80CPP {

class Memory {
   std::unique_ptr<uint8_t[]> m_bytes;    // Raw Memory Bytes
   uint16_t m_size = 0;                   // Size of the memory

   const uint8_t& get(uint16_t pos) const;
public:
   Memory(uint16_t size);
   
   void     fill(uint8_t v);
   uint8_t&       operator[](uint16_t pos)         { return const_cast<uint8_t&>(get(pos)); }
   const uint8_t& operator[](uint16_t pos) const   { return get(pos); }
   uint16_t size() const { return m_size; }
};

}; // Namespace Z80CPP