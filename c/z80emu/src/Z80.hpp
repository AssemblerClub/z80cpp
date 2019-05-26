#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>
#include <array>

namespace Z80CPP {

#if __BYTE_ORDER == __LITTLE_ENDIAN 
   #define REGPAIR(NAME, HI, LO) \
      union { uint16_t NAME; \
              struct{ uint8_t LO, HI; }; }
#else
   #define REGPAIR(NAME, HI, LO) \
      union { uint16_t NAME; \
              struct{ uint8_t HI, LO; }; }
#endif

struct Registers {
   struct {
      REGPAIR(AF, A, F);
      REGPAIR(BC, B, C);
      REGPAIR(DE, D, E);
      REGPAIR(HL, H, L);
   } main;
   struct {
      REGPAIR(AF, A, F);
      REGPAIR(BC, B, C);
      REGPAIR(DE, D, E);
      REGPAIR(HL, H, L);
   } alt;
   REGPAIR(IX, IXh, IXl);
   REGPAIR(IY, IYh, IYl);
   REGPAIR(WZ,   W,   Z);
   uint16_t PC, SP;
   uint8_t  I, R, IR;
   Registers() {
      std::memset(this, 0, sizeof(Registers));
   }
};

enum class Signal : uint16_t {
      MREQ  = 0x80
   ,  RD    = 0x40
   ,  WR    = 0x20
   ,  M1    = 0x10
};

class Z80;
struct TState {
   using TOp = void(Z80::*)();
   uint16_t signals = 0;
   uint16_t addr    = 0; 
   uint8_t  data    = 0;
   uint8_t  io      = 0;
   TOp      op      = nullptr;
};

struct TVecOps {
   std::array<TState, 16> ops;
   uint8_t next = 0;
   uint8_t last = 0;
};

class Z80 {
   // Member variables
   Registers   m_reg;            // Register Banks
   uint16_t    m_signals = 0xFF; // Signal pins information
   uint16_t    m_address = 0;    // Address Bus information
   uint8_t     m_data    = 0;    // Data Bus information 
   uint8_t     m_io      = 0;    // I/O Bus information
   uint8_t     m_tstate  = 0;    // T-state of current execution
   uint64_t    m_ticks   = 0;    // Total ticks of operation transcurred
   TVecOps     m_ops;            // Queue of pending operations


   void  fetch();
   void  wait();
   void  decode();
   void  execute();
   void  t (TState& t);

   void  memRd   (const uint16_t& reg);
   void  memRdWZ ();
   void  memRdHL ();
   void  memRdBC ();
   void  memRdDE ();

   void  data_in(uint8_t& reg);
   void  data_in_W();
   void  data_in_Z();
   void  data_in_IR();

   void  LD_r_n();
public:
   Z80();
   void     setData(uint8_t in)  { m_data = in; }
   void     setSignal(Signal s)  { m_signals |=  (uint16_t)s; }
   void     rstSignal(Signal s)  { m_signals &= ~(uint16_t)s; }
   bool     signal(Signal s)     { return m_signals & (uint16_t)s; }
   uint8_t  data()               { return m_data; }
   uint16_t address()            { return m_address; }
   //Registers& reg()              { return m_reg; }

   void tick();
   void print(std::ostream& out) const;
};

}; // Namespace Z80CPP
