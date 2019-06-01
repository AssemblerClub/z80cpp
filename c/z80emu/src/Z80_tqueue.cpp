#include <Z80_tqueue.hpp>
#include <Z80.hpp>
#include <cstdint>

namespace Z80CPP {

// Const signals conversions to uint16_t for clarity and brevity
const uint16_t S_M1     = (uint16_t)Signal::M1;
const uint16_t S_MREQ   = (uint16_t)Signal::MREQ;
const uint16_t S_RD     = (uint16_t)Signal::RD;
const uint16_t S_WR     = (uint16_t)Signal::WR;
const uint16_t S_RFSH   = (uint16_t)Signal::RFSH;
const uint16_t S_HALT   = (uint16_t)Signal::HALT;

void 
TVecOps::addM1() {
   ops[last].set(S_M1                 , &cpureg.PC, nullptr, nullptr, TZ80Op(&Z80::inc, cpureg.PC));
   inc(last);
   ops[last].set(S_M1 | S_MREQ | S_RD , nullptr   , nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(S_RFSH               , &cpureg.IR, nullptr, nullptr, TZ80Op(&Z80::decode));
   inc(last);
   ops[last].set(S_MREQ | S_RFSH      , nullptr   , nullptr, nullptr, TZ80Op(&Z80::inc7, cpureg.R));
   inc(last);
}

void 
TVecOps::addHALTNOP() {
   ops[last].set(S_HALT | S_M1                 , &cpureg.PC, nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(S_HALT | S_M1 | S_MREQ | S_RD , nullptr   , nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(S_HALT | S_RFSH               , &cpureg.IR, nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(S_HALT | S_MREQ | S_RFSH      , nullptr   , nullptr, nullptr, TZ80Op(&Z80::inc7, cpureg.R));
   inc(last);
}


void 
TVecOps::addM23Read(uint16_t& addr, uint8_t& in_reg, TZ80Op&& t0) {
   // Default Machine Re7ad Cycle (By default, reads from PC)
   //|      M2           |
   //| MREQ | WAIT| DIN  | 
   //|   RD |     |      | 
   ops[last].set(0            , &addr     , nullptr, nullptr, std::move(t0));
   inc(last);
   ops[last].set(S_MREQ | S_RD, nullptr   , nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(0            , nullptr   , nullptr, nullptr, TZ80Op(&Z80::data_in, in_reg));
   inc(last);
}

void 
TVecOps::addM45Write(uint16_t& addr, uint8_t& data, TZ80Op&& t) {
   // Default Machine Write Cycle (By default, writes to (PC))
   //|      M3           |
   //| MREQ | WR  | DIN  | 
   //|     -DOUT-----    |
   ops[last].set(0            , &addr   , nullptr  , nullptr, TZ80Op());
   inc(last);
   ops[last].set(S_MREQ       , nullptr , &data    , nullptr, std::move(t));
   inc(last);
   ops[last].set(S_MREQ | S_WR, nullptr , nullptr  , nullptr, TZ80Op());
   inc(last);
}

void 
TVecOps::extendM(TZ80Op&& t) {
   // Extends current machine cycle with an extra waiting tstate
   ops[last].set(0 , nullptr , nullptr , nullptr, std::move(t));
   inc(last);
}


} // Namespace Z80CPP