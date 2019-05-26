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
   // 8-bit registers in their order with respect to opcodes
   m_REGS8 = {{
         &m_reg.main.B, &m_reg.main.C, &m_reg.main.D, &m_reg.main.E
      ,  &m_reg.main.H, &m_reg.main.L,       nullptr, &m_reg.main.A
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
void Z80::data_in_B()            { data_in(m_reg.main.B);   }
void Z80::data_in_C()            { data_in(m_reg.main.C);   }
void Z80::data_in_D()            { data_in(m_reg.main.D);   }
void Z80::data_in_E()            { data_in(m_reg.main.E);   }
void Z80::data_in_H()            { data_in(m_reg.main.H);   }
void Z80::data_in_L()            { data_in(m_reg.main.L);   }
void Z80::data_in_W()            { data_in(m_reg.W);        }
void Z80::data_in_Z()            { data_in(m_reg.Z);        }
void Z80::data_in_IR()           { data_in(m_reg.IR);       }

void 
Z80::exe_LD_r_N(TZ80Op op) {
   auto states = m_MRD;
   states[2].op = op;
   for(auto& t : states) m_ops.add(t);
}

void
Z80::exe_LD_r_r(uint8_t opcode) {
   uint8_t* rd = m_REGS8[ (opcode & 0x38) >> 3 ];
   uint8_t* rs = m_REGS8[ (opcode & 0x07)      ];
   if (!rd) {
      if (!rs) std::cout << "HALT\n";
      else     std::cout << "LD (HL), r\n";
   } else if (!rs) {
      std::cout << "LD r, (HL)\n";
   } else {
      std::cout << "LD r, r\n";
      *rd = *rs;         
   }
}

void 
Z80::exe_LD_sRRs_N(uint16_t& reg) {
   exe_LD_r_N(&Z80::data_in_W);
   auto states = m_MWR;
   states[0].addr = &reg;
   states[0].data = &m_reg.W;
   for(auto& t : states) m_ops.add(t);
}

void 
Z80::decode() {
   std::cout << "Decode: ";

   // Is a 8-bit R-R Load?
   if ( (m_reg.IR & 0xC0) == 0x40) exe_LD_r_r(m_reg.IR);

   switch( m_reg.IR ) {
      case 0x06: exe_LD_r_N(&Z80::data_in_B);  std::cout<<"LD B,n\n";   break;
      case 0x0E: exe_LD_r_N(&Z80::data_in_C);  std::cout<<"LD C,n\n";   break;
      case 0x16: exe_LD_r_N(&Z80::data_in_D);  std::cout<<"LD D,n\n";   break;
      case 0x1E: exe_LD_r_N(&Z80::data_in_E);  std::cout<<"LD E,n\n";   break;
      case 0x26: exe_LD_r_N(&Z80::data_in_H);  std::cout<<"LD H,n\n";   break;
      case 0x2E: exe_LD_r_N(&Z80::data_in_L);  std::cout<<"LD L,n\n";   break;
      case 0x36: exe_LD_sRRs_N(m_reg.main.HL); std::cout<<"LD (HL),n\n";break;
      case 0x3E: exe_LD_r_N(&Z80::data_in_A);  std::cout<<"LD A,n\n";   break;
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
   //out << "Signals |"<< std::bitset<16>((uint16_t)m_signals); // <<< This Allocs!
   out << "Signals:";
   if (! (m_signals & (uint16_t)Signal::MREQ)) out << "|MREQ";
   if (! (m_signals & (uint16_t)Signal::RD))   out << "|RD";
   if (! (m_signals & (uint16_t)Signal::WR))   out << "|WR";
   if (! (m_signals & (uint16_t)Signal::M1))   out << "|M1";
   if (! (m_signals & (uint16_t)Signal::RFSH)) out << "|RFSH";
   out << "|\n";
   out << "Ticks: " << std::dec << m_ticks << "\n";
}

}; // Namespace Z80CPP
