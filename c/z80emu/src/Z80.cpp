#include <Z80.hpp>
#include <bitset>
#include <Printer.hpp>

namespace Z80CPP {

void 
Z80::fetch() {
   rstSignal(Signal::MREQ);
   rstSignal(Signal::RD);
   rstSignal(Signal::M1);
   m_address = m_reg.PC;      
   ++m_reg.PC;
}

void 
Z80::decode() {
   std::cout << "Decode\n";
   m_reg.IR = m_data;

   // Check first 2 bits from Opcode
   uint8_t op = m_reg.IR & 0xC0;
   switch( m_reg.IR ) {
      case 0x3E: fetch(); break;
   }
}

//|         M1           |      M2           |
//             | REFRESH |            
//| MREQ | DIN | DEC | W | MREQ | DIN | EXEC | 

void 
Z80::execute() {
   std::cout << "Execute\n";
   switch(m_reg.IR) {
      case 0x3E: m_reg.main.A = m_data; break;
   }
}

void
Z80::wait() {
   std::cout << "Waiting for memory\n";
}

void 
Z80::tick() {
   static uint8_t o = 0;
   using TFun = decltype(&Z80::fetch);
   const uint8_t numops = 4;
   const TFun ops[numops] = { &Z80::fetch, &Z80::wait, &Z80::decode, &Z80::execute };

   m_signals = 0xFF;    // Reset Signals
   (this->*ops[o])();   // Tick
   o = ++o % numops;    // Select next operation
   ++m_ticks;           // One tick more
}

void 
Z80::print(std::ostream& out) const {
   Printer p(out);
   auto pr = [&p](const char*n, uint16_t r) { p.printRegister(n, r); };
   
   pr("AF", m_reg.main.AF); pr("AF'", m_reg.alt.AF); out << "\n";
   pr("BC", m_reg.main.BC); pr("BC'", m_reg.alt.BC); out << "\n";
   pr("DE", m_reg.main.DE); pr("DE'", m_reg.alt.DE); out << "\n";
   pr("HL", m_reg.main.HL); pr("HL'", m_reg.alt.HL); out << "\n";
   pr("IX",      m_reg.IX); pr("IY ",     m_reg.IY); out << "\n";
   pr("PC",      m_reg.PC); pr("SP ",     m_reg.SP); out << "\n";
   pr(" I",      m_reg.I ); pr(" R ",     m_reg.R ); out << "\n";
   pr("WZ", (m_reg.W << 8) | m_reg.Z);
   pr("IR ",     m_reg.IR); out << "\n";
   out << "Signals{"<< std::bitset<8>((uint16_t)m_signals) << "}\n";
   out << "Ticks: " << std::dec << m_ticks << "\n";
}

}; // Namespace Z80CPP