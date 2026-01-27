/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_blocks.cpp, gs_blocks.h                                                          |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Blocks                                                                           |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_BLOCKS_H
#define GS_BLOCKS_H


//================================================================================================
// Include Game System (GS) header files.
// -----------------------------------------------------------------------------------------------
#include "gs_main.h"
#include "game_block.h"
//================================================================================================

//================================================================================================
// Include standard C library header files.
// -----------------------------------------------------------------------------------------------
#include <math.h>
#include <stdio.h>
#include <time.h>
//================================================================================================


//==============================================================================================
// Game defines.
// ---------------------------------------------------------------------------------------------
#define SETTINGS_FILE "settings.dat"
#define HISCORES_FILE "hiscores.dat"
// ---------------------------------------------------------------------------------------------
#define INTERNAL_RES_X 640
#define INTERNAL_RES_Y 480
// ---------------------------------------------------------------------------------------------
#define GAME_INTRO   0
#define TITLE_INTRO  1
#define TITLE_SCREEN 2
#define GAME_OPTIONS 3
#define VIEW_HISCORE 4
#define NEW_HISCORE  5
#define MODE_SELECT  6
#define LEVEL_SELECT 7
#define PLAY_GAME    8
#define PAUSE_GAME   9
#define CLEAR_LINES  10
#define LEVEL_UP     11
#define END_GAME     12
#define GAME_OVER    13
#define QUIT_GAME    14
// ---------------------------------------------------------------------------------------------
#define MAX_SCORES 10
// ---------------------------------------------------------------------------------------------
#define MAX_LEVEL 9
// ---------------------------------------------------------------------------------------------
#define MAX_SPECIAL_ELEMENTS 5
#define SPECIAL_ELEMENT      9
// ---------------------------------------------------------------------------------------------
#define MUSIC_TITLE 0
#define MUSIC_GAME  1
// ---------------------------------------------------------------------------------------------
#define SCORE_BASE 10
#define SCORE_MAX  999999L
// ---------------------------------------------------------------------------------------------
#define SAMPLE_OPTION     0
#define SAMPLE_SELECT     1
#define SAMPLE_BOOM       2
#define SAMPLE_BIG_BOOM   3
#define SAMPLE_PLACE      4
#define SAMPLE_MOVE       5
#define SAMPLE_TURN       6
#define SAMPLE_DROP       7
#define SAMPLE_SPECIAL    8
#define SAMPLE_GOOD_LUCK  9
#define SAMPLE_WELL_DONE  10
#define SAMPLE_FANTASTIC  11
#define SAMPLE_GAME_OVER  12
#define SAMPLE_GOOD_SCORE 13
// ---------------------------------------------------------------------------------------------
#define AREA_ROWS  23
#define AREA_COLS  12
// ---------------------------------------------------------------------------------------------
#define BLOCK_ROWS  4
#define BLOCK_COLS  4
#define BLOCK_TYPES 7
#define BLOCK_SIZE 20
// ---------------------------------------------------------------------------------------------
#define MOVE_UP    0
#define MOVE_DOWN  1
#define MOVE_LEFT  2
#define MOVE_RIGHT 3
//==============================================================================================


//==============================================================================================
// Game structures.
// ---------------------------------------------------------------------------------------------
typedef struct GAMESETTINGS
{
    BOOL bIncreaseLevel; // Wether the game difficulty is progressive or fixed.
    BOOL bPreviewDrop;   // Wether the drop position should be previewed or not.
    BOOL bChallengeMode; // Wether game is in normal or challenge mode.
    BOOL bWindowedMode;  // Wether game is in fullscreen or windowed mode.
    BOOL bEnableVSync;   // Wether display updates should be synced to refresh rates.
    BOOL bUseParticles;  // Wether we should use particle effects or not.
    int  nMusicVolume;   // The volume of the music.
    int  nEffectsVolume; // The volume of the sound effects.
} GameSettings;
// ---------------------------------------------------------------------------------------------
typedef struct GAMESCORE
{
    char szPlayerName[12];
    long lPlayerScore;
    int  nPlayerLevel;
    int  nLinesCleared;
} GameScore;
//==============================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_Blocks : public GS_Application
{

private:

    GS_Keyboard m_gsKeyboard;  // Keyboard object.
    GS_Mouse    m_gsMouse;     // Mouse object.

    GS_OGLDisplay m_gsDisplay; // OpenGL display object.
    GS_FmodSound  m_gsSound;   // FMOD sound object.

    GS_Timer m_gsGameTimer; // Timer object.

    GS_OGLSprite   m_gsBackgroundSprite; // Sprite used for the game background.
    GS_OGLSprite   m_gsTitleSprite;      // Sprite used for the title.
    GS_OGLSpriteEx m_gsBlockSpriteEx;    // Sprite used for blocks.

    GS_OGLTexture  m_gsFontTexture; // Texture used for game font and main menu.
    GS_OGLFont     m_gsGameFont;    // Small font used for the game.
    GS_OGLMenu     m_gsGameMenu;    // Menu object for the game menus.

    GS_OGLParticle m_gsParticles; // Particle object for particle effects.

    GS_OGLCollide m_gsCollide; // Collision detection.

    GameSettings m_GameSettings;           // Option settings.
    GameScore    m_GameScores[MAX_SCORES]; // Array of hiscores.

    int m_nGameProgress;    // Keeps track of the game progress.
    int m_nOldGameProgress; // Keeps track of the game progress.

    int  m_nGameLevel;  // Keeps track of the game level.
    long m_lGameScore;  // Keeps track of the game score.
    int  m_nScoreIndex; // Keeps track of the last score.

    int m_nLinesToNextLevel;  // How many lines need to be cleared to level up.
    int m_nLinesCleared;      // The current number of lines cleared.
    int m_nTotalLinesCleared; // The current number of lines cleared.

    int m_nSpecialElements; // Keeps track of how many special elements there are.

    int m_PlayArea[AREA_ROWS][AREA_COLS]; // The entire area in which the blocks will be
    // displayed and manipulated.

    int m_nBlockID;     // What kind of a block we're dealing with (1-7).
    int m_nNextBlockID; // The next block to be used.

    int m_nBlockRow; // Row where block is at in the play area.
    int m_nBlockCol; // Column where block is at in the play area.

    int m_nBlockMoveDistance; // How many pixels the block will move downwards in a second.

    BOOL m_bIsBlockTurned; // Wether the block has been turned or not.

    GameBlock m_ActiveBlock; // The block currently in play.

    GameBlock m_Block_1; // The 7 kinds of blocks supported.
    GameBlock m_Block_2;
    GameBlock m_Block_3;
    GameBlock m_Block_4;
    GameBlock m_Block_5;
    GameBlock m_Block_6;
    GameBlock m_Block_7;

    BOOL  m_bIsInitialized;  // Has a method been initialized?
    BOOL  m_bWasKeyReleased; // Wether a key has been released.
    int   m_nOptionSelected; // Which menu option was selected.
    int   m_nCounter;        // Used for counting throughout the game.

    float m_fRotate;   // Used for rotation effects.
    float m_fScale;    // Used for scale effects.
    float m_fAlpha;    // Transparency for fade effects.
    float m_fInterval; // Used to time events.

    GS_OGLColor m_gsMenuTextColor; // Normal menu text color.
    GS_OGLColor m_gsMenuHighColor; // Menu highlight text color.
    GS_OGLColor m_gsSelectColor;   // General highlight text color.

    int m_nAddParticleX[MAX_PARTICLES]; // Used for particle effects.
    int m_nAddParticleY[MAX_PARTICLES];
    int m_nParticleRow;

protected:

    // Methods that override base class methods.
    BOOL GameSetup();
    BOOL GameInit();
    BOOL GameShutdown();
    BOOL GameRelease();
    BOOL GameRestore();
    BOOL GameLoop();
    void OnChangeMode();

public:

    // The constuctor and destructor.
    GS_Blocks();
    ~GS_Blocks();

    // Message procedure that override the base class message procedure.
    LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Game progress methods.
    BOOL GameIntro();
    BOOL TitleIntro();
    BOOL TitleScreen();
    BOOL GameOptions();
    BOOL ViewHiscore();
    BOOL NewHiscore();
    BOOL ModeSelect();
    BOOL LevelSelect();
    BOOL LevelUp();
    BOOL PlayGame();
    BOOL ClearLines();
    BOOL PauseGame();
    BOOL EndGame();
    BOOL GameOver();
    BOOL QuitGame();

    // Methods for setting level properties.
    void SetLevelProperties();

    // Methods for manipulating the active block.
    BOOL PlotBlock( int nBlockRow, int nBlockCol);
    void PlaceBlock(int nBlockRow, int nBlockCol, BOOL bShowBlock = TRUE);
    void TurnBlock();
    void RenderBlock(int nDestX, int nDestY, int        nBlockID,  float fAlpha = 1.0f);
    void RenderBlock(int nDestX, int nDestY, GameBlock& TempBlock, float fAlpha = 1.0f);
    void RenderNextBlock(float fAlpha = 1.0f);

    // Methods for manipulating the play area.
    void ClearPlayArea();
    int  FindPlayAreaRowToClear();
    void ClearPlayAreaRow(int nRowToClear);
    int  PlayAreaColToX(int nCol);
    int  PlayAreaRowToY(int nRow);
    void RenderPlayAreaElement(int nRow, int nCol, float fAlpha = 1.0f);
    void RenderPlayArea(float fAlpha = 1.0f);

    // Other rendering methods (for rendering game stats etc).
    void RenderBackground(float fAlpha = 1.0f);
    void RenderGameStats(float fAlpha = 1.0f);
    void RenderFrameRate(float fAlpha = 1.0f);
    void RenderParticles(int nNumParticles = 0, int nAreaRow = -1, int nAreaCol = -1);

    // Methods for loading and saving data.
    BOOL LoadSettings();
    BOOL SaveSettings();
    BOOL LoadHiscores();
    BOOL SaveHiscores();

    // Helper methods.
    void SetRenderScaling(int nWidth, int nHeight, bool bKeepAspect);
    float GetActionInterval(float fActionsPerSecond);
};

////////////////////////////////////////////////////////////////////////////////////////////////


#endif
