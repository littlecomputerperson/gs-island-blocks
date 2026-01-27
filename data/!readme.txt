* Game graphics:

  image_01.pcx - Background image. Used as a background in the game.
               - 1024x512, 8-bit.
               - Note: Only the first 640x480 pixels, starting from the 
                 bottom left of the image, are displayed.

  image_02.tga - Title image. Used as the title on the title screen.
               - 512x256, 24-bit, alpha.

  image_03.tga - Font image, used for displaying text in the game.
               - 256x64, 24-bit, alpha.
               - Note: The font cosists of characters with dimensions
                 of 16x16 pixels. There are 20 characters per line.

  image_04.tga - Menu image, used for menus in the game.
               - 256x16, 24-bit, alpha.
               - Note: The menu image has 16 elements (of which only
                 the first 12 is used) of 16x16 pixels each, as 
                 follows: 

                 03 04 04 04 04 04 04 04 04 04 04 05
                 01 12 12 12 12 12 12 12 12 12 12 02
                 09 10 10 10 10 10 10 10 10 10 10 11
                 01 12 12 12 12 12 12 12 12 12 12 02
                 01 12 12 12 12 12 12 12 12 12 12 02
                 01 12 12 12 12 12 12 12 12 12 12 02
                 06 07 07 07 07 07 07 07 07 07 07 08

  image_05.tga - Blocks image, used to create the game blocks.
               - 256x32, 24-bit, alpha.
               - Note: There are a total of 9 elements of 20x20 pixels
                 each. The first element is used as a background for 
                 the play area and the rest to compose the individual 
                 blocks themselves.

  image_06.tga - Particle image, used for particle effects.
               - 16x16, 24-bit, alpha.

* Game music:

  music_01.mid - Music that continues to play in the backgound on the
                 title and menu screens.

  music_02.mid - Music that continues to play in the backgound while 
                 the game is being played.

* Game sound:

  sound_01.wav - Used when navigating through menus.
  sound_02.wav - Used when a menu item is selected.
  sound_03.wav - Used when 1, 2, or 3 lines are cleared.
  sound_04.wav - Used when 4 lines are cleared.
  sound_05.wav - Used when a block lands on another block.
  sound_06.wav - Used when a block is moved by the player.
  sound_07.wav - Used when a block is turned.
  sound_08.wav - Used when a block is dropped quickly.
  sound_09.wav - Used when a challenge block is cleared.
  sound_10.wav - Played just before the game is started (good luck).
  sound_11.wav - Played when going to the next level (well done).
  sound_12.wav - Played when level up in challenge mode (fantastic).
  sound_13.wav - Played when game is over (game over).
  sound_14.wav - Played when a high score is achieved (good score).
