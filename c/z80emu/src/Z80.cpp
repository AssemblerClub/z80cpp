#include <Z80.hpp>
#include <Printer.hpp>
#include <bitset>

namespace Z80CPP {

// Definition of static const signal groups
const uint16_t Z80::MS_NOSIGNAL   = 0xFFFF;
const uint16_t Z80::MS_MREQ       = (uint16_t)(~((uint16_t)Signal::MREQ));
const uint16_t Z80::MS_MREQ_RD    = (uint16_t)(~((uint16_t)Signal::MREQ | (uint16_t)Signal::RD));
const uint16_t Z80::MS_MREQ_WR    = (uint16_t)(~((uint16_t)Signal::MREQ | (uint16_t)Signal::WR));
const uint16_t Z80::MS_MREQ_RD_M1 = (uint16_t)(Z80::MS_MREQ_RD & ~((uint16_t)Signal::M1));
const uint16_t Z80::MS_MREQ_RFSH  = (uint16_t)(~((uint16_t)Signal::MREQ | (uint16_t)Signal::RFSH));
const uint16_t Z80::MS_RFSH       = (uint16_t)(~((uint16_t)Signal::RFSH));

// Constructor
Z80::Z80() {
   // Default Machine Cycle 1
   m_M1 = {{
         {  .signals = &MS_MREQ_RD_M1, .addr = &m_reg.PC, .op = &Z80::fetch }
      ,  {   }  // Just Wait
      ,  {  .signals = &MS_MREQ_RFSH,  .op= &Z80::data_in_IR   }  // Data IN
      ,  {  .signals = &MS_RFSH,       .op= &Z80::decode       }  // Decode
   }};
   // Default Machine Read Cycle (By default, reads from PC)
   m_MRD = {{
         {  .signals = &MS_MREQ_RD, .addr = &m_reg.PC, .op = &Z80::fetch }
      ,  {   }  // Just Wait
      ,  {  .signals = &MS_NOSIGNAL, .op= &Z80::data_in_W   }  // Data IN
   }};
   // Default Machine Write Cycle (By default, writes to (PC))
   m_MWR = {{
         {  .signals = &MS_MREQ, .addr = &m_reg.PC, .data = &m_data }
      ,  {  .signals = &MS_MREQ_WR     } 
      ,  {  .signals = &MS_NOSIGNAL    } 
   }};
}

void
Z80::process_tstate (const TState& t) {
   if (t.signals) m_signals = *t.signals;
   if (t.addr)    m_address = *t.addr;
   if (t.data)    m_data    = *t.data;
   if (t.io)      m_io      = *t.io;
   if (t.op)      (this->*t.op)();
}

void Z80::fetch()                { ++m_reg.PC;              }
void Z80::data_in(uint8_t& reg)  { reg = m_data;            }
void Z80::data_in_A()            { data_in(m_reg.main.A);   }
void Z80::data_in_W()            { data_in(m_reg.W);        }
void Z80::data_in_Z()            { data_in(m_reg.Z);        }
void Z80::data_in_IR()           { data_in(m_reg.IR);       }


void 
Z80::decode() {
   std::cout << "Decode\n";

   // Check first 2 bits from Opcode
   switch( m_reg.IR ) {
      case 0x3E: 
         auto ldAN = m_MRD;
         ldAN[2].op = &Z80::data_in_A;
         for(auto& t : ldAN) m_ops.add(t);
      break;
   }
}

//|         M1             |      M2           |
//             | REFRESH   |                   |
//| MREQ | W   | DIN | DEC | MREQ | W   | DIN  | 

void
Z80::wait() {
   std::cout << "Waiting for memory\n";
}

void 
Z80::tick() {
   // When there are no machine operations
   // pending, add a new M1 Cycle to fetch and decode
   // next instruction
   if ( m_ops.empty() ) {
      for(auto& t: m_M1)
         m_ops.add(t);
   }

   // Now process next T-state in pending operations
   process_tstate( m_ops.pop() );
   ++m_ticks;
}

//void 
//Z80::tick() {
//   static uint8_t o = 0;
//   using TFun = decltype(&Z80::fetch);
//   const uint8_t numops = 4;
//   const TFun ops[numops] = { &Z80::fetch, &Z80::wait, &Z80::decode, &Z80::execute };
//
//   m_signals = 0xFF;    // Reset Signals
//   (this->*ops[o])();   // Tick
//   o = ++o % numops;    // Select next operation
//   ++m_ticks;           // One tick more
//}

void 
Z80::print(std::ostream& out) const {
   Printer p(out);
   auto pr = [&p](const char*n, uint16_t r) { p.printRegister(n, r); };
   
   out << "------------------------------------\n";
   pr("AF", m_reg.main.AF); pr("AF'", m_reg.alt.AF); out << "   BUS\n";
   pr("BC", m_reg.main.BC); pr("BC'", m_reg.alt.BC); out << "---------\n";
   pr("DE", m_reg.main.DE); pr("DE'", m_reg.alt.DE); pr("ADD", m_address); out << "\n";
   pr("HL", m_reg.main.HL); pr("HL'", m_reg.alt.HL); pr("DAT", m_data);    out << "\n";
   pr("IX",      m_reg.IX); pr("IY ",     m_reg.IY); out << "\n";
   pr("PC",      m_reg.PC); pr("SP ",     m_reg.SP); out << "\n";
   pr(" I",      m_reg.I ); pr(" R ",     m_reg.R ); out << "\n";
   pr("WZ", (m_reg.W << 8) | m_reg.Z);
   pr("IR ",     m_reg.IR); out << "\n";
   out << "Signals |"<< std::bitset<16>((uint16_t)m_signals);
   if (! (m_signals & (uint16_t)Signal::MREQ)) out << "|MREQ";
   if (! (m_signals & (uint16_t)Signal::RD))   out << "|RD";
   if (! (m_signals & (uint16_t)Signal::WR))   out << "|WR";
   if (! (m_signals & (uint16_t)Signal::M1))   out << "|M1";
   if (! (m_signals & (uint16_t)Signal::RFSH)) out << "|RFSH";
   out << "|\n";
   out << "Ticks: " << std::dec << m_ticks << "\n";
}

}; // Namespace Z80CPP
