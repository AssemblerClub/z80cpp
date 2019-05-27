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

//
// Declares a struct with all Z80 Registers
//
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
// TState: Status of the Z80 after a given TState
//   It defines how pinouts should be, and an operation
//   to be performed
//
class Z80;
using TZ80Op = void(Z80::*)();
struct TState {
   const uint16_t* signals = nullptr;
   const uint16_t* addr    = nullptr; 
   const uint8_t*  data    = nullptr;
   const uint8_t*  io      = nullptr;
          TZ80Op   op      = nullptr;
};

//
// TVecOps: Class for encapsulating a fixed-length 
// vector of pending operations (t-states) to perform
//
class TVecOps {
   static constexpr uint8_t length = 16;
   std::array<TState, length> ops;
   uint8_t next = 0;
   uint8_t last = 0;
   
   inline void inc(uint8_t& v) { if(++v == length) v = 0;         }
public:
   void add(TState& newop) { ops[last] = newop; inc(last);        }
   const TState&  get()    { return ops[next];                    }
   const TState&  pop()    { auto& t=get(); inc(next); return t;  }
   bool           empty()  { return next == last;                 }
};

//
// Z80 CPU Class Declaration
//
class Z80 {
   // Type aliases
   using TStateArr4 = std::array<TState, 4>;
   using TStateArr3 = std::array<TState, 3>;

   // Member variables
   Registers  m_reg;             // Register Banks
   uint16_t   m_signals = 0xFFFF;// Signal pins information
   uint16_t   m_address = 0;     // Address Bus information
   uint8_t    m_data    = 0;     // Data Bus information 
   uint8_t    m_io      = 0;     // I/O Bus information
   uint64_t   m_ticks   = 0;     // Total ticks of operation transcurred
   TVecOps    m_ops;             // Queue of pending operations
   TStateArr4 m_M1;              // Array containing the 4 states of default M1 Machine cycle
   TStateArr3 m_MRD, m_MWR;      // Array containing the 3 states of default MREAD/MWRITE Machine cycle
   std::array<uint8_t*, 8> m_REGS8;    // 8-bit Register order with respect to Opcode bits
   std::array<TZ80Op, 8> m_REGS8DATAIN;// 8-bit Register order (wrapper data_in functions) with respect to Opcode bits   

   // Member constants

   // Signal groups
   static const uint16_t MS_NOSIGNAL;
   static const uint16_t MS_MREQ;
   static const uint16_t MS_MREQ_RD;
   static const uint16_t MS_MREQ_WR;
   static const uint16_t MS_MREQ_RD_M1;
   static const uint16_t MS_MREQ_RFSH;
   static const uint16_t MS_RFSH;

   
   // Private member functions

   void  fetch();
   void  wait();
   void  decode();
   void  process_tstate (const TState& t);
   void  data_in(uint8_t& reg);
   void  data_in_A();
   void  data_in_B();
   void  data_in_C();
   void  data_in_D();
   void  data_in_E();
   void  data_in_H();
   void  data_in_L();
   void  data_in_W();
   void  data_in_Z();
   void  data_in_IR();

   void  exe_LD_r_r   (uint8_t opcode);
   void  exe_LD_r_N   (TZ80Op op);
   void  exe_LD_sRRs_N(uint16_t& reg);
   void  exe_LD_sRRs_r(uint16_t& rd16, uint8_t& rs8);
   void  exe_LD_r_sRRs(TZ80Op op, uint16_t& rs16);

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

   void tick();
   void print(std::ostream& out) const;
};

}; // Namespace Z80CPP
