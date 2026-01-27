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


//==============================================================================================
// Include header files.
// ---------------------------------------------------------------------------------------------
#include "gs_blocks.h"
//==============================================================================================


//==============================================================================================
// Global variables.
// ---------------------------------------------------------------------------------------------
GS_Blocks g_gsBlocks;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GS_Blocks():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Blocks::GS_Blocks() : GS_Application()
{

    m_GameSettings.bIncreaseLevel = TRUE;
    m_GameSettings.bPreviewDrop   = TRUE;
    m_GameSettings.bChallengeMode = FALSE;
    m_GameSettings.bWindowedMode  = TRUE;
    m_GameSettings.bEnableVSync   = FALSE;
    m_GameSettings.bUseParticles  = TRUE;
    m_GameSettings.nMusicVolume   = 90;
    m_GameSettings.nEffectsVolume = 255;

    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        lstrcpy(m_GameScores[nLoop].szPlayerName, "..........");
        m_GameScores[nLoop].lPlayerScore  = 0l;
        m_GameScores[nLoop].nPlayerLevel  = 0;
        m_GameScores[nLoop].nLinesCleared = 0;
    }

    m_nGameProgress    = 0;
    m_nOldGameProgress = 0;

    m_nGameLevel  = 0;
    m_lGameScore  = 0;
    m_nScoreIndex = 0;

    m_nLinesToNextLevel  = 0;
    m_nLinesCleared      = 0;
    m_nTotalLinesCleared = 0;

    m_nSpecialElements = 0;

    ZeroMemory(&m_PlayArea, sizeof(m_PlayArea));

    m_nBlockID     = 0;
    m_nNextBlockID = -1;

    m_nBlockMoveDistance = 0;

    m_nBlockRow = AREA_ROWS - BLOCK_ROWS;
    m_nBlockCol = ((AREA_COLS - BLOCK_COLS) / 2);

    m_bIsBlockTurned = FALSE;

    m_Block_1.SetData(1,1,1,1,
                      0,0,0,0,
                      0,0,0,0,
                      0,0,0,0);

    m_Block_2.SetData(0,2,2,2,
                      0,0,2,0,
                      0,0,0,0,
                      0,0,0,0);

    m_Block_3.SetData(0,3,3,0,
                      0,3,3,0,
                      0,0,0,0,
                      0,0,0,0);

    m_Block_4.SetData(0,0,0,4,
                      0,4,4,4,
                      0,0,0,0,
                      0,0,0,0);

    m_Block_5.SetData(0,5,5,5,
                      0,0,0,5,
                      0,0,0,0,
                      0,0,0,0);


    m_Block_6.SetData(0,0,6,6,
                      0,6,6,0,
                      0,0,0,0,
                      0,0,0,0);

    m_Block_7.SetData(0,7,7,0,
                      0,0,7,7,
                      0,0,0,0,
                      0,0,0,0);

    m_bIsInitialized  = FALSE;
    m_bWasKeyReleased = FALSE;
    m_nOptionSelected = -1;
    m_nCounter = 0;

    m_fRotate   = 0.0f;
    m_fScale    = 0.0f;
    m_fAlpha    = 0.0f;
    m_fInterval = 0.0f;

    m_gsMenuTextColor.fRed   = 1.0f;
    m_gsMenuTextColor.fGreen = 1.0f;
    m_gsMenuTextColor.fBlue  = 1.0f;
    m_gsMenuTextColor.fAlpha = 0.3f;

    m_gsMenuHighColor.fRed   = 1.0f;
    m_gsMenuHighColor.fGreen = 1.0f;
    m_gsMenuHighColor.fBlue  = 1.0f;
    m_gsMenuHighColor.fAlpha = 1.0f;

    m_gsSelectColor.fRed   = 1.0f;
    m_gsSelectColor.fGreen = 0.6f;
    m_gsSelectColor.fBlue  = 0.5f;
    m_gsSelectColor.fAlpha = 1.0f;

    for (int nLoop = 0; nLoop < MAX_PARTICLES; nLoop++)
    {
        m_nAddParticleX[nLoop] = 0;
        m_nAddParticleY[nLoop] = 0;

    }

    this->GameSetup();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::~GS_Blocks():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Blocks::~GS_Blocks()
{

    // Does nothing at the moment.
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Setup Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GameSetup():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameSetup()
{

    // Load high scores from file.
    this->LoadHiscores();

    // Load game settings from file.
    this->LoadSettings();

    // Set the display mode to the appropriate settings.
    this->SetMode(640, 480, 32, m_GameSettings.bWindowedMode);

    // Set render scaling values
    this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);

    // Let the game run as fast as it can
    this->SetFrameRate(0.0f);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize/Shutdown Methods /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameInit():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameInit()
{

    // Seed the timer for random numbers.
    time_t t;
    srand((unsigned) time(&t));

    // Setup the text in the window title bar.
    this->SetTitle("Island Blocks v1.43");

    // Are we in fullscreen mode?
    // if (!this->IsWindowed())
    // {
        // Hide the mouse cursor.
        m_gsMouse.HideCursor(TRUE);
    // }

    // Create the OpenGL display.
    if (!m_gsDisplay.Create(this->GetDevice(), this->GetWidth(), this->GetHeight(), this->GetColorDepth()))
    {
        GS_Error::Report("GS_BLOCKS.CPP", 232, "Failed to create OpenGL display!");
        return FALSE;
    }

    // Enable or disable vertical sync.
    m_gsDisplay.EnableVSync(m_GameSettings.bEnableVSync);

    // Enable blending for transparency in textures.
    m_gsDisplay.EnableBlending(TRUE);

    // Setup the display for rendering in 2D only.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Were we able to initialize the sound system?
    if (!m_gsSound.Initialize(GetWindow()))
    {
        GS_Error::Report("GS_BLOCKS.CPP", 247, "Failed to initialize FMOD sound!");
        m_gsDisplay.Destroy();
        return FALSE;
    }

    // Create the background image for the game.
    BOOL bResult = m_gsBackgroundSprite.Create("data/image_01.pcx");
    // Set source rectangle to the display size.
    m_gsBackgroundSprite.SetSourceRect(0, INTERNAL_RES_Y, INTERNAL_RES_X, 0);
    // Postion the image at bottom left of screen.
    m_gsBackgroundSprite.SetDestXY(0, 0);

    // Were we successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_BLOCKS.CPP", 261, "Failed to load background image!");
    }

    // Create the sprite (unfiltered) used for the game title image.
    bResult = m_gsTitleSprite.Create("data/image_02.tga", FALSE);

    // Were we successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_BLOCKS.CPP", 269, "Failed to load title image!");
    }

    // Create an unfiltered mipmap texture for the game font.
    bResult = m_gsFontTexture.Create("data/image_03.tga", GS_MIPMAP, GL_NEAREST, GL_NEAREST);

    // Were we successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_BLOCKS.CPP", 277, "Failed to load font image!");
    }

    // Create the game font with 20 characters per line and 16x16 size per character.
    bResult = m_gsGameFont.Create(m_gsFontTexture.GetID(), m_gsFontTexture.GetWidth(),
                                  m_gsFontTexture.GetHeight(), 20, 16, 16);

    // Were we successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_BLOCKS.CPP", 286, "Failed to create game font!");
    }

    // Create a game menu using "image_04.tga" and the font texture created earlier.
    bResult = m_gsGameMenu.Create("data/image_04.tga", m_gsFontTexture.GetID(),
                                  m_gsFontTexture.GetWidth(), m_gsFontTexture.GetHeight(),
                                  20, 16, 16);

    // Were we successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_BLOCKS.CPP", 296, "Failed to create game menu!");
    }

    // Create block sprites and do not filter the texture.
    bResult = m_gsBlockSpriteEx.Create("data/image_05.tga", 9, 9, 20, 20, FALSE);

    // Were we successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_BLOCKS.CPP", 304, "Failed to load blocks image!");
    }

    // Create 48 particles using the "image_06.tga" image.
    bResult = m_gsParticles.Create("data/image_06.tga", 48);

    // Were we successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_BLOCKS.CPP", 312, "Failed to load particle image!");
    }

    // Add music to list.
    m_gsSound.AddMusic("data/music_01.mp3");
    m_gsSound.AddMusic("data/music_02.mp3");

    // Add samples to list.
    m_gsSound.AddSample("data/sound_01.wav");
    m_gsSound.AddSample("data/sound_02.wav");
    m_gsSound.AddSample("data/sound_03.wav");
    m_gsSound.AddSample("data/sound_04.wav");
    m_gsSound.AddSample("data/sound_05.wav");
    m_gsSound.AddSample("data/sound_06.wav");
    m_gsSound.AddSample("data/sound_07.wav");
    m_gsSound.AddSample("data/sound_08.wav");
    m_gsSound.AddSample("data/sound_09.wav");
    m_gsSound.AddSample("data/sound_10.wav");
    m_gsSound.AddSample("data/sound_11.wav");
    m_gsSound.AddSample("data/sound_12.wav");
    m_gsSound.AddSample("data/sound_13.wav");
    m_gsSound.AddSample("data/sound_14.wav");

    // Set the master volume for all the sound effects.
    m_gsSound.SetSampleMaster(m_GameSettings.nEffectsVolume);

    // Are we already busy playing the game?
    if ((m_nGameProgress >= PLAY_GAME) && (m_nGameProgress < GAME_OVER))
    {
        // Start playing music at full volume, normal speed and looping infinitely.
        m_gsSound.PlayMusic(MUSIC_GAME, m_GameSettings.nMusicVolume, 1.0f, TRUE);
    }
    else if (m_nGameProgress > 0)
    {
        // Start playing music at full volume, normal speed and looping infinitely.
        m_gsSound.PlayMusic(MUSIC_TITLE, m_GameSettings.nMusicVolume, 1.0f, TRUE);
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameShutdown():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameShutdown()
{

    // Reset keyboard data.
    m_gsKeyboard.Reset();

    // Reset mouse data.
    m_gsMouse.Reset();

    // Reset display after rendering 2D.
    m_gsDisplay.EndRender2D();

    // Destroy the OpenGL display.
    m_gsDisplay.Destroy();

    // Shutdown the sound system.
    m_gsSound.Shutdown();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Release/Restore Methods /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameRelease():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameRelease()
{

    if (!GS_Application::IsActive())
    {
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset the current modelview matrix.
    glLoadIdentity();
    // Swap buffers (double buffering) to display results.
    SwapBuffers(GetDevice());

    // Pause all sound.
    m_gsSound.Pause(TRUE);

    // Pause the game.
    this->Pause(TRUE);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameRestore():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameRestore()
{

    if (GS_Application::IsActive())
    {
        return TRUE;
    }

    // Unpause all sound.
    m_gsSound.Pause(FALSE);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Main Loop ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameLoop():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameLoop()
{

    // Run part of game depending on progress.
    switch (m_nGameProgress)
    {
    case GAME_INTRO:
        GameIntro();
        break;
    case TITLE_INTRO:
        TitleIntro();
        break;
    case TITLE_SCREEN:
        TitleScreen();
        break;
    case GAME_OPTIONS:
        GameOptions();
        break;
    case VIEW_HISCORE:
        ViewHiscore();
        break;
    case NEW_HISCORE:
        NewHiscore();
        break;
    case MODE_SELECT:
        ModeSelect();
        break;
    case LEVEL_SELECT:
        LevelSelect();
        break;
    case PLAY_GAME:
        PlayGame();
        break;
    case PAUSE_GAME:
        PauseGame();
        break;
    case CLEAR_LINES:
        ClearLines();
        break;
    case LEVEL_UP:
        LevelUp();
        break;
    case END_GAME:
        EndGame();
        break;
    case GAME_OVER:
        GameOver();
        break;
    case QUIT_GAME:
        QuitGame();
        break;
    default:
        m_nGameProgress = 0;
        break;
    } // end switch

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Message Handling ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::OnChangeMode():
// ---------------------------------------------------------------------------------------------
// Purpose: Overrides the OnChangeMode() function of the base class
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_Blocks::OnChangeMode()
{
    // Change to windowed or fullscreen mode as selected by the user
    this->SetMode(640, 480, 32, !this->IsWindowed());

    // Set values for scaling
    this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);

    // Has the mode change been successful?
    m_GameSettings.bWindowedMode = this->IsWindowed();

    // Save game settings.
    this->SaveSettings();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::MsgProc():
// ---------------------------------------------------------------------------------------------
// Purpose: Overrrides the main WndProc() and MsgProc() of the base class, so the application
//          can do custom message handling (e.g. processing mouse, keyboard, or menu commands).
// ---------------------------------------------------------------------------------------------
// Returns: LRESULT depending on the message processed.
//==============================================================================================

LRESULT GS_Blocks::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    static int nPrevMusicVolume = m_GameSettings.nMusicVolume;
    static int nPrevSoundVolume = m_GameSettings.nEffectsVolume;

    switch (uMsg)
    {

    // Received when a WM_KEYDOWN message is translated by the TranslateMessage()
    // function. WM_CHAR contains the character code of the key that was pressed.
    case WM_CHAR:
        // wParam contains information on which key was pressed.
        switch (wParam)
        {
        /* User pressed 'b' or 'B'.
        case 'B':
        case 'b':
          // Are we not entering a name into the hiscore table?
          if (m_nGameProgress != NEW_HISCORE) {
              // Are we using alpha blending?
              if (m_gsDisplay.IsBlendingEnabled()) {
                 // Disable blending.
                 m_gsDisplay.EnableBlending(FALSE);
                 }
              else {
                 // Enable blending.
                 m_gsDisplay.EnableBlending(TRUE);
                 }
              }
           break; */
        // User pressed 'm' or 'M'.
        case 'M':
        case 'm':
            // Are we not entering a name into the hiscore table?
            if (m_nGameProgress != NEW_HISCORE)
            {
                // Is music enabled?
                if (m_GameSettings.nMusicVolume == 0)
                {
                    m_GameSettings.nMusicVolume = nPrevMusicVolume;
                    m_gsSound.SetMusicVolume(m_GameSettings.nMusicVolume);
                }
                else
                {
                    nPrevMusicVolume = m_GameSettings.nMusicVolume;
                    m_GameSettings.nMusicVolume = 0;
                    m_gsSound.SetMusicVolume(m_GameSettings.nMusicVolume);
                }
            }
            break;
        // User pressed 'p' or 'P'.
        case 'P':
        case 'p':
            // Pause application if unpause, unpause if paused.
            if (this->IsPaused())
            {
                this->Pause(FALSE);
            }
            else
            {
                this->Pause(TRUE);
            }
            break;
        // User pressed 's' or 'S'.
        case 'S':
        case 's':
            // Are we not entering a name into the hiscore table?
            if (m_nGameProgress != NEW_HISCORE)
            {
                // Is sound effects enabled?
                if (m_GameSettings.nEffectsVolume == 0)
                {
                    m_GameSettings.nEffectsVolume = nPrevSoundVolume;
                    m_gsSound.SetSampleMaster(m_GameSettings.nEffectsVolume);
                }
                else
                {
                    nPrevSoundVolume = m_GameSettings.nEffectsVolume;
                    m_GameSettings.nEffectsVolume = 0;
                    m_gsSound.SetSampleMaster(m_GameSettings.nEffectsVolume);
                }
            }
            break;
        /* User pressed 't' or 'T'.
        case 'T':
        case 't':
          // Are we not entering a name into the hiscore table?
          if (m_nGameProgress != NEW_HISCORE) {
              // Is the frame rate greater than 0?
              if (this->GetFrameRate() > 0.0f) {
                 SetFrameRate(0.0f);
                 }
               else {
                 this->SetFrameRate(60.0f);
                 }
              }
           break; */
        // Default processing.
        default:
            break;
        } // end switch (wParam...
        break;

    // Received when a nonsystem key is pressed. A nonsystem key is a key
    // that is pressed when the ALT key is not pressed.
    case WM_KEYDOWN:
        // Mark key being held down.
        m_gsKeyboard.KeyDown(wParam);
        // Add key to keyboard buffer.
        m_gsKeyboard.AddKeyToBuffer(wParam);
        break;

    // Received when a nonsystem key is released. A nonsystem key is a key
    // that is pressed  when the ALT key is not pressed.
    case WM_KEYUP:
        // Mark key that was released.
        m_gsKeyboard.KeyUp(wParam);
        break;

    // Received when the user holds down the ALT key and then presses another key.
    case WM_SYSKEYDOWN:
        // Mark key being held down.
        m_gsKeyboard.KeyDown(wParam);
        // Add key to keyboard buffer.
        m_gsKeyboard.AddKeyToBuffer(wParam);
        return 0L;

    // Received when the user releases a key that was pressed while the ALT key held down.
    case WM_SYSKEYUP:
        // Mark key that was released.
        m_gsKeyboard.KeyUp(wParam);
        return 0L;

    // Received when the cursor moves within a window and mouse input is not captured.
    case WM_SETCURSOR:
        // Are we in windowed mode?
        if (this->IsWindowed())
        {
            // Is the mouse cursor in the window client?
            if (m_gsMouse.IsInClient(hWnd))
            {
                // Hide the cursor.
                SetCursor(NULL);
                return TRUE;
            }
        }
        break;

    // The WM_MOUSEMOVE message is posted to a window when the cursor moves.
    case WM_MOUSEMOVE:
        // Save mouse coordinates (coordinates are relative to the top left of the client
        // area of the window). Note that the Y coordinates are inverted for OpenGL.
        m_gsMouse.SetX(LOWORD(lParam));
        m_gsMouse.SetY(INTERNAL_RES_Y - HIWORD(lParam) - 1);
        return 0L;

    // The WM_LBUTTONDOWN message is posted when the user presses the left mouse
    // button while/ the cursor is in the client area of a window.
    case WM_LBUTTONDOWN:
        // Set left mouse button down.
        m_gsMouse.LeftPressed(TRUE);
        return 0L;

    // The WM_MBUTTONDOWN message is posted when the user presses the middle
    // mouse button while the cursor is in the client area of a window.
    case WM_MBUTTONDOWN:
        // Set middle mouse button down.
        m_gsMouse.MiddlePressed(TRUE);
        return 0L;

    // The WM_RBUTTONDOWN message is posted when the user presses the right
    // mouse button while the cursor is in the client area of a window.
    case WM_RBUTTONDOWN:
        // Set right mouse button down.
        m_gsMouse.RightPressed(TRUE);
        return 0L;

    // The WM_LBUTTONUP message is posted when the user releases the left mouse
    // button while the cursor is in the client area of a window.
    case WM_LBUTTONUP:
        // Set left mouse button up.
        m_gsMouse.LeftPressed(FALSE);
        return 0L;

    // The WM_MBUTTONUP message is posted when the user releases the middle
    // mouse button while the cursor is in the client area of a window.
    case WM_MBUTTONUP:
        // Set middle mouse button up.
        m_gsMouse.MiddlePressed(FALSE);
        return 0L;

    // The WM_RBUTTONUP message is posted when the user releases the right
    // mouse button while the cursor is in the client area of a window.
    case WM_RBUTTONUP:
        // Set right mouse button up.
        m_gsMouse.RightPressed(FALSE);
        return 0L;
    } // end switch (uMsg...

    // Provide default processing by the base class.
    return GS_Application::MsgProc(hWnd, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Game Methods ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameIntro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameIntro()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fInterval = 0.0f;
        m_nCounter  = 0;
        m_fAlpha    = 0.0f;
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Initialization completed.
        m_bIsInitialized = TRUE;
        // Return to prevent the time it took to initialize the game, which constitutes
        // the first measured frame, from affecting the timing of the game.
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Game Intro Animation //////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Add interval if action is to be taken 60 times a second.
    m_fInterval += this->GetActionInterval(60);

    // Is it time to take action?
    while (m_fInterval >= 1.0f)
    {
        // Increase counter.
        m_nCounter++;
        // Fade in or out depending on how many counts have elapsed.
        if ((m_nCounter >= 50) && (m_nCounter < 150))
        {
            // if (m_fAlpha < 1.0f) {
            m_fAlpha += 0.025;
            // }
        }
        else if ((m_nCounter >= 150) && (m_nCounter < 250))
        {
            if (m_fAlpha > 0.0f)
            {
                m_fAlpha -= 0.025;
            }
        }
        else if (m_nCounter >= 250)
        {
            // Go to the title screen introduction.
            m_nGameProgress = TITLE_INTRO;
        }
        // Decrease interval.
        m_fInterval -= 1.0f;
    }

    // Exit method if user presses any key.
    if (m_gsKeyboard.GetBufferedKey() > 0)
    {
        // Go to the title screen introduction.
        m_nGameProgress = TITLE_INTRO;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Set the alpha value (transparency) for the font.
    m_gsGameFont.SetModulateColor(-1.0, -1.0, -1.0, m_fAlpha);

    // Display a message centered in the screen.
    m_gsGameFont.SetText("LITTLE COMPUTER PERSON");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight()) / 2 + 16);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("PRESENTS");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight()) / 2 - 16);
    m_gsGameFont.Render();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != GAME_INTRO)
    {
        // Remember where we've come from.
        m_nOldGameProgress = GAME_INTRO;
        // Is music not already playing?
        if (!m_gsSound.IsMusicPlaying(MUSIC_TITLE))
        {
            // Start playing music at full volume, normal speed and looping infinitely.
            m_gsSound.PlayMusic(MUSIC_TITLE, m_GameSettings.nMusicVolume, 1.0f, TRUE);
        }
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::TitleIntro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::TitleIntro()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fAlpha = 0.0f;
        // Reset scale, rotation and scroll of title image.
        m_gsTitleSprite.SetScaleXY(1.0f, 1.0f);
        m_gsTitleSprite.SetRotateX(0.0f);
        m_gsTitleSprite.SetRotateY(0.0f);
        m_gsTitleSprite.SetRotateZ(0.0f);
        m_gsTitleSprite.SetScrollX(0.0f);
        m_gsTitleSprite.SetScrollY(0.0f);
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Title Image Animation /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are images not entirely solid?
    if (m_fAlpha < 1.0f)
    {
        // Decrease transparency.
        m_fAlpha += (this->GetActionInterval(60) * 0.025f);
    }
    else
    {
        // Go to the title screen section.
        m_nGameProgress = TITLE_SCREEN;
        // No transparency.
        m_fAlpha = 1.0f;
    }

    // Set the transparency of the title image.
    m_gsTitleSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, m_fAlpha);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Render the background, with transparency.
    this->RenderBackground(m_fAlpha);

    // Position the title image centered towards the top of the screen.
    m_gsTitleSprite.SetDestX((INTERNAL_RES_X - m_gsTitleSprite.GetScaledWidth()) / 2);
    m_gsTitleSprite.SetDestY(208);

    // Render the title image to the back surface.
    m_gsTitleSprite.Render();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != TITLE_INTRO)
    {
        // Remember where we've come from.
        m_nOldGameProgress = TITLE_INTRO;
        // Reset the title image to normal size and rotation.
        m_gsTitleSprite.SetScaleXY(1.0f, 1.0f);
        m_gsTitleSprite.SetRotateZ(0.0f);
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::TitleScreen():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::TitleScreen()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare And Initialize Variables /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    int KeyList[4] = { GSK_ENTER, GSK_UP, GSK_DOWN, GSK_ESCAPE };

    static BOOL bAnimFlag = FALSE;
    static int  nAnimType = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_bWasKeyReleased = FALSE;
        m_nOptionSelected = -1;
        m_fInterval       = 0.0f;
        m_nCounter        = 0;
        bAnimFlag         = FALSE;
        nAnimType         = 0;
        // Clear all current menu items.
        m_gsGameMenu.ClearOptions();
        // Set menu with no title.
        m_gsGameMenu.SetTitle("");
        // Add menu options.
        m_gsGameMenu.AddOption("PLAY GAME    ");
        m_gsGameMenu.AddOption("VIEW HISCORES");
        m_gsGameMenu.AddOption("SET OPTIONS  ");
        m_gsGameMenu.AddOption("QUIT         ");
        // Highlight the first option.
        m_gsGameMenu.SetHighlight(0);
        // Reset scale, rotation and scroll of title image.
        m_gsTitleSprite.SetScaleXY(1.0f, 1.0f);
        m_gsTitleSprite.SetRotateX(0.0f);
        m_gsTitleSprite.SetRotateY(0.0f);
        m_gsTitleSprite.SetRotateZ(0.0f);
        m_gsTitleSprite.SetScrollX(0.0f);
        m_gsTitleSprite.SetScrollY(0.0f);
        // Set the number of active particles to 24.
        m_gsParticles.SetNumParticles(24);
        // Set initial values for title animation.
        m_fScale  = 0.005f;
        m_fRotate = -0.1f;
        // Is music not already playing?
        if (!m_gsSound.IsMusicPlaying(MUSIC_TITLE))
        {
            // Start playing music at full volume, normal speed and looping infinitely.
            m_gsSound.PlayMusic(MUSIC_TITLE, m_GameSettings.nMusicVolume, 1.0f, TRUE);
        }
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(4, KeyList))
    {
        // Set flag to indicate that all the keys were released.
        m_bWasKeyReleased = TRUE;
    }

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        // Was this key released?
        if (m_bWasKeyReleased)
        {
            // Highlight the previous option.
            m_gsGameMenu.HighlightPrev();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
            // Key is pressed.
            m_bWasKeyReleased = FALSE;
        }
        // Reset counter.
        m_nCounter = 0;
        break;
    // Was the down key pressed?
    case GSK_DOWN:
        // Was this key released?
        if (m_bWasKeyReleased)
        {
            // Highlight the next option.
            m_gsGameMenu.HighlightNext();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
            // Key is pressed.
            m_bWasKeyReleased = FALSE;
        }
        // Reset counter.
        m_nCounter = 0;
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
        // Was this key released?
        if (m_bWasKeyReleased)
        {
            // Save the selected menu option.
            m_nOptionSelected = m_gsGameMenu.GetHighlight();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_SELECT);
            // Key is pressed.
            m_bWasKeyReleased = FALSE;
        }
        // Reset counter.
        m_nCounter = 0;
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
        // Was this key released?
        if (m_bWasKeyReleased)
        {
            // Have the wuit option been highlighted?
            if (m_gsGameMenu.GetHighlight() == 3)
            {
                // Set game progress to quit game.
                m_nGameProgress = QUIT_GAME;
            }
            else
            {
                // Highlight the quit option.
                m_gsGameMenu.SetHighlight(3);
            }
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
            // Key is pressed.
            m_bWasKeyReleased = FALSE;
        }
        // Reset counter.
        m_nCounter = 0;
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Process menu command if selected.
    switch (m_nOptionSelected)
    {
    case 0:
        // Set game progress.
        m_nGameProgress = MODE_SELECT;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    case 1:
        // Set game progress.
        m_nGameProgress = VIEW_HISCORE;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    case 2:
        // Set game progress.
        m_nGameProgress = GAME_OPTIONS;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    case 3:
        // Set game progress.
        m_nGameProgress = QUIT_GAME;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    default:
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Timed Events and Animations //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Have the title image reached the maximum size?
    if (m_gsTitleSprite.GetScaleX() >= 1.1f)
    {
        m_gsTitleSprite.SetScaleX(1.1f);
        m_fScale  = -0.005;
    }
    // Have the title image reached the minimum size?
    else if (m_gsTitleSprite.GetScaleX() <= 0.9f)
    {
        m_gsTitleSprite.SetScaleX(0.9f);
        m_fScale = +0.005;
    }

    // Always do scale animation of the title image.
    m_gsTitleSprite.AddScaleX(+m_fScale * this->GetActionInterval(60));
    m_gsTitleSprite.AddScaleY(-m_fScale * this->GetActionInterval(60));

    // Are we animating the title image?
    if (bAnimFlag == TRUE)
    {
        // Scroll title image right on x-axis?
        if (nAnimType == 0)
        {
            m_gsTitleSprite.AddScrollX(+10.0f * this->GetActionInterval(60));
            // Have the title image scrolled all the way?
            if (m_gsTitleSprite.GetScrollX() <= (-m_gsTitleSprite.GetTextureWidth()+10))
            {
                m_gsTitleSprite.SetScrollX(0.0f);
                bAnimFlag = FALSE;
            }
        }
        // Scroll title image on left x-axis?
        else if (nAnimType == 1)
        {
            m_gsTitleSprite.AddScrollX(-10.0f * this->GetActionInterval(60));
            // Have the title image scrolled all the way?
            if (m_gsTitleSprite.GetScrollX() >= (m_gsTitleSprite.GetTextureWidth()-10))
            {
                m_gsTitleSprite.SetScrollX(0.0f);
                bAnimFlag = FALSE;
            }
        }
        // Scroll title image on down y-axis?
        else if (nAnimType == 2)
        {
            m_gsTitleSprite.AddScrollY(-5.0f * this->GetActionInterval(60));
            // Have the title image scrolled all the way?
            if (m_gsTitleSprite.GetScrollY() >= (m_gsTitleSprite.GetTextureHeight()-5))
            {
                m_gsTitleSprite.SetScrollY(0.0f);
                bAnimFlag = FALSE;
            }
        }
        // Scroll title image on up y-axis?
        else if (nAnimType == 3)
        {
            m_gsTitleSprite.AddScrollY(+5.0f * this->GetActionInterval(60));
            // Have the title image scrolled all the way?
            if (m_gsTitleSprite.GetScrollY() <= (-m_gsTitleSprite.GetTextureHeight()+5))
            {
                m_gsTitleSprite.SetScrollY(0.0f);
                bAnimFlag = FALSE;
            }
        }
        // Flip title image left around x-axis?
        else if (nAnimType == 4)
        {
            m_gsTitleSprite.AddRotateX(10.0f * this->GetActionInterval(60));
            // Have the title image flipped all the way?
            if (m_gsTitleSprite.GetRotateX() >= 360.0f)
            {
                m_gsTitleSprite.SetRotateX(0.0f);
                bAnimFlag = FALSE;
            }
        }
        // Flip title image right around x-axis?
        else if (nAnimType == 5)
        {
            m_gsTitleSprite.AddRotateX(-10.0f * this->GetActionInterval(60));
            // Have the title image flipped all the way?
            if (m_gsTitleSprite.GetRotateX() <= -360.0f)
            {
                m_gsTitleSprite.SetRotateX(0.0f);
                bAnimFlag = FALSE;
            }
        }
        // Flip title image down around y-axis?
        else if (nAnimType == 6)
        {
            m_gsTitleSprite.AddRotateY(10.0f * this->GetActionInterval(60));
            // Have the title image flipped all the way?
            if (m_gsTitleSprite.GetRotateY() >= 360.0f)
            {
                m_gsTitleSprite.SetRotateY(0.0f);
                bAnimFlag = FALSE;
            }
        }
        // Flip title image up around y-axis?
        else if (nAnimType == 7)
        {
            m_gsTitleSprite.AddRotateY(-10.0f * this->GetActionInterval(60));
            // Have the title image flipped all the way?
            if (m_gsTitleSprite.GetRotateY() <= -360.0f)
            {
                m_gsTitleSprite.SetRotateY(0.0f);
                bAnimFlag = FALSE;
            }
        }
    }

    // Add interval if action is to be taken 60 times a second.
    m_fInterval += this->GetActionInterval(60);

    // Is it time to take action?
    while (m_fInterval >= 1.0f)
    {
        // Do animation every 200 counts.
        if ((m_nCounter%200 == 0) && (bAnimFlag == FALSE) && (m_nCounter >= 120))
        {
            // Determine type of animation.
            nAnimType = rand()%8;
            bAnimFlag = TRUE;
        }
        // Have counter reached specified amount?
        if ((++m_nCounter >= 1800)  && (bAnimFlag == FALSE))
        {
            // Set game progress to view hiscores.
            m_nGameProgress = VIEW_HISCORE;
        }
        // Decrease interval.
        m_fInterval -= 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the title background.
    this->RenderBackground();

    // Position the title image centered towards the top of the screen.
    m_gsTitleSprite.SetDestX((INTERNAL_RES_X - m_gsTitleSprite.GetScaledWidth()) / 2);
    m_gsTitleSprite.SetDestY(208);

    // Render the title image to the back surface.
    m_gsTitleSprite.Render();

    // Position the menu centered in the bottom half of the screen.
    m_gsGameMenu.SetDestX((INTERNAL_RES_X - m_gsGameMenu.GetWidth()) / 2);
    m_gsGameMenu.SetDestY(((INTERNAL_RES_Y / 2) - m_gsGameMenu.GetHeight()) / 2);

    // Render the menu to the back surface.
    m_gsGameMenu.Render(m_gsMenuTextColor, m_gsMenuHighColor);

    // Render the current frame rate.
    this->RenderFrameRate();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave this section?
    if (m_nGameProgress != TITLE_SCREEN)
    {
        // Remember where we've come from.
        m_nOldGameProgress = TITLE_SCREEN;
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameOptions():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameOptions()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare And Initialize Variables /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    int KeyList[6] = { GSK_ENTER, GSK_UP, GSK_DOWN, GSK_LEFT, GSK_RIGHT, GSK_ESCAPE };

    char cTempString[64] = { 0 };

    BOOL bLeftPressed = TRUE;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_bWasKeyReleased = FALSE;
        m_nOptionSelected = -1;
        // Clear all current menu items.
        m_gsGameMenu.ClearOptions();
        // Set menu title.
        m_gsGameMenu.SetTitle("   SELECT AN OPTION:  ");
        // Highlight the first option.
        m_gsGameMenu.SetHighlight(1);
        // Set game display mode to current mode.
        m_GameSettings.bWindowedMode = this->IsWindowed();
        m_GameSettings.bEnableVSync  = m_gsDisplay.IsVSyncEnabled();
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Set Menu Options /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear all current menu items.
    m_gsGameMenu.ClearOptions();

    // Add menu options depending on settings.
    m_gsGameMenu.AddOption("");
    if (m_GameSettings.bIncreaseLevel)
    {
        m_gsGameMenu.AddOption("INCREASE LEVELS....YES");
    }
    else
    {
        m_gsGameMenu.AddOption("INCREASE LEVELS.....NO");
    }
    if (m_GameSettings.bPreviewDrop)
    {
        m_gsGameMenu.AddOption("PREVIEW DROP.......YES");
    }
    else
    {
        m_gsGameMenu.AddOption("PREVIEW DROP........NO");
    }
    m_gsGameMenu.AddOption("");
    if (m_GameSettings.bWindowedMode)
    {
        m_gsGameMenu.AddOption("FULLSCREEN MODE.....NO");
    }
    else
    {
        m_gsGameMenu.AddOption("FULLSCREEN MODE....YES");
    }
    if (m_GameSettings.bEnableVSync)
    {
        m_gsGameMenu.AddOption("VERTICAL SYNC......YES");
    }
    else
    {
        m_gsGameMenu.AddOption("VERTICAL SYNC.......NO");
    }
    if (m_GameSettings.bUseParticles)
    {
        m_gsGameMenu.AddOption("PARTICLE EFFECTS...YES");
    }
    else
    {
        m_gsGameMenu.AddOption("PARTICLE EFFECTS....NO");
    }
    m_gsGameMenu.AddOption("");
    sprintf(cTempString, "MUSIC VOLUME.......%03d", m_GameSettings.nMusicVolume);
    m_gsGameMenu.AddOption(cTempString);
    sprintf(cTempString, "SOUND FX VOLUME....%03d", m_GameSettings.nEffectsVolume);
    m_gsGameMenu.AddOption(cTempString);
    m_gsGameMenu.AddOption("");
    m_gsGameMenu.AddOption("");
    m_gsGameMenu.AddOption("         DONE         ");

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset the selected option.
    m_nOptionSelected = -1;

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(6, KeyList))
    {
        m_bWasKeyReleased = TRUE;
    }

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        if (m_bWasKeyReleased)
        {
            // Highlight previous option.
            m_gsGameMenu.HighlightPrev();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the down key pressed?
    case GSK_DOWN:
        if (m_bWasKeyReleased)
        {
            // Highlight next option.
            m_gsGameMenu.HighlightNext();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the left key pressed?
    case GSK_LEFT:
        if (m_bWasKeyReleased)
        {
            if (m_gsGameMenu.GetHighlight() != 11)
            {
                m_nOptionSelected = m_gsGameMenu.GetHighlight();
                bLeftPressed = TRUE;
            }
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the right key pressed?
    case GSK_RIGHT:
        if (m_bWasKeyReleased)
        {
            if (m_gsGameMenu.GetHighlight() != 11)
            {
                m_nOptionSelected = m_gsGameMenu.GetHighlight();
                bLeftPressed = FALSE;
            }
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
        if (m_bWasKeyReleased)
        {
            // Remeber the option selected.
            m_nOptionSelected = m_gsGameMenu.GetHighlight();
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
        if (m_bWasKeyReleased)
        {
            // Return to the title screen.
            m_nGameProgress = TITLE_INTRO;
            m_bWasKeyReleased = FALSE;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Act depending on option selected.
    switch (m_nOptionSelected)
    {
    // Determine wether to increase levels or not.
    case 1:
        if (m_GameSettings.bIncreaseLevel)
        {
            m_GameSettings.bIncreaseLevel = FALSE;
        }
        else
        {
            m_GameSettings.bIncreaseLevel = TRUE;
        }
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Determine wether to preview drops or not.
    case 2:
        if (m_GameSettings.bPreviewDrop)
        {
            m_GameSettings.bPreviewDrop = FALSE;
        }
        else
        {
            m_GameSettings.bPreviewDrop = TRUE;
        }
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Fullscreen or windowed mode?
    case 4:
        if (m_GameSettings.bWindowedMode)
        {
            m_GameSettings.bWindowedMode = FALSE;
        }
        else
        {
            m_GameSettings.bWindowedMode = TRUE;
        }
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Enable vertical sync?
    case 5:
        if (m_GameSettings.bEnableVSync)
        {
            m_GameSettings.bEnableVSync = FALSE;
        }
        else
        {
            m_GameSettings.bEnableVSync = TRUE;
        }
        // Enable or disable vertical sync.
        m_gsDisplay.EnableVSync(m_GameSettings.bEnableVSync);
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Use particle effects?
    case 6:
        if (m_GameSettings.bUseParticles)
        {
            m_GameSettings.bUseParticles = FALSE;
        }
        else
        {
            m_GameSettings.bUseParticles = TRUE;
        }
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Determine music volume.
    case 8:
        if (bLeftPressed)
        {
            m_GameSettings.nMusicVolume -= 15;
            if (m_GameSettings.nMusicVolume < 0)
            {
                m_GameSettings.nMusicVolume = 255;
            }
        }
        else
        {
            m_GameSettings.nMusicVolume += 15;
            if (m_GameSettings.nMusicVolume > 255)
            {
                m_GameSettings.nMusicVolume = 0;
            }
        }
        m_gsSound.SetMusicVolume(m_GameSettings.nMusicVolume);
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Determine sound effects volume.
    case 9:
        if (bLeftPressed)
        {
            m_GameSettings.nEffectsVolume -= 15;
            if (m_GameSettings.nEffectsVolume < 0)
            {
                m_GameSettings.nEffectsVolume = 255;
            }
        }
        else
        {
            m_GameSettings.nEffectsVolume += 15;
            if (m_GameSettings.nEffectsVolume > 255)
            {
                m_GameSettings.nEffectsVolume = 0;
            }
        }
        m_gsSound.SetSampleMaster(m_GameSettings.nEffectsVolume);
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Done setting options.
    case 12:
        // Go to the title intro method.
        m_nGameProgress = TITLE_INTRO;
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the title background.
    this->RenderBackground();

    // Center the menu horizontally and vertically on the screen.
    m_gsGameMenu.SetDestX((INTERNAL_RES_X - m_gsGameMenu.GetWidth())  / 2);
    m_gsGameMenu.SetDestY((INTERNAL_RES_Y - m_gsGameMenu.GetHeight()) / 2);

    // Render the menu to the back surface using the specified colors.
    m_gsGameMenu.Render(m_gsMenuTextColor, m_gsSelectColor);

    // Render the current frame rate.
    this->RenderFrameRate();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave this section?
    if (m_nGameProgress != GAME_OPTIONS)
    {
        // Remember where we've come from.
        m_nOldGameProgress = GAME_OPTIONS;
        // Have we changed the graphics mode?INTERNAL_RES_Y
        if (m_GameSettings.bWindowedMode != this->IsWindowed())
        {
            // Change to windowed or fullscreen mode as selected by the user.
            this->SetMode(640, 480, 32, m_GameSettings.bWindowedMode);
            // Set render scaling values
            this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
            // Has the mode change been successful?
            m_GameSettings.bWindowedMode = this->IsWindowed();
        }
        // Save ini settings.
        this->SaveSettings();
        // Reset variables.
        m_bIsInitialized  = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::ViewHiscore():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::ViewHiscore()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare And Initialize Variables /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    char szTempString[128] = { 0 };

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fInterval -= 0.0f;
        m_nCounter   = 0;
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Exit method if user presses any key.
    if (m_gsKeyboard.GetBufferedKey() > 0)
    {
        // Return to the title screen.
        m_nGameProgress = TITLE_INTRO;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Timed Events /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Add interval if action is to be taken 60 times a second.
    m_fInterval += this->GetActionInterval(60);

    // Is it time to take action?
    while (m_fInterval >= 1.0f)
    {
        // Have counter reached specified amount?
        if (++m_nCounter >= 900)
        {
            // Return to the title screen.
            m_nGameProgress = TITLE_INTRO;
        }
        // Decrease interval.
        m_fInterval -= 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the hiscore background.
    this->RenderBackground();

    // Display the hiscore headings.
    m_gsGameFont.SetDestX(32);
    m_gsGameFont.SetDestY(INTERNAL_RES_Y - (m_gsGameFont.GetTextHeight() * 9));
    m_gsGameFont.SetText("RANK NAME       CLEARED LEVEL SCORE ");
    m_gsGameFont.Render();

    // Display all the current hiscores.
    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        // Set color to highlight the first score
        if (nLoop == m_nScoreIndex)
        {
            m_gsGameFont.SetModulateColor(m_gsSelectColor);
        }
        // Set the position of the next hiscore.
        m_gsGameFont.SetDestY(INTERNAL_RES_Y - (m_gsGameFont.GetTextHeight() * (nLoop+11)));
        // Setup a temporary string with all the relevant hiscore data.
        sprintf(szTempString, " %02d  %s  %05d   %02d   %06ld",nLoop+1,
                m_GameScores[nLoop].szPlayerName,
                m_GameScores[nLoop].nLinesCleared,
                m_GameScores[nLoop].nPlayerLevel,
                m_GameScores[nLoop].lPlayerScore);
        // Assign the string and render it to the back surface.
        m_gsGameFont.SetText(szTempString);
        m_gsGameFont.Render();
        // Reset modulate color.
        m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    // Display a message centered at the bottom of the screen.
    m_gsGameFont.SetText("PRESS ANY KEY TO CONTINUE");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth()) / 2);
    m_gsGameFont.SetDestY(m_gsGameFont.GetTextHeight() * 2);
    m_gsGameFont.Render();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave this section?
    if (m_nGameProgress != VIEW_HISCORE)
    {
        // Remember where we've come from.
        m_nOldGameProgress = VIEW_HISCORE;
        // Reset variables.
        m_nScoreIndex    = 0;
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::NewHiscore():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::NewHiscore()
{

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare And Initialize Variables /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static int nCharCount  = 0;

    static char szTempString[12] = { 0 };

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_bWasKeyReleased = FALSE;
        m_nScoreIndex = 0;
        nCharCount    = 0;
        // Loop through all the hiscores starting from the lowest.
        for (int nLoop = MAX_SCORES-1; nLoop >= 0; nLoop--)
        {
            // Is the current score is greater than the hiscore?
            if (m_lGameScore > m_GameScores[nLoop].lPlayerScore)
            {
                // Save place where score should be inserted.
                m_nScoreIndex = nLoop;
            }
        }
        // Setup a temporary string for the player name.
        lstrcpy(szTempString, "..........");
        // Play the appropriate sound.
        m_gsSound.PlaySample(SAMPLE_GOOD_SCORE);
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetBufferedKey();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        // Go to the next character.
        szTempString[nCharCount]++;
        // Select only certain characters.
        if (szTempString[nCharCount] == 47)
        {
            szTempString[nCharCount] = 48;
        }
        else if (szTempString[nCharCount] == 58)
        {
            szTempString[nCharCount] = 65;
        }
        else if (szTempString[nCharCount] > 90)
        {
            szTempString[nCharCount] = 46;
        }
        // Play the appropriate sound.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the down key pressed?
    case GSK_DOWN:
        // Go to the previous character.
        szTempString[nCharCount]--;
        // Select only certain characters.
        if (szTempString[nCharCount] == 45)
        {
            szTempString[nCharCount] = 90;
        }
        else if (szTempString[nCharCount] == 47)
        {
            szTempString[nCharCount] = 46;
        }
        else if (szTempString[nCharCount] == 64)
        {
            szTempString[nCharCount] = 57;
        }
        // Play the appropriate sound.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the left key pressed?
    case GSK_LEFT:
        // Go to the previous letter.
        nCharCount--;
        if (nCharCount < 0)
        {
            nCharCount = 9;
        }
        // Play the appropriate sound.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the right key pressed?
    case GSK_RIGHT:
        // Go to the next letter.
        nCharCount++;
        if (nCharCount > 9)
        {
            nCharCount = 0;
        }
        // Play the appropriate sound.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the backspace key pressed?
    case GSK_BACK:
        // Are we not at the first letter?
        if (nCharCount > 0)
        {
            // Shift all letters one back starting at active letter.
            for (int nLoop = nCharCount-1; nLoop < 9; nLoop++)
            {
                szTempString[nLoop] = szTempString[nLoop+1];
            }
            // Reset last letter.
            szTempString[9] = 46;
            // Move on letter back.
            nCharCount--;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
        // Loop through all the scores lower than the current score.
        for (int nLoop = MAX_SCORES-1; nLoop > m_nScoreIndex; nLoop--)
        {
            // Shift scores to make place for the new score.
            m_GameScores[nLoop] = m_GameScores[nLoop-1];
        }
        // Save the player name and statistics.
        if( strcmp(szTempString, "..........") == 0 ) lstrcpy(szTempString, "JOHN.DOE..");
        lstrcpy(m_GameScores[m_nScoreIndex].szPlayerName, szTempString);
        m_GameScores[m_nScoreIndex].lPlayerScore  = m_lGameScore;
        m_GameScores[m_nScoreIndex].nPlayerLevel  = m_nGameLevel + 1;
        m_GameScores[m_nScoreIndex].nLinesCleared = m_nTotalLinesCleared;
        // Play the appropriate sound.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Progress to the next section.
        m_nGameProgress = VIEW_HISCORE;
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
        // Play the appropriate sound.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Progress to the next section.
        m_nGameProgress = VIEW_HISCORE;
        m_nScoreIndex = 0;
        break;
    default:
        // Was the key pressed a recognized key?
        if (((nKey >= GSK_A) && (nKey <= GSK_Z)) || ((nKey >= GSK_0) && (nKey <= GSK_9)))
        {
            // Assign key and go to the next letter.
            szTempString[nCharCount] = (char)nKey;
            nCharCount++;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
        }
        else if ((nKey == GSK_GREATER) || (nKey == GSK_SPACE))
        {
            // Assign key and go to the next letter.
            szTempString[nCharCount] = 46;
            nCharCount++;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
        }
        // Have we reached the end of the string?
        if (nCharCount > 9)
        {
            nCharCount = 9;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the hiscore background.
    this->RenderBackground();

    // Display a message centered on the screen.
    m_gsGameFont.SetDestXY(192, 248);
    m_gsGameFont.SetText("ENTER YOUR NAME:");
    m_gsGameFont.Render();

    // Display all the letters of the temporary string.
    for (int nLoop = 0; nLoop < 10; nLoop++)
    {
        // Set the screen coordinates for each letter.
        m_gsGameFont.SetDestXY((240+(nLoop*m_gsGameFont.GetLetterWidth())), 216);
        // Are we at the currently active letter?
        if (nLoop == nCharCount)
        {
            // Set modulate color to tranparent for the active letter.
            m_gsGameFont.SetModulateColor(m_gsSelectColor);
        }
        else
        {
            // Set modulate color to solid for other letters.
            m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
        // Render the selected letter.
        m_gsGameFont.RenderChar(szTempString[nLoop]);
    }

    // Reset the modulate color of the game font.
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave this section?
    if (m_nGameProgress != NEW_HISCORE)
    {
        // Remember where we've come from.
        m_nOldGameProgress = NEW_HISCORE;
        // Save hiscores to file.
        this->SaveHiscores();
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::ModeSelect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::ModeSelect()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare And Initialize Variables /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    int KeyList[4] = { GSK_ENTER, GSK_UP, GSK_DOWN, GSK_ESCAPE };

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_bWasKeyReleased = FALSE;
        m_nOptionSelected = -1;
        // Clear all current menu items.
        m_gsGameMenu.ClearOptions();
        // Set menu title.
        m_gsGameMenu.SetTitle(" TYPE OF GAME: ");
        // Add menu options.
        m_gsGameMenu.AddOption("  NORMAL GAME  ");
        m_gsGameMenu.AddOption(" ADVANCED GAME ");
        // Highlight the first option.
        m_gsGameMenu.SetHighlight(0);
        // Is music playing?
        if (m_gsSound.IsMusicPlaying(MUSIC_TITLE))
        {
            // Stop game music from playing.
            m_gsSound.StopMusic(MUSIC_TITLE);
        }
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(4, KeyList))
    {
        m_bWasKeyReleased = TRUE;
    }

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        if (m_bWasKeyReleased)
        {
            // Highlight previous option.
            m_gsGameMenu.HighlightPrev();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the down key pressed?
    case GSK_DOWN:
        if (m_bWasKeyReleased)
        {
            // Highlight next option.
            m_gsGameMenu.HighlightNext();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
        if (m_bWasKeyReleased)
        {
            // Remeber the option selected.
            m_nOptionSelected = m_gsGameMenu.GetHighlight();
            m_gsSound.PlaySample(SAMPLE_SELECT);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
        if (m_bWasKeyReleased)
        {
            // Return to the title screen.
            m_nGameProgress = TITLE_INTRO;
            m_bWasKeyReleased = FALSE;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Act depending on option selected.
    switch (m_nOptionSelected)
    {
    // Play in rugular mode.
    case 0:
        m_GameSettings.bChallengeMode = FALSE;
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Go to the play game method.
        m_nGameProgress = LEVEL_SELECT;
        break;
    // Play in challenge mode.
    case 1:
        m_GameSettings.bChallengeMode = TRUE;
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Go to the play game method.
        m_nGameProgress = LEVEL_SELECT;
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Render the background.
    this->RenderBackground();

    // Center the menu horizontally and vertically on the screen.
    m_gsGameMenu.SetDestX((INTERNAL_RES_X - m_gsGameMenu.GetWidth())  / 2);
    m_gsGameMenu.SetDestY((INTERNAL_RES_Y - m_gsGameMenu.GetHeight()) / 2);

    // Render the menu to the back surface using the specified colors.
    m_gsGameMenu.Render(m_gsMenuTextColor, m_gsSelectColor);

    // Display frame rate.
    this->RenderFrameRate();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != MODE_SELECT)
    {
        // Remember where we've come from.
        m_nOldGameProgress = MODE_SELECT;
        // Reset variables.
        m_bIsInitialized  = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::LevelSelect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::LevelSelect()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare And Initialize Variables /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    int KeyList[4] = { GSK_ENTER, GSK_UP, GSK_DOWN, GSK_ESCAPE };

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_bWasKeyReleased = FALSE;
        m_nOptionSelected = -1;
        // Clear all current menu items.
        m_gsGameMenu.ClearOptions();
        // Set menu title.
        m_gsGameMenu.SetTitle(" STARTING LEVEL: ");
        // Add menu options.
        m_gsGameMenu.AddOption("    LEVEL 01     ");
        m_gsGameMenu.AddOption("    LEVEL 02     ");
        m_gsGameMenu.AddOption("    LEVEL 03     ");
        m_gsGameMenu.AddOption("    LEVEL 04     ");
        m_gsGameMenu.AddOption("    LEVEL 05     ");
        m_gsGameMenu.AddOption("    LEVEL 06     ");
        m_gsGameMenu.AddOption("    LEVEL 07     ");
        m_gsGameMenu.AddOption("    LEVEL 08     ");
        m_gsGameMenu.AddOption("    LEVEL 09     ");
        m_gsGameMenu.AddOption("    LEVEL 10     ");
        // Highlight the first option.
        m_gsGameMenu.SetHighlight(0);
        // Is music playing?
        if (m_gsSound.IsMusicPlaying(MUSIC_TITLE))
        {
            // Stop game music from playing.
            m_gsSound.StopMusic(MUSIC_TITLE);
        }
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(4, KeyList))
    {
        m_bWasKeyReleased = TRUE;
    }

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        if (m_bWasKeyReleased)
        {
            // Highlight previous option.
            m_gsGameMenu.HighlightPrev();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the down key pressed?
    case GSK_DOWN:
        if (m_bWasKeyReleased)
        {
            // Highlight next option.
            m_gsGameMenu.HighlightNext();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
        if (m_bWasKeyReleased)
        {
            // Remeber the option selected.
            m_nOptionSelected = m_gsGameMenu.GetHighlight();
            m_gsSound.PlaySample(SAMPLE_SELECT);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
        if (m_bWasKeyReleased)
        {
            // Return to the title screen.
            m_nGameProgress = MODE_SELECT;
            m_bWasKeyReleased = FALSE;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has a valid option been selected?
    if (m_nOptionSelected != -1)
    {
        // Set the level depending on the option.
        m_nGameLevel = m_nOptionSelected;
        // Go to the play game method.
        m_nGameProgress = PLAY_GAME;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Set Level Properties /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has a valid option been selected?
    if (m_nOptionSelected != -1)
    {
        // Set level properties.
        this->SetLevelProperties();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Render the background.
    this->RenderBackground();

    // Center the menu horizontally and vertically on the screen.
    m_gsGameMenu.SetDestX((INTERNAL_RES_X - m_gsGameMenu.GetWidth())  / 2);
    m_gsGameMenu.SetDestY((INTERNAL_RES_Y - m_gsGameMenu.GetHeight()) / 2);

    // Render the menu to the back surface using the specified colors.
    m_gsGameMenu.Render(m_gsMenuTextColor, m_gsSelectColor);

    // Display frame rate.
    this->RenderFrameRate();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != LEVEL_SELECT)
    {
        // Remember where we've come from.
        m_nOldGameProgress = LEVEL_SELECT;
        // Reset variables.
        m_bIsInitialized  = FALSE;
    }

    // Are we about to start playing the game?
    if (m_nGameProgress == PLAY_GAME)
    {
        // Start playing music at full volume, normal speed and looping infinitely.
        m_gsSound.PlayMusic(MUSIC_GAME, m_GameSettings.nMusicVolume, 1.0f, TRUE);
        // Play good luck sample.
        m_gsSound.PlaySample(SAMPLE_GOOD_LUCK);
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::PlayGame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::PlayGame()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        m_nOldGameProgress = m_nGameProgress;
        m_nGameProgress = PAUSE_GAME;
        m_gsSound.PlaySample(SAMPLE_OPTION);
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare Game Variables ///////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static int nMoveDirection = -1;

    static BOOL bIsBlockDropping = FALSE;
    static BOOL bIsBlockActive   = FALSE;
    static BOOL bIsBlockStopped  = FALSE;
    static BOOL bIsBlockMoving   = FALSE;

    static float fMoveFraction = 0.0f;
    static int   nMoveAdjust   = 0;

    int nPreviewRow = 0;

    int KeyList[1] = { GSK_SPACE };

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize Game Variables ////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we starting a new game?
    if (m_nOldGameProgress == LEVEL_SELECT)
    {
        // Reset class variables.
        m_nOldGameProgress   = PLAY_GAME;
        m_bWasKeyReleased    = FALSE;
        m_nNextBlockID       = -1;
        m_nLinesCleared      = 0;
        m_nTotalLinesCleared = 0;
        m_lGameScore         = 0;
        // Reset method variables.
        nMoveDirection    = -1;
        bIsBlockDropping  = FALSE;
        bIsBlockActive    = FALSE;
        bIsBlockStopped   = FALSE;
        bIsBlockMoving    = FALSE;
        fMoveFraction     = 0.0f;
        nMoveAdjust       = 0;
        // Set number of active particles to 0.
        m_gsParticles.SetNumParticles(0);
        // Clear the game area.
        this->ClearPlayArea();
        // Is challenge mode enabled?
        if (m_GameSettings.bChallengeMode)
        {
            // Place MAX_SPECIAL_ELEMENTS special elements randomly in the play area.
            for (int nLoop = 0; nLoop < MAX_SPECIAL_ELEMENTS; nLoop++)
            {
                // Determine special element coordinates.
                int nCol = rand()%AREA_COLS;
                int nRow = rand()%(AREA_ROWS/2);
                // Determine new coordinates if already contains a special element.
                while (m_PlayArea[nRow][nCol] == SPECIAL_ELEMENT)
                {
                    nCol = rand()%AREA_COLS;
                    nRow = rand()%(AREA_ROWS/2);
                }
                //  Assign special element.
                m_PlayArea[nRow][nCol] = SPECIAL_ELEMENT;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Dealing With User Input //////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(1, KeyList))
    {
        // Set flag to indicate that all the keys were released.
        m_bWasKeyReleased = TRUE;
    }

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetBufferedKey();

    // Clear the keyboard buffer.
    m_gsKeyboard.ClearBuffer();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        // Is the block not dropping?
        if (!bIsBlockDropping)
        {
            // Turn the block to the right.
            this->TurnBlock();
        }
        break;
    // Was the up key pressed?
    case GSK_DOWN:
        // Is the block not dropping?
        if (!bIsBlockDropping)
        {
            // Move the block downwards.
            nMoveDirection = MOVE_DOWN;
        }
        break;
    // Was the up key pressed?
    case GSK_LEFT:
        // Is the block not dropping?
        if (!bIsBlockDropping)
        {
            // Move the block to the left.
            nMoveDirection = MOVE_LEFT;
        }
        break;
    // Was the up key pressed?
    case GSK_RIGHT:
        // Is the block not dropping?
        if (!bIsBlockDropping)
        {
            // Move the block to the right.
            nMoveDirection = MOVE_RIGHT;
        }
        break;
    // Was the up key pressed?
    case GSK_SPACE:
        // Was this key released?
        if (m_bWasKeyReleased)
        {
            // Is the block not dropping?
            if (!bIsBlockDropping)
            {
                // Move the block downwards.
                nMoveDirection = MOVE_DOWN;
                // Start dropping the block.
                bIsBlockDropping = TRUE;
                m_fInterval      = 0.0f;
                // Play appropriate sound.
                m_gsSound.PlaySample(SAMPLE_DROP);
            }
            else
            {
                // No direction defined.
                nMoveDirection = -1;
                // Stop dropping the block.
                bIsBlockDropping = FALSE;
            }
            // Key is pressed.
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
        // Was this key released?
        if (m_bWasKeyReleased)
        {
            // Set game progress.
            m_nGameProgress = END_GAME;
            // Key is pressed.
            m_bWasKeyReleased = FALSE;
        }
        break;
    default:
        // Is the block not dropping?
        if (!bIsBlockDropping)
        {
            // No direction defined.
            nMoveDirection = -1;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Setting Up An Active Block ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Do we already have an active block?
    if (!bIsBlockActive)
    {
        // Has a next block been generated yet?
        if (m_nNextBlockID > 0)
        {
            // The active block is previous next block.
            m_nBlockID = m_nNextBlockID;
        }
        else
        {
            // Select a random block (1-7).
            m_nBlockID = (rand()%BLOCK_TYPES) + 1;
        }
        // Select the block depending on the block ID.
        switch(m_nBlockID)
        {
        case 1:
            m_ActiveBlock = m_Block_1;
            break;
        case 2:
            m_ActiveBlock = m_Block_2;
            break;
        case 3:
            m_ActiveBlock = m_Block_3;
            break;
        case 4:
            m_ActiveBlock = m_Block_4;
            break;
        case 5:
            m_ActiveBlock = m_Block_5;
            break;
        case 6:
            m_ActiveBlock = m_Block_6;
            break;
        case 7:
            m_ActiveBlock = m_Block_7;
            break;
        default:
            break;
        }
        // Select a random block (1-7) to be the next block.
        m_nNextBlockID = (rand()%BLOCK_TYPES) + 1;
        // Set new block postion centered at top of screen.
        m_nBlockRow = AREA_ROWS - BLOCK_ROWS;
        m_nBlockCol = ((AREA_COLS - BLOCK_COLS) / 2);
        // Can active block not be placed for the first time?
        if (!PlotBlock(m_nBlockRow, m_nBlockCol))
        {
            // Game is over.
            m_nGameProgress = GAME_OVER;
        }
        // The block is active and can start moving.
        bIsBlockActive = TRUE;
        bIsBlockMoving = TRUE;
        // Block has not been turned.
        m_bIsBlockTurned = FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Dealing With Block Movement //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the user specified a direction in which to move?
    switch(nMoveDirection)
    {
    case MOVE_DOWN:
        // Is the block dropping?
        if (bIsBlockDropping)
        {
            // Add interval if action is to be taken 60 times a second.
            m_fInterval += this->GetActionInterval(60);
            // Is it time to take action?
            while (m_fInterval >= 1.0f)
            {
                // Are we able to move the block down?
                if (PlotBlock(m_nBlockRow-1, m_nBlockCol))
                {
                    // Move one place down.
                    m_nBlockRow--;
                }
                // Decrease interval.
                m_fInterval -= 1.0f;
            }
        }
        else
        {
            // Are we able to move the block down?
            if (PlotBlock(m_nBlockRow-1, m_nBlockCol))
            {
                // Move one place down.
                m_nBlockRow--;
                // Play appropriate sound.
                m_gsSound.PlaySample(SAMPLE_MOVE);
            }
        }
        break;
    case MOVE_LEFT:
        // Is the block being adjusted from its current position?
        if (nMoveAdjust != 0)
        {
            // Are we able to move the block left one row down?
            if (PlotBlock(m_nBlockRow-1, m_nBlockCol-1))
            {
                // Are we able to move the block left?
                if (PlotBlock(m_nBlockRow, m_nBlockCol-1))
                {
                    // Move one place left.
                    m_nBlockCol--;
                    // Play appropriate sound.
                    m_gsSound.PlaySample(SAMPLE_MOVE);
                }
            }
        }
        else
        {
            // Are we able to move the block left?
            if (PlotBlock(m_nBlockRow, m_nBlockCol-1))
            {
                // Move one place left.
                m_nBlockCol--;
                // Play appropriate sound.
                m_gsSound.PlaySample(SAMPLE_MOVE);
            }
        }
        break;
    case MOVE_RIGHT:
        // Is the block being adjusted from its current position?
        if (nMoveAdjust != 0)
        {
            // Are we able to move the block right one row down?
            if (PlotBlock(m_nBlockRow-1, m_nBlockCol+1))
            {
                // Are we able to move the block right?
                if (PlotBlock(m_nBlockRow, m_nBlockCol+1))
                {
                    // Move one place right.
                    m_nBlockCol++;
                    // Play appropriate sound.
                    m_gsSound.PlaySample(SAMPLE_MOVE);
                }
            }
        }
        else
        {
            // Are we able to move the block right?
            if (PlotBlock(m_nBlockRow, m_nBlockCol+1))
            {
                // Move one place left.
                m_nBlockCol++;
                // Play appropriate sound.
                m_gsSound.PlaySample(SAMPLE_MOVE);
            }
        }
        break;
    }

    // Show a preview of where the block will land if it is dropped?
    if (m_GameSettings.bPreviewDrop)
    {
        // Start one down from the active block position.
        nPreviewRow = m_nBlockRow -1;
        // Plot downwards until the block can go no further.
        while (PlotBlock(nPreviewRow--, m_nBlockCol))
        {
        }
        // Do final adjustments.
        if (nPreviewRow < m_nBlockRow -1)
        {
            nPreviewRow+=2;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // NOTES ON BLOCK MOVEMENT: /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Block movement is mainly controlled by the m_nBlockRow and m_nBlockCol variables. For
    // every 1 added (or subtracted) to one of these variables, the block is actually moved
    // the entire size of the block (BLOCK_SIZE) in the chosen direction.

    // Downwards movement is timed by adding distances a fraction of the height of the block
    // until the height of the block is reached. When the height of the block is reached and if
    // the block is able to move down, the m_nBlockRow variable is decremented by 1, causing
    // the block to move down by the height of the block. The larger the fraction, the quicker
    // the height of the block will be reached, causing the block to move down quicker. The
    // smaller the fraction, the longer it will take.

    // In order to create the illusion of smooth movement the fractions added to determine when
    // the block should move is used as a modifier to the block position when it is rendered as
    // follows: (actual block postion + fraction of height). This means that the block will be
    // displayed ahead of its actual position until the fractions equal the height of the block,
    // at which time the process will repeat itself.

    // Since we only check wether a block can be moved (or not) every time when the fractions
    // equal the height of the block, this means that when the block has actually reached the
    // position where it can no longer be moved, the fractions will continue to be drawn
    // making it seem asif the block has moved passed the point where it should have stopped.
    // To prevent this, we only add the fractions to the block position modifier (nMoveAdjust)
    // when the block can actually move down, although the fractions are still being added and
    // tested to the height of the block to allow the user to make some final adjustments before
    // the block is actually placed.

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Add the movement distance to previous fractions.
    // fMoveFraction += m_nBlockMoveDistance;
    fMoveFraction += this->GetActionInterval(m_nBlockMoveDistance);

    // If the accumelated fractions is equal to or greater than the height of the
    // block, it's time to move the block downwards by the height of the block.
    if (fMoveFraction >= BLOCK_SIZE)
    {
        // Are we able to move the block down?
        if (PlotBlock(m_nBlockRow-1, m_nBlockCol))
        {
            // Move one place down.
            m_nBlockRow--;
        }
        else
        {
            // Place the active block into the play area.
            PlaceBlock(m_nBlockRow, m_nBlockCol);
            // Block has stopped moving.
            bIsBlockMoving = FALSE;
            // Block has stopped dropping.
            bIsBlockDropping = FALSE;
        }
        // Reset fractions to restart counting.
        fMoveFraction -= BLOCK_SIZE;
    }

    // Are we able to move down from the current position?
    if (PlotBlock(m_nBlockRow-1, m_nBlockCol))
    {
        // Adjust the display postion of the block by a fraction of the block height to create
        // the illusion that the block is moving down smootly instead of BLOCK_SIZE at a time.
        nMoveAdjust = (int)fMoveFraction;
        // Block is still moving.
        bIsBlockStopped = FALSE;
    }
    else
    {
        // Has the block stopped?
        if (!bIsBlockStopped)
        {
            // Block has stopped.
            bIsBlockStopped = TRUE;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_PLACE);
        }
        // Do not adjust the display position.
        nMoveAdjust = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Dealing With Clearing Lines //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the block stopped moving downwards?
    if (!bIsBlockMoving)
    {
        // Find any lines that need to be cleared.
        if (this->FindPlayAreaRowToClear() >= 0)
        {
            // Clear all the lines indicated.
            m_nGameProgress = CLEAR_LINES;
        }
        // Time to process the next block.
        bIsBlockActive = FALSE;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////
    // Dealing With Leveling Up /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we in normal mode?
    if (!m_GameSettings.bChallengeMode)
    {
        // Are we allowed to go to the next level?
        if ((m_GameSettings.bIncreaseLevel) && (m_nGameLevel < MAX_LEVEL))
        {
            // Have we reached the target for this level?
            if (m_nLinesCleared >= m_nLinesToNextLevel)
            {
                // Determine if extra lines were cleared.
                m_nLinesCleared -= m_nLinesToNextLevel;
                // Go to next game level.
                m_nGameLevel++;
                // Play appropriate sound.
                m_gsSound.PlaySample(SAMPLE_WELL_DONE);
                // Go to the level up method.
                m_nGameProgress = LEVEL_UP;
            }
        }
    }
    // We are in challenge mode.
    else
    {
        // Has the level been cleared of special elements?
        if (m_nSpecialElements <= 0)
        {
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_FANTASTIC);
            // Clear the game area.
            this->ClearPlayArea();
            // Place MAX_SPECIAL_ELEMENTS special elements randomly in the play area.
            for (int nLoop = 0; nLoop < MAX_SPECIAL_ELEMENTS; nLoop++)
            {
                // Determine special element coordinates.
                int nCol = rand()%AREA_COLS;
                int nRow = rand()%(AREA_ROWS/2);
                // Determine new coordinates if already contains a special element.
                while (m_PlayArea[nRow][nCol] == SPECIAL_ELEMENT)
                {
                    nCol = rand()%AREA_COLS;
                    nRow = rand()%(AREA_ROWS/2);
                }
                //  Assign special element.
                m_PlayArea[nRow][nCol] = SPECIAL_ELEMENT;
            }
            // Are we allowed to go to the next level?
            if ((m_GameSettings.bIncreaseLevel) && (m_nGameLevel < MAX_LEVEL))
            {
                // Go to next game level.
                m_nGameLevel++;
                // Go to the level up method.
                m_nGameProgress = LEVEL_UP;
            }
            else
            {
                // Play the same level again.
                this->SetLevelProperties();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All The Rendering /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the game background.
    this->RenderBackground();

    // Display the play area on screen.
    this->RenderPlayArea();

    // Are we showing a preview block?
    if (m_GameSettings.bPreviewDrop)
    {
        // Render the preview block as far down in the play area as it can go.
        RenderBlock(this->PlayAreaColToX(m_nBlockCol), this->PlayAreaRowToY(nPreviewRow),
                    m_ActiveBlock, 0.35f);
    }

    // Is the active block moving downwards?
    if (bIsBlockMoving)
    {
        // Draw the active block at the current position in the play area, adjusting the vertical
        // position, to create the illusion of smooth downwards movement.
        RenderBlock(this->PlayAreaColToX(m_nBlockCol), this->PlayAreaRowToY(m_nBlockRow) -
                    nMoveAdjust, m_ActiveBlock, 1.0f);
    }

    // Render particle effects.
    this->RenderParticles();

    // Display the next block on screen.
    this->RenderNextBlock();

    // Display game information.
    this->RenderGameStats();

    // Display frame rate.
    this->RenderFrameRate();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave the game method?
    if (m_nGameProgress != PLAY_GAME)
    {
        // Remember where we've come from.
        m_nOldGameProgress = PLAY_GAME;
        m_bWasKeyReleased  = FALSE;
    }

    // Are we going to the end game method?
    if (m_nGameProgress == END_GAME)
    {
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_OPTION);
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::PauseGame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::PauseGame()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Fade colors for background image and game blocks.
        m_gsBackgroundSprite.SetModulateColor(0.5f, 0.5f, 0.5f, 1.0f);
        m_gsBlockSpriteEx.SetModulateColor(0.5f, 0.5f, 0.5f, 1.0f);
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Is the game still paused?
    if (this->IsPaused())
    {
        // Clear the screen to screen the specified color.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Reset display.
        glLoadIdentity();
        // Display the game background.
        this->RenderBackground();
        // Display the play area on screen.
        this->RenderPlayArea();
        // Display the next block on screen.
        this->RenderNextBlock();
        // Display game information.
        this->RenderGameStats(0.5f);
        // Display frame rate.
        this->RenderFrameRate(0.5f);
        // Display a message that the game is paused centered on the screen.
        m_gsGameFont.SetText("GAME PAUSED");
        m_gsGameFont.SetDestX((INTERNAL_RES_X  - m_gsGameFont.GetTextWidth()) /2);
        m_gsGameFont.SetDestY(((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight())/2)+16);
        m_gsGameFont.Render();
        m_gsGameFont.SetText("PRESS P");
        m_gsGameFont.SetDestX((INTERNAL_RES_X  - m_gsGameFont.GetTextWidth()) /2);
        m_gsGameFont.SetDestY(((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight())/2)-16);
        m_gsGameFont.Render();
        // Swap buffers to display results.
        SwapBuffers(GetDevice());
    }
    else
    {
        // Return to the section we came from.
        m_nGameProgress = m_nOldGameProgress;
        // Remember where we've come from.
        m_nOldGameProgress = PAUSE_GAME;
        // Restore colors for background image and game blocks.
        m_gsBackgroundSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_gsBlockSpriteEx.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::ClearLines():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::ClearLines()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        m_nOldGameProgress = m_nGameProgress;
        m_nGameProgress = PAUSE_GAME;
        m_gsSound.PlaySample(SAMPLE_OPTION);
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare Game Variables ///////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static int nLinesToClear[4] = { -1, -1, -1, -1 };
    int nLineIndex = 0;

    BOOL bIsLineFull = TRUE;
    BOOL bNormalRender;

    static BOOL bLinesFound = FALSE;
    static BOOL bAllCleared = FALSE;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        for (int nLoop = 0; nLoop<4; nLoop++)
        {
            nLinesToClear[nLoop] = -1;
        }
        m_fAlpha    = 0.0f;
        bLinesFound = FALSE;
        bAllCleared = FALSE;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Determine Lines To Clear /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Have the lines not been determined before?
    while (!bLinesFound)
    {
        // Go through all the elements of the play area.
        for (int nRows = 0; nRows < AREA_ROWS; nRows++)
        {
            for (int nCols = 0; nCols < AREA_COLS; nCols++)
            {
                // Is any element in the current row empty?
                if (m_PlayArea[nRows][nCols] == 0)
                {
                    // Line is not full.
                    bIsLineFull = FALSE;
                }
            }
            // Were all the elements in the row non-zero?
            if (bIsLineFull)
            {
                // We can only clear four lines at a time.
                if (nLineIndex < 4)
                {
                    // Remember which line to clear.
                    nLinesToClear[nLineIndex] = nRows;
                    // Set index to remember next line.
                    nLineIndex++;
                }
            }
            // Assume line is full.
            bIsLineFull = TRUE;
        }
        // Set transparency.
        m_fAlpha = 1.0f;
        // All lines have been found.
        bLinesFound = TRUE;
    }

    // Were no lines found?
    if (nLinesToClear[0] < 0)
    {
        // There are no lines to fade.
        m_fAlpha = 0.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render Play Area And Fade Lines //////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Decrease alpha value to fade lines.
    m_fAlpha -= (this->GetActionInterval(60) * 0.03);
    if (m_fAlpha < 0.0f)
    {
        m_fAlpha = 0.0f;
    }

    // Reset the line index.
    nLineIndex = 0;

    // Are the lines still visible?
    if (m_fAlpha > 0.0f)
    {
        // Clear the screen to the specified clear color.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Reset display.
        glLoadIdentity();
        // Display the game background.
        this->RenderBackground();
        // Go through all the rows of the play area.
        for (int nRows = 0; nRows < AREA_ROWS; nRows++)
        {
            // Should the current line be cleared?
            if (nLinesToClear[nLineIndex] == nRows)
            {
                // Render line with transparency.
                bNormalRender = FALSE;
                // Set index to next line to be cleared.
                if (nLineIndex < 3)
                {
                    nLineIndex++;
                }
            }
            else
            {
                // Render the line normally.
                bNormalRender = TRUE;
            }
            // Go through all the columns of the play area.
            for (int nCols = 0; nCols < AREA_COLS; nCols++)
            {
                // Should the line be rendered normally?
                if (bNormalRender)
                {
                    // Set the alpha value of the sprite to solid.
                    m_gsBlockSpriteEx.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
                    // Select the type of element indicated by the play area.
                    m_gsBlockSpriteEx.SetFrame(m_PlayArea[nRows][nCols]);
                    // Determine the screen coordinates of the play area element.
                    m_gsBlockSpriteEx.SetDestX(this->PlayAreaColToX(nCols));
                    m_gsBlockSpriteEx.SetDestY(this->PlayAreaRowToY(nRows));
                    // Draw the play area element.
                    m_gsBlockSpriteEx.Render();
                }
                else
                {
                    // Set the alpha value of the sprite to solid.
                    m_gsBlockSpriteEx.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
                    // Select the type of element indicated by the play area.
                    m_gsBlockSpriteEx.SetFrame(0);
                    // Determine the screen coordinates of the play area element.
                    m_gsBlockSpriteEx.SetDestX(this->PlayAreaColToX(nCols));
                    m_gsBlockSpriteEx.SetDestY(this->PlayAreaRowToY(nRows));
                    // Draw the play area element.
                    m_gsBlockSpriteEx.Render();
                    // Set the alpha value of the sprite to transparent.
                    m_gsBlockSpriteEx.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
                    // Select the type of element indicated by the play area.
                    m_gsBlockSpriteEx.SetFrame(m_PlayArea[nRows][nCols]);
                    // Determine the screen coordinates of the play area element.
                    m_gsBlockSpriteEx.SetDestX(this->PlayAreaColToX(nCols));
                    m_gsBlockSpriteEx.SetDestY(this->PlayAreaRowToY(nRows));
                    // Draw the play area element.
                    m_gsBlockSpriteEx.Render();
                    // Restore the transparency of the sprite to solid.
                    m_gsBlockSpriteEx.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
        }
        // Display the next block on screen.
        this->RenderNextBlock();
        // Display game information.
        this->RenderGameStats();
        // Display frame rate.
        this->RenderFrameRate();
        // Swap buffers to display results.
        SwapBuffers(GetDevice());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Clear Lines //////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Have all the lines been faded?
    if (m_fAlpha <= 0.0f)
    {
        // Find any lines that need to be cleared.
        int nLineToClear = this->FindPlayAreaRowToClear();
        // Was a full line found?
        if (nLineToClear > -1)
        {
            // Are we in special mode?
            if (m_GameSettings.bChallengeMode)
            {
                // Go through each element of the line to be cleared.
                for (int nCols = 0; nCols < AREA_COLS; nCols++)
                {
                    // Is the element a special element?
                    if (m_PlayArea[nLineToClear][nCols] == SPECIAL_ELEMENT)
                    {
                        // Give bonus score for clearing a special element.
                        m_lGameScore += 10 * m_nGameLevel;
                        // Decrease the number of special elements.
                        m_nSpecialElements--;
                        // Play an appropriate sound effect.
                        m_gsSound.PlaySample(SAMPLE_SPECIAL);
                    }
                }
            }
            // Clear the line indicated.
            this->ClearPlayAreaRow(nLineToClear);
            // Keep track of how many lines.
            m_nLinesCleared++;
            if (m_nTotalLinesCleared < 30000)
            {
                m_nTotalLinesCleared++;
            }
        }
        else
        {
            // All lines have been cleared.
            bAllCleared = TRUE;
            // Continue playing the game.
            m_nGameProgress = PLAY_GAME;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Add Score ////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Have all the lines been cleared?
    if (bAllCleared)
    {
        // Have four lines been cleared?
        if (nLinesToClear[3] != -1)
        {
            // Add to the game score and play the appropriate sound sample.
            m_lGameScore += (((SCORE_BASE * m_nGameLevel) + (SCORE_BASE * 4)) * 4);
            m_gsSound.PlaySample(SAMPLE_BIG_BOOM, 255);
            // Start particle effects at the first row.
            this->RenderParticles(48, nLinesToClear[0]);
        }
        // Have three lines been cleared?
        else if (nLinesToClear[2] != -1)
        {
            // Add to the game score and play the appropriate sound sample.
            m_lGameScore += (((SCORE_BASE * m_nGameLevel) + (SCORE_BASE * 3)) * 3);
            m_gsSound.PlaySample(SAMPLE_BOOM, 255);
            // Start particle effects at the first row.
            this->RenderParticles(36, nLinesToClear[0]);
        }
        // Have two lines been cleared?
        else if (nLinesToClear[1] != -1)
        {
            // Add to the game score and play the appropriate sound sample.
            m_lGameScore += (((SCORE_BASE * m_nGameLevel) + (SCORE_BASE * 2)) * 2);
            m_gsSound.PlaySample(SAMPLE_BOOM, 205);
            // Start particle effects at the first row.
            this->RenderParticles(24, nLinesToClear[0]);
        }
        // Have one line been cleared?
        else
        {
            // Add to the game score and play the appropriate sound sample.
            m_lGameScore += (((SCORE_BASE * m_nGameLevel) + (SCORE_BASE * 1)) * 1);
            m_gsSound.PlaySample(SAMPLE_BOOM, 155);
            // Start particle effects at the first row.
            this->RenderParticles(12, nLinesToClear[0]);
        }
        // Have the score exceeded the maximum?
        if (m_lGameScore > SCORE_MAX)
        {
            m_lGameScore = SCORE_MAX;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != CLEAR_LINES)
    {
        // Remember where we've come from.
        m_nOldGameProgress = CLEAR_LINES;
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::LevelUp():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::LevelUp()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        m_nOldGameProgress = m_nGameProgress;
        m_nGameProgress = PAUSE_GAME;
        m_gsSound.PlaySample(SAMPLE_OPTION);
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Declare And Initialize Variables /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static float fAdjustY = 0.0f;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fAlpha    = 1.0f;
        fAdjustY    = 0.0f;
        // Set level properties.
        this->SetLevelProperties();
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Text Animation ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Add interval if action is to be taken 60 times a second.
    fAdjustY += this->GetActionInterval(60) * 2;

    // Is text not entirely transparent?
    if (m_fAlpha >= 0.0f)
    {
        // Make text more transparent
        m_fAlpha -= (this->GetActionInterval(60) * 0.02f);
    }
    else
    {
        // Continue playing the game.
        m_nGameProgress = PLAY_GAME;
        m_fAlpha = 0.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All The Rendering /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the game background.
    this->RenderBackground();

    // Display the play area on screen.
    this->RenderPlayArea();

    // Display the next block on screen.
    this->RenderNextBlock();

    // Display game information.
    this->RenderGameStats();

    // Display frame rate.
    this->RenderFrameRate();

    // Render particle effects.
    this->RenderParticles();

    // Display a horizontally centered, upwards moving message on screen.
    m_gsGameFont.SetText("LEVEL UP!");
    m_gsGameFont.SetDestX((INTERNAL_RES_X  - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight()) / 2 +
                          int(fAdjustY) - 40);
    m_gsGameFont.SetModulateColor(-1.0f, -1.0f, -1.0f, m_fAlpha);
    m_gsGameFont.Render();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != LEVEL_UP)
    {
        // Remember where we've come from.
        m_nOldGameProgress = LEVEL_UP;
        // Reset the modulate color for the font.
        m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::EndGame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::EndGame()
{

    int KeyList[6] = { GSK_ENTER, GSK_UP, GSK_DOWN, GSK_Y, GSK_N, GSK_ESCAPE };

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    // if (m_bIsInitialized == FALSE) {
    // Initialize method variables.
    // m_bWasKeyReleased = FALSE;
    // Initialization completed.
    // m_bIsInitialized = TRUE;
    // }

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_bWasKeyReleased = FALSE;
        m_nOptionSelected = -1;
        // Clear keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Fade colors for background image and game blocks.
        m_gsBackgroundSprite.SetModulateColor(0.25f, 0.25f, 0.25f, 1.0f);
        m_gsBlockSpriteEx.SetModulateColor(0.25f, 0.25f, 0.25f, 1.0f);
        // Clear all current menu items.
        m_gsGameMenu.ClearOptions();
        // Set menu title.
        m_gsGameMenu.SetTitle("  END GAME?  ");
        // Highlight the first option.
        m_gsGameMenu.SetHighlight(0);
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Set Menu Options /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear all current menu items.
    m_gsGameMenu.ClearOptions();

    // Add menu options depending on settings.
    m_gsGameMenu.AddOption("     YES     ");
    m_gsGameMenu.AddOption("     NO      ");

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset the selected option.
    m_nOptionSelected = -1;

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(6, KeyList))
    {
        m_bWasKeyReleased = TRUE;
    }

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        if (m_bWasKeyReleased)
        {
            // Highlight previous option.
            m_gsGameMenu.HighlightPrev();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the down key pressed?
    case GSK_DOWN:
        if (m_bWasKeyReleased)
        {
            // Highlight next option.
            m_gsGameMenu.HighlightNext();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the up key pressed?
    case GSK_Y:
        if (m_bWasKeyReleased)
        {
            // Highlight yes option.
            m_gsGameMenu.SetHighlight(0);
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the up key pressed?
    case GSK_N:
        if (m_bWasKeyReleased)
        {
            // Highlight no option.
            m_gsGameMenu.SetHighlight(1);
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
        if (m_bWasKeyReleased)
        {
            // Remeber the option selected.
            m_nOptionSelected = m_gsGameMenu.GetHighlight();
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
        if (m_bWasKeyReleased)
        {
            // The no option was selected.
            m_nOptionSelected = 1;
            m_bWasKeyReleased = FALSE;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Act depending on option selected.
    switch (m_nOptionSelected)
    {
    // Determine wether to increase levels or not.
    case 0:
        // Stop playing the game.
        m_nGameProgress = GAME_OVER;
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Determine wether to preview drops or not.
    case 1:
        // Continue playing the game.
        m_nGameProgress = PLAY_GAME;
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the game background.
    this->RenderBackground();

    // Display the play area on screen.
    this->RenderPlayArea();

    // Display the next block on screen.
    this->RenderNextBlock();

    // Display game information.
    this->RenderGameStats(0.5f);

    // Display frame rate.
    this->RenderFrameRate(0.5f);

    // Center the menu horizontally and vertically on the screen.
    m_gsGameMenu.SetDestX((INTERNAL_RES_X - m_gsGameMenu.GetWidth())  / 2);
    m_gsGameMenu.SetDestY((INTERNAL_RES_Y - m_gsGameMenu.GetHeight()) / 2);

    // Render the menu to the back surface using the specified colors.
    m_gsGameMenu.Render(m_gsMenuTextColor, m_gsSelectColor);

    // Render the current frame rate.
    this->RenderFrameRate();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != END_GAME)
    {
        // Remember where we've come from.
        m_nOldGameProgress = END_GAME;
        // Reset colors for background image and game blocks.
        m_gsBackgroundSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_gsBlockSpriteEx.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Reset variables.
        m_bIsInitialized  = FALSE;
    }

    return TRUE;

    /*
       // Has method been initialized yet?
       if (m_bIsInitialized == FALSE) {
          // Initialize method variables.
          m_bWasKeyReleased = FALSE;
          // Clear keyboard buffer.
          m_gsKeyboard.ClearBuffer();
          // Fade colors for background image and game blocks.
          m_gsBackgroundSprite.SetModulateColor(0.5f, 0.5f, 0.5f, 1.0f);
          m_gsBlockSpriteEx.SetModulateColor(0.5f, 0.5f, 0.5f, 1.0f);
          // Initialization completed.
          m_bIsInitialized = TRUE;
          }

       /////////////////////////////////////////////////////////////////////////////////////////////
       // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
       /////////////////////////////////////////////////////////////////////////////////////////////

       // Check to see wether a key was pressed.
       int nKey = m_gsKeyboard.GetBufferedKey();

       // Act depending on key pressed.
       switch (nKey) {
          // Was the 'Y' key pressed?
          case GSK_Y:
    */
    /* Is the player score good enough to be a hiscore?
    if (m_lGameScore > m_GameScores[MAX_SCORES-1].lPlayerScore) {
       // Add the score to the hiscore table.
       m_nGameProgress = NEW_HISCORE;
       }
    else {
       // End game and go to title screen.
       m_nGameProgress = TITLE_INTRO;
       } */
    /*
             // Stop playing the game.
             m_nGameProgress = GAME_OVER;
             // Play appropriate sound.
             m_gsSound.PlaySample(SAMPLE_SELECT);
             break;
          // Was the 'N' key pressed?
          case GSK_N:
             // Continue playing the game.
             m_nGameProgress = PLAY_GAME;
             // Play appropriate sound.
             m_gsSound.PlaySample(SAMPLE_SELECT);
             break;
          }

       /////////////////////////////////////////////////////////////////////////////////////////////
       // Do All Rendering /////////////////////////////////////////////////////////////////////////
       /////////////////////////////////////////////////////////////////////////////////////////////

       // Clear the screen to screen the specified color.
       glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // Reset display.
       glLoadIdentity();

       // Display the game background.
       this->RenderBackground();

       // Display the play area on screen.
       this->RenderPlayArea();

       // Display the next block on screen.
       this->RenderNextBlock();

       // Display game information.
       this->RenderGameStats(0.5f);

       // Display frame rate.
       this->RenderFrameRate(0.5f);

       // Display a message centered in the screen.
       m_gsGameFont.SetText("END GAME?");
       m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
       m_gsGameFont.SetDestY((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight()) / 2 + 16);
       m_gsGameFont.Render();
       m_gsGameFont.SetText("Y OR N");
       m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
       m_gsGameFont.SetDestY((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight()) / 2 - 16);
       m_gsGameFont.Render();

       // Swap buffers to display results.
       SwapBuffers(GetDevice());

       /////////////////////////////////////////////////////////////////////////////////////////////
       // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
       /////////////////////////////////////////////////////////////////////////////////////////////

       // Are we leaving this section?
       if (m_nGameProgress != END_GAME) {
          // Remember where we've come from.
          m_nOldGameProgress = END_GAME;
          // Reset colors for background image and game blocks.
          m_gsBackgroundSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
          m_gsBlockSpriteEx.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
          // Reset variables.
          m_bIsInitialized  = FALSE;
          }
    */
    /* Are we ending the current game?
    if ((m_nGameProgress == NEW_HISCORE) || (m_nGameProgress == TITLE_INTRO)) {
       // Is music playing?
       if (m_gsSound.IsMusicPlaying(MUSIC_GAME)) {
          // Stop game music from playing.
          m_gsSound.StopMusic(MUSIC_GAME);
          }
       } */
    /*
       return TRUE;
    */
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GameOver():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::GameOver()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fInterval = 0.0f;
        m_fAlpha    = 1.0f;
        m_nCounter  = 0;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Fade All To Black ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Add interval if action is to be taken 60 times a second.
    m_fInterval += this->GetActionInterval(60);

    // Is it time to take action?
    while (m_fInterval >= 1.0f)
    {
        // Is text not entirely transparent?
        if (m_fAlpha > 0.0f)
        {
            // Make text more transparent
            m_fAlpha -= 0.01f;
        }
        else
        {
            // Cannot be less tha zero.
            m_fAlpha = 0.0f;
        }
        // Add to counter.
        if (m_nCounter++ >= 120)
        {
            // Is the player score good enough to be a hiscore?
            if (m_lGameScore > m_GameScores[MAX_SCORES-1].lPlayerScore)
            {
                // Add the score to the hiscore table.
                m_nGameProgress = NEW_HISCORE;
            }
            else
            {
                // Go to the view hiscore section.
                m_nGameProgress = VIEW_HISCORE;
            }
        }
        // Play game over sound after 10 counts.
        if (m_nCounter == 10)
        {
            m_gsSound.PlaySample(SAMPLE_GAME_OVER);
        }
        // Decrease interval.
        m_fInterval -= 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the game background.
    this->RenderBackground(m_fAlpha);

    // Display the play area on screen.
    this->RenderPlayArea(m_fAlpha);

    // Display the next block on screen.
    this->RenderNextBlock(m_fAlpha);

    // Display game information.
    this->RenderGameStats(m_fAlpha);

    // Display frame rate.
    this->RenderFrameRate(m_fAlpha);

    // Display a message centered in the screen.
    m_gsGameFont.SetText("GAME OVER");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY((INTERNAL_RES_Y - m_gsGameFont.GetTextHeight()) / 2);
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_gsGameFont.Render();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != GAME_OVER)
    {
        // Remember where we've come from.
        m_nOldGameProgress = GAME_OVER;
        // Clear the play area.
        this->ClearPlayArea();
        // Is music playing?
        if (m_gsSound.IsMusicPlaying(MUSIC_GAME))
        {
            // Stop game music from playing.
            m_gsSound.StopMusic(MUSIC_GAME);
        }
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::QuitGame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Blocks::QuitGame()
{

    // Make sure we have a valid display before drawing.
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do One Time Initialization ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has method been initialized yet?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fInterval = 0.0f;
        m_nCounter  = 0;
        m_fAlpha    = 1.0f;
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Is music playing?
        if (m_gsSound.IsMusicPlaying(MUSIC_TITLE))
        {
            // Stop game music from playing.
            m_gsSound.StopMusic(MUSIC_TITLE);
        }
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Exit method if user presses any key.
    if (m_gsKeyboard.GetBufferedKey() > 0)
    {
        // Quit the game.
        this->Quit();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Game Quit Animation ///////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Add interval if action is to be taken 60 times a second.
    m_fInterval += this->GetActionInterval(60);

    // Is it time to take action?
    if (m_fInterval >= 1.0f)
    {
        // Increase counter.
        m_nCounter++;
        // Fade in or out depending on how many counts have elapsed.
        if ((m_nCounter >= 350) && (m_nCounter < 400))
        {
            m_fAlpha -= 0.025;
        }
        // Decrease interval.
        m_fInterval -= 1.0f;
    }

    // Is it time to quit the game?
    if (m_nCounter >= 400)
    {
        // Quit the game.
        this->Quit();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do All Rendering /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to screen the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Display the game credits centered in the screen.
    m_gsGameFont.SetText("GAME CREDITS");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(432);
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("DEVELOPED BY");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(384);
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("LITTLE COMPUTER PERSON");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(352);
    m_gsGameFont.SetModulateColor(m_gsSelectColor.fRed, m_gsSelectColor.fGreen,
                                  m_gsSelectColor.fBlue, m_fAlpha);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("SPECIAL THANKS");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(304);
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("T BOTHA.....   N MEYER.....");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(272);
    m_gsGameFont.SetModulateColor(m_gsSelectColor.fRed, m_gsSelectColor.fGreen,
                                  m_gsSelectColor.fBlue, m_fAlpha);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("S NEL.......   S NOLTE.....");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(240);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("C PRINSLOO..   M PRINSLOO..");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(208);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("R SCHOEMAN..   B VAN ZYL...");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(176);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("J VAN ZYL...   MM VAN ZYL..");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(144);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("THANKS FOR PLAYING!");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(80);
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsGameFont.Render();
    m_gsGameFont.SetText("PRESS ANY KEY TO EXIT");
    m_gsGameFont.SetDestX((INTERNAL_RES_X - m_gsGameFont.GetTextWidth())  / 2);
    m_gsGameFont.SetDestY(32);
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsGameFont.Render();

    // Swap buffers to display results.
    SwapBuffers(GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Final Cleanup /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != QUIT_GAME)
    {
        // Remember where we've come from.
        m_nOldGameProgress = QUIT_GAME;
        // Reset variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Level Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::SetLevelProperties():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::SetLevelProperties()
{

    // Set game speed depending on level.
    switch (m_nGameLevel)
    {
    case 0:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 60; // 1.0f;
        break;
    case 1:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 80; // 75; // 1.24f;
        break;
    case 2 :
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 100; // 90; // 1.5f;
        break;
    case 3:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 120; // 105; // 1.75f;
        break;
    case 4:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 140; // 120; // 2.0f;
        break;
    case 5:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 160; // 144; // 2.4f;
        break;
    case 6:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 180; // 168; // 2.8f;
        break;
    case 7:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 200; // 192; // 3.2f;
        break;
    case 8:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 10;
        }
        m_nBlockMoveDistance = 220; // 216; // 3.6f;
        break;
    case 9:
        if (m_GameSettings.bChallengeMode)
        {
            m_nSpecialElements = MAX_SPECIAL_ELEMENTS;
        }
        else
        {
            m_nLinesToNextLevel = 0;
        }
        m_nBlockMoveDistance = 240; // 4.0f;
        break;
    default:
        break;
    }

}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Block Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::PlotBlock():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if block can be place in the play area, FALSE if not.
//==============================================================================================

BOOL GS_Blocks::PlotBlock(int nBlockRow, int nBlockCol)
{

    // Check each element of the active block.
    for (int nRow = 0; nRow < BLOCK_ROWS; nRow++)
    {
        for (int nCol = 0; nCol < BLOCK_COLS; nCol++)
        {
            // Is the element in question a non-zero element?
            if (m_ActiveBlock.GetData(BLOCK_ROWS-nRow-1, nCol) > 0)
            {
                // Is the element in question outside the game area?
                if ((nBlockRow+nRow >= AREA_ROWS) || (nBlockRow+nRow < 0) ||
                        (nBlockCol+nCol >= AREA_COLS) || (nBlockCol+nCol < 0))
                {
                    // Block cannot be placed.
                    return FALSE;
                }
                // Element is inside the game area.
                else
                {
                    // Is the space where the block must be placed already filled?
                    if (m_PlayArea[nBlockRow+nRow][nBlockCol+nCol] > 0)
                    {
                        // Block cannot be placed.
                        return FALSE;
                    }
                }
            } // end if m_ActiveBlock
        } // end for nCol
    } // end for nRow

    // Block can be placed.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::PlaceBlock():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::PlaceBlock(int nBlockRow, int nBlockCol, BOOL bShowBlock)
{

    // Check each element of the active block.
    for (int nRow = 0; nRow < BLOCK_ROWS; nRow++)
    {
        for (int nCol = 0; nCol < BLOCK_COLS; nCol++)
        {
            // Is the active block element a non-zero element?
            if (m_ActiveBlock.GetData(BLOCK_ROWS-nRow-1, nCol) > 0)
            {
                // Is active block element inside the play field?
                if ((nBlockRow+nRow < AREA_ROWS) && (nBlockRow+nRow >= 0) &&
                        (nBlockCol+nCol < AREA_COLS) && (nBlockCol+nCol >= 0))
                {
                    // Should we show the block or clear it?
                    if (bShowBlock == TRUE)
                    {
                        // Place the active block element in the play area.
                        m_PlayArea[nBlockRow+nRow][nBlockCol+nCol] =
                            m_ActiveBlock.GetData(BLOCK_ROWS-nRow-1, nCol);
                    }
                    else
                    {
                        // Clear the active block element in the play area.
                        m_PlayArea[nBlockRow+nRow][nBlockCol+nCol] = 0;
                    }
                }
            } // end if m_ActiveBlock
        } // end for nCol
    } // end for nRow
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::TurnBlock():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::TurnBlock()
{

    // Do not turn the square block.
    if (m_nBlockID == 3)
    {
        return;
    }

    static int nOldBlockID = m_nBlockID;
    static int nNumTurns   = 0;

    // Has the block been turned before?
    if (m_bIsBlockTurned == FALSE)
    {
        // Save the active block ID.
        nOldBlockID = m_nBlockID;
        // Block has not been turned.
        nNumTurns = 0;
    }

    // Save the active block position.
    int nOldBlockRow = m_nBlockRow;
    int nOldBlockCol = m_nBlockCol;

    // Save the active block to a temporary block.
    GameBlock TempBlock = m_ActiveBlock;

    // Turn the active block to the right.
    m_ActiveBlock.TurnRight();

    // Block has been turned once more.
    nNumTurns++;

    // Has the block been turned all the way around?
    if (nNumTurns >= 5)
    {
        nNumTurns = 1;
    }

    // Modify block position to keep block centered when turning.
    switch (m_nBlockID)
    {
    case 1:
        if (nNumTurns==1)
        {
            m_nBlockRow+=1;
            m_nBlockCol-=2;
        }
        else if (nNumTurns==2)
        {
            m_nBlockRow+=2;
            m_nBlockCol+=2;
        }
        else if (nNumTurns==3)
        {
            m_nBlockRow-=2;
            m_nBlockCol+=1;
        }
        else if (nNumTurns==4)
        {
            m_nBlockRow-=1;
            m_nBlockCol-=1;
        }
        break;
    case 2:
        if (nNumTurns==1)
        {
            m_nBlockRow+=2;
            m_nBlockCol-=1;
        }
        else if (nNumTurns==2)
        {
            m_nBlockRow+=1;
            m_nBlockCol+=2;
        }
        else if (nNumTurns==3)
        {
            m_nBlockRow-=2;
            m_nBlockCol+=1;
        }
        else if (nNumTurns==4)
        {
            m_nBlockRow-=1;
            m_nBlockCol-=2;
        }
        break;
    case 4:
        if (nNumTurns==1)
        {
            m_nBlockRow+=1;
            m_nBlockCol+=0;
        }
        else if (nNumTurns==2)
        {
            m_nBlockRow+=0;
            m_nBlockCol+=1;
        }
        else if (nNumTurns==3)
        {
            m_nBlockRow-=1;
            m_nBlockCol+=0;
        }
        else if (nNumTurns==4)
        {
            m_nBlockRow-=0;
            m_nBlockCol-=1;
        }
        break;
    case 5:
        if (nNumTurns==1)
        {
            m_nBlockRow+=2;
            m_nBlockCol-=1;
        }
        else if (nNumTurns==2)
        {
            m_nBlockRow+=1;
            m_nBlockCol+=2;
        }
        else if (nNumTurns==3)
        {
            m_nBlockRow-=2;
            m_nBlockCol+=1;
        }
        else if (nNumTurns==4)
        {
            m_nBlockRow-=1;
            m_nBlockCol-=2;
        }
        break;
    case 6: // Fall through.
    case 7:
        if (nNumTurns==1)
        {
            m_nBlockRow+=1;
            m_nBlockCol-=1;
        }
        else if (nNumTurns==2)
        {
            m_nBlockRow+=1;
            m_nBlockCol+=2;
        }
        else if (nNumTurns==3)
        {
            m_nBlockRow-=2;
            m_nBlockCol+=0;
        }
        else if (nNumTurns==4)
        {
            m_nBlockRow-=0;
            m_nBlockCol-=1;
        }
    default:
        break;
    }

    // Adjust the block to turn at the edges of the play area.
    switch (m_nBlockID)
    {
    case 1:
        if (nNumTurns==1)
        {
            if (m_nBlockRow==AREA_ROWS-3)
            {
                m_nBlockRow--;
            }
        }
        else if ((nNumTurns==2) || (nNumTurns==4))
        {
            if (m_nBlockCol==AREA_COLS-2)
            {
                m_nBlockCol-=2;
            }
            if (m_nBlockCol==AREA_COLS-3)
            {
                m_nBlockCol--;
            }
            if (m_nBlockCol==-1)
            {
                m_nBlockCol++;
            }
        }
        break;
    case 2:
        if (nNumTurns==1)
        {
            if (m_nBlockRow==AREA_ROWS-2)
            {
                m_nBlockRow--;
            }
        }
        else if (nNumTurns==2)
        {
            if (m_nBlockCol==AREA_COLS-2)
            {
                m_nBlockCol--;
            }
        }
        else if (nNumTurns==4)
        {
            if (m_nBlockCol==-2)
            {
                m_nBlockCol++;
            }
        }
        break;
    case 4:
        if (nNumTurns==2)
        {
            if (m_nBlockCol==-1)
            {
                m_nBlockCol++;
            }
        }
        else if (nNumTurns==4)
        {
            if (m_nBlockCol==AREA_COLS-3)
            {
                m_nBlockCol--;
            }
        }
        break;
    case 5:
        if (nNumTurns==1)
        {
            if (m_nBlockRow==AREA_ROWS-2)
            {
                m_nBlockRow--;
            }
        }
        else if (nNumTurns==2)
        {
            if (m_nBlockCol==AREA_COLS-2)
            {
                m_nBlockCol--;
            }
        }
        else if (nNumTurns==4)
        {
            if (m_nBlockCol==-2)
            {
                m_nBlockCol++;
            }
        }
        break;
    case 6: // Fall through.
    case 7:
        if (nNumTurns==2)
        {
            if (m_nBlockCol==AREA_COLS-2)
            {
                m_nBlockCol--;
            }
        }
        else if (nNumTurns==4)
        {
            if (m_nBlockCol==AREA_COLS-3)
            {
                m_nBlockCol--;
            }
        }
        break;
    default:
        break;
    }

    // Block has now been turned.
    m_bIsBlockTurned = TRUE;

    // Is the position occupied by the new block valid?
    if (!PlotBlock(m_nBlockRow, m_nBlockCol))
    {
        // Restore the old active block.
        m_ActiveBlock = TempBlock;
        // Restore old block postion.
        m_nBlockRow = nOldBlockRow;
        m_nBlockCol = nOldBlockCol;
        // Block was not turned.
        nNumTurns--;
        // Block is not turned.
        if (nNumTurns == 0)
        {
            m_bIsBlockTurned = FALSE;
        }
    }
    else
    {
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_TURN);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderBlock():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderBlock(int nDestX, int nDestY, int nBlockID, float fAlpha)
{

    GameBlock TempBlock;

    // Select the block depending on the block ID.
    switch(nBlockID)
    {
    case 1:
        TempBlock = m_Block_1;
        break;
    case 2:
        TempBlock = m_Block_2;
        break;
    case 3:
        TempBlock = m_Block_3;
        break;
    case 4:
        TempBlock = m_Block_4;
        break;
    case 5:
        TempBlock = m_Block_5;
        break;
    case 6:
        TempBlock = m_Block_6;
        break;
    case 7:
        TempBlock = m_Block_7;
        break;
    default:
        break;
    }

    // Set the alpha value to determine the transparency of the block.
    m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Go through all the elements of the block.
    for (int nRows = 0; nRows < BLOCK_ROWS; nRows++)
    {
        for (int nCols = 0; nCols < BLOCK_COLS; nCols++)
        {
            // Is the block element non-zero?
            if (TempBlock.GetData(BLOCK_ROWS-nRows-1, nCols) > 0)
            {
                // Select the type of block element to render next.
                m_gsBlockSpriteEx.SetFrame(TempBlock.GetData(BLOCK_ROWS-nRows-1, nCols));
                // Determine the screen coordinates at which to draw the block element.
                m_gsBlockSpriteEx.SetDestX(nDestX+nCols*BLOCK_SIZE);
                m_gsBlockSpriteEx.SetDestY(nDestY+nRows*BLOCK_SIZE);
                // Draw the block element.
                m_gsBlockSpriteEx.Render();
            }
        }
    }

    // Is sprite semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the sprite.
        m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderBlock():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderBlock(int nDestX, int nDestY, GameBlock& TempBlock, float fAlpha)
{

    // Set the alpha value to determine the transparency of the block.
    m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Go through all the elements of the block.
    for (int nRows = 0; nRows < BLOCK_ROWS; nRows++)
    {
        for (int nCols = 0; nCols < BLOCK_COLS; nCols++)
        {
            // Is the block element non-zero?
            if (TempBlock.GetData(BLOCK_ROWS-nRows-1, nCols) > 0)
            {
                // Select the type of block element to render next.
                m_gsBlockSpriteEx.SetFrame(TempBlock.GetData(BLOCK_ROWS-nRows-1, nCols));
                // Determine the screen coordinates at which to draw the block element.
                m_gsBlockSpriteEx.SetDestX(nDestX+nCols*BLOCK_SIZE);
                m_gsBlockSpriteEx.SetDestY(nDestY+nRows*BLOCK_SIZE);
                // Draw the block element.
                m_gsBlockSpriteEx.Render();
            }
        }
    }

    // Is sprite semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the sprite.
        m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderNextBlock():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderNextBlock(float fAlpha)
{

    // Determine the alpha component of the sprite (0.0f is transparent).
    m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Display a darkened area around the next block.
    for (int nRows = 0; nRows < 4; nRows++)
    {
        for (int nCols = 0; nCols < 4; nCols++)
        {
            // Select the first block element.
            m_gsBlockSpriteEx.SetFrame(0);
            // Determine the screen coordinates at which to draw the block element.
            m_gsBlockSpriteEx.SetDestX(500 + (nCols*m_gsBlockSpriteEx.GetScaledWidth()));
            m_gsBlockSpriteEx.SetDestY(348 + (nRows*m_gsBlockSpriteEx.GetScaledHeight()));
            // Draw the play block element.
            m_gsBlockSpriteEx.Render();
        }
    }

    // Display the next block on screen.
    this->RenderBlock(500, 332, m_nNextBlockID, fAlpha);

    // Is font semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the sprite.
        m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Play Area Methods ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::ClearPlayArea():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::ClearPlayArea()
{

    // Go through all the elements of the play area.
    for (int nRows = 0; nRows < AREA_ROWS; nRows++)
    {
        for (int nCols = 0; nCols < AREA_COLS; nCols++)
        {
            // Clear the selected element.
            m_PlayArea[nRows][nCols] = 0;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::FindPlayAreaRowToClear():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The number of the first row containing all non-zeroes, or -1 if no row were found.
//==============================================================================================

int GS_Blocks::FindPlayAreaRowToClear()
{

    BOOL bIsRowFull = TRUE;

    // Go through all the elements of the play area.
    for (int nRows = 0; nRows < AREA_ROWS; nRows++)
    {
        for (int nCols = 0; nCols < AREA_COLS; nCols++)
        {
            // Is any element in the current row empty?
            if (m_PlayArea[nRows][nCols] == 0)
            {
                // Line is not full.
                bIsRowFull = FALSE;
            }
        }
        // Were all the elements in the row non-zero?
        if (bIsRowFull)
        {
            // Return the row number.
            return nRows;
        }
        // Assume line is full.
        bIsRowFull = TRUE;
    }

    // No full lines were found.
    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::ClearPlayAreaRow():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::ClearPlayAreaRow(int nRowToClear)
{

    // Is the line within bounds?
    if ((nRowToClear < 0) || (nRowToClear >= AREA_ROWS))
    {
        return;
    }

    // Shift all the values of the play area down to remove the line.
    for (int nRows = nRowToClear; nRows < AREA_ROWS-1; nRows++)
    {
        for (int nCols = 0; nCols < AREA_COLS; nCols++)
        {
            // Set all the elements in this row to the elements of the row above it.
            m_PlayArea[nRows][nCols] = m_PlayArea[nRows+1][nCols];
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Blocks::PlayAreaColToXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

int GS_Blocks::PlayAreaColToX(int nCol)
{

    return (((INTERNAL_RES_X-AREA_COLS * BLOCK_SIZE) / 2) + nCol * BLOCK_SIZE);
}

////////////////////////////////////////////////////////////////////////////////////////////////

int GS_Blocks::PlayAreaRowToY(int nRow)
{

    return (nRow * BLOCK_SIZE);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderPlayAreaElement():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderPlayAreaElement(int nCol, int nRow, float fAlpha)
{

    // Determine the alpha component of the sprite (0.0f is transparent).
    m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Select the type of element indicated by the play area.
    m_gsBlockSpriteEx.SetFrame(m_PlayArea[nRow][nCol]);
    // Determine the screen coordinates at which to draw the play area element.
    m_gsBlockSpriteEx.SetDestX(this->PlayAreaColToX(nCol));
    m_gsBlockSpriteEx.SetDestY(this->PlayAreaRowToY(nRow));
    // Draw the play area element.
    m_gsBlockSpriteEx.Render();

    // Is sprite semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the sprite.
        m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderPlayArea():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderPlayArea(float fAlpha)
{

    // Determine the alpha component of the sprite (0.0f is transparent).
    m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Go through all the elements of the play area.
    for (int nRows = 0; nRows < AREA_ROWS; nRows++)
    {
        for (int nCols = 0; nCols < AREA_COLS; nCols++)
        {
            // Select the type of element indicated by the play area.
            m_gsBlockSpriteEx.SetFrame(m_PlayArea[nRows][nCols]);
            // Determine the screen coordinates at which to draw the play area element.
            m_gsBlockSpriteEx.SetDestX(this->PlayAreaColToX(nCols));
            m_gsBlockSpriteEx.SetDestY(this->PlayAreaRowToY(nRows));
            // Draw the play area element.
            m_gsBlockSpriteEx.Render();
        }
    }

    // Is sprite semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the sprite.
        m_gsBlockSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Other Rendering Methods /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderBackground():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderBackground(float fAlpha)
{

    // Set the alpha value to determine the transparency of the background.
    m_gsBackgroundSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Display the background image on the screen.
    m_gsBackgroundSprite.Render();

    // Is background semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the background.
        m_gsBackgroundSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderGameStats():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderGameStats(float fAlpha)
{

    // Set the modulate color for all the headings.
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, fAlpha);

    // Display all the stat headings.

    m_gsGameFont.SetText("SCORE:");
    m_gsGameFont.SetDestXY(16, 416);
    m_gsGameFont.Render();

    m_gsGameFont.SetText("LEVEL:");
    m_gsGameFont.SetDestXY(16, 288);
    m_gsGameFont.Render();

    m_gsGameFont.SetText("CLEAR:");
    m_gsGameFont.SetDestXY(16, 192);
    m_gsGameFont.Render();

    m_gsGameFont.SetText("LEFT:");
    m_gsGameFont.SetDestXY(16, 96);
    m_gsGameFont.Render();

    m_gsGameFont.SetText("NEXT");
    m_gsGameFont.SetDestXY(508, 320);
    m_gsGameFont.Render();

    // Set the modulate color for all the stats.
    m_gsGameFont.SetModulateColor(m_gsSelectColor.fRed, m_gsSelectColor.fGreen,
                                  m_gsSelectColor.fBlue, fAlpha);

    // Display the game score.
    m_gsGameFont.SetText(" %06ld", m_lGameScore);
    m_gsGameFont.SetDestXY(16, 384);
    m_gsGameFont.Render();

    // Display the game level.
    m_gsGameFont.SetText(" %02d", m_nGameLevel+1);
    m_gsGameFont.SetDestXY(16, 256);
    m_gsGameFont.Render();

    // Display how many lines were cleared.

    // Are we in normal mode?
    if (!m_GameSettings.bChallengeMode)
    {
        m_gsGameFont.SetText(" %02d", m_nLinesCleared);
    }
    else
    {
        m_gsGameFont.SetText(" %02d", MAX_SPECIAL_ELEMENTS - m_nSpecialElements);
    }
    m_gsGameFont.SetDestXY(16, 160);
    m_gsGameFont.Render();

    // Display how many lines are left.

    // Are we in normal mode?
    if (!m_GameSettings.bChallengeMode)
    {
        // Are we allowed to go up in levels?
        if ((m_GameSettings.bIncreaseLevel) && (m_nGameLevel < MAX_LEVEL))
        {
            m_gsGameFont.SetText(" %02d", m_nLinesToNextLevel - m_nLinesCleared);
        }
        else
        {
            m_gsGameFont.SetText(" NONE");
        }
    }
    // We are in challenge mode.
    else
    {
        // Are there any special elements left?
        if (m_nSpecialElements > 0)
        {
            m_gsGameFont.SetText(" %02d", m_nSpecialElements);
        }
        else
        {
            m_gsGameFont.SetText(" NONE");
        }
    }
    m_gsGameFont.SetDestXY(16, 64);
    m_gsGameFont.Render();

    // Reset the alpha value (transparency) of the font.
    m_gsGameFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderFrameRate():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderFrameRate(float fAlpha)
{

    // Set the transparency of the font (0.0f is totally transparent).
    m_gsGameFont.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Display the frame rate at the bottom right of the screen.
    m_gsGameFont.SetText("FPS:%0.0f", this->GetCurrentFrameRate());
    m_gsGameFont.SetDestXY(INTERNAL_RES_X - m_gsGameFont.GetTextWidth() -16, 16);
    m_gsGameFont.Render();

    // Is font semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the font.
        m_gsGameFont.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::RenderParticles():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Blocks::RenderParticles(int nNumParticles, int nAreaRow, int nAreaCol)
{

    // Is particle effects disabled?
    if (!m_GameSettings.bUseParticles)
    {
        return;
    }

    static float fAlpha    = 0.0f;
    static float fInterval = 0.0f;
    float fScale;

    // Is there one or more particles?
    if (nNumParticles > 0)
    {
        // Set the number of active particles.
        m_gsParticles.SetNumParticles(nNumParticles);
        // For each active particle.
        for (int nLoop = 0; nLoop < nNumParticles; nLoop++)
        {
            // Was an area column specified?
            if (nAreaCol < 0)
            {
                // Determine a random horizontal position within the play area.
                m_gsParticles.SetDestX(nLoop, PlayAreaColToX(0)
                                       + rand()%(PlayAreaColToX(AREA_COLS)-PlayAreaColToX(0)));
            }
            else
            {
                m_gsParticles.SetDestX(nLoop, PlayAreaColToX(nAreaCol));
            }
            // Was an area row specified?
            if (nAreaRow < 0)
            {
                // Determine a random vertical position within the play area.
                m_gsParticles.SetDestY(nLoop, PlayAreaRowToY(0)
                                       + rand()%(PlayAreaRowToY(AREA_ROWS) - PlayAreaRowToY(0)));
            }
            else
            {
                m_gsParticles.SetDestY(nLoop, PlayAreaRowToY(nAreaRow));
            }
            // How many particles are there?
            if (nNumParticles <= 24)
            {
                // Determine a new random movement speed.
                m_nAddParticleX[nLoop] = rand()%2+0;
                m_nAddParticleY[nLoop] = rand()%2+0;
                // Determine random particle scale (1.0 - 4.0).
                fScale = 1.0f + (float(rand()%22) / 7.0f);
            }
            else if (nNumParticles <= 32)
            {
                // Determine a new random movement speed.
                m_nAddParticleX[nLoop] = rand()%2+0;
                m_nAddParticleY[nLoop] = rand()%2+0;
                // Determine random particle scale (1.0 - 5.0).
                fScale = 1.0f + (float(rand()%25) / 6.0f);
            }
            else if (nNumParticles <= 48)
            {
                // Determine a new random movement speed.
                m_nAddParticleX[nLoop] = rand()%2+1;
                m_nAddParticleY[nLoop] = rand()%6+1;
                // Determine random particle scale (1.0 - 6.0).
                fScale = 1.0f + (float(rand()%26) / 5.0f);
            }
            // Set horizontal and vertical particle scale.
            m_gsParticles.SetScaleX(nLoop, fScale);
            m_gsParticles.SetScaleY(nLoop, fScale);
            // Set horizontal direction randomly.
            if (rand()%2 == 1)
            {
                m_nAddParticleX[nLoop] = -m_nAddParticleX[nLoop];
            }
        }
        // Make particles solid initialy.
        fAlpha = 1.0f;
        // Particles initialized.
        return;
    }

    // Assign the number of active particles.
    nNumParticles = m_gsParticles.GetNumParticles();

    // Are there no active particles?
    if (nNumParticles > 0)
    {
        // Add interval if action is to be taken 60 times a second.
        fInterval += this->GetActionInterval(60);
        // Is it time to take action?
        if (fInterval >= 1.0f)
        {
            // For each currently active particle.
            for (int nLoop = 0; nLoop < nNumParticles; nLoop++)
            {
                // Set the transparency for the specified particle.
                m_gsParticles.SetModulateColor(nLoop, -1.0f, -1.0f, -1.0f, fAlpha);
                // Move the specified particle on the x and y-axis.
                m_gsParticles.AddDestX(nLoop, m_nAddParticleX[nLoop]);
                m_gsParticles.AddDestY(nLoop, m_nAddParticleY[nLoop]);
                // Scale the specified particle.
                m_gsParticles.AddScaleX(nLoop, +0.025f);
                m_gsParticles.AddScaleY(nLoop, +0.025f);
            }
            // Reset interval.
            fInterval = 0.0f;
        }
        // Render all the particles.
        m_gsParticles.Render();
        // How many particles are there?
        if (nNumParticles <= 12)
        {
            // Increase transparency.
            fAlpha -= (this->GetActionInterval(60) * 0.02f);
        }
        else if (nNumParticles <= 24)
        {
            // Increase transparency.
            fAlpha -= (this->GetActionInterval(60) * 0.0175f);
        }
        else if (nNumParticles <= 36)
        {
            // Increase transparency.
            fAlpha -= (this->GetActionInterval(60) * 0.015f);
        }
        else
        {
            // Increase transparency.
            fAlpha -= (this->GetActionInterval(60) * 0.01f);
        }
    }

    // Are particles transparent?
    if (fAlpha <= 0.0f)
    {
        // Set number of active particles to 0.
        m_gsParticles.SetNumParticles(0);
        fInterval = 0.0f;
        fAlpha    = 0.0f;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Load/Save Methods ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::LoadSettings():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Blocks::LoadSettings()
{

    char szTempString[_MAX_PATH] = { 0 };
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, SETTINGS_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // Read game settings.
    int nResult =  gsIniFile.ReadInt("Game", "IncreaseLevel", -1);
    if (nResult >= 0)
    {
        m_GameSettings.bIncreaseLevel = nResult;
    }
    else
    {
        m_GameSettings.bIncreaseLevel = TRUE;
    }

    nResult = gsIniFile.ReadInt("Game", "PreviewDrop", -1);
    if (nResult >= 0)
    {
        m_GameSettings.bPreviewDrop = nResult;
    }
    else
    {
        m_GameSettings.bPreviewDrop = TRUE;
    }

    // Read display settings.
    nResult = gsIniFile.ReadInt("Display", "WindowMode", -1);
    if (nResult >= 0)
    {
        m_GameSettings.bWindowedMode = nResult;
    }
    else
    {
        m_GameSettings.bWindowedMode = TRUE;
    }

    nResult = gsIniFile.ReadInt("Display", "EnableVSync", -1);
    if (nResult >= 0)
    {
        m_GameSettings.bEnableVSync = nResult;
    }
    else
    {
        m_GameSettings.bEnableVSync = FALSE;
    }

    nResult = gsIniFile.ReadInt("Display", "UseParticles", -1);
    if (nResult >= 0)
    {
        m_GameSettings.bUseParticles = nResult;
    }
    else
    {
        m_GameSettings.bUseParticles = TRUE;
    }

    // Read sound settings.
    nResult = gsIniFile.ReadInt("Sound", "MusicVolume", -1);
    if (nResult >= 0)
    {
        m_GameSettings.nMusicVolume = nResult;
    }
    else
    {
        m_GameSettings.nMusicVolume = 90;
    }

    nResult = gsIniFile.ReadInt("Sound", "EffectsVolume", -1);
    if (nResult >= 0)
    {
        m_GameSettings.nEffectsVolume = nResult;
    }
    else
    {
        m_GameSettings.nEffectsVolume = 255;
    }

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::SaveSettings():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Blocks::SaveSettings()
{

    char szTempString[_MAX_PATH] = { 0 };
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, SETTINGS_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // Write game settings.
    gsIniFile.WriteInt("Game", "IncreaseLevel", m_GameSettings.bIncreaseLevel);
    gsIniFile.WriteInt("Game", "PreviewDrop",   m_GameSettings.bPreviewDrop);

    // Write display settings.
    gsIniFile.WriteInt("Display", "WindowMode",  m_GameSettings.bWindowedMode);
    gsIniFile.WriteInt("Display", "EnableVSync", m_GameSettings.bEnableVSync);
    gsIniFile.WriteInt("Display", "UseParticles",  m_GameSettings.bUseParticles);

    // Write sound settings.
    gsIniFile.WriteInt("Sound", "MusicVolume",   m_GameSettings.nMusicVolume);
    gsIniFile.WriteInt("Sound", "EffectsVolume", m_GameSettings.nEffectsVolume);

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::LoadHiscores():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Blocks::LoadHiscores()
{

    char szTempString[_MAX_PATH] = { 0 };
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, HISCORES_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // For every score in the hiscores array.
    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        // Determine the score heading.
        sprintf(szTempString, "Score%d", nLoop);
        // Load the player name.
        lstrcpy(m_GameScores[nLoop].szPlayerName,
                gsIniFile.ReadString(szTempString, "PlayerName", ".........."));
        // Load how many lines were cleared.
        int nResult = gsIniFile.ReadInt(szTempString, "LinesCleared", -1);
        if (nResult >= 0)
        {
            m_GameScores[nLoop].nLinesCleared = nResult;
        }
        else
        {
            m_GameScores[nLoop].nLinesCleared = 0;
        }
        // Load the player level.
        nResult = gsIniFile.ReadInt(szTempString, "PlayerLevel", -1);
        if (nResult >= 0)
        {
            m_GameScores[nLoop].nPlayerLevel = nResult;
        }
        else
        {
            m_GameScores[nLoop].nPlayerLevel = 0;
        }
        // Load the player score.
        long lResult = long(gsIniFile.ReadFloat(szTempString, "PlayerScore", -1));
        if (lResult >= 0)
        {
            m_GameScores[nLoop].lPlayerScore = lResult;
        }
        else
        {
            m_GameScores[nLoop].lPlayerScore = 0l;
        }
    }

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::SaveHiscores():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Blocks::SaveHiscores()
{

    char szTempString[_MAX_PATH] = { 0 };
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, HISCORES_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // For every score in the hiscores array.
    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        // Save the score heading.
        sprintf(szTempString, "Score%d", nLoop);
        // Save player name, lines cleared, level and score.
        gsIniFile.WriteString(szTempString,"PlayerName",   m_GameScores[nLoop].szPlayerName);
        gsIniFile.WriteInt(szTempString,   "LinesCleared", m_GameScores[nLoop].nLinesCleared);
        gsIniFile.WriteInt(szTempString,   "PlayerLevel",  m_GameScores[nLoop].nPlayerLevel);
        gsIniFile.WriteFloat(szTempString, "PlayerScore",(float)m_GameScores[nLoop].lPlayerScore);
    }

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Methods //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::SetRenderScaling():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the values used for upscaling or downscaling when rendering
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_Blocks::SetRenderScaling(int nWidth, int nHeight, bool bKeepAspect)
{
    // Is no scaling required?
    if( (nWidth == INTERNAL_RES_X) && (nHeight == INTERNAL_RES_Y) )
    {
        // Reset the default scaling values
        GS_OGLDisplay::SetScaleFactorX(1.0f);
        GS_OGLDisplay::SetScaleFactorY(1.0f);

        GS_OGLDisplay::SetRenderModX(0.0f);
        GS_OGLDisplay::SetRenderModY(0.0f);

        return;
    }

    // Determine the aspect ratio of the internal resolution
    float aspect = (float)INTERNAL_RES_X / (float)INTERNAL_RES_Y;

    // Set up default values for upscaling or downscaling
    float fAspectWidth  = (float)nWidth;
    float fAspectHeight = (float)nHeight;
    float fRenderModX = 0;
    float fRenderModY = 0;

    // Should we keep the aspect ratio?
    if( bKeepAspect )
    {
        // Is the horizontal dimension greater than the vertical?
        if( INTERNAL_RES_X >= INTERNAL_RES_Y )
        {
            // Try to scale the rendering to the given width
            fAspectWidth  = (float)nWidth;
            fAspectHeight = (float)fAspectWidth / aspect;
            fRenderModX = 0;
            fRenderModY = (nHeight - fAspectHeight) / 2.0f;

            // Have we scaled beyond the given height
            if( fAspectHeight > (float)nHeight )
            {
                // Scale the rendering to the given height
                fAspectHeight = (float)nHeight;
                fAspectWidth  = fAspectHeight * aspect;
                fRenderModX = (nWidth - fAspectWidth) / 2.0f;
                fRenderModY = 0;
            }
        }
        else
        {
            // Try to scale the rendering to the given height
            fAspectHeight = (float)nHeight;
            fAspectWidth  = fAspectHeight * aspect;
            fRenderModX = (nWidth - fAspectWidth) / 2.0f;
            fRenderModY = 0;

            // Have we scaled beyond the given width
            if( fAspectWidth > (float)nWidth )
            {
                // Scale the rendering to the given width
                fAspectWidth  = (float)nWidth;
                fAspectHeight = (float)fAspectWidth / aspect;
                fRenderModX = 0;
                fRenderModY = (nHeight - fAspectHeight) / 2.0f;
            }
        }
    }

    // Set the amount with which the rendering coordinates should be modified
    GS_OGLDisplay::SetRenderModX(fRenderModX);
    GS_OGLDisplay::SetRenderModY(fRenderModY);

    // Set the scale factor to effect upscaling or downscaling depending on the resolution
    GS_OGLDisplay::SetScaleFactorX((float)fAspectWidth  / (float)INTERNAL_RES_X);
    GS_OGLDisplay::SetScaleFactorY((float)fAspectHeight / (float)INTERNAL_RES_Y);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Blocks::GetActionInterval():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The interval into which any action or actions must be divided in order to be
//          completed in a second at the current frame rate of the application.
//==============================================================================================

float GS_Blocks::GetActionInterval(float fActionsPerSecond)
{

    // Convert frame time to seconds and multiply it by the number of actions to determine
    // what fraction of the action needs to be completed in order to complete the action in
    // one second (at the current frame rate).
    return ((this->GetFrameTime() / 1000) * fActionsPerSecond);
}


////////////////////////////////////////////////////////////////////////////////////////////////

// GS_Error::Report("GS_BLOCKS.CPP", 441, "Break Point!");
