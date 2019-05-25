#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <bitset>
#include <Printer.hpp>

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

   void read_mem_from_PC() {
      rstSignal(Signal::MREQ);
      rstSignal(Signal::RD);
      m_address = m_reg.PC;      
   }

   void fetch() {
      std::cout << "Fetch\n";
      read_mem_from_PC();
      ++m_reg.PC;
   }

   void decode() {
      std::cout << "Decode\n";
      m_reg.IR = m_data;
      switch(m_data) {
         case 0x3E: std::cout << "LDA!\n"; break;
         default:   std::cout << (uint16_t)m_data << "\n";
      }
      read_mem_from_PC(); ++m_reg.PC;
   }

   void execute() {
      std::cout << "Execute\n";
      switch(m_reg.IR) {
         case 0x3E: m_reg.A = m_data; break;
      }
   }

   void tick() {
      static uint8_t o = 0;
      using TFun = decltype(&Z80::fetch);
      const uint8_t numops = 3;
      const TFun ops[numops] = { &Z80::fetch, &Z80::decode, &Z80::execute };

      m_signals = 0xFF;    // Reset Signals
      (this->*ops[o])();   // Tick
      o = ++o % numops;    // Select next operation
   }

   void     print(std::ostream& out) {
      out << "AF|"; printHex(out,m_reg.AF); out << "|AF'|"; printHex(out,m_reg.AFp); out << "|\n";
      out << "BC|"; printHex(out,m_reg.BC); out << "|BC'|"; printHex(out,m_reg.BCp); out << "|\n";
      out << "DE|"; printHex(out,m_reg.DE); out << "|DE'|"; printHex(out,m_reg.DEp); out << "|\n";
      out << "HL|"; printHex(out,m_reg.HL); out << "|HL'|"; printHex(out,m_reg.HLp); out << "|\n";
      out << "IX|"; printHex(out,m_reg.IX); out << "| I |"; printHex(out,m_reg.I); out << "|\n";
      out << "IY|"; printHex(out,m_reg.IY); out << "| R |"; printHex(out,m_reg.R); out << "|\n";
      out << "SP|"; printHex(out,m_reg.SP); out << "| IR|"; printHex(out,m_reg.IR); out << "|\n";
      out << "PC|"; printHex(out,m_reg.PC); out << "|\n";
      out << "Signals{"<< std::bitset<8>((uint16_t)m_signals) << "}\n";
   }
};
