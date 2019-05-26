#pragma once

#include <memory>
#include <cstdint>
#include <iostream>

namespace Z80CPP {

class Memory {
   std::unique_ptr<uint8_t[]> m_bytes;    // Raw Memory Bytes
   uint16_t m_size = 0;                   // Size of the memory

public:
   Memory(uint16_t size);
   
   void     fill(uint8_t v);
   uint8_t& operator[](uint16_t pos);
   void     print(std::ostream& out, uint16_t pos, uint16_t blocks) const;
};

}; // Namespace Z80CPP