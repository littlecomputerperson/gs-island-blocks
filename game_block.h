/*============================================================================================+
 | Blocks!                                                                                    |
 |--------------------------------------------------------------------------------------------|
 | FILES: game_block.cpp, game_block.h                                                        |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GameBlock                                                                           |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GAME_BLOCK_H
#define GAME_BLOCK_H


//==============================================================================================
// Include Windows header files.
// ---------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
// #include <windows.h>

#ifndef BOOL
   typedef int BOOL;
   #define TRUE 1
   #define FALSE 0
#endif
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class GameBlock {

   private:
      
      int m_Data[4][4];

   protected:

      // No protected members.

   public:
      
      GameBlock();
      GameBlock(int nVal_0_0, int nVal_0_1, int nVal_0_2, int nVal_0_3,
                int nVal_1_0, int nVal_1_1, int nVal_1_2, int nVal_1_3,
                int nVal_2_0, int nVal_2_1, int nVal_2_2, int nVal_2_3,
                int nVal_3_0, int nVal_3_1, int nVal_3_2, int nVal_3_3);
      GameBlock(GameBlock& TempBlock);
      GameBlock operator = (GameBlock& TempBlock);
      
      int  GetData(int nRow, int nCol);

      void SetData(int nValue);
      void SetData(GameBlock& TempBlock);
      void SetData(int nVal_0_0, int nVal_0_1, int nVal_0_2, int nVal_0_3,
                   int nVal_1_0, int nVal_1_1, int nVal_1_2, int nVal_1_3,
                   int nVal_2_0, int nVal_2_1, int nVal_2_2, int nVal_2_3,
                   int nVal_3_0, int nVal_3_1, int nVal_3_2, int nVal_3_3);
      BOOL SetData(int nRow, int nCol, int nValue);

      void TurnLeft();
      void TurnRight();
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif