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
      KeysPressed_t keys;

      if (IsKeyPressed(KeyboardKey::KEY_ONE))
      {
         keys.keyPressed = true;
         keys.key[0x1] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_TWO))
      {
         keys.keyPressed = true;
         keys.key[0x2] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_THREE))
      {
         keys.keyPressed = true;
         keys.key[0x3] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_Q))
      {
         keys.keyPressed = true;
         keys.key[0x4] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_W))
      {
         keys.keyPressed = true;
         keys.key[0x5] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_E))
      {
         keys.keyPressed = true;
         keys.key[0x6] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_A))
      {
         keys.keyPressed = true;
         keys.key[0x7] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_S))
      {
         keys.keyPressed = true;
         keys.key[0x8] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_D))
      {
         keys.keyPressed = true;
         keys.key[0x9] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_Z))
      {
         keys.keyPressed = true;
         keys.key[0xA] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_X))
      {
         keys.keyPressed = true;
         keys.key[0x0] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_C))
      {
         keys.keyPressed = true;
         keys.key[0xB] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_FOUR))
      {
         keys.keyPressed = true;
         keys.key[0xC] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_E))
      {
         keys.keyPressed = true;
         keys.key[0xD] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_D))
      {
         keys.keyPressed = true;
         keys.key[0xE] = true;
      }
      if (IsKeyPressed(KeyboardKey::KEY_C))
      {
         keys.keyPressed = true;
         keys.key[0xF] = true;
      }

      proc.SetPressedKeys(keys);


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