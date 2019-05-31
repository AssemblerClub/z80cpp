#include <Z80_tqueue.hpp>
#include <Z80.hpp>
#include <cstdint>

namespace Z80CPP {

// Definition of static const signal groups
const uint16_t MS_NOSIGNAL   = 0xFFFF;
const uint16_t MS_MREQ       = (uint16_t)(~((uint16_t)Signal::MREQ));
const uint16_t MS_MREQ_RD    = (uint16_t)(~((uint16_t)Signal::MREQ | (uint16_t)Signal::RD));
const uint16_t MS_MREQ_WR    = (uint16_t)(~((uint16_t)Signal::MREQ | (uint16_t)Signal::WR));
const uint16_t MS_MREQ_RD_M1 = (uint16_t)(MS_MREQ_RD & ~((uint16_t)Signal::M1));
const uint16_t MS_MREQ_RFSH  = (uint16_t)(~((uint16_t)Signal::MREQ | (uint16_t)Signal::RFSH));
const uint16_t MS_RFSH       = (uint16_t)(~((uint16_t)Signal::RFSH));


void 
TVecOps::addM1(uint16_t& addr, uint8_t& in_reg) {
   ops[last].set(&MS_MREQ_RD_M1, &addr     , nullptr, nullptr, TZ80Op(&Z80::fetch));
   inc(last);
   ops[last].set(nullptr       , nullptr   , nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(&MS_MREQ_RFSH , nullptr   , nullptr, nullptr, TZ80Op(&Z80::data_in, in_reg));
   inc(last);
   ops[last].set(&MS_RFSH      , nullptr   , nullptr, nullptr, TZ80Op(&Z80::decode));
   inc(last);
}

void 
TVecOps::addM2Read(uint16_t& addr, uint8_t& in_reg, TZ80Op&& t0) {
   // Default Machine Re7ad Cycle (By default, reads from PC)
   //|      M2           |
   //| MREQ | WAIT| DIN  | 
   //|   RD |     |      | 
   ops[last].set(&MS_MREQ_RD   , &addr     , nullptr, nullptr, std::move(t0));
   inc(last);
   ops[last].set(nullptr       , nullptr   , nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(&MS_NOSIGNAL  , nullptr   , nullptr, nullptr, TZ80Op(&Z80::data_in, in_reg));
   inc(last);
}

void 
TVecOps::addM3Write(uint16_t& addr, uint8_t& data) {
   // Default Machine Write Cycle (By default, writes to (PC))
   //|      M3           |
   //| MREQ | WR  | DIN  | 
   //|     -DOUT-----    |
   ops[last].set(&MS_MREQ     , &addr     , &data  , nullptr, TZ80Op());
   inc(last);
   ops[last].set(&MS_MREQ_WR  , nullptr   , nullptr, nullptr, TZ80Op());
   inc(last);
   ops[last].set(&MS_NOSIGNAL , nullptr   , nullptr, nullptr, TZ80Op());
   inc(last);
}


} // Namespace Z80CPP