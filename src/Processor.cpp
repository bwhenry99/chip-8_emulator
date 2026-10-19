#include "Processor.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

#include <raylib.h>


inline uint8_t GetRegIdx(uint16_t opCode)
{
   return (opCode & 0x0F00) >> 8;
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

   uint8_t nibbleMask = 0b1111;
   uint8_t nibble1 = ((nibbleMask << 12) & opCode) >> 12;

   switch (nibble1)
   {
   case 0x1:
   {
      // jump instruction 1NNN
      // set PC to 12 bit address specificed by NNN
      mPC = opCode & 0x0FFF;
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
      uint8_t xRegIdx = (opCode & 0x0F00) >> 8;
      uint8_t yRegIdx = (opCode & 0x00F0) >> 4;
      uint8_t height  = opCode & 0x000F;
      DrawToScreenBuffer(mRegisters[xRegIdx], mRegisters[yRegIdx], mIndexReg, height);
      break;
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