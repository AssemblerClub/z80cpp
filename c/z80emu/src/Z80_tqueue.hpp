#pragma once
#include <array>

namespace Z80CPP {

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
   void set(const uint16_t* s, const uint16_t* a, const uint8_t* d, const uint8_t* i, TZ80Op&& o) {
      signals = s; addr = a; data = d;
      io = i; op = std::move(o);
   }
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
   void addM1(uint16_t& addr, uint8_t& in_reg);
   void addM2Read(uint16_t& addr, uint8_t& in_reg, TZ80Op&& t0);
   void addM3Write(uint16_t& addr, uint8_t& data);

   void add(TState& newop) { ops[last] = newop; inc(last);        }
   const TState&  get()    { return ops[next];                    }
   const TState&  pop()    { auto& t=get(); inc(next); return t;  }
   bool           empty()  { return next == last;                 }
};

} // Namespace Z80CPP