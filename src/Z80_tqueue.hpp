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
   REGPAIR(BUF, BFh, BFl); // Data Buffer (not really on Z80, but useful)
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
      ,  _2PU8Ref
      ,  _2PU16Ref
      ,  _2PU16U8Ref
      ,  _3PU8U16U16Ref
   };
   // Type aliases for function pointers
   using VOIDFp       = void(Z80::*)(void);
   using _1PU8RefFp   = void(Z80::*)(uint8_t&);
   using _1PU16RefFp  = void(Z80::*)(uint16_t&);
   using _2PU8RefFp   = void(Z80::*)(uint8_t&,uint8_t&);
   using _2PU16RefFp  = void(Z80::*)(uint16_t&,uint16_t&);
   using _2PU16U8RefFp = void(Z80::*)(uint16_t&,uint8_t&);
   using _3PU8U16U16RefFp = void(Z80::*)(uint8_t&,uint16_t&,uint16_t&);

   // Explicit constructors for functions types
   explicit TZ80Op()                : m_type(Type::NOP) {} 
   explicit TZ80Op( VOIDFp f )      : m_type(Type::VOID), m_fv(f) {}
   explicit TZ80Op( _1PU8RefFp f, uint8_t& p ) 
      : m_type(Type::_1PU8Ref), m_f1u8r(f), m_f1u8r_p(&p) {}
   explicit TZ80Op( _1PU16RefFp f, uint16_t& p ) 
      : m_type(Type::_1PU16Ref), m_f1u16r(f), m_f1u16r_p(&p) {}
   explicit TZ80Op( _2PU8RefFp f, uint8_t& p1, uint8_t& p2 ) 
      : m_type(Type::_2PU8Ref), m_f2u8r(f), m_f2u8r_p1(&p1), m_f2u8r_p2(&p2) {}
   explicit TZ80Op( _2PU16RefFp f, uint16_t& p1, uint16_t& p2 ) 
      : m_type(Type::_2PU16Ref), m_f2u16r(f), m_f2u16r_p1(&p1), m_f2u16r_p2(&p2) {}
   explicit TZ80Op( _2PU16U8RefFp f, uint16_t& p1, uint8_t& p2 ) 
      : m_type(Type::_2PU16U8Ref), m_f2u16u8r(f), m_f2u16u8r_p1(&p1), m_f2u16u8r_p2(&p2) {}
   explicit TZ80Op( _3PU8U16U16RefFp f, uint8_t& p1, uint16_t& p2, uint16_t& p3 ) 
      : m_type(Type::_3PU8U16U16Ref), m_f3u8u16u16r(f), m_f3u8u16u16r_p1(&p1), m_f3u8u16u16r_p2(&p2), m_f3u8u16u16r_p3(&p3) {}

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
   void set( _2PU8RefFp f, uint8_t& p1, uint8_t& p2 ) { 
      m_type = Type::_2PU8Ref;
      m_f2u8r = f;
      m_f2u8r_p1 = &p1;
      m_f2u8r_p2 = &p2;
   }
   void set( _2PU16RefFp f, uint16_t& p1, uint16_t& p2 ) { 
      m_type = Type::_2PU16Ref;
      m_f2u16r = f;
      m_f2u16r_p1 = &p1;
      m_f2u16r_p2 = &p2;
   }
   void set( _2PU16U8RefFp f, uint16_t& p1, uint8_t& p2 ) { 
      m_type = Type::_2PU16U8Ref;
      m_f2u16u8r = f;
      m_f2u16u8r_p1 = &p1;
      m_f2u16u8r_p2 = &p2;
   }
   void set(_3PU8U16U16RefFp f, uint8_t& p1, uint16_t& p2, uint16_t& p3 ) { 
      m_type = Type::_3PU8U16U16Ref;
      m_f3u8u16u16r = f;
      m_f3u8u16u16r_p1 = &p1;
      m_f3u8u16u16r_p2 = &p2;
      m_f3u8u16u16r_p3 = &p3;
   }

   // Call contained function
   void operator()(Z80& cpu) const {
      // Produce member function call depending on m_type
      switch(m_type) {
         case Type::VOID:      (cpu.*m_fv    )(); break;
         case Type::_1PU8Ref:  (cpu.*m_f1u8r )(*m_f1u8r_p ); break;
         case Type::_1PU16Ref: (cpu.*m_f1u16r)(*m_f1u16r_p); break;
         case Type::_2PU8Ref:  (cpu.*m_f2u8r) (*m_f2u8r_p1, *m_f2u8r_p2); break;
         case Type::_2PU16Ref: (cpu.*m_f2u16r)(*m_f2u16r_p1, *m_f2u16r_p2); break;
         case Type::_2PU16U8Ref:(cpu.*m_f2u16u8r)(*m_f2u16u8r_p1, *m_f2u16u8r_p2); break;
         case Type::_3PU8U16U16Ref:(cpu.*m_f3u8u16u16r)(*m_f3u8u16u16r_p1, *m_f3u8u16u16r_p2, *m_f3u8u16u16r_p3); break;
         case Type::NOP: break;         
      }
   }

   bool empty() const { return m_type == Type::NOP; }

private:
   Type m_type;     //< Type of operation
   union {
      // Type 0: void (Z80::*)()
      struct { VOIDFp m_fv; };
      // Type 1: void (Z80::*)(uint8_t&)
      struct {
         _1PU8RefFp  m_f1u8r;
         uint8_t*    m_f1u8r_p;
      };
      // Type 2: void (Z80::*)(uint16_t&)
      struct {
         _1PU16RefFp  m_f1u16r;
         uint16_t*    m_f1u16r_p;
      };
      // Type 3: void (Z80::*)(uint8_t&, uint8_t&)
      struct {
         _2PU8RefFp  m_f2u8r;
         uint8_t*    m_f2u8r_p1;
         uint8_t*    m_f2u8r_p2;
      };
      // Type 4: void (Z80::*)(uint16_t&, uint16_t&)
      struct {
         _2PU16RefFp  m_f2u16r;
         uint16_t*    m_f2u16r_p1;
         uint16_t*    m_f2u16r_p2;
      };
      // Type 5: void (Z80::*)(uint16_t&, int16_t)
      struct {
         _2PU16U8RefFp m_f2u16u8r;
         uint16_t*    m_f2u16u8r_p1;
         uint8_t*     m_f2u16u8r_p2;
      };
      // Type 6: void (Z80::*)(uint8_t&, uint16_t&, int16_t)
      struct {
         _3PU8U16U16RefFp m_f3u8u16u16r;
         uint8_t*     m_f3u8u16u16r_p1;
         uint16_t*    m_f3u8u16u16r_p2;
         uint16_t*    m_f3u8u16u16r_p3;
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
   ,  WSAMP = 0x0040  //< WAIT SAMPLE DOES NOT EXIST IN THE Z80 (Just used to know when it has to sample WAIT INPUT SIGNAL)
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
          TZ80Op   op      = TZ80Op();
   TState() = default;
   TState(uint16_t s, const uint16_t* a, const uint8_t* d, TZ80Op&& o)
      : signals(s), addr(a), data(d), op(std::move(o)) {}
   void set(uint16_t s, const uint16_t* a, const uint8_t* d, TZ80Op&& o) {
      signals = s; addr = a; data = d;
      op = std::move(o);
   }
};

//
// Compiletime Bitwise checking that a unit 
// greater than 0 is a power of 2
// It requires to have only one bit on
//
constexpr bool isPowerOf2(uint8_t p) {
   if (!p) return false;
   while (p > 1) {
      if (p & 1) return false;
      p >>= 1;
   }
   return true;
}

//
// TVecOps: Class for encapsulating a fixed-length 
// vector of pending operations (t-states) to perform
//
class TVecOps {
   static constexpr uint8_t length = 32;  // Must be a power of 2!
   static_assert(isPowerOf2(length));
   
   std::array<TState, length> ops;        // Queue of T-state operations
   uint8_t next = 0;                      // ID of next operation to be performed
   uint8_t last = 0;                      // Last operation queued
   Z80& cpu;                              // Access to the CPU status
   
   // Increment an ID accessing the array in a circular way 
   // When ++ arrives at the length, it returns to 0. This
   // Assumes length is a power of two to do modulus using AND
   inline void inc(uint8_t& v) { v = (v+1) & (length-1); }
public:
   TVecOps(Z80& c) : cpu(c) {}
   void addM1();
   void addHALTNOP();
   void addM23Read      (uint16_t& addr, uint8_t& in_reg, TZ80Op&& t0 = TZ80Op());
   void addM3ReadAssign (uint16_t& read_reg, uint8_t& in_reg, uint16_t& to_reg, uint16_t& from_reg);
   void addM45Read      (uint16_t& read_addr, uint8_t& in_reg, TZ80Op&& t);
   void addM45Write     (uint16_t& addr, uint8_t& data, TZ80Op&& t = TZ80Op());
   void addM3alu        (uint8_t ts, TZ80Op&& tend);

   void extendM(TZ80Op&& t = TZ80Op());

   void add(TState& newop) { ops[last] = newop; inc(last);  }
   const TState&  get()    { return ops[next];              }
   void pop()              { inc(next);                     }
   bool empty()            { return next == last;           }
};

} // Namespace Z80CPP