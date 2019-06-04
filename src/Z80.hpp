#pragma once

#include <cstdint>
#include <iostream>
#include <Z80_tqueue.hpp>

namespace Z80CPP {

// Foward declares
class Printer;

//
// Z80 CPU Class Declaration
//
class Z80 {
   friend class TVecOps;
   using FNextM1 = void(TVecOps::*)();

   // Member variables
   uint16_t   m_signals    = 0;  // Signal pins information (Positive logic (1=ON))
   uint16_t   m_in_signals = 0;  // Only input signals (Positive logic (1=ON))
   uint16_t   m_address = 0;     // Address Bus information
   uint8_t    m_data    = 0;     // Data Bus information 
   uint64_t   m_ticks   = 0;     // Total ticks of operation transcurred
   Registers  m_reg;             // Register Banks
   TVecOps    m_ops = TVecOps(*this);     // Queue of pending operations
   FNextM1    m_nextM1 = &TVecOps::addM1; // Next M1 Cycle operation to perform (for halt situations)

   // Private member functions

   // Z80 T-state processing operations
   void  process_tstate (const TState& t);

   void  read2BytesFrom(uint8_t& rdhi, uint8_t& rdlo, uint16_t& rs16);

   // Z80 Instructions (Execution)

   // Basic
   void  exe_NOP       (); 
   void  exe_HALT      ();
   void  exe_EX_rp_rp  (uint16_t& r1, uint16_t& r2);
   void  exe_EXX       ();
   void  exe_EX_ISPI_rp(uint16_t& rd16, uint8_t& rhi, uint8_t& rlo);
   // Load
   void  exe_LD_r_r    (uint8_t& rd, uint8_t& rs);
   void  exe_LD_rp_rp  (uint16_t& rd, uint16_t& rs);
   void  exe_LD_r_n    (uint8_t& reg);
   void  exe_LD_r_InnI (uint8_t& rd8);
   void  exe_LD_r_IrpI (uint8_t& rd8, uint16_t& rs16);
   void  exe_LD_rp_nn  (uint8_t& rhi, uint8_t& rlo);
   void  exe_LD_rp_InnI(uint8_t& rhi, uint8_t& rlo);
   void  exe_LD_IrpI_n (uint16_t& reg);
   void  exe_LD_IrpI_r (uint16_t& rd16, uint8_t& rs8);
   void  exe_LD_InnI_r (uint8_t& rs8);
   void  exe_LD_InnI_rp(uint8_t& rhi, uint8_t& rlo);

   // INC/DEC
   void  exe_INC_rp (uint16_t& reg);
   void  exe_DEC_rp (uint16_t& reg);

   // JUMP
   void  exe_JR_n ();
   void  exe_JP_nn();
   void  exe_JP_IrpI(uint16_t& reg);

   // Private API for friend class
   Registers& registers_r() { return m_reg;     }
   uint8_t&   data_r()      { return m_data;    }
   uint16_t&  address_r()   { return m_address; }
public:
   Z80() = default;
   void     setData(uint8_t in)     { m_data = in; }
   void     setSignal(Signal s)     { m_in_signals |=  (uint16_t)s; }
   void     rstSignal(Signal s)     { m_in_signals &= ~(uint16_t)s; }
   bool     signal(Signal s) const  { return m_signals & (uint16_t)s; }
   uint16_t signals() const         { return (uint16_t)m_signals; }
   uint8_t  data() const            { return m_data; }
   uint16_t address() const         { return m_address; }
   uint64_t ticks() const           { return m_ticks; }
   void     setPC(uint16_t pc)      { m_reg.PC = pc;  }
   uint16_t pc() const              { return m_reg.PC;  }
   const Registers& registers() const { return m_reg; }

   // Processing operations
   void  decode();
   void  inc7(uint8_t& reg)      { reg = (reg+1) & 0x7F; }
   void  inc(uint8_t& reg)       { ++reg; }
   void  inc(uint16_t& reg)      { ++reg; }
   void  dec(uint16_t& reg)      { --reg; }
   void  assign(uint8_t& rd, uint8_t& rs)   { rd = rs; }
   void  assign(uint16_t& rd, uint16_t& rs) { rd = rs; }
   void  data_in(uint8_t& reg)   { reg = m_data;  }
   void  data_in_assign(uint8_t& rin, uint16_t& rd, uint16_t& rs) { data_in(rin); assign(rd, rs); }
   void  add(uint16_t& reg, uint8_t& offset){ reg += (int8_t)offset; }

   void  tick();
};

} // Namespace Z80CPP
