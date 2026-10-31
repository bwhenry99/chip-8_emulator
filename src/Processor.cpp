#include "Processor.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>

#include <raylib.h>


inline uint8_t GetRegIdx(uint16_t aOpCode)
{
   return (aOpCode & 0x0F00) >> 8;
}

inline std::pair<uint8_t, uint8_t> Get2RegIdx(uint16_t aOpCode)
{
   return { (aOpCode & 0x0F00) >> 8, (aOpCode & 0x00F0) >> 4 };
}

bool IsBlack(Color aColor)
{
   return aColor.r == 0 && aColor.g == 0 && aColor.b == 0;
}

bool IsWhite(Color aColor)
{
   return aColor.r == 255 && aColor.g == 255 && aColor.b == 255;
}

chip8::Processor::Processor()
{
   // add font data to memory
   uint8_t font[5 * 16] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F  
   };

   std::memcpy(&mMemory[cFONT_START_ADDRESS], &font, 5 * 16);

   // initiialize frame buffer and clear to black
   mFrameBuffer = new Color[64 * 32];
   std::fill(mFrameBuffer, mFrameBuffer + (64 * 32), BLACK);
}

chip8::Processor::~Processor()
{
   delete mFrameBuffer;
}

bool chip8::Processor::LoadRom(std::string aFilename)
{
   std::ifstream rom(aFilename, std::ios::binary | std::ios::ate);

   if (!rom.is_open())
   {
      std::cout << "Failed to load ROM" << std::endl;
      return false;
   }

   // get rom size
   size_t size = rom.tellg();

   //return to front
   rom.seekg(0, std::ios::beg);

   // make sure the ROM will fit into the allocated memory
   if (size + 0x200 >= 4096)
   {
      std::cout << "ROM too large for memory" << std::endl;
      return false;
   }

   if (rom.read(reinterpret_cast<char*>(&mMemory[0x200]), size))
   {
      std::cout << "Successfully loaded ROM" << std::endl;
   }

   // set instruction pointer to start of ROM
   mPC = 0x200;
   return true;
}

bool chip8::Processor::ExecuteNextInstruction()
{
   // get 2 byte opcode
   uint16_t opCode = (static_cast<uint16_t>(mMemory[mPC]) << 8) + mMemory[mPC + 1];
   // increment instruction address
   mPC += 2;

   if (opCode == 0x00E0)
   {
      // clear screen code, no other processing required
      ClearScreenBuffer();
      return true;
   }

   if (opCode == 0x00EE)
   {
      // return from subroutine opcode
      // pop entry from stack and set PC to retrieved address
      
      if (mStackIndex < 0)
      {
         // we're not in a subroutine and there's no address to pop from the stack
         return false;
      }

      mPC = mStack[mStackIndex];
      mStackIndex--;
      return true;
   }

   uint8_t nibble1 = (0xF000 & opCode) >> 12;

   switch (nibble1)
   {
   case 0x1:
   {
      // jump instruction 1NNN
      // set PC to 12 bit address specificed by NNN
      mPC = opCode & 0x0FFF;
      break;
   }
   case 0x2:
   {
      // subroutine instruction 2NNN
      // add current PC to stack, increment stack index, set PC to address NNN
      mStackIndex++;
      mStack[mStackIndex] = mPC;
      mPC = opCode & 0x0FFF;
      break;
   }
   case 0x3:
   {
      // skip instruction 3XNN
      // skip next instruction if value in register X is equal to NN
      uint8_t regIdx = GetRegIdx(opCode);
      uint16_t value = opCode & 0x00FF;
      if (mRegisters[regIdx] == value)
      {
         // increment PC a second time to skip next instruction
         mPC += 2;
      }
      break;
    }
   case 0x4:
   {
      // skip instruction 4XNN
      // skip next instruction if value in register X is not equal to NN
      uint8_t regIdx = GetRegIdx(opCode);
      uint16_t value = opCode & 0x00FF;
      if (mRegisters[regIdx] != value)
      {
         // increment PC a second time to skip next instruction
         mPC += 2;
      }
      break;
   }
   case 0x5:
   {
      // skip instruction 5XY0
      // skip next instruction if values in registers X and Y are equal
      std::pair<uint8_t, uint8_t> regIdxs = Get2RegIdx(opCode);
      if (mRegisters[regIdxs.first] == mRegisters[regIdxs.second])
      {
         // increment PC a second time to skip next instruction
         mPC += 2;
      }
      break;
   }
   case 0x6:
   {
      // set instruction 6XNN
      // set resiger X to value NN
      uint8_t regIdx = GetRegIdx(opCode);
      mRegisters[regIdx] = opCode & 0xFF;
      break;
   }
   case 0x7:
   {
      // add instruction 7XNN
      // add value NN to register X, allow overflow 
      uint8_t regIdx = GetRegIdx(opCode);
      mRegisters[regIdx] += opCode & 0xFF;
      break;
   }
   case 0x8:
   {
      // family of register instructions 8XY#
      // # specifies instruction to be exeucted
      std::pair<uint8_t, uint8_t> regIdxs = Get2RegIdx(opCode);
      uint8_t instruction = 0x000F & opCode;

      switch (instruction)
      {
      case 0:
      {
         // set intruction 8XY0
         // set value in register X to value in register Y
         mRegisters[regIdxs.first] = mRegisters[regIdxs.second];
         break;
      }
      case 1:
      {
         // binary or instruction 8XY1
         // set value in register X to binary or of values in register X and Y
         mRegisters[regIdxs.first] = mRegisters[regIdxs.first] | mRegisters[regIdxs.second];
         break;
      }
      case 2:
      {
         // binary and instruction 8XY2
         // set value in register X to binary and of values in register X and Y
         mRegisters[regIdxs.first] = mRegisters[regIdxs.first] & mRegisters[regIdxs.second];
         break;
      }
      case 3:
      {
         // binary or instruction 8XY1
         // set value in register X to binary or of values in register X and Y
         mRegisters[regIdxs.first] = mRegisters[regIdxs.first] ^ mRegisters[regIdxs.second];
         break;
      }
      case 4:
      {
         // add instruction 8XY4
         // sets register X to sum of values in registers X and Y, sets carry flag on overflow

         // set carry flag to 0
         mRegisters[0xF] = 0;
         // sum to 16 bit int to catch overflow
         uint16_t sum = mRegisters[regIdxs.first] + mRegisters[regIdxs.second];
         // if value overflows 8 bit set carry flag to 1
         if (sum > 255)
         {
            mRegisters[0xF] = 1;
         }
         // move sum to register X, cast to 8 bit value (data loss is captured in carry flag)
         mRegisters[regIdxs.first] = static_cast<uint8_t>(sum);
         break;
      }
      case 5:
      {
         // substract instruction 8XY5
         // sets register X to value in register X - value in register Y, sets carry flag on if no underflow
         
         // set carry flag to 1
         mRegisters[0xF] = 1;

         // if subtraction will not underflow, set carry flag to 0
         if (mRegisters[regIdxs.first] >= mRegisters[regIdxs.second])
         {
            mRegisters[0xF] = 0;
         }

         // set register X to value of subtraction
         mRegisters[regIdxs.first] = mRegisters[regIdxs.first] - mRegisters[regIdxs.second];
         break;
      }
      case 6:
      {

         break;
      }
      case 7:
      {
         // substract instruction 8XY
         // sets register X to value in register Y - value in register X, sets carry flag on if no underflow

         // set carry flag to 1
         mRegisters[0xF] = 1;

         // if subtraction will not underflow, set carry flag to 0
         if (mRegisters[regIdxs.second] >= mRegisters[regIdxs.first])
         {
            mRegisters[0xF] = 0;
         }

         // set register X to value of subtraction
         mRegisters[regIdxs.first] = mRegisters[regIdxs.second] - mRegisters[regIdxs.first];
         break;
      }
      default:
      {
         std::cout << "Unknown set instruction type." << std::endl;
         return false;
      }
      }
      break;

   }
   case 0x9:
   {
      // skip instruction 9XY0
      // skip next instruction if values in registers X and Y are not equal
      std::pair<uint8_t, uint8_t> regIdxs = Get2RegIdx(opCode);
      if (mRegisters[regIdxs.first] != mRegisters[regIdxs.second])
      {
         // increment PC a second time to skip next instruction
         mPC += 2;
      }
      break;
   }
   case 0xA:
   {
      // set index register ANNN
      // set index registter to address NNN
      mIndexReg = opCode & 0x0FFF;
      break;
   }
   case 0xD:
   {
      // draw instruction DXYN
      // draw N pixel tall sprite at address specified by index register
      // at x coord specified in register X and y coord specified at register Y
      std::pair<uint8_t, uint8_t> regIdxs = Get2RegIdx(opCode);
      uint8_t height  = opCode & 0x000F;
      DrawToScreenBuffer(mRegisters[regIdxs.first], mRegisters[regIdxs.second], mIndexReg, height);
      break;
   }
   case 0xF:
   {
      // family of instructions FX##
      // X is a register index and ## specifies the instrucion to be executed

      uint8_t regIdx = GetRegIdx(opCode);
      uint8_t instruction = opCode & 0x00FF;

      switch (instruction)
      {
      case 0x07:
      {
         // timer instruction FX07
         // set value in register VX to current value of delay timer
         mRegisters[regIdx] = mDelayTimer;
         break;
      }
      case 0x15:
      {
         // timer instruction FX15
         // set delay timer to value in register X
         mDelayTimer = mRegisters[regIdx];
         break;
      }
      case 0x18:
      {
         // timer instruction FX18
         // sets sound timer to value in register X
         mSoundTimer = mRegisters[regIdx];
         break;
      }
      case 0x1E:
      {
         // add to index instructions FX1E
         // add value in register X to register index
         // if the value exceeds 0x1000 set the carry flag
         mIndexReg += mRegisters[regIdx];

         if (mIndexReg > 0x1000)
         {
            mRegisters[0xF] = 1;
         }
         break;
      }
      case 0x0A:
         // get key instruction FX0A
         // puts hex value of currently pressed key in register X
         // insctucion is blocking, if no key is pressed repeat until input is received

         if (!mKey.keyPressed)
         {
            // decrement instruction pointer by 2 to repeat instruction
            mPC -= 2;
            break;
         }

         mRegisters[regIdx] = mKey.keyValue;

      }
   }
   default:
   {
      std::cout << "Unknown Op Code nibble:" << nibble1 << std::endl;
      return false;
   }
   }
   return true;
}

void chip8::Processor::ClearScreenBuffer()
{
   std::fill(mFrameBuffer, mFrameBuffer + (64 * 32), BLACK);
}

void chip8::Processor::DrawToScreenBuffer(uint8_t aX, uint8_t aY, uint16_t aSpriteAddr, uint8_t aHeight)
{
   // constrain starting point to screen space
   uint8_t x = aX % 64;
   uint8_t y = aY % 32;

   // set flag register to 0
   mRegisters[0xF] = 0;

   // for the height of the sprite
   for (uint8_t i = 0; i < aHeight; i++)
   {
      uint8_t row = y + i;
      if (row >= 32)
      {
         // row index out of bounds, break
         break;
      }
      uint8_t spriteRow = mMemory[aSpriteAddr + i];
      // for the 8 bit width of the sprite
      for (uint8_t j = 0; j <= 7; j++)
      {
         uint8_t col = x + j;
         if (col >= 64)
         {
            // colunm index out of bounds, break
            break;
         }

         // if the sprite draws to the point in this row
         int index = col + (64 * row);
         if (spriteRow & (0b1 << (7 - j)))
         {
            if (IsBlack(mFrameBuffer[col + (64 * row)]))
            {
               mFrameBuffer[col + (64 * row)] = WHITE;
            }
            else if (IsWhite(mFrameBuffer[col + (64 * row)]))
            {
               mFrameBuffer[col + (64 * row)] = BLACK;
               mRegisters[0xF] = 1;
            }
         }
      }
   }
}

void chip8::Processor::SetPressedKey(KeyPressed_t& aPressedKey)
{
   mKey.keyPressed = aPressedKey.keyPressed;
   mKey.keyValue = aPressedKey.keyValue;
}