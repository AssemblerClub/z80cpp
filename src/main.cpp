#include <cstdint>
#include <fstream>
#include <string>
#include <Memory.hpp>
#include <Z80.hpp>
#include <Timer.hpp>
#include <Printer.hpp>

class Computer {
   static const uint16_t MS_MAXMEM = 4096;
   Z80CPP::Z80      m_cpu;
   Z80CPP::Memory   m_mem = Z80CPP::Memory(MS_MAXMEM);
   Z80CPP::Printer  m_print = Z80CPP::Printer(std::cout);

public:
   Computer() = default;

   void step() {
      {
         // Simulate Amstrad CPC's Gate-Array WAIT Cycle (3-1)
         static uint8_t wait = 2;

         // Activate WAIT signal
         wait = (wait+1) & 0x3;
         if (wait) m_cpu.setSignal(Z80CPP::Signal::WAIT);
         else      m_cpu.rstSignal(Z80CPP::Signal::WAIT);
      }

      // Tick the CPU
      m_cpu.tick();

      // Simulate a simple bus that connects to Memory through
      // RD and WR signals
      if ( m_cpu.signal(Z80CPP::Signal::MREQ)) {
         uint16_t addr = m_cpu.address();
         if        ( m_cpu.signal(Z80CPP::Signal::RD) ) {
            m_cpu.setData( m_mem[ addr ] );
         } else if ( m_cpu.signal(Z80CPP::Signal::WR) ) {
            m_mem[ addr ] = m_cpu.data();
         }
      }
   }

   void printStatus() {
      // Print CPU and Memory
      m_print.printCPUStatus(m_cpu);
      m_print.printMemoryContents(m_mem, m_cpu.address(), 3);
   }

   void gettoken(std::string& tok, std::string& com, char delim) {
      std::size_t pos = com.find(delim);
      tok = com.substr(0, pos);
      if(pos != std::string::npos) ++pos;
      com.erase(0, pos);
   }

   void doNsteps(uint32_t steps) {
      uint64_t ticks = m_cpu.ticks();
      Z80CPP::Timer<uint64_t> t;

      do { 
         step(); 
      } while(--steps);

      uint64_t ns = t.ns();
      std::cout << std::dec << "Passed: " << ns << " ns\n";
      ticks = m_cpu.ticks() - ticks;
      std::cout << "Ticks:  " << ticks << ". TPS: ";
      std::cout << ticks*1000000000/ns << " MHZ: " << (float)ticks*1000/ns << "\n";
   }

   void autorun(uint32_t ticks) {
      doNsteps(ticks);
      printStatus();
   }

   void run() {
      std::string command;
      std::string token;
      printStatus();
      do {
         std::getline(std::cin, command);
         gettoken(token, command, ' ');
         if (token == "s") {
            uint32_t steps = 1;
            if ( !command.empty() ) 
               steps = std::stoul(command);
            doNsteps(steps);
            printStatus();
         } else if (token == "m") {
            uint16_t addr = 0;
            if ( !command.empty() ) 
               addr = std::stoul(command, nullptr, 0);
            m_print.printMemoryContents(m_mem, addr, 3);
         }
      } while (token != "q");
   }

   void loadbin(const char *filename, uint16_t load, uint16_t run) {
      std::ifstream f(filename, std::ifstream::binary);

      if (!f.is_open()) { std::cerr << "Could not open " << filename << "\n"; return; }
      if (load >= MS_MAXMEM || run >= MS_MAXMEM) {
         std::cerr << "Load or run address too high\n";
         std::cerr << "MAXMEM: " << MS_MAXMEM << " LOAD: " << load << "RUN: " << run << "\n";
         return;
      }

      // get file size using buffer's members
      std::filebuf* pbuf = f.rdbuf();
      uint16_t size = pbuf->pubseekoff (0,f.end,f.in);
      pbuf->pubseekpos (0,f.in);

      if (load >= MS_MAXMEM || load+size >= MS_MAXMEM) {
         std::cerr << "Load address too high or program too big to fit into memory.\n";
         std::cerr << "MAXMEM: " << MS_MAXMEM << " LOAD: " << load;
         std::cerr << "SIZE: " << size << " LOAD+SIZE: " << load+size << "\n";
      } else {
         // Load program into memory
         pbuf->sgetn ((char*)(&m_mem[load]), size);
         m_cpu.setPC(run);
      }
   }
};


void usage() {
   std::cerr << "USAGE:\n";
   std::cerr << "   z80emu <binfile> [ticks]\n\n";
   exit(1);
}

int main(int argc, char*argv[]) {
   if (argc < 2 || argc > 3)
      usage();

   Computer K;
   K.loadbin(argv[1], 0, 0);
   if (argc == 3)
      K.autorun(std::atoi(argv[2]));
   else
      K.run();

   return 0;
}