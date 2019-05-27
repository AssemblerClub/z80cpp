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
   //|         M1             |
   //|   M1 |     | RFSH      |
   //| MREQ | WAIT| DIN | dec |
   //|   RD |     |     |     |
   m_M1 = {{
         {  .signals = &MS_MREQ_RD_M1, .addr = &m_reg.PC, .op = &Z80::fetch }
      ,  {   }  // Just Wait
      ,  {  .signals = &MS_MREQ_RFSH,  .op= &Z80::data_in_IR   }  // Data IN
      ,  {  .signals = &MS_RFSH,       .op= &Z80::decode       }  // Decode
   }};
   // Default Machine Read Cycle (By default, reads from PC)
   //|      M2           |
   //| MREQ | WAIT| DIN  | 
   //|   RD |     |      | 
   m_MRD = {{
         {  .signals = &MS_MREQ_RD, .addr = &m_reg.PC, .op = &Z80::fetch }
      ,  {   }  // Just Wait
      ,  {  .signals = &MS_NOSIGNAL, .op= &Z80::data_in_W   }  // Data IN
   }};
   // Default Machine Write Cycle (By default, writes to (PC))
   //|      M3           |
   //| MREQ | WR  | DIN  | 
   //|     -DOUT-----    |
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
   // 8-bit registers data_in wrapper functions in their order with respect to opcodes
   m_REGS8DATAIN = {{
         &Z80::data_in_B, &Z80::data_in_C, &Z80::data_in_D, &Z80::data_in_E
      ,  &Z80::data_in_H, &Z80::data_in_L,         nullptr, &Z80::data_in_A
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
void Z80::data_in_S()            { data_in(m_reg.S);        }
void Z80::data_in_P()            { data_in(m_reg.P);        }
void Z80::data_in_W()            { data_in(m_reg.W);        }
void Z80::data_in_Z()            { data_in(m_reg.Z);        }
void Z80::data_in_IR()           { data_in(m_reg.IR);       }

void 
Z80::exe_LD_r_n(TZ80Op op) {
   auto states = m_MRD;
   states[2].op = op;
   for(auto& t : states) m_ops.add(t);
}

void 
Z80::exe_LD_r_IrpI(TZ80Op op, uint16_t& rs16) {
   auto states = m_MRD;
   states[0].addr = &rs16;
   states[0].op   = nullptr;
   states[2].op   = op;
   for(auto& t : states) m_ops.add(t);
}

void
Z80::exe_NOP () {
   // std::cout << "NOP\n";
}

void
Z80::exe_HALT() {
   //std::cout << "HALT\n";
   rstSignal(Signal::HALT);
}

void
Z80::exe_LD_r_r(uint8_t& rd, uint8_t& rs) {
   // Simple LD r, r. Just assign registers
   //std::cout << "LD r, r\n";
   rd = rs;
}

void 
Z80::exe_LD_IrpI_r(uint16_t& rd16, uint8_t& rs8) {
   auto states = m_MWR;
   states[0].addr = &rd16;
   states[0].data = &rs8;
   for(auto& t : states) m_ops.add(t);
}

void
Z80::exe_LD_rp_nn(TZ80Op rhi, TZ80Op rlo) {
   auto states = m_MRD;
   states[2].op = rlo;
   for(auto& t : states) m_ops.add(t);
   states[2].op = rhi;
   for(auto& t : states) m_ops.add(t);
}

void 
Z80::exe_LD_IrpI_n(uint16_t& reg) {
   exe_LD_r_n(&Z80::data_in_W);
   exe_LD_IrpI_r(reg, m_reg.W);
}

void 
Z80::decode() {
   // Aliases for brevity
   auto&  r     = m_reg.main;
   auto&& din_B = &Z80::data_in_B;
   auto&& din_C = &Z80::data_in_C;
   auto&& din_D = &Z80::data_in_D;
   auto&& din_E = &Z80::data_in_E;
   auto&& din_H = &Z80::data_in_H;
   auto&& din_L = &Z80::data_in_L;
   auto&& din_A = &Z80::data_in_A;
   auto&& din_S = &Z80::data_in_S;
   auto&& din_P = &Z80::data_in_P;
   
   //std::cout << "Decode: ";
   // Is a 8-bit R-R Load?
   //if ( (m_reg.IR & 0xC0) == 0x40) exe_LD_r_r(m_reg.IR);

   switch( m_reg.IR ) {
      // Basicos
      case 0x00: exe_NOP      ();      break;
      case 0x01: exe_LD_rp_nn (din_B, din_C); break;
      case 0x11: exe_LD_rp_nn (din_D, din_E); break;
      case 0x21: exe_LD_rp_nn (din_H, din_L); break;
      case 0x31: exe_LD_rp_nn (din_S, din_P); break;
      case 0x06: exe_LD_r_n   (din_B); break;
      case 0x0E: exe_LD_r_n   (din_C); break;
      case 0x16: exe_LD_r_n   (din_D); break;
      case 0x1E: exe_LD_r_n   (din_E); break;
      case 0x26: exe_LD_r_n   (din_H); break;
      case 0x2E: exe_LD_r_n   (din_L); break;
      case 0x36: exe_LD_IrpI_n( r.HL); break;
      case 0x3E: exe_LD_r_n   (din_A); break;
      // 0x40-0x47 [[ LD B, r ]]
      case 0x40: exe_LD_r_r   (  r.B, r.B ); break;
      case 0x41: exe_LD_r_r   (  r.B, r.C ); break;
      case 0x42: exe_LD_r_r   (  r.B, r.D ); break;
      case 0x43: exe_LD_r_r   (  r.B, r.E ); break;
      case 0x44: exe_LD_r_r   (  r.B, r.H ); break;
      case 0x45: exe_LD_r_r   (  r.B, r.L ); break;
      case 0x46: exe_LD_r_IrpI(din_B, r.HL); break;
      case 0x47: exe_LD_r_r   (  r.B, r.A ); break;
      // 0x48-0x4F [[ LD C, r ]]
      case 0x48: exe_LD_r_r   (  r.C, r.B ); break;
      case 0x49: exe_LD_r_r   (  r.C, r.C ); break;
      case 0x4A: exe_LD_r_r   (  r.C, r.D ); break;
      case 0x4B: exe_LD_r_r   (  r.C, r.E ); break;
      case 0x4C: exe_LD_r_r   (  r.C, r.H ); break;
      case 0x4D: exe_LD_r_r   (  r.C, r.L ); break;
      case 0x4E: exe_LD_r_IrpI(din_C, r.HL); break;
      case 0x4F: exe_LD_r_r   (  r.C, r.A ); break;
      // 0x50-0x57 [[ LD D, r ]]
      case 0x50: exe_LD_r_r   (  r.D, r.B ); break;
      case 0x51: exe_LD_r_r   (  r.D, r.C ); break;
      case 0x52: exe_LD_r_r   (  r.D, r.D ); break;
      case 0x53: exe_LD_r_r   (  r.D, r.E ); break;
      case 0x54: exe_LD_r_r   (  r.D, r.H ); break;
      case 0x55: exe_LD_r_r   (  r.D, r.L ); break;
      case 0x56: exe_LD_r_IrpI(din_D, r.HL); break;
      case 0x57: exe_LD_r_r   (  r.D, r.A ); break;
      // 0x50-0x57 [[ LD E, r ]]
      case 0x58: exe_LD_r_r   (  r.E, r.B ); break;
      case 0x59: exe_LD_r_r   (  r.E, r.C ); break;
      case 0x5A: exe_LD_r_r   (  r.E, r.D ); break;
      case 0x5B: exe_LD_r_r   (  r.E, r.E ); break;
      case 0x5C: exe_LD_r_r   (  r.E, r.H ); break;
      case 0x5D: exe_LD_r_r   (  r.E, r.L ); break;
      case 0x5E: exe_LD_r_IrpI(din_E, r.HL); break;
      case 0x5F: exe_LD_r_r   (  r.E, r.A ); break;
      // 0x60-0x67 [[ LD H, r ]]
      case 0x60: exe_LD_r_r   (  r.H, r.B ); break;
      case 0x61: exe_LD_r_r   (  r.H, r.C ); break;
      case 0x62: exe_LD_r_r   (  r.H, r.D ); break;
      case 0x63: exe_LD_r_r   (  r.H, r.E ); break;
      case 0x64: exe_LD_r_r   (  r.H, r.H ); break;
      case 0x65: exe_LD_r_r   (  r.H, r.L ); break;
      case 0x66: exe_LD_r_IrpI(din_H, r.HL); break;
      case 0x67: exe_LD_r_r   (  r.H, r.A ); break;
      // 0x68-0x6F [[ LD L, r ]]
      case 0x68: exe_LD_r_r   (  r.L, r.B ); break;
      case 0x69: exe_LD_r_r   (  r.L, r.C ); break;
      case 0x6A: exe_LD_r_r   (  r.L, r.D ); break;
      case 0x6B: exe_LD_r_r   (  r.L, r.E ); break;
      case 0x6C: exe_LD_r_r   (  r.L, r.H ); break;
      case 0x6D: exe_LD_r_r   (  r.L, r.L ); break;
      case 0x6E: exe_LD_r_IrpI(din_L, r.HL); break;
      case 0x6F: exe_LD_r_r   (  r.L, r.A ); break;
      // 0x70-0x77 [[ LD (HL), r ]]
      case 0x70: exe_LD_IrpI_r( r.HL, r.B ); break;
      case 0x71: exe_LD_IrpI_r( r.HL, r.C ); break;
      case 0x72: exe_LD_IrpI_r( r.HL, r.D ); break;
      case 0x73: exe_LD_IrpI_r( r.HL, r.E ); break;
      case 0x74: exe_LD_IrpI_r( r.HL, r.H ); break;
      case 0x75: exe_LD_IrpI_r( r.HL, r.L ); break;
      case 0x76: exe_HALT();                 break;
      case 0x77: exe_LD_IrpI_r( r.HL, r.A ); break;
      // 0x78-0x7F [[ LD A, r ]]
      case 0x78: exe_LD_r_r   (  r.A, r.B ); break;
      case 0x79: exe_LD_r_r   (  r.A, r.C ); break;
      case 0x7A: exe_LD_r_r   (  r.A, r.D ); break;
      case 0x7B: exe_LD_r_r   (  r.A, r.E ); break;
      case 0x7C: exe_LD_r_r   (  r.A, r.H ); break;
      case 0x7D: exe_LD_r_r   (  r.A, r.L ); break;
      case 0x7E: exe_LD_r_IrpI(din_A, r.HL); break;
      case 0x7F: exe_LD_r_r   (  r.A, r.A ); break;
   }
}

void
Z80::wait() {
//   std::cout << "Waiting for memory\n";
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
   if (! (m_signals & (uint16_t)Signal::M1))   out << "|M1";
   if (! (m_signals & (uint16_t)Signal::MREQ)) out << "|MREQ";
   if (! (m_signals & (uint16_t)Signal::IORQ)) out << "|IORQ";
   if (! (m_signals & (uint16_t)Signal::RD))   out << "|RD";
   if (! (m_signals & (uint16_t)Signal::WR))   out << "|WR";
   if (! (m_signals & (uint16_t)Signal::RFSH)) out << "|RFSH";
   if (! (m_signals & (uint16_t)Signal::HALT)) out << "|HALT";
   if (! (m_signals & (uint16_t)Signal::WAIT)) out << "|WAIT";
   out << "|\n";
   out << "Ticks: " << std::dec << m_ticks << "\n";
}

}; // Namespace Z80CPP
