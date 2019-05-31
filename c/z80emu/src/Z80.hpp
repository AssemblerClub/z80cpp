#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>
#include <Z80_tqueue.hpp>

namespace Z80CPP {

#if __BYTE_ORDER == __LITTLE_ENDIAN 
   #define REGPAIR(NAME, HI, LO) \
      union { uint16_t NAME; \
              struct { uint8_t LO, HI; }; }
#else
   #define REGPAIR(NAME, HI, LO) \
      union { uint16_t NAME; \
              struct{ uint8_t HI, LO; }; }
#endif

//
// Declares a struct with all Z80 Registers
//
struct Registers {
   struct MainRegs {
      REGPAIR(AF, A, F);
      REGPAIR(BC, B, C);
      REGPAIR(DE, D, E);
      REGPAIR(HL, H, L);
   } main;
   struct AltRegs {
      REGPAIR(AF, A, F);
      REGPAIR(BC, B, C);
      REGPAIR(DE, D, E);
      REGPAIR(HL, H, L);
   } alt;
   REGPAIR(IX, IXh, IXl);
   REGPAIR(IY, IYh, IYl);
   REGPAIR(WZ,   W,   Z);
   REGPAIR(SP,   S,   P);
   uint16_t PC;
   uint8_t  I, R, IR;
   Registers() { 
      // Set all register memory to 0 at once
      std::memset(this, 0, sizeof(Registers));
   }
};

//
// Signal: Defines an individual Z80 signal 
//  (the state of one of the pins)
//
enum class Signal : uint16_t {
      M1    = 0x8000
   ,  MREQ  = 0x4000
   ,  IORQ  = 0x2000
   ,  RD    = 0x1000
   ,  WR    = 0x0800
   ,  RFSH  = 0x0400
   ,  HALT  = 0x0200
   ,  WAIT  = 0x0100
};


//
// Z80 CPU Class Declaration
//
class Z80 {
   // Member variables
   Registers  m_reg;             // Register Banks
   uint16_t   m_signals = 0xFFFF;// Signal pins information
   uint16_t   m_address = 0;     // Address Bus information
   uint8_t    m_data    = 0;     // Data Bus information 
   uint8_t    m_io      = 0;     // I/O Bus information
   uint64_t   m_ticks   = 0;     // Total ticks of operation transcurred
   TVecOps    m_ops;             // Queue of pending operations

   // Private member functions

   // Z80 T-state processing operations
   void  process_tstate (const TState& t);

   // Z80 Instructions (Execution)
   void  exe_NOP      (); 
   void  exe_HALT     ();
   void  exe_LD_r_r   (uint8_t& rd, uint8_t& rs);
   void  exe_LD_r_n   (uint8_t& reg);
   void  exe_LD_IrpI_n(uint16_t& reg);
   void  exe_LD_IrpI_r(uint16_t& rd16, uint8_t& rs8);
   void  exe_LD_r_IrpI(uint8_t& rd8, uint16_t& rs16);
   void  exe_LD_rp_nn (uint8_t& rhi, uint8_t& rlo);
public:
   Z80();
   void     setData(uint8_t in)  { m_data = in; }
   void     setSignal(Signal s)  { m_signals |=  (uint16_t)s; }
   void     rstSignal(Signal s)  { m_signals &= ~(uint16_t)s; }
   bool     signal(Signal s)     { return m_signals & (uint16_t)s; }
   uint8_t  data()               { return m_data; }
   uint16_t address()            { return m_address; }
   uint64_t ticks()              { return m_ticks; }
   void     setPC(uint16_t pc)   { m_reg.PC = pc;  }
   uint16_t pc()                 { return m_reg.PC;  }
   //Registers& reg()              { return m_reg; }

   // Processing operations
   void  fetch();
   void  decode();
   void  data_in(uint8_t& reg);

   void tick();
   void print(std::ostream& out) const;
};

}; // Namespace Z80CPP
