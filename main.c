/*-------------------------------------
   Space Invaders Demo for GBA
   
   Author: Héctor Morales Piloni, MSc.
   Date:   November 27, 2004
--------------------------------------*/

#include <mygba.h>

#include "gfx/master.pal.c"
#include "gfx/objects.pal.c"
#include "gfx/welcome.map.c"
#include "gfx/welcome.raw.c"
#include "gfx/level1.map.c"
#include "gfx/level1.raw.c"
#include "gfx/level2.map.c"
#include "gfx/level2.raw.c"
#include "gfx/level3.map.c"
#include "gfx/level3.raw.c"
#include "gfx/player.raw.c"
#include "gfx/monster01.raw.c"
#include "gfx/monster02.raw.c"
#include "gfx/monster03.raw.c"
#include "gfx/monster04.raw.c"
#include "gfx/blocks.raw.c"
#include "gfx/bullet.raw.c"
#include "gfx/bullet2.raw.c"
#include "gfx/explosion.raw.c"

//monster directions
#define LEFT   0
#define RIGHT  1
#define DOWNL  2
#define DOWNR  3
//game difficulty levels
#define EASY   30
#define MEDIUM 15
#define HARD   10
//screen size
#define WIDTH  240
#define HEIGH  160
//player&monster states
#define IDLE   0
#define DEAD   1
#define SHOOT  2
#define DIYING 3
#define ALIVE  4
//sprites sizes
#define PLAYER_WIDTH 16
#define PLAYER_HEIGH 16
#define BLOCKS_WIDTH 32
#define BLOCKS_HEIGH 16
#define MONSTER_WIDTH 16
#define MONSTER_HEIGH 16
#define EXPLOSION_WIDTH 16
#define EXPLOSION_HEIGH 16

#define MIXER_FREQ 11467

//function prototypes
void gameInit(void);
void vblFunc(void);
void process_input(void);
void move_monsters(void);
void Print(int left,int top,char *str, unsigned short color);
void DrawChar(int left,int top,char letter,unsigned short color);
int bullet_collide(void);
int bullet2_collide(int);
int isNewLevel(void);
void setLevel1(void);
void setLevel2(void);
void setLevel3(void);
void init_player(void);
void init_bullets(void);
void init_monsters(void);
void init_blocks(void);
void delay_display(char* str,int delay, int x, int y);
void game_over(void);
void process_input(void);
void update_player(void);
void update_monsters(void);
void update_explosion(void);
void update_bullet2(void);

//GLOBALS
map_fragment_info_ptr mWelcome;
map_fragment_info_ptr mLevel1;
map_fragment_info_ptr mLevel2;
map_fragment_info_ptr mLevel3;

//Sprites
unsigned char monster01[10];
unsigned char monster02[10];
unsigned char monster03[10];
unsigned char monster04[10];
unsigned char blocks[4];
unsigned char player;
unsigned char explosion;
unsigned char bullet;
unsigned char bullet2[5];

//player position
signed short int player_x, player_y;
//bullet position
signed short int bullet_x, bullet_y;
signed short int bullet2_x[5],bullet2_y[5];
//explosion position
signed short int explosion_x,explosion_y;

//monsters position
signed short int monster01_x[10], monster01_y[10];
signed short int monster02_x[10], monster02_y[10];
signed short int monster03_x[10], monster03_y[10];
signed short int monster04_x[10], monster04_y[10];

//player state
unsigned char player_state;

//monsters state
unsigned char monster01_state[10];
unsigned char monster02_state[10];
unsigned char monster03_state[10];
unsigned char monster04_state[10];

//blocks position
unsigned char blocks_x[4],blocks_y[4];

//blocks state
unsigned char blocks_state[4];
unsigned char blocks_frame[4];

unsigned char left_bound,right_bound,bottom_bound;
unsigned char counter;
unsigned char difficulty;
unsigned char explosion_frame;
unsigned char explosion_counter;
unsigned char explosion_visible;
unsigned char bullet_visible;
unsigned char bullet2_visible[5];
unsigned char monsters_dir;
unsigned char monster_frame;
unsigned char player_frame;
unsigned char player_vblCount;
unsigned short int bullet2_speed;
unsigned short int score;
signed short int lives;

int seconds = 0;
int timer_enabled = 0;

//pointers to wave data
extern const WaveData _binary_monsterexp_raw_start;
extern const WaveData _binary_laser_raw_start;
extern const WaveData _binary_loop_raw_start;
sample_info *music_samples[3];

int main(void)
{
   int i;
   
   ham_Init();
   gameInit();

   ham_InitText(0);
   ham_SetBgMode(0);

   //load palettes
   ham_LoadBGPal((void*)master_Palette,256);
   ham_LoadObjPal((void*)objects_Palette,256);

   //load welcome_screen image
   ham_bg[1].ti = ham_InitTileSet((void*)welcome_Tiles,SIZEOF_16BIT(welcome_Tiles),1,1);
   ham_bg[1].mi = ham_InitMapEmptySet(0,0);
   mWelcome = ham_InitMapFragment((void*)welcome_Map,30,20,0,0,30,20,0);
   ham_InsertMapFragment(mWelcome,1,0,0);
   ham_InitBg(1,1,0,0);

   //wait for any input
   while(!F_CTRLINPUT_START_PRESSED &&
         !F_CTRLINPUT_UP_PRESSED &&
         !F_CTRLINPUT_DOWN_PRESSED &&
         !F_CTRLINPUT_LEFT_PRESSED &&
         !F_CTRLINPUT_RIGHT_PRESSED &&
         !F_CTRLINPUT_SELECT_PRESSED &&
         !F_CTRLINPUT_B_PRESSED &&
         !F_CTRLINPUT_A_PRESSED &&
         !F_CTRLINPUT_L_PRESSED &&
         !F_CTRLINPUT_R_PRESSED);

   setLevel1();

   
   //Player sprites
	player = ham_CreateObj(&player_Bitmap,OBJ_SIZE_16X16,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,player_x,player_y);

   //monsters sprites
   for(i=0; i<10; i++)
   {
      monster01[i] = ham_CreateObj(&monster01_Bitmap,OBJ_SIZE_16X16,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,monster01_x[i],monster01_y[i]);
      monster02[i] = ham_CreateObj(&monster02_Bitmap,OBJ_SIZE_16X16,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,monster02_x[i],monster02_y[i]);
      monster03[i] = ham_CreateObj(&monster03_Bitmap,OBJ_SIZE_16X16,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,monster03_x[i],monster03_y[i]);
      monster04[i] = ham_CreateObj(&monster04_Bitmap,OBJ_SIZE_16X16,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,monster04_x[i],monster04_y[i]);
   }
   //blocks sprites
   for(i=0; i<4; i++)
      blocks[i] = ham_CreateObj(&blocks_Bitmap,OBJ_SIZE_32X16,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,blocks_x[i],blocks_y[i]);

   //explosion sprites
   explosion = ham_CreateObj(&explosion_Bitmap,OBJ_SIZE_16X16,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,10,10);
   ham_SetObjVisible(explosion,0);

   //bullet sprites
   bullet = ham_CreateObj(&bullet_Bitmap,OBJ_SIZE_8X8,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,0,0);
   ham_SetObjVisible(bullet,0);
   for(i=0; i<5; i++)
   {
      bullet2[i]= ham_CreateObj(&bullet2_Bitmap,OBJ_SIZE_8X8,OBJ_MODE_NORMAL,1,0,0,0,0,0,0,0,0);
      ham_SetObjVisible(bullet2[i],0);
   }

   ham_SetTextCol(254,0);

   ham_InitMixer(MIXER_FREQ);
   ham_StartIntHandler(INT_TYPE_VBL,&vblFunc);
   delay_display("LEVEL 1 GET READY!",2,5,8);

	music_samples[0] =  ham_InitSample((u8*)_binary_monsterexp_raw_start.data,
                   _binary_monsterexp_raw_start.size,
                   _binary_monsterexp_raw_start.freq>>10);
	music_samples[1] =  ham_InitSample((u8*)_binary_laser_raw_start.data,
                   _binary_laser_raw_start.size,
                   _binary_laser_raw_start.freq>>10);
	music_samples[2] =  ham_InitSample((u8*)_binary_loop_raw_start.data,
                   _binary_loop_raw_start.size,
                   _binary_loop_raw_start.freq>>10);

   ham_PlaySample(music_samples[2]);

   //loop forever
   while(1){
   }

   return 0;
}

/*-------------------------
Initializes some important
variables of the game
--------------------------*/
void gameInit(void)
{
   lives = 2;
   score = 0;
   monsters_dir = RIGHT;
   counter = 0;
   difficulty = EASY;
   bullet2_speed= 500;
   monster_frame = 0;
   player_frame = 0;
   explosion_frame = 0;
   explosion_counter = 0;
   explosion_visible = 0;
   bullet_visible = 0;
   player_vblCount = 0;
   bullet_x = 0;
   bullet_y = 0;
   left_bound = right_bound = bottom_bound = 0;

   init_player();
   init_bullets();
   init_blocks();
   init_monsters();
}

/*----------------------
initializes variables
for playing a new level
increasing difficulty
------------------------*/
void new_level()
{
   ham_StopIntHandler(INT_TYPE_VBL);
   
   monsters_dir = RIGHT;
   counter = 0;
   monster_frame = 0;
   player_frame = 0;
   explosion_frame = 0;
   explosion_counter = 0;
   explosion_visible = 0;
   bullet_visible = 0;
   player_vblCount = 0;
   bullet_x = 0;
   bullet_y = 0;
   left_bound = right_bound = bottom_bound = 0;

   init_player();
   init_bullets();
   init_blocks();
   init_monsters();
   ham_SetObjVisible(explosion,0);
   ham_StartIntHandler(INT_TYPE_VBL,&vblFunc);
   
   switch(difficulty){
      case EASY:
         setLevel2();
         delay_display("LEVEL 2 GET READY",2,5,8);
         difficulty = MEDIUM;
         bullet2_speed = 200;
         break;
      case MEDIUM:
         setLevel3();
         delay_display("LEVEL 3 GET READY",2,5,8);
         difficulty = HARD;
         bullet2_speed = 100;
         break;
      case HARD:
         //it shouldn't get here!
         game_over();
         break;
   }
}

/*--------------------------
actions performed whenever
a vBlank is reached
--------------------------*/
void vblFunc(void)
{
   ham_SyncMixer();
   update_explosion();

   if(timer_enabled)
      seconds++;
   else
   {
      process_input();
      update_player();
      update_monsters();
      update_bullet2();

      //display score
      ham_DrawText(0,19,"Score: %d",score);
      //display lives
      ham_DrawText(15,19,"Lives: %d",lives);

      if(isNewLevel()){
         if(difficulty == HARD){
            delay_display("CONGRATULATIONS",2,8,8);
            game_over();
         }
         delay_display("LEVEL COMPLETED",2,6,8);
         new_level();
      }
   }

   if(!music_samples[2]->playing)
      ham_PlaySample(music_samples[2]);
   ham_UpdateMixer();
   ham_CopyObjToOAM();
}

/*------------------
processes input from
player
--------------------*/
void process_input(void)
{
   if(player_state == DIYING)
      return;
      
   if(F_CTRLINPUT_LEFT_PRESSED)
      player_x-=1;
   if(F_CTRLINPUT_RIGHT_PRESSED)
      player_x+=1;
   if(F_CTRLINPUT_A_PRESSED || F_CTRLINPUT_B_PRESSED)
      player_state = SHOOT;
      
   if(player_x < 0)
      player_x=0;
   if(player_x > 240-16)
      player_x = 240-16;
      
   ham_SetObjX(player,player_x);
}

/*------------------------
update monster's position
-------------------------*/
void move_monsters(void)
{
   int i,j;
   
   switch(monsters_dir){
   case RIGHT:
      monster01_x[0] += 4;
      monster02_x[0] += 4;
      monster03_x[0] += 4;
      monster04_x[0] += 4;

      for(i=1; i<10; i++) {
         monster01_x[i] = monster01_x[i-1]+16;
         monster02_x[i] = monster02_x[i-1]+16;
         monster03_x[i] = monster03_x[i-1]+16;
         monster04_x[i] = monster04_x[i-1]+16;
      }
      break;
   case LEFT:
      monster01_x[0] -= 4;
      monster02_x[0] -= 4;
      monster03_x[0] -= 4;
      monster04_x[0] -= 4;

      for(i=1; i<10; i++) {
         monster01_x[i] = monster01_x[i-1]+16;
         monster02_x[i] = monster02_x[i-1]+16;
         monster03_x[i] = monster03_x[i-1]+16;
         monster04_x[i] = monster04_x[i-1]+16;
      }
      break;
   case DOWNL:
   case DOWNR:
      monster01_y[0] += 8;
      monster02_y[0] += 8;
      monster03_y[0] += 8;
      monster04_y[0] += 8;

      for(i=1; i<10; i++)
      {
         monster01_y[i] = monster01_y[0];
         monster02_y[i] = monster02_y[0];
         monster03_y[i] = monster03_y[0];
         monster04_y[i] = monster04_y[0];
      }
      break;
   }
   
   //change monsters direction
   if(monsters_dir == DOWNR)
      monsters_dir = LEFT;
   else if(monsters_dir == DOWNL)
      monsters_dir = RIGHT;

   //get a left bound, check what column has a monster at
   //the leftmost position
   for(i=0; i<10; i++)
   {
      if(monster01_state[i] == ALIVE ||
         monster02_state[i] == ALIVE ||
         monster03_state[i] == ALIVE ||
         monster04_state[i] == ALIVE) {
         left_bound = i;
         break;
      }
   }
   
   //get a right bound, check what column has a monster at
   //the leftmost position
   for(i=9; i>=0; i--)
   {
      if(monster01_state[i] == ALIVE ||
         monster02_state[i] == ALIVE ||
         monster03_state[i] == ALIVE ||
         monster04_state[i] == ALIVE) {
         right_bound = i;
         break;
      }
   }
   
   //get a bottom bound, check what row has a monster at
   //the bottommost position
   bottom_bound=0;
   for(i=4; i>=1; i--){
      switch(i){
      case 1:
         for(j=0; j<10; j++) {
            if(monster01_state[j] == ALIVE){
               bottom_bound = 1;
               break;
            }
         }
         break;
      case 2:
         for(j=0; j<10; j++) {
            if(monster02_state[j] == ALIVE){
               bottom_bound = 2;
               break;
            }
         }
         break;
      case 3:
         for(j=0; j<10; j++) {
            if(monster03_state[j] == ALIVE){
               bottom_bound = 3;
               break;
            }
         }
         break;
      case 4:
         for(j=0; j<10; j++) {
            if(monster04_state[j] == ALIVE){
               bottom_bound = 4;
               break;
            }
         }
         break;
      }//switch
      if(bottom_bound > 0)
         break;
   }

   if(monster01_x[right_bound] > (240-16) && monsters_dir == RIGHT)
      monsters_dir = DOWNR;
   if(monster01_x[left_bound] < 0 && monsters_dir == LEFT)
      monsters_dir = DOWNL;

   //check if monsters have reached the bottom
   switch(bottom_bound){
   case 1:
      if(monster01_y[0]+MONSTER_HEIGH > (HEIGH-(BLOCKS_HEIGH*2)-8))
         game_over();
      break;
   case 2:
      if(monster02_y[0]+MONSTER_HEIGH > (HEIGH-(BLOCKS_HEIGH*2)-8))
         game_over();
      break;
   case 3:
      if(monster03_y[0]+MONSTER_HEIGH > (HEIGH-(BLOCKS_HEIGH*2)-8))
         game_over();
      break;
   case 4:
      if(monster04_y[0]+MONSTER_HEIGH > (HEIGH-(BLOCKS_HEIGH*2)-8))
         game_over();
      break;
   }

   //update monsters positions
   for(i=0; i<10; i++)
   {
      ham_SetObjX(monster01[i],monster01_x[i]);
      ham_SetObjY(monster01[i],monster01_y[i]);
      ham_SetObjX(monster02[i],monster02_x[i]);
      ham_SetObjY(monster02[i],monster02_y[i]);
      ham_SetObjX(monster03[i],monster03_x[i]);
      ham_SetObjY(monster03[i],monster03_y[i]);
      ham_SetObjX(monster04[i],monster04_x[i]);
      ham_SetObjY(monster04[i],monster04_y[i]);
   }

  //update monsters sprites
   monster_frame = (monster_frame?0:1);

   for(i=0; i<10; i++)
   {
      if(monster01_state[i] == ALIVE)
         ham_UpdateObjGfx(monster01[i],(void*)&monster01_Bitmap[monster_frame*(MONSTER_WIDTH*MONSTER_HEIGH)]);

      if(monster02_state[i] == ALIVE)
         ham_UpdateObjGfx(monster02[i],(void*)&monster02_Bitmap[monster_frame*(MONSTER_WIDTH*MONSTER_HEIGH)]);

      if(monster03_state[i] == ALIVE)
         ham_UpdateObjGfx(monster03[i],(void*)&monster03_Bitmap[monster_frame*(MONSTER_WIDTH*MONSTER_HEIGH)]);

      if(monster04_state[i] == ALIVE)
         ham_UpdateObjGfx(monster04[i],(void*)&monster04_Bitmap[monster_frame*(MONSTER_WIDTH*MONSTER_HEIGH)]);
   }
}

/*---------------------
checks for collisions
with blocks and monsters
----------------------*/
int bullet_collide()
{
   int i;
   
   //collisions with blocks
   for(i=0; i<4; i++)
   {
      if((bullet_y >= blocks_y[i]) && (bullet_y <= blocks_y[i]+BLOCKS_HEIGH))
         if((bullet_x >= blocks_x[i]) && (bullet_x <= blocks_x[i]+BLOCKS_WIDTH))
            if(blocks_state[i] == ALIVE)
               return 1;
   }

   //collisions with monsters
   for(i=0; i<10; i++)
   {
      if((bullet_y >= monster04_y[i]) && (bullet_y <= monster04_y[i]+MONSTER_HEIGH))
         if((bullet_x >= monster04_x[i]) && (bullet_x <= monster04_x[i]+MONSTER_WIDTH))
            if(monster04_state[i] == ALIVE){
               monster04_state[i] = DEAD;
               ham_SetObjVisible(monster04[i],0);
               ham_SetObjVisible(explosion,1);
               ham_SetObjX(explosion,monster04_x[i]);
               ham_SetObjY(explosion,monster04_y[i]);
               if(music_samples[0]->playing)
                  ham_KillSample(music_samples[0]);
               ham_PlaySample(music_samples[0]);
               explosion_visible = 1;
               score+=10;
               return 1;
            }
      if((bullet_y >= monster03_y[i]) && (bullet_y <= monster03_y[i]+MONSTER_HEIGH))
         if((bullet_x >= monster03_x[i]) && (bullet_x <= monster03_x[i]+MONSTER_WIDTH))
            if(monster03_state[i] == ALIVE){
               monster03_state[i] = DEAD;
               ham_SetObjVisible(monster03[i],0);
               ham_SetObjVisible(explosion,1);
               ham_SetObjX(explosion,monster03_x[i]);
               ham_SetObjY(explosion,monster03_y[i]);
               if(music_samples[0]->playing)
                  ham_KillSample(music_samples[0]);
               ham_PlaySample(music_samples[0]);
               explosion_visible = 1;
               score+=10;
               return 1;
            }
      if((bullet_y >= monster02_y[i]) && (bullet_y <= monster02_y[i]+MONSTER_HEIGH))
         if((bullet_x >= monster02_x[i]) && (bullet_x <= monster02_x[i]+MONSTER_WIDTH))
            if(monster02_state[i] == ALIVE){
               monster02_state[i] = DEAD;
               ham_SetObjVisible(monster02[i],0);
               ham_SetObjVisible(explosion,1);
               ham_SetObjX(explosion,monster02_x[i]);
               ham_SetObjY(explosion,monster02_y[i]);
               if(music_samples[0]->playing)
                  ham_KillSample(music_samples[0]);
               ham_PlaySample(music_samples[0]);
               explosion_visible = 1;
               score+=10;
               return 1;
            }
      if((bullet_y >= monster01_y[i]) && (bullet_y <= monster01_y[i]+MONSTER_HEIGH))
         if((bullet_x >= monster01_x[i]) && (bullet_x <= monster01_x[i]+MONSTER_WIDTH))
            if(monster01_state[i] == ALIVE){
               monster01_state[i] = DEAD;
               ham_SetObjVisible(monster01[i],0);
               ham_SetObjVisible(explosion,1);
               ham_SetObjX(explosion,monster01_x[i]);
               ham_SetObjY(explosion,monster01_y[i]);
               if(music_samples[0]->playing)
                  ham_KillSample(music_samples[0]);
               ham_PlaySample(music_samples[0]);
               explosion_visible = 1;
               score+=10;
               return 1;
            }
   }

   return 0;
}

int bullet2_collide(i)
{
   int j;
   //check collisions with blocks
   for(j=0; j<4; j++){
      if((bullet2_y[i]>=blocks_y[j]) && (bullet2_y[i]<=(blocks_y[j]+BLOCKS_HEIGH)))
         if((bullet2_x[i]>=blocks_x[j]) && (bullet2_x[i]<=(blocks_x[j]+BLOCKS_WIDTH)))
         {
            if(blocks_frame[j]<=4){
               ham_UpdateObjGfx(blocks[j],(void*)&blocks_Bitmap[blocks_frame[j]*(BLOCKS_WIDTH*BLOCKS_HEIGH)]);
               blocks_frame[j]++;
            }
            else{
               blocks_state[j] = DEAD;
               ham_SetObjVisible(blocks[j],0);
            }
            if(blocks_state[j] == ALIVE)
               return 1;
            else
               return 0;
         }
   }
   
   //check collisions with player
   if((bullet2_y[i]>=player_y) && (bullet2_y[i]<=(player_y+PLAYER_HEIGH)))
      if((bullet2_x[i]>=player_x) && (bullet2_x[i]<=(player_x+PLAYER_WIDTH))){
         lives--;
         player_state = DIYING;
         ham_SetObjVisible(explosion,1);
         ham_SetObjX(explosion,player_x);
         ham_SetObjY(explosion,player_y);
         if(!music_samples[0]->playing)
            ham_PlaySample(music_samples[0]);
         explosion_visible = 1;
         if(lives < 0)
            game_over();
         return 1;
      }
   
   return 0;
}

/*-----------------------
checks wether we reached
a new level state
------------------------*/
int isNewLevel()
{
   int result = 1;
   int i;
   
   for(i=0; i<10; i++)
   {
      if((monster01_state[i] == ALIVE) ||
         (monster02_state[i] == ALIVE) ||
         (monster03_state[i] == ALIVE) ||
         (monster04_state[i] == ALIVE)){
         result = 0;
         break;
      }
   }
   
   return result;
}

void setLevel1()
{
   ham_DeInitBg(1);
   ham_bg[1].ti = ham_InitTileSet((void*)level1_Tiles,SIZEOF_16BIT(level1_Tiles),1,1);
   ham_bg[1].mi = ham_InitMapEmptySet(0,0);
   mLevel1 = ham_InitMapFragment((void*)level1_Map,30,20,0,0,30,20,0);
   ham_InsertMapFragment(mLevel1,1,0,0);
   ham_InitBg(1,1,0,0);
}

void setLevel2()
{
   ham_DeInitBg(1);
   ham_bg[1].ti = ham_InitTileSet((void*)level2_Tiles,SIZEOF_16BIT(level2_Tiles),1,1);
   ham_bg[1].mi = ham_InitMapEmptySet(0,0);
   mLevel2 = ham_InitMapFragment((void*)level2_Map,30,20,0,0,30,20,0);
   ham_InsertMapFragment(mLevel2,1,0,0);
   ham_InitBg(1,1,0,0);
}

void setLevel3()
{
   ham_DeInitBg(1);
   ham_bg[1].ti = ham_InitTileSet((void*)level3_Tiles,SIZEOF_16BIT(level3_Tiles),1,1);
   ham_bg[1].mi = ham_InitMapEmptySet(0,0);
   mLevel3 = ham_InitMapFragment((void*)level3_Map,30,20,0,0,30,20,0);
   ham_InsertMapFragment(mLevel3,1,0,0);
   ham_InitBg(1,1,0,0);
}

void delay_display(char *str, int delay, int x, int y)
{
   int i;
   seconds=0;
   timer_enabled = 1;
   while(seconds < delay*60)
      ham_DrawText(x,y,str);
   //clear that line
   for(i=0; i<30; i++)
      ham_DrawText(i,y," ");
   timer_enabled = 0;
}

void init_player()
{
   player_x = (WIDTH/2)-(PLAYER_WIDTH/2);
   player_y = HEIGH-PLAYER_HEIGH-8;
   player_state = IDLE;
   ham_SetObjX(player,player_x);
   ham_SetObjY(player,player_y);
}

void init_bullets()
{
   int i;
   
   for(i=0; i<5; i++)
   {
      bullet2_visible[i] = 0;
      bullet2_x[i] = 0;
      bullet2_y[i] = 0;
      ham_SetObjVisible(bullet2[i],0);
      ham_SetObjX(bullet2[i],0);
      ham_SetObjY(bullet2[i],0);
   }

   ham_SetObjVisible(bullet,0);
}

void init_blocks()
{
   int i;
   
   for(i=0; i<4; i++)
   {
      blocks_x[i] = (i*BLOCKS_WIDTH)+(20*i)+20;
      blocks_y[i] = HEIGH-(BLOCKS_HEIGH*2)-10;
      blocks_state[i] = ALIVE;
      blocks_frame[i] = 0;
      ham_SetObjVisible(blocks[i],1);
      ham_SetObjX(blocks[i],blocks_x[i]);
      ham_SetObjY(blocks[i],blocks_y[i]);
      ham_UpdateObjGfx(blocks[i],(void*)&blocks_Bitmap[0]);
   }
}

void init_monsters()
{
   int i;
   
   for(i=0; i<10; i++)
   {
      monster01_state[i] = ALIVE;
      monster02_state[i] = ALIVE;
      monster03_state[i] = ALIVE;
      monster04_state[i] = ALIVE;
      ham_SetObjVisible(monster01[i],1);
      ham_SetObjVisible(monster02[i],1);
      ham_SetObjVisible(monster03[i],1);
      ham_SetObjVisible(monster04[i],1);
   }

   monster01_x[0] = 0;
   monster02_x[0] = 0;
   monster03_x[0] = 0;
   monster04_x[0] = 0;

   for(i=1; i<10; i++)
   {
      monster01_x[i] = monster01_x[i-1]+16;
      monster02_x[i] = monster02_x[i-1]+16;
      monster03_x[i] = monster03_x[i-1]+16;
      monster04_x[i] = monster04_x[i-1]+16;
   }

   monster01_y[0] = 0;
   monster02_y[0] = 16;
   monster03_y[0] = 32;
   monster04_y[0] = 48;

   for(i=1; i<10; i++)
   {
      monster01_y[i] = monster01_y[0];
      monster02_y[i] = monster02_y[0];
      monster03_y[i] = monster03_y[0];
      monster04_y[i] = monster04_y[0];
   }

   for(i=0; i<10; i++)
   {
      ham_SetObjX(monster01[i],monster01_x[i]);
      ham_SetObjY(monster01[i],monster01_y[i]);
      ham_SetObjX(monster02[i],monster02_x[i]);
      ham_SetObjY(monster02[i],monster02_y[i]);
      ham_SetObjX(monster03[i],monster03_x[i]);
      ham_SetObjY(monster03[i],monster03_y[i]);
      ham_SetObjX(monster04[i],monster04_x[i]);
      ham_SetObjY(monster04[i],monster04_y[i]);
      ham_UpdateObjGfx(monster01[i],(void*)&monster01_Bitmap[0]);
      ham_UpdateObjGfx(monster02[i],(void*)&monster02_Bitmap[0]);
      ham_UpdateObjGfx(monster03[i],(void*)&monster03_Bitmap[0]);
      ham_UpdateObjGfx(monster04[i],(void*)&monster04_Bitmap[0]);
   }
}

void update_explosion()
{
   //update explosion sprite ever 4 vBlanks
   if(explosion_visible)
   {
      explosion_counter++;
      if(explosion_counter==4) {
         ham_UpdateObjGfx(explosion,(void*)&explosion_Bitmap[explosion_frame*(EXPLOSION_WIDTH*EXPLOSION_HEIGH)]);
         explosion_frame++;
         explosion_counter = 0;
      }
      else if(explosion_frame>4)
      {
         explosion_frame = 0;
         explosion_counter = 0;
         explosion_visible = 0;
         ham_SetObjVisible(explosion,0);
         if(player_state == DIYING){
            ham_SetObjVisible(player,1);
            delay_display("GET READY!",1,10,8);
            if(bullet_visible)
               player_state = SHOOT;
            else
               player_state = IDLE;
         }
      }
   }
}

void update_player()
{
   //update player sprite every 5 vBlanks
   if(player_vblCount++ == 5){
      player_frame = (player_frame?0:1);
      ham_UpdateObjGfx(player,(void*)&player_Bitmap[player_frame*(PLAYER_WIDTH*PLAYER_HEIGH)]);
      player_vblCount=0;
   }

   switch(player_state){
   case SHOOT:
      if(!bullet_visible){
         if(music_samples[1]->playing)
            ham_KillSample(music_samples[1]);
         ham_PlaySample(music_samples[1]);
         bullet_y = player_y;
         bullet_x = player_x+(PLAYER_WIDTH/2)-4;
         bullet_visible = 1;
         ham_SetObjX(bullet,bullet_x);
         ham_SetObjY(bullet,bullet_y);
         ham_SetObjVisible(bullet,1);
      }
      else{
         bullet_y-=3;
         ham_SetObjY(bullet,bullet_y);
         if(bullet_y < 0 || bullet_collide()){
            bullet_visible = 0;
            ham_SetObjVisible(bullet,0);
            player_state = IDLE;
         }
      }
      break;
   case DIYING:
      ham_SetObjVisible(player,0);
   }//switch
}

void update_monsters()
{
   //update monsters every 10, 15 or 20 vBlanks, based on difficulty
   //less is hardest
   counter++;
   if(counter==difficulty){
      move_monsters();
      counter=0;
   }
}

void update_bullet2()
{
   int i,j;
   
   //used for monster fire based on difficulty
   //more difficult (less bullet2_speed), more bullets
   i = rand()%bullet2_speed;
   if(i < 10)
   {
      //check what monster row has a monster available to fire
      //i.e. a monster that has no obstacles (like other monster below)
      for(j=0; j<5; j++){
         if(!bullet2_visible[j]){
            if(monster04_state[i] == ALIVE){
               bullet2_x[j] = monster04_x[i]+(MONSTER_WIDTH/2)-4;
               bullet2_y[j] = monster04_y[i]+MONSTER_HEIGH;
            }
            else if(monster03_state[i] == ALIVE){
               bullet2_x[j] = monster03_x[i]+(MONSTER_WIDTH/2)-4;
               bullet2_y[j] = monster03_y[i]+MONSTER_HEIGH;
            }
            else if(monster02_state[i] == ALIVE){
               bullet2_x[j] = monster02_x[i]+(MONSTER_WIDTH/2)-4;
               bullet2_y[j] = monster02_y[i]+MONSTER_HEIGH;
            }
            else if(monster01_state[i] == ALIVE){
               bullet2_x[j] = monster01_x[i]+(MONSTER_WIDTH/2)-4;
               bullet2_y[j] = monster01_y[i]+MONSTER_HEIGH;
            }
            bullet2_visible[j] = 1;
            ham_SetObjX(bullet2[j],bullet2_x[j]);
            ham_SetObjY(bullet2[j],bullet2_y[j]);
            ham_SetObjVisible(bullet2[j],1);
            break;
         }//if
      }//for
   }

   for(j=0; j<5; j++){
      if(bullet2_visible[j]){
         bullet2_y[j]+=1;
         ham_SetObjY(bullet2[j],bullet2_y[j]);
         if(bullet2_y[j] > (HEIGH-16)  || bullet2_collide(j)){
            bullet2_visible[j] = 0;
            ham_SetObjVisible(bullet2[j],0);
         }
      }
   }
}

void game_over()
{
   int i;
   for(i=0; i<3; i++)
      ham_DeInitSample(music_samples[i]);

   ham_DeInitMixer();
   ham_DrawText(10,8,"GAME OVER!");
   ham_StopIntHandler(INT_TYPE_VBL);
}

