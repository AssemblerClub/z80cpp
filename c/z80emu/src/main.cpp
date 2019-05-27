#include <cstdint>
#include <fstream>
#include <string>
#include <Memory.hpp>
#include <Z80.hpp>

class Computer {
   static const uint16_t MS_MAXMEM = 4096;
   Z80CPP::Z80      m_cpu;
   Z80CPP::Memory   m_mem = Z80CPP::Memory(MS_MAXMEM);

public:
   Computer() {
      //uint8_t program[16] = { 0x3E, 0x11, 0x06, 0x22, 0x0E, 0x33, 0x16, 0x44
      //                     ,  0x1E, 0x55, 0x26, 0x00, 0x2E, 0x02, 0x36, 0x88 };
      //uint8_t program[16] = {    0x3E, 0x11, 0x7F, 0x47, 0x4F, 0x57, 0x5F, 0x67
      //                       ,   0x6F, 0x7D, 0x26, 0x00, 0x2E, 0x04, 0x36, 0x6C };
      //std::memcpy(&m_mem[0], program, 16);
   }

   void step() {
      m_cpu.tick();
      if        ( !m_cpu.signal(Z80CPP::Signal::RD) ) {
         m_cpu.setData( m_mem[ m_cpu.address() ] );
      } else if ( !m_cpu.signal(Z80CPP::Signal::WR) ) {
         m_mem[ m_cpu.address() ] = m_cpu.data();
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
            do { step(); } while(--steps);
            print(std::cout, m_cpu.pc());
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