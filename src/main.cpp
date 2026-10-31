#include <iostream>

#include <raylib.h>

#include "Processor.hpp"

constexpr int cVIEW_HEIGHT = 512;
constexpr int cVIEW_WIDTH = 1024;

int main()
{
   // set up processor
   std::string title = "CHIP-8";
   chip8::Processor proc;
   proc.LoadRom("C:\\Users\\bwhen\\source\\repos\\chip-8_emulator\\resources\\ibm.ch8");


   InitWindow(cVIEW_WIDTH, cVIEW_HEIGHT, title.c_str());

   // set up frame buffer
   Color* frameBuffer = proc.GetFrameBuffer(); // non owning pointer to frame buffer (maybe not the best way to do this?)
   Image screenImage{ frameBuffer, 64, 32, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
   Texture2D screen = LoadTextureFromImage(screenImage);

   // start with simple implemtation of 60 fps w/ 10 instructions executed per frame
   SetTargetFPS(60);
   while (!WindowShouldClose())
   {
      //TODO set up a better clock with seperate screen and cpu event handling

      // Handle pressed keys
      KeyPressed_t key;

      if (IsKeyPressed(KeyboardKey::KEY_ONE))
      {
         key.keyPressed = true;
         key.keyValue = 0x1;
      }
      if (IsKeyPressed(KeyboardKey::KEY_TWO))
      {
         key.keyPressed = true;
         key.keyValue = 0x2;
      }
      if (IsKeyPressed(KeyboardKey::KEY_THREE))
      {
         key.keyPressed = true;
         key.keyValue = 0x3;
      }
      if (IsKeyPressed(KeyboardKey::KEY_Q))
      {
         key.keyPressed = true;
         key.keyValue = 0x4;
      }
      if (IsKeyPressed(KeyboardKey::KEY_W))
      {
         key.keyPressed = true;
         key.keyValue = 0x5;
      }
      if (IsKeyPressed(KeyboardKey::KEY_E))
      {
         key.keyPressed = true;
         key.keyValue = 0x6;
      }
      if (IsKeyPressed(KeyboardKey::KEY_A))
      {
         key.keyPressed = true;
         key.keyValue = 0x7;
      }
      if (IsKeyPressed(KeyboardKey::KEY_S))
      {
         key.keyPressed = true;
         key.keyValue = 0x8;
      }
      if (IsKeyPressed(KeyboardKey::KEY_D))
      {
         key.keyPressed = true;
         key.keyValue = 0x9;
      }
      if (IsKeyPressed(KeyboardKey::KEY_Z))
      {
         key.keyPressed = true;
         key.keyValue = 0xA;
      }
      if (IsKeyPressed(KeyboardKey::KEY_X))
      {
         key.keyPressed = true;
         key.keyValue = 0x0;
      }
      if (IsKeyPressed(KeyboardKey::KEY_C))
      {
         key.keyPressed = true;
         key.keyValue = 0xB;
      }
      if (IsKeyPressed(KeyboardKey::KEY_FOUR))
      {
         key.keyPressed = true;
         key.keyValue = 0xC;
      }
      if (IsKeyPressed(KeyboardKey::KEY_E))
      {
         key.keyPressed = true;
         key.keyValue = 0xD;
      }
      if (IsKeyPressed(KeyboardKey::KEY_D))
      {
         key.keyPressed = true;
         key.keyValue = 0xE;
      }
      if (IsKeyPressed(KeyboardKey::KEY_C))
      {
         key.keyPressed = true;
         key.keyValue = 0xF;
      }

      proc.SetPressedKey(key);


      // execute 10 instructions
      for (int i = 0; i < 10; i++)
      {
         if (!proc.ExecuteNextInstruction())
         {
            std::cout << "Instruction failure. Exiting." << std::endl;
            return -1;
         }
      }

      UpdateTexture(screen, frameBuffer);

      BeginDrawing();

      Rectangle sourceRec = { 0.0f, 0.0f, (float)64, (float)32 };
      Rectangle destRec = { 0.0f, 0.0f, (float)cVIEW_WIDTH, (float)cVIEW_HEIGHT };
      Vector2 origin = { 0.0f, 0.0f };
      float rotation = 0.0f;

      DrawTexturePro(screen, sourceRec, destRec, origin, rotation, WHITE);


      EndDrawing();

   }
   return 0;
}