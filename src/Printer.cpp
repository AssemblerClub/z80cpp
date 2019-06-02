#include <Printer.hpp>
#include <iomanip>
#include <Z80.hpp>
#include <Memory.hpp>

namespace Z80CPP {

void
Printer::printRegister(const char* name, uint16_t value) {
   m_out << name << "|";
   m_out << std::hex << std::setw(4) << std::setfill('0');
   m_out << value << "|";
}

void
Printer::printCPUStatus(const Z80& cpu) {
   auto pr = [this](const char*n, uint16_t r) { printRegister(n, r); };
   auto& r  = cpu.registers();
   auto& rm = r.main;
   auto& ra = r.alt;
   
   m_out << "------------------------------------\n";
   pr("AF", rm.AF); pr("AF'", ra.AF); m_out << "   BUS\n";
   pr("BC", rm.BC); pr("BC'", ra.BC); m_out << "---------\n";
   pr("DE", rm.DE); pr("DE'", ra.DE); pr("ADD", cpu.address()); m_out << "\n";
   pr("HL", rm.HL); pr("HL'", ra.HL); pr("DAT", cpu.data());    m_out << "\n";
   pr("IX",  r.IX); pr("IY ",  r.IY); m_out << "\n";
   pr("PC",  r.PC); pr("SP ",  r.SP); m_out << "\n";
   pr("IR",  r.IR); pr("WZ ",  r.WZ); m_out << "\n";
   m_out << "Signals:(" << cpu.signals() << "):";
   if ( cpu.signal(Signal::M1)    ) m_out << "|M1";
   if ( cpu.signal(Signal::MREQ)  ) m_out << "|MREQ";
   if ( cpu.signal(Signal::IORQ)  ) m_out << "|IORQ";
   if ( cpu.signal(Signal::RD)    ) m_out << "|RD";
   if ( cpu.signal(Signal::WR)    ) m_out << "|WR";
   if ( cpu.signal(Signal::RFSH)  ) m_out << "|RFSH";
   if ( cpu.signal(Signal::HALT)  ) m_out << "|HALT";
   if ( cpu.signal(Signal::WAIT)  ) m_out << "|WAIT";
   if ( cpu.signal(Signal::WSAMP) ) m_out << "|WSMP";
   m_out << "|\n";
   m_out << "Ticks: " << std::dec << cpu.ticks() << "\n";
}

uint16_t
adjustMemAddr(uint16_t addr, uint32_t max) {
   // Adjust address to show 2 rows around given address
   if (addr >= max) addr = max;
   addr &= 0xFFF0;
   if (addr >= 0x10) addr -= 0x10;

   return addr;
}


void
Printer::printMemoryContents(const Memory& mem, uint16_t pos, uint16_t blocks) {
   auto higlight= pos;
   auto size    = mem.size();
        pos     = adjustMemAddr(pos, size);
   auto maxaddr = adjustMemAddr(pos + 16*blocks, size) + 0x10;
   auto* pmem   = &mem[pos];
   blocks       = 1 + (maxaddr - pos) / 16;
   
   m_out << "dddd|  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |\n";
   m_out << "----|-------------------------------------------------|\n";
   while(--blocks) {
      m_out << std::hex << std::setw(4) << std::setfill('0');
      m_out << pos << "|";
      uint8_t i = 17;
      while(--i) { 
         uint16_t v = static_cast<uint16_t>(*pmem);
         if      (higlight == pos  ) m_out << "[";
         else if (higlight == pos-1) m_out << "]";
         else                        m_out << " ";
         m_out << std::setw(2) << std::setfill('0') << v;
         ++pmem;
         ++pos;
      }
      m_out << " |\n";
   }
}

}; // Namespace Z80CPP