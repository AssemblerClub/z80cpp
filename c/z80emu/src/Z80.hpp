#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>

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
   REGPAIR(AF, A, F);
   REGPAIR(BC, B, C);
   REGPAIR(DE, D, E);
   REGPAIR(HL, H, L);
   REGPAIR(AFp, Ap, Fp);
   REGPAIR(BCp, Bp, Cp);
   REGPAIR(DEp, Dp, Ep);
   REGPAIR(HLp, Hp, Lp);
   REGPAIR(IX, IXh, IXl);
   REGPAIR(IY, IYh, IYl);
   uint16_t PC, SP;
   uint8_t  I, R, IR;
   Registers() 
      :  AF(0),AFp(0),BC(0),BCp(0),DE(0),DEp(0),HL(0),HLp(0)
        ,IX(0),IY(0),PC(0),SP(0),I(0),R(0),IR(0)
   {}
};

enum class Signal : uint16_t {
      MREQ  = 0x80
   ,  RD    = 0x40
   ,  WR    = 0x20
};

class Z80 {
   Registers m_reg;          // Register Banks
   uint16_t m_signals = 0xFF;// Signal pins information
   uint16_t m_address = 0;   // Address Bus information
   uint8_t  m_data    = 0;   // Data Bus information 

public:
   void     setData(uint8_t in)  { m_data = in; }
   void     setSignal(Signal s)  { m_signals |=  (uint16_t)s; }
   void     rstSignal(Signal s)  { m_signals &= ~(uint16_t)s; }
   bool     signal(Signal s)     { return m_signals & (uint16_t)s; }
   uint8_t  data()               { return m_data; }
   uint16_t address()            { return m_address; }
   //Registers& reg()              { return m_reg; }

   void read_mem_from_PC();
   void fetch();
   void decode();
   void execute();
   void tick();
   void print(std::ostream& out) const;
};

}; // Namespace Z80CPP
