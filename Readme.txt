
SPACE INVADERS ADVANCE (Dec-2004)
---------------------------------

1. SUMMARY 
	This is a demo of the oldie classic Space Invaders. 
	Ported to the Game boy advance and made with:
	*HAM (www.ngine.de)
	*Visual HAM (www.console-dev.de)

2. REQUIREMENTS TO RUN THE GBA ROM
	-I recommend Visual Boy Advance (vba.ngemu.com)
	-Sound Card
	-You can copy the GBA to a Flash card and play it on your GBA hardware
	
3. HOW TO PLAY THE DEMO
	- Left/Right 	=> moves your ship 
	- A/B 		=> fire	 
	
4. HOW TO COMPILE
	-The easiest way to compile the game is to get HAM which includes Visual HAM
	from www.ngine.de
	-Install HAM, run Visual HAM and open the project file invaders.vhw
	-Press F5 to compile or F7 to compile and run on the GBA emulator

5. CODE STURCTURE
	-All the graphics (gfx directory) were converted from bitmaps to char 
	representation using the tool gfx2gba included with HAM.
	-There's only one main.c file basically the main function initializes
	the HAM engine, the game variables and load all the graphics and sounds.
	-The main game loop is performed whenever a Vblank interrupt occurs; in HAM
	you define a callback function for that purpose and where we process input
	and update graphics, sound, collision detection, etc.

