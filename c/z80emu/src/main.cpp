#include <cstdint>
#include <Memory.hpp>
#include <Z80.hpp>

class Computer {
   Z80      m_cpu;
   Memory   m_mem = Memory(4096);

public:
   Computer() {
      uint8_t program[6] = { 0x3E, 0x11, 0x3E, 0xF5, 0x3E, 0x3E };
      std::memcpy(&m_mem[0], program, 6);
   }

   void run(uint16_t cycles) {
      ++cycles;
      while (--cycles) {
         m_cpu.tick();
         if        ( !m_cpu.signal(Signal::RD) ) {
            m_cpu.setData( m_mem[ m_cpu.address() ] );
         } else if ( !m_cpu.signal(Signal::WR) ) {
            m_mem[ m_cpu.address() ] = m_cpu.data();
         }
         m_cpu.print(std::cout);
         uint8_t a;
         std::cin >> a;
      } 
   }
};


int main() {
   Computer K;
   K.run(9);

   return 0;
}