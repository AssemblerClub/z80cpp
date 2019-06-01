#include <cstdint>
#include <fstream>
#include <string>
#include <Memory.hpp>
#include <Z80.hpp>
#include <Timer.hpp>

class Computer {
   static const uint16_t MS_MAXMEM = 4096;
   Z80CPP::Z80      m_cpu;
   Z80CPP::Memory   m_mem = Z80CPP::Memory(MS_MAXMEM);

public:
   Computer() = default;

   void step() {
      // Tick the CPU
      m_cpu.tick();

      // Simulate a simple bus that connects to Memory through
      // RD and WR signals
      if ( m_cpu.signal(Z80CPP::Signal::MREQ)) {
         if        ( m_cpu.signal(Z80CPP::Signal::RD) ) {
            m_cpu.setData( m_mem[ m_cpu.address() ] );
         } else if ( m_cpu.signal(Z80CPP::Signal::WR) ) {
            m_mem[ m_cpu.address() ] = m_cpu.data();
         }
      }
   }

   void print(std::ostream& out, uint16_t addr) {
      m_cpu.print(out);
      m_mem.print(out, addr & 0xFFF0, 2);      
   }

   void gettoken(std::string& tok, std::string& com, char delim) {
      std::size_t pos = com.find(delim);
      tok = com.substr(0, pos);
      if(pos != std::string::npos) ++pos;
      com.erase(0, pos);
   }

   void doNsteps(uint32_t steps) {
      uint64_t ticks = m_cpu.ticks();
      Timer<uint64_t> t;

      do { 
         step(); 
      } while(--steps);

      uint64_t ns = t.ns();
      std::cout << std::dec << "Passed: " << ns << " ns\n";
      ticks = m_cpu.ticks() - ticks;
      std::cout << "Ticks:  " << ticks << ". TPS: ";
      std::cout << ticks*1000000000/ns << " MHZ: " << (float)ticks*1000/ns << "\n";
   }

   void run() {
      std::string command;
      std::string token;
      print(std::cout, m_cpu.pc());
      do {
         std::getline(std::cin, command);
         gettoken(token, command, ' ');
         if (token == "s") {
            uint32_t steps = 1;
            if ( !command.empty() ) 
               steps = std::stoul(command);
            doNsteps(steps);
            print(std::cout, m_cpu.pc());
         } else if (token == "m") {
            uint16_t addr = 0;
            if ( !command.empty() ) 
               addr = std::stoul(command, nullptr, 0);
            m_mem.print(std::cout, addr & 0xFFF0, 2);
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
   std::cerr << "   z80emu [binfile]\n\n";
   exit(1);
}

int main(int argc, char*argv[]) {
   if (argc != 2)
      usage();

   Computer K;
   K.loadbin(argv[1], 0, 0);
   K.run();

   return 0;
}