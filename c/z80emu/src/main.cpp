#include <cstdint>
#include <Memory.hpp>
#include <Z80.hpp>

class Computer {
   Z80CPP::Z80      m_cpu;
   Z80CPP::Memory   m_mem = Z80CPP::Memory(4096);

public:
   Computer() {
      //uint8_t program[16] = { 0x3E, 0x11, 0x06, 0x22, 0x0E, 0x33, 0x16, 0x44
      //                     ,  0x1E, 0x55, 0x26, 0x00, 0x2E, 0x02, 0x36, 0x88 };
      uint8_t program[16] = {    0x3E, 0x11, 0x7F, 0x47, 0x4F, 0x57, 0x5F, 0x67
                             ,   0x6F, 0x7D, 0x26, 0x00, 0x2E, 0x04, 0x36, 0x6C };
      std::memcpy(&m_mem[0], program, 16);
   }

   void run() {
      uint8_t inp = ' ';
      while (inp != 'q') {
         m_cpu.print(std::cout);
         m_mem.print(std::cout, 0x0000, 2);
         std::cin >> inp;
         m_cpu.tick();
         if        ( !m_cpu.signal(Z80CPP::Signal::RD) ) {
            m_cpu.setData( m_mem[ m_cpu.address() ] );
         } else if ( !m_cpu.signal(Z80CPP::Signal::WR) ) {
            m_mem[ m_cpu.address() ] = m_cpu.data();
         }
      } 
   }
};


int main() {
   Computer K;
   K.run();

   return 0;
}