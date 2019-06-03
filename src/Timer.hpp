#pragma once

#include <chrono>

namespace Z80CPP {

template <typename UNIT>
class Timer {
   using Clock  = std::chrono::high_resolution_clock;
   using TimePt = Clock::time_point;

   TimePt m_start;

public:
   Timer() : m_start(Clock::now()) {}
   void     reset()        { m_start = Clock::now(); }
   double   passed() const {
      using Duration = std::chrono::duration<double>;
      Duration passed = std::chrono::duration_cast<Duration>(Clock::now() - m_start);
      return passed.count();
   }
   UNIT     secs() const   { return passed();              }
   UNIT     ms()   const   { return passed() * 1000;       }
   UNIT     us()   const   { return passed() * 1000000;    }
   UNIT     ns()   const   { return passed() * 1000000000; }
};

} // Namespace Z80CPP