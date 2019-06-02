#pragma once

#include <cstdint>
#include <iostream>
#include <Z80_tqueue.hpp>

namespace Z80CPP {

//
// Z80 CPU Class Declaration
//
class Z80 {
   using FNextM1 = void(TVecOps::*)();

   // Member variables
   uint16_t   m_signals    = 0;  // Signal pins information (Positive logic (1=ON))
   uint16_t   m_in_signals = 0;  // Only input signals (Positive logic (1=ON))
   uint16_t   m_address = 0;     // Address Bus information
   uint8_t    m_data    = 0;     // Data Bus information 
   uint8_t    m_io      = 0;     // I/O Bus information
   uint64_t   m_ticks   = 0;     // Total ticks of operation transcurred
   Registers  m_reg;             // Register Banks
   TVecOps    m_ops = TVecOps(m_reg);     // Queue of pending operations
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
public:
   Z80() = default;
   void     setData(uint8_t in)  { m_data = in; }
   void     setSignal(Signal s)  { m_in_signals |=  (uint16_t)s; }
   void     rstSignal(Signal s)  { m_in_signals &= ~(uint16_t)s; }
   bool     signal(Signal s)     { return m_signals & (uint16_t)s; }
   uint8_t  data()               { return m_data; }
   uint16_t address()            { return m_address; }
   uint64_t ticks()              { return m_ticks; }
   void     setPC(uint16_t pc)   { m_reg.PC = pc;  }
   uint16_t pc()                 { return m_reg.PC;  }
   //Registers& reg()              { return m_reg; }

   // Processing operations
   void  decode();
   void  inc7(uint8_t& reg)      { reg = (reg+1) & 0x7F; }
   void  inc(uint8_t& reg)       { ++reg; }
   void  inc(uint16_t& reg)      { ++reg; }
   void  dec(uint16_t& reg)      { --reg; }
   void  assign(uint16_t& rd, uint16_t& rs) { rd = rs; }
   void  data_in(uint8_t& reg);

   void tick();
   void print(std::ostream& out) const;
};

}; // Namespace Z80CPP
