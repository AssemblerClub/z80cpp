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
const uint16_t S_WSMP   = (uint16_t)Signal::WSAMP;

void 
TVecOps::addM1() {
   Registers& r    = cpu.registers_r();
   uint16_t&  addr = cpu.address_r();
   uint8_t&   data = cpu.data_r();
   
   ops[last].set(S_M1                           , &r.PC, &data, TZ80Op(&Z80::inc, r.PC));
   inc(last);
   ops[last].set(S_M1 | S_MREQ | S_RD | S_WSMP  , &addr, &data, TZ80Op());
   inc(last);
   ops[last].set(S_RFSH                         , &r.IR, &data, TZ80Op(&Z80::decode));
   inc(last);
   ops[last].set(S_MREQ | S_RFSH                , &addr, &data, TZ80Op(&Z80::inc7, r.R));
   inc(last);
}

void 
TVecOps::addHALTNOP() {
   Registers& r    = cpu.registers_r();
   uint16_t&  addr = cpu.address_r();
   uint8_t&   data = cpu.data_r();

   ops[last].set(S_HALT | S_M1                         , &r.PC, &data, TZ80Op());
   inc(last);
   ops[last].set(S_HALT | S_M1 | S_MREQ | S_RD | S_WSMP, &addr, &data, TZ80Op());
   inc(last);
   ops[last].set(S_HALT | S_RFSH                       , &r.IR, &data, TZ80Op());
   inc(last);
   ops[last].set(S_HALT | S_MREQ | S_RFSH              , &addr, &data, TZ80Op(&Z80::inc7, r.R));
   inc(last);
}


void 
TVecOps::addM23Read(uint16_t& read_addr, uint8_t& in_reg, TZ80Op&& t0) {
   uint16_t&  addr = cpu.address_r();
   uint8_t&   data = cpu.data_r();

   // Default Machine Re7ad Cycle (By default, reads from PC)
   //|      M2           |
   //| MREQ | WAIT| DIN  | 
   //|   RD |     |      | 
   ops[last].set(0                     , &read_addr, &data, std::move(t0));
   inc(last);
   ops[last].set(S_MREQ | S_RD | S_WSMP, &addr     , &data, TZ80Op());
   inc(last);
   ops[last].set(0                     , &addr     , &data, TZ80Op(&Z80::data_in, in_reg));
   inc(last);
}

void 
TVecOps::addM3alu(uint8_t ts, TZ80Op&& tend) {
   uint16_t&  addr = cpu.address_r();
   uint8_t&   data = cpu.data_r();  

   while(--ts) {
      ops[last].set(0, &addr, &data, TZ80Op());
      inc(last);
   }
   ops[last].set(0, &addr, &data, std::move(tend));
   inc(last);
}

void 
TVecOps::addM45Write(uint16_t& wr_addr, uint8_t& wr_data, TZ80Op&& t) {
   uint16_t&  addr = cpu.address_r();
   uint8_t&   data = cpu.data_r();  

   // Default Machine Write Cycle (By default, writes to (PC))
   //|      M3           |
   //| MREQ | WR  | DIN  | 
   //|     -DOUT-----    |
   ops[last].set(0               , &wr_addr, &data    , TZ80Op());
   inc(last);
   ops[last].set(S_MREQ | S_WSMP , &addr   , &wr_data , std::move(t));
   inc(last);
   ops[last].set(S_MREQ | S_WR   , &addr   , &data    , TZ80Op());
   inc(last);
}

void 
TVecOps::extendM(TZ80Op&& t) {
   uint16_t&  addr = cpu.address_r();
   uint8_t&   data = cpu.data_r();  

   // Extends current machine cycle with an extra waiting tstate
   ops[last].set(0 , &addr , &data , std::move(t));
   inc(last);
}


} // Namespace Z80CPP