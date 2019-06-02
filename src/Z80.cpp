#include <Z80.hpp>

namespace Z80CPP {

void
Z80::process_tstate (const TState& t) {
   m_signals = t.signals | m_in_signals;
   if (t.addr)        m_address = *t.addr;
   if (t.data)        m_data    = *t.data;
   if (t.io)          m_io      = *t.io;
   if (!t.op.empty()) t.op(*this);
}

void Z80::data_in(uint8_t& reg)  { reg = m_data;  }

void 
Z80::exe_JR_n() {
   m_ops.addM23Read(m_reg.PC, m_data, TZ80Op(&Z80::inc, m_reg.PC));
   m_ops.addM3alu(2, TZ80Op(&Z80::assign, m_reg.BUF, m_reg.PC));
   m_ops.addM3alu(1, TZ80Op(&Z80::add,    m_reg.BUF, m_data));
   m_ops.addM3alu(1, TZ80Op(&Z80::assign, m_reg.WZ,  m_reg.BUF));
   m_ops.addM3alu(1, TZ80Op(&Z80::assign, m_reg.PC,  m_reg.WZ));   
}

void 
Z80::exe_LD_r_n(uint8_t& reg) {
   m_ops.addM23Read(m_reg.PC, reg, TZ80Op(&Z80::inc, m_reg.PC));
}

void 
Z80::exe_LD_r_IrpI(uint8_t& rd8, uint16_t& rs16) {
   m_ops.addM23Read(rs16, rd8);
}

void 
Z80::read2BytesFrom(uint8_t& rdhi, uint8_t& rdlo, uint16_t& rs16) {
   m_ops.addM23Read (rs16, rdlo, TZ80Op(&Z80::inc, rs16));
   m_ops.addM23Read (rs16, rdhi, TZ80Op(&Z80::inc, rs16));
}

void 
Z80::exe_LD_InnI_r(uint8_t& rs8) {
   read2BytesFrom   (m_reg.W, m_reg.Z, m_reg.PC);
   m_ops.addM45Write(m_reg.WZ, rs8, TZ80Op(&Z80::inc, m_reg.WZ));
}

void 
Z80::exe_LD_InnI_rp(uint8_t& rhi, uint8_t& rlo) {
   read2BytesFrom   (m_reg.W, m_reg.Z, m_reg.PC);
   m_ops.addM45Write(m_reg.WZ,     rlo, TZ80Op(&Z80::inc, m_reg.WZ));
   m_ops.addM45Write(m_reg.WZ,     rhi, TZ80Op(&Z80::inc, m_reg.WZ));
}

void 
Z80::exe_LD_r_InnI(uint8_t& rd8) {
   read2BytesFrom  (m_reg.W, m_reg.Z, m_reg.PC);
   m_ops.addM23Read(m_reg.WZ,     rd8, TZ80Op(&Z80::inc, m_reg.WZ));
}

void 
Z80::exe_LD_rp_InnI(uint8_t& rhi, uint8_t& rlo) {
   read2BytesFrom  (m_reg.W, m_reg.Z, m_reg.PC);
   read2BytesFrom  (    rhi,     rlo, m_reg.WZ);
}


void
Z80::exe_NOP () {}

void
Z80::exe_HALT() {
   m_nextM1 = &TVecOps::addHALTNOP;
}

void
Z80::exe_LD_r_r(uint8_t& rd, uint8_t& rs) {
   rd = rs;
}

void
Z80::exe_LD_rp_rp (uint16_t& rd, uint16_t& rs) {
   rd = rs;  
}


void 
Z80::exe_LD_IrpI_r(uint16_t& rd16, uint8_t& rs8) {
   m_ops.addM45Write(rd16, rs8);
}

void
Z80::exe_LD_rp_nn(uint8_t& rhi, uint8_t& rlo) {
   read2BytesFrom(rhi, rlo, m_reg.PC);
}

void 
Z80::exe_LD_IrpI_n(uint16_t& reg) {
   exe_LD_r_n   (m_reg.BFl);
   exe_LD_IrpI_r(reg, m_reg.BFl);
}

void
Z80::exe_INC_rp (uint16_t& reg) {
   m_ops.extendM(TZ80Op(&Z80::inc, reg));
   m_ops.extendM();
}

void
Z80::exe_DEC_rp (uint16_t& reg) {
   m_ops.extendM(TZ80Op(&Z80::dec, reg));
   m_ops.extendM();
}

void
Z80::exe_EX_rp_rp (uint16_t& r1, uint16_t& r2) {
   uint16_t tmp = r1;
   r1 = r2; r2 = tmp;
}

void
Z80::exe_EX_ISPI_rp (uint16_t& rd16, uint8_t& rhi, uint8_t& rlo) {
   m_reg.BUF = m_reg.SP + 1; // Should be done with WZ
   m_ops.addM23Read (m_reg.SP , m_reg.Z);
   m_ops.addM23Read (m_reg.BUF, m_reg.W);
   m_ops.extendM();
   m_ops.addM45Write(m_reg.BUF, rhi);
   m_ops.addM45Write(m_reg.SP,  rlo);
   m_ops.extendM( TZ80Op(&Z80::assign, rd16, m_reg.WZ) );
   m_ops.extendM();
}

void
Z80::exe_EXX () {
   exe_EX_rp_rp(m_reg.main.BC, m_reg.alt.BC);
   exe_EX_rp_rp(m_reg.main.DE, m_reg.alt.DE);
   exe_EX_rp_rp(m_reg.main.HL, m_reg.alt.HL);
}

void 
Z80::decode() {
   // Aliases for brevity
   auto&  r     = m_reg;
   auto&  rm    = r.main;
   auto&  ra    = r.alt;

   // Instruction jump table   
   switch( m_data ) {
      // Basics
      case 0x00: exe_NOP       ();            break;
      case 0x08: exe_EX_rp_rp  (rm.AF, ra.AF);break;
      case 0xEB: exe_EX_rp_rp  (rm.DE, rm.HL);break;
      case 0xE3: exe_EX_ISPI_rp(rm.HL, rm.H, rm.L); break;
      case 0xD9: exe_EXX       ();            break;

      // JUMP
      case 0x18: exe_JR_n      ();            break;

      // 0x[0-3]1 [[ LD rp, nn ]]
      case 0x01: exe_LD_rp_nn  (rm.B , rm.C); break;
      case 0x11: exe_LD_rp_nn  (rm.D , rm.E); break;
      case 0x21: exe_LD_rp_nn  (rm.H , rm.L); break;
      case 0x31: exe_LD_rp_nn  ( r.S ,  r.P); break;

      // 0x[0-1]A [[ LD a, (rp) ]]
      case 0x0A: exe_LD_r_IrpI (rm.A ,rm.BC); break;
      case 0x1A: exe_LD_r_IrpI (rm.A ,rm.DE); break;
      // 0x2A [[ LD hl, (nn) ]]
      case 0x2A: exe_LD_rp_InnI(rm.H , rm.L); break;
      // 0x3A [[ LD  a, (nn) ]]
      case 0x3A: exe_LD_r_InnI (rm.A);        break;

      // 0x[0-1]2 [[ LD (rp), r ]] 
      case 0x02: exe_LD_IrpI_r (rm.BC, rm.A); break;
      case 0x12: exe_LD_IrpI_r (rm.DE, rm.A); break;
      // 0x[2-3]2 [[ LD (nn), rp/r ]] 
      case 0x22: exe_LD_InnI_rp(rm.H , rm.L); break;
      case 0x32: exe_LD_InnI_r (rm.A);        break;

      // 0x[0-4]3 [[ inc rp ]] 
      case 0x03: exe_INC_rp (rm.BC); break;
      case 0x13: exe_INC_rp (rm.DE); break;
      case 0x23: exe_INC_rp (rm.HL); break;
      case 0x33: exe_INC_rp ( r.SP); break;

      // 0x[0-4]B [[ dec rp ]] 
      case 0x0B: exe_DEC_rp (rm.BC); break;
      case 0x1B: exe_DEC_rp (rm.DE); break;
      case 0x2B: exe_DEC_rp (rm.HL); break;
      case 0x3B: exe_DEC_rp ( r.SP); break;


      // 0x[0-3][6|E] [[ LD r, n ]] 
      case 0x06: exe_LD_r_n   (rm.B ); break;
      case 0x16: exe_LD_r_n   (rm.D ); break;
      case 0x26: exe_LD_r_n   (rm.H ); break;
      case 0x0E: exe_LD_r_n   (rm.C ); break;
      case 0x1E: exe_LD_r_n   (rm.E ); break;
      case 0x2E: exe_LD_r_n   (rm.L ); break;
      case 0x3E: exe_LD_r_n   (rm.A ); break;
      // 0x[36] [[ LD (HL), n ]] 
      case 0x36: exe_LD_IrpI_n(rm.HL); break;

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

      // 0xF9
      case 0xF9: exe_LD_rp_rp (r.SP, rm.HL); break;
   }
}

void 
Z80::tick() {
   // When there are no machine operations
   // pending, add a new M1 Cycle to fetch and decode
   // next instruction
   if ( m_ops.empty() )
      (m_ops.*m_nextM1)();

   // Now process next T-state in pending operations
   process_tstate( m_ops.get() );
   m_ops.pop( m_signals & (uint16_t)Signal::WAIT );
   ++m_ticks;
}

}; // Namespace Z80CPP
