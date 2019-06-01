#include <Z80.hpp>
#include <Printer.hpp>

namespace Z80CPP {

void
Z80::process_tstate (const TState& t) {
   m_signals = t.signals;
   if (t.addr)        m_address = *t.addr;
   if (t.data)        m_data    = *t.data;
   if (t.io)          m_io      = *t.io;
   if (!t.op.empty()) t.op(*this);
}

void Z80::data_in(uint8_t& reg)  { reg = m_data;  }

void 
Z80::exe_LD_r_n(uint8_t& reg) {
   m_ops.addM23Read(m_reg.PC, reg, TZ80Op(&Z80::inc, m_reg.PC));
}

void 
Z80::exe_LD_r_IrpI(uint8_t& rd8, uint16_t& rs16) {
   m_ops.addM23Read(rs16, rd8, TZ80Op());
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
   m_ops.addM4Write(rd16, rs8);
}

void
Z80::exe_LD_rp_nn(uint8_t& rhi, uint8_t& rlo) {
   m_ops.addM23Read(m_reg.PC, rlo, TZ80Op(&Z80::inc, m_reg.PC));
   m_ops.addM23Read(m_reg.PC, rhi, TZ80Op(&Z80::inc, m_reg.PC));
}

void 
Z80::exe_LD_IrpI_n(uint16_t& reg) {
   exe_LD_r_n   (m_reg.W);
   exe_LD_IrpI_r(reg, m_reg.W);
}

void 
Z80::decode() {
   // Aliases for brevity
   auto&  r     = m_reg;
   auto&  rm    = r.main;

   // Instruction jump table   
   switch( m_data ) {
   //switch( m_reg.IR ) {
      // Basicos
      case 0x00: exe_NOP      ();      break;
      case 0x01: exe_LD_rp_nn (rm.B, rm.C); break;
      case 0x11: exe_LD_rp_nn (rm.D, rm.E); break;
      case 0x21: exe_LD_rp_nn (rm.H, rm.L); break;
      case 0x31: exe_LD_rp_nn ( r.S,  r.P); break;
      case 0x06: exe_LD_r_n   (rm.B ); break;
      case 0x0E: exe_LD_r_n   (rm.C ); break;
      case 0x16: exe_LD_r_n   (rm.D ); break;
      case 0x1E: exe_LD_r_n   (rm.E ); break;
      case 0x26: exe_LD_r_n   (rm.H ); break;
      case 0x2E: exe_LD_r_n   (rm.L ); break;
      case 0x36: exe_LD_IrpI_n(rm.HL); break;
      case 0x3E: exe_LD_r_n   (rm.A ); break;
      // 0x40-0x47 [[ LD B, r ]]
      case 0x40: exe_LD_r_r   (rm.B, rm.B ); break;
      case 0x41: exe_LD_r_r   (rm.B, rm.C ); break;
      case 0x42: exe_LD_r_r   (rm.B, rm.D ); break;
      case 0x43: exe_LD_r_r   (rm.B, rm.E ); break;
      case 0x44: exe_LD_r_r   (rm.B, rm.H ); break;
      case 0x45: exe_LD_r_r   (rm.B, rm.L ); break;
      case 0x46: exe_LD_r_IrpI(rm.B, rm.HL); break;
      case 0x47: exe_LD_r_r   (rm.B, rm.A ); break;
      // 0x48-0x4F [[ LD C, r ]]
      case 0x48: exe_LD_r_r   (rm.C, rm.B ); break;
      case 0x49: exe_LD_r_r   (rm.C, rm.C ); break;
      case 0x4A: exe_LD_r_r   (rm.C, rm.D ); break;
      case 0x4B: exe_LD_r_r   (rm.C, rm.E ); break;
      case 0x4C: exe_LD_r_r   (rm.C, rm.H ); break;
      case 0x4D: exe_LD_r_r   (rm.C, rm.L ); break;
      case 0x4E: exe_LD_r_IrpI(rm.C, rm.HL); break;
      case 0x4F: exe_LD_r_r   (rm.C, rm.A ); break;
      // 0x50-0x57 [[ LD D, r ]]
      case 0x50: exe_LD_r_r   (rm.D, rm.B ); break;
      case 0x51: exe_LD_r_r   (rm.D, rm.C ); break;
      case 0x52: exe_LD_r_r   (rm.D, rm.D ); break;
      case 0x53: exe_LD_r_r   (rm.D, rm.E ); break;
      case 0x54: exe_LD_r_r   (rm.D, rm.H ); break;
      case 0x55: exe_LD_r_r   (rm.D, rm.L ); break;
      case 0x56: exe_LD_r_IrpI(rm.D, rm.HL); break;
      case 0x57: exe_LD_r_r   (rm.D, rm.A ); break;
      // 0x50-0x57 [[ LD E, r ]]
      case 0x58: exe_LD_r_r   (rm.E, rm.B ); break;
      case 0x59: exe_LD_r_r   (rm.E, rm.C ); break;
      case 0x5A: exe_LD_r_r   (rm.E, rm.D ); break;
      case 0x5B: exe_LD_r_r   (rm.E, rm.E ); break;
      case 0x5C: exe_LD_r_r   (rm.E, rm.H ); break;
      case 0x5D: exe_LD_r_r   (rm.E, rm.L ); break;
      case 0x5E: exe_LD_r_IrpI(rm.E, rm.HL); break;
      case 0x5F: exe_LD_r_r   (rm.E, rm.A ); break;
      // 0x60-0x67 [[ LD H, r ]]
      case 0x60: exe_LD_r_r   (rm.H, rm.B ); break;
      case 0x61: exe_LD_r_r   (rm.H, rm.C ); break;
      case 0x62: exe_LD_r_r   (rm.H, rm.D ); break;
      case 0x63: exe_LD_r_r   (rm.H, rm.E ); break;
      case 0x64: exe_LD_r_r   (rm.H, rm.H ); break;
      case 0x65: exe_LD_r_r   (rm.H, rm.L ); break;
      case 0x66: exe_LD_r_IrpI(rm.H, rm.HL); break;
      case 0x67: exe_LD_r_r   (rm.H, rm.A ); break;
      // 0x68-0x6F [[ LD L, r ]]
      case 0x68: exe_LD_r_r   (rm.L, rm.B ); break;
      case 0x69: exe_LD_r_r   (rm.L, rm.C ); break;
      case 0x6A: exe_LD_r_r   (rm.L, rm.D ); break;
      case 0x6B: exe_LD_r_r   (rm.L, rm.E ); break;
      case 0x6C: exe_LD_r_r   (rm.L, rm.H ); break;
      case 0x6D: exe_LD_r_r   (rm.L, rm.L ); break;
      case 0x6E: exe_LD_r_IrpI(rm.L, rm.HL); break;
      case 0x6F: exe_LD_r_r   (rm.L, rm.A ); break;
      // 0x70-0x77 [[ LD (HL), r ]]
      case 0x70: exe_LD_IrpI_r(rm.HL, rm.B ); break;
      case 0x71: exe_LD_IrpI_r(rm.HL, rm.C ); break;
      case 0x72: exe_LD_IrpI_r(rm.HL, rm.D ); break;
      case 0x73: exe_LD_IrpI_r(rm.HL, rm.E ); break;
      case 0x74: exe_LD_IrpI_r(rm.HL, rm.H ); break;
      case 0x75: exe_LD_IrpI_r(rm.HL, rm.L ); break;
      case 0x76: exe_HALT();                 break;
      case 0x77: exe_LD_IrpI_r(rm.HL, rm.A ); break;
      // 0x78-0x7F [[ LD A, r ]]
      case 0x78: exe_LD_r_r   (rm.A, rm.B ); break;
      case 0x79: exe_LD_r_r   (rm.A, rm.C ); break;
      case 0x7A: exe_LD_r_r   (rm.A, rm.D ); break;
      case 0x7B: exe_LD_r_r   (rm.A, rm.E ); break;
      case 0x7C: exe_LD_r_r   (rm.A, rm.H ); break;
      case 0x7D: exe_LD_r_r   (rm.A, rm.L ); break;
      case 0x7E: exe_LD_r_IrpI(rm.A, rm.HL); break;
      case 0x7F: exe_LD_r_r   (rm.A, rm.A ); break;
   }
}

void 
Z80::tick() {
   // When there are no machine operations
   // pending, add a new M1 Cycle to fetch and decode
   // next instruction
   if ( m_ops.empty() )
      m_ops.addM1();

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
   pr("IR",      m_reg.IR); pr("WZ ",     m_reg.WZ); out << "\n";
   out << "Signals:(" << m_signals << "):";
   if ( m_signals & (uint16_t)Signal::M1   ) out << "|M1";
   if ( m_signals & (uint16_t)Signal::MREQ ) out << "|MREQ";
   if ( m_signals & (uint16_t)Signal::IORQ ) out << "|IORQ";
   if ( m_signals & (uint16_t)Signal::RD   ) out << "|RD";
   if ( m_signals & (uint16_t)Signal::WR   ) out << "|WR";
   if ( m_signals & (uint16_t)Signal::RFSH ) out << "|RFSH";
   if ( m_signals & (uint16_t)Signal::HALT ) out << "|HALT";
   if ( m_signals & (uint16_t)Signal::WAIT ) out << "|WAIT";
   out << "|\n";
   out << "Ticks: " << std::dec << m_ticks << "\n";
}

}; // Namespace Z80CPP
