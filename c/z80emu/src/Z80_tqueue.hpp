#pragma once
#include <array>
#include <cstring>

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
   REGPAIR(IR,   I,   R);
   uint16_t PC;
   Registers() { 
      // Set all register memory to 0 at once
      std::memset(this, 0, sizeof(Registers));
   }
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
      ,  _1PU16Ref
   };
   // Type aliases for function pointers
   using VOIDFp      = void(Z80::*)(void);
   using _1PU8RefFp  = void(Z80::*)(uint8_t&);
   using _1PU16RefFp = void(Z80::*)(uint16_t&);

   // Explicit constructors for functions types
   explicit TZ80Op()                : m_type(Type::NOP) {} 
   explicit TZ80Op( VOIDFp f )      : m_type(Type::VOID), m_fv(f) {}
   explicit TZ80Op( _1PU8RefFp f, uint8_t& p ) 
      : m_type(Type::_1PU8Ref), m_f1u8r(f), m_f1u8r_p(&p) {}
   explicit TZ80Op( _1PU16RefFp f, uint16_t& p ) 
      : m_type(Type::_1PU16Ref), m_f1u16r(f), m_f1u16r_p(&p) {}

   // Set new function and type
   void reset() {
      m_type = Type::NOP;
   }
   void set( VOIDFp     f ) { 
      m_type = Type::VOID;
      m_fv = f; 
   }
   void set( _1PU8RefFp f, uint8_t& p ) { 
      m_type = Type::_1PU8Ref;
      m_f1u8r = f;
      m_f1u8r_p = &p;
   }
   void set( _1PU16RefFp f, uint16_t& p ) { 
      m_type = Type::_1PU16Ref;
      m_f1u16r = f;
      m_f1u16r_p = &p;
   }

   // Call contained function
   void operator()(Z80& cpu) const {
      // Produce member function call depending on m_type
      switch(m_type) {
         case Type::VOID:      (cpu.*m_fv    )(); break;
         case Type::_1PU8Ref:  (cpu.*m_f1u8r )(*m_f1u8r_p ); break;
         case Type::_1PU16Ref: (cpu.*m_f1u16r)(*m_f1u16r_p); break;
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
      // Type 2: Void function, 1 16-bits parameter by reference
      struct {
         _1PU16RefFp  m_f1u16r;
         uint16_t*    m_f1u16r_p;
      };
   };
};

//
// Signal: Defines an individual Z80 signal 
//  (the state of one of the pins)
//
enum class Signal : uint16_t {
      M1    = 0x4000
   ,  MREQ  = 0x2000
   ,  IORQ  = 0x1000
   ,  RD    = 0x0800
   ,  WR    = 0x0400
   ,  RFSH  = 0x0200
   ,  HALT  = 0x0100
   ,  WAIT  = 0x0080
};

//
// TState: Status of the Z80 after a given TState
//   It defines how pinouts should be, and an operation
//   to be performed
// WARNING: 
//    Z80 uses negative logic for all in/out signals (0 = ON), except
// data and address buses. However, this codifies signals as positive
// logic (1 = ON) for clarity.
//
struct TState {
         uint16_t  signals = 0;
   const uint16_t* addr    = nullptr; 
   const uint8_t*  data    = nullptr;
   const uint8_t*  io      = nullptr;
          TZ80Op   op      = TZ80Op();
   TState() = default;
   TState(uint16_t s, const uint16_t* a, const uint8_t* d, const uint8_t* i, TZ80Op&& o)
      : signals(s), addr(a), data(d), io(i), op(std::move(o)) {}
   void set(uint16_t s, const uint16_t* a, const uint8_t* d, const uint8_t* i, TZ80Op&& o) {
      signals = s; addr = a; data = d;
      io = i; op = std::move(o);
   }
};

//
// TVecOps: Class for encapsulating a fixed-length 
// vector of pending operations (t-states) to perform
//
class TVecOps {
   static constexpr uint8_t length = 24;
   std::array<TState, length> ops;
   uint8_t next = 0;
   uint8_t last = 0;
   Registers& cpureg;
   
   inline void inc(uint8_t& v) { if(++v == length) v = 0;         }
public:
   TVecOps(Registers& cr) : cpureg(cr) {}
   void addM1();
   void addM23Read(uint16_t& addr, uint8_t& in_reg, TZ80Op&& t0);
   void addM4Write(uint16_t& addr, uint8_t& data);

   void add(TState& newop) { ops[last] = newop; inc(last);        }
   const TState&  get()    { return ops[next];                    }
   const TState&  pop()    { auto& t=get(); inc(next); return t;  }
   bool           empty()  { return next == last;                 }
};

} // Namespace Z80CPP