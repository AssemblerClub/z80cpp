#include <Z80.hpp>
#include <Printer.hpp>
#include <bitset>

namespace Z80CPP {

Z80::Z80() {
   const uint16_t S_MREQ_RD_M1 = ~((uint16_t)Signal::MREQ | (uint16_t)Signal::RD | (uint16_t)Signal::M1);
   const std::array<TState, 4> M1 = {{
         {  .signals= S_MREQ_RD_M1, .addr= m_reg.PC, .op= &Z80::fetch }
      ,  {  .signals= S_MREQ_RD_M1 }  // Just Wait
      ,  {  .data= m_data, .op= &Z80::data_in_IR   }  // Data IN
      ,  {  .op= &Z80::decode       }  // Decode
   }};
}

void
Z80::t (TState& t) {
   m_signals = t.signals;
   m_address = t.addr;
   m_data    = t.data;
   m_io      = t.io;
   if (t.op) (this->*t.op)();
}

void 
Z80::fetch() { ++m_reg.PC; }


void 
Z80::memRd(const uint16_t& reg) {
   rstSignal(Signal::MREQ);
   rstSignal(Signal::RD);
   m_address = reg;
}
void Z80::memRdWZ() { memRd(m_reg.WZ); }
void Z80::memRdHL() { memRd(m_reg.main.HL); }
void Z80::memRdBC() { memRd(m_reg.main.BC); }
void Z80::memRdDE() { memRd(m_reg.main.DE); }

void
Z80::data_in(uint8_t& reg) {
   reg = m_data;
}
void Z80::data_in_W()  { data_in(m_reg.W); }
void Z80::data_in_Z()  { data_in(m_reg.Z); }
void Z80::data_in_IR() { data_in(m_reg.IR); }


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
