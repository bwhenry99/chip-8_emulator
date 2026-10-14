#include <cinttypes>
#include <string>


namespace chip8
{
static const uint16_t cFONT_START_ADDRESS = 0x50;
static const uint16_t cROM_START_ADDRESS = 0x200;

class Processor
{
public:
   Processor();
   ~Processor();

   bool LoadRom(std::string aFilename);

private:
   // number of instructions processed per second
   uint16_t mClockRate = 700;

   uint8_t mMemory[4096] = { 0 };
   // program counter that points to current instruction to execute
   uint16_t mPC = 0;
   // stack of memory addreses for subroutine execution
   uint16_t mStack[8] = { 0 }; 
   uint8_t mStackIndex = 0;

   // processor registers V0 - VF
   uint8_t mRegisters[16] = { 0 };
};
}