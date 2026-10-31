#include <cinttypes>
#include <string>

struct Color;
typedef Color Color;

typedef struct KeyPressed
{
   bool keyPressed = false;
   uint8_t keyValue = 0x0;
} KeyPressed_t;

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
   bool ExecuteNextInstruction();
   Color* GetFrameBuffer() { return mFrameBuffer; };
   void ClearScreenBuffer();
   void DrawToScreenBuffer(uint8_t aX, uint8_t aY, uint16_t aSpriteAddr, uint8_t aHeight);
   void SetPressedKey(KeyPressed_t& aPressedKey);

private:
   // number of instructions processed per second
   uint16_t mClockRate = 700;

   uint8_t mMemory[4096] = { 0 };
   // program counter that points to current instruction to execute
   uint16_t mPC = 0;
   // stack of memory addreses for subroutine execution
   uint16_t mStack[8] = { 0 }; 
   int8_t mStackIndex = -1;

   // processor registers V0 - VF
   uint8_t mRegisters[16] = { 0 };
   uint16_t mIndexReg = 0;

   // timer registers
   uint8_t mDelayTimer = 0;
   uint8_t mSoundTimer = 0;

   Color* mFrameBuffer = nullptr;

   KeysPressed_t mKey;
};
}