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
// TZ80Op: Encapsulates an operation (member function) to be performed 
// (called) on a given TState. The operation is stored in a union to
// save space, along with its parameters (if it has). m_type contains
// the type of operation being contained, which lets us use the appropriate
// bytes of the union and call a valid member function.
//
class Z80;
struct TZ80Op {
   // Classes of functions that this object manages
   enum class Type {
         NOP
      ,  VOID
      ,  _1PU8Ref
   };
   // Type aliases for function pointers
   using VOIDFp      = void(Z80::*)(void);
   using _1PU8RefFp  = void(Z80::*)(uint8_t&);

   // Explicit constructors for functions types
   explicit TZ80Op()                : m_type(Type::NOP) {} 
   explicit TZ80Op( VOIDFp f )      : m_type(Type::VOID), m_fv(f) {}
   explicit TZ80Op( _1PU8RefFp f, uint8_t& p ) 
      : m_type(Type::_1PU8Ref), m_f1u8r(f), m_f1u8r_p(&p) {}

   // Set new function and type
   void reset() {
      m_type = Type::NOP;
   }
   void set( VOIDFp     f ) { 
      m_type = Type::VOID;
      m_fv = f; 
   }
   void set( _1PU8RefFp f, uint8_t& p0 ) { 
      m_type = Type::_1PU8Ref;
      m_f1u8r = f;
      m_f1u8r_p = &p0;
   }

   // Call contained function
   void operator()(Z80& cpu) const {
      // Produce member function call depending on m_type
      switch(m_type) {
         case Type::VOID:     (cpu.*m_fv)(); break;
         case Type::_1PU8Ref: (cpu.*m_f1u8r)(*m_f1u8r_p); break;
         case Type::NOP: break;         
      }
   }

   bool empty() const { return m_type == Type::NOP; }

private:
   Type m_type;     //< Type of operation
   union {
      // Type 0: Void function, no parameters
      struct { VOIDFp m_fv; };
      // Type 1: Void function, 1 8-bits parameter by reference
      struct {
         _1PU8RefFp  m_f1u8r;
         uint8_t*    m_f1u8r_p;
      };
   };
};


//
// TState: Status of the Z80 after a given TState
//   It defines how pinouts should be, and an operation
//   to be performed
//
struct TState {
   const uint16_t* signals = nullptr;
   const uint16_t* addr    = nullptr; 
   const uint8_t*  data    = nullptr;
   const uint8_t*  io      = nullptr;
          TZ80Op   op      = TZ80Op();
   TState() = default;
   TState(const uint16_t* s, const uint16_t* a, const uint8_t* d, const uint8_t* i, TZ80Op&& o)
      : signals(s), addr(a), data(d), io(i), op(std::move(o)) {}
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

   // Z80 T-state processing operations
   void  process_tstate (const TState& t);
   void  fetch();
   void  decode();
   void  data_in(uint8_t& reg);

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

   void tick();
   void print(std::ostream& out) const;
};

}; // Namespace Z80CPP
