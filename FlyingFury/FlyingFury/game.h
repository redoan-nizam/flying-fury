
//  game.h  -  all the numbers, structs and variables

//  Only simple things are used here: #define, struct, arrays


#ifndef GAME_H
#define GAME_H

#include <time.h>
#include <stdio.h>      // FILE, fopen, fprintf, fscanf - used to save the scores


//CFG - every physics tuning number lives here

struct PhysicsConfig
{
	double gravity;        //pull, per physics step
	double fixedStep;      //seconds of one physics step
	double maxFrame;       //never simulate more than this in one go
	double maxSpeed;       //speed cap, stops tunnelling
	double restWood, restStone, restGlass;   //bounce
	double fricWood, fricStone, fricGlass;   //friction
	double linDamp, angDamp;                 //damping
	double sleepLinear, sleepAngular;        //below this a body may sleep
	double extentGrowMax;                    //cap on how much a turned box grows
	double correctPercent;                   //how much overlap is fixed per step
	double correctSlop;                      //overlap this small is ignored
	int    launchPull;                       //MAX drag distance (launch speed)
	double reloadDelay;                      //seconds before the next bird
	double levelDoneDelay;                   //seconds before the popup
	double pigMinHitSpeed;                   //a direct hit above this always hurts
	double impactMin;                        //below this it is resting, not a hit
	double torqueScale;                      //spin from an off-centre hit
	double breakForce;                       //force needed per 1 damage
};

struct PhysicsConfig CFG =
{
	0.40,           // gravity     (lower = longer reach, launch speed untouched)
	0.020,          // fixedStep   (50 physics steps a second)
	0.100,          // maxFrame
	48.0,           // maxSpeed
	0.20, 0.10, 0.35,   // restitution  wood / stone / glass
	0.55, 0.70, 0.30,   // friction     wood / stone / glass
	0.995, 0.94,        // linDamp, angDamp
	0.60, 0.35,         // sleepLinear, sleepAngular
	1.8,                // extentGrowMax
	0.20,               // correctPercent
	0.50,               // correctSlop
	154,                // launchPull  (restored to the old value)
	1.0,                // reloadDelay      RELOAD_DELAY
	1.5,                // levelDoneDelay   LEVEL_COMPLETE_DELAY
	3.0,                // pigMinHitSpeed
	2.0,                // impactMin
	0.0016,             // torqueScale
	6.0                 // breakForce
};

double physAccum = 0.0;
unsigned long physLastTick = 0;

#include <math.h>

#define PI 3.14159265   // used when working out a turned block box

// ---------- screen ----------

#define SCREEN_WIDTH   1400
#define SCREEN_HEIGHT  840
#define GROUND_Y       168        // the grass line in the background picture

// ---------- slingshot ----------
#define SLING_X        168        // left-bottom corner of the slingshot picture
#define SLING_W        98
#define SLING_H        238
#define BIRD_START_X   189        // where the bird waits before it is fired
#define BIRD_START_Y   350
#define MAX_PULL       CFG.launchPull   //how far back the mouse can pull
                                  //(bigger = you can reach further across
                                  //the level, so every base is aimable)

// ---------- physics ----------
#define GRAVITY        CFG.gravity   //pulls everything down every step
#define POWER          0.16       //pull distance  ->  speed

// The bird can fly more than 15 pixels in one frame, which is wider
// than a thin plank. So every frame is cut into small steps and the
// bird is moved a little bit at a time. That stops it from jumping
// straight THROUGH a block without ever touching it (tunnelling).
#define PHYSICS_STEPS  8

// The pictures have transparent space around the edges, so the box we
// use for hitting things is a bit SMALLER than the picture. Without
// this a bird looked like it flew past a pig and still hit it.

#define BIRD_HIT_PART  0.80       // the bird hits with 80% of its picture
#define PIG_HIT_PART   0.86       // the pig is hit on 86% of its picture

#define FLAT_PULL      0.03       // how strongly a tilted block falls flat
#define BOUNCE         0.45       // how much speed is kept after a bounce
#define FRICTION       0.90       // rubbing on the ground
#define SPIN_DAMP      0.94       // spinning slows down
#define REST_SPEED     0.49       // slower than this counts as "stopped"
#define REST_MOVE      1.20       // moved less than this in a frame = stopped

// ---------- how much punishment a pig can take ----------
// A pig is hurt by ANY hard enough knock: a bird, a falling block, a
// block sliding into it, a blast, or landing hard itself. Below this
// speed the knock is too gentle to hurt it at all.

#define PIG_HURT_SPEED 4.5        // slower knocks do no damage
#define PIG_HURT_DIV   2.0        // bigger number = pigs are tougher
#define PIG_HURT_COOL  10         // frames before the same pig can be hurt again

// When a pig has taken enough damage it does NOT disappear at once.
// It stays on screen for about 2 seconds, getting smaller, and only
// then vanishes. The level already counts it as beaten straight away.
// 25 x 0.757 = 19 frames, so the pig now vanishes in about 0.38 s

#define PIG_DIE_FRAMES 25         // 25 frames x 20 ms = about 0.5 seconds

// A smashed block stops holding anything up straight away, and is
// cleared off the screen a moment later.
#define BLOCK_FALL_FRAMES 5       // 5 frames x 20 ms = about 0.10 seconds
#define BLOCK_FLASH_FRAMES 5      // the break effect lasts the same time

// ---------- what a block is made of ----------
#define WOOD   0
#define STONE  1
#define GLASS  2

// ---------- sizes of the pictures on screen ----------
#define BIRD_SIZE      73         // normal bird picture size
#define TERENCE_SIZE   119        // Terence is much bigger
#define EGG_SIZE       25

// ---------- how many things can exist ----------
#define MAX_BLOCKS     20
#define MAX_PIGS       10       // room for the extra pigs outside
#define MAX_POWERUPS   4        // level 5 only
#define POWERUP_SIZE   46       // how big the star is drawn
#define MAX_BIRDS      3          // the blue bird splits into 3
#define QUEUE_SIZE     3          // how many upcoming birds are shown
#define LEVEL_DONE_FRAMES 100     // 100 frames x 20 ms = about 2 seconds
#define TOTAL_LEVELS   5

// ---------- the 7 birds ----------
#define BIRD_RED       0
#define BIRD_CHUCK     1          // yellow, speed boost
#define BIRD_BOMB      2          // black, explodes
#define BIRD_MATILDA   3          // green, flies backwards
#define BIRD_BLUE      4          // splits into 3
#define BIRD_WHITE     5          // drops an egg
#define BIRD_TERENCE   6          // big and heavy
#define TOTAL_BIRD_TYPES 7

// ---------- what the bird is doing ----------
#define WAITING        0          // sitting on the slingshot
#define FLYING         1          // in the air
#define DONE           2          // finished, cannot be used again

// ---------- which page is on the screen ----------
#define PAGE_MENU      0
#define PAGE_HELP      1
#define PAGE_GAME      2
#define PAGE_WIN       3
#define PAGE_LOSE      4
#define PAGE_LEVEL_DONE 5         // the short "Level Complete" message
#define PAGE_SCORE     6         // the Score page, opened from the homepage

// ---------- scores ----------
//  Breaking a block is worth a little. Popping a pig is worth a lot,
//  no matter HOW it was popped - a bird, a falling block or a blast
//  all count the same. The big prize is the bonus for every bird you
//  did NOT need, so finishing a level with fewer birds scores much
//  higher than grinding through all of them.
//  The whole game is balanced so that a PERFECT run of all 5 levels
//  comes to about 2000 points and can never go above it. The exact
//  sum is worked out from the real levels by the test, and it must
//  never pass 2000.
#define SCORE_PER_BLOCK        11
#define SCORE_PER_PIG          17
#define SCORE_PER_UNUSED_BIRD  18
#define SCORE_POWERUP          25       // picking up a star
#define SCORE_SPECIAL_PIG      25       // popping the star-marked pig


//  MATERIALS
//  Glass breaks easily, wood is normal, stone is heavy and hard.

int materialHealth(int material)
{
	if (material == GLASS)
		return 2;    // one decent hit
	if (material == STONE)
		return 7;    // needs a strong bird
	return 4;                           // wood
}
double materialBounce(int material)
{
	if (material == GLASS) 
		return CFG.restGlass;
	if (material == STONE) 
		return CFG.restStone;
	return CFG.restWood;
}
double materialFriction(int material)
{
	if (material == GLASS)
		return CFG.fricGlass;
	if (material == STONE)
		return CFG.fricStone;
	return CFG.fricWood;
}
double materialMass(int material)
{
	if (material == GLASS)
		return 0.6;  // light, flies away easily
	if (material == STONE) 
		return 2.0;  // heavy, hard to push
	return 1.0;                         // wood
}

// ============================================================
//  HOW STRONG IS EACH BIRD ?
//  This number is taken away from the health of a block or a pig
//  when the bird hits it. A bigger number = a stronger bird.
// ============================================================
int birdPower(int type)
{
	if (type == BIRD_RED)    
		return 1;   // weakest, only breaks weak things
	if (type == BIRD_CHUCK)  
		return 2;   // fast, so it hits harder
	if (type == BIRD_BOMB)   
		return 3;   // explosive
	if (type == BIRD_MATILDA)
		return 1;   // its trick is the boomerang
	if (type == BIRD_BLUE)   
		return 1;   // weak, but there are 3 of them
	if (type == BIRD_WHITE)  
		return 1;   // its egg does the real damage
	if (type == BIRD_TERENCE) 
		return 4;   // heaviest, breaks anything
	return 1;
}

// How heavy each bird is. A heavy bird pushes blocks harder.
double birdMass(int type)
{
	if (type == BIRD_TERENCE)
		return 2.5;
	if (type == BIRD_BLUE)  
		return 0.6;
	return 1.0;
}

// The name of each bird, written on the screen.
const char *birdName(int type)
{
	if (type == BIRD_CHUCK)  
		return "CHUCK";
	if (type == BIRD_BOMB)  
		return "BOMB";
	if (type == BIRD_MATILDA)
		return "MATILDA";
	if (type == BIRD_BLUE) 
		return "BLUE";
	if (type == BIRD_WHITE) 
		return "WHITE";
	if (type == BIRD_TERENCE) 
		return "TERENCE";
	return "RED";
}

// ============================================================
//  STRUCTS  (a struct is just a box that holds a few variables)
// ============================================================

struct Bird
{
	double x, y;        // position on screen
	double vx, vy;      // speed
	int size;           // how big the picture is drawn
	int type;           // BIRD_RED, BIRD_CHUCK, ...
	int state;          // WAITING, FLYING or DONE
	int powerUsed;      // 1 after the special power has been used
	int hitCool;        // counts down after a hit, so ONE touch = ONE hit
	int restCount;      // how many frames it has been standing still
	int lifeTime;       // how many frames since it was fired
	int boostFrames;    // Chuck speeds up over a few frames, not instantly
	int turnFrames;     // Matilda turns around smoothly over a few frames
	double turnPush;    // how much her speed changes each of those frames
	double mass;        // Terence is heavy, so he pushes blocks much further
};

struct Block
{
	double x, y;        // bottom-left corner (double, so it can move smoothly)
	double w, h;        // width and height on screen
	double vx, vy;      // speed - a block can be pushed and can fall
	double angle;       // how much it is turned, in degrees
	double spin;        // how fast it is turning
	double impactVX;    // the speed it was REALLY travelling this frame,
	double impactVY;    // saved before it is stopped by the ground
	int picture;        // 0 = post, 1 = beam, 2 = thin post
	int material;       // WOOD, STONE or GLASS
	int health;         // damage it can still take
	int maxHealth;
	int hurtCool;       // so one touch hurts once, not once per frame
	int broken;         // 1 = smashed: it still falls, but holds nothing up
	int breakTimer;     // counts down while the broken piece is falling
	int flash;          // counts down while the break effect is showing
	int alive;          // 1 = draw it , 0 = gone for good
};

struct Pig
{
	double x, y;        // bottom-left corner (double, a pig can fall)
	double vx, vy;      // speed
	int size;           // width and height on screen
	int picture;        // 0 = pig.png , 1 = pig2.png , 2 = pig3.png
	int health;         // damage it can still take
	int maxHealth;
	int hurtCool;       // so one knock hurts once, not once per frame
	int special;        // 1 = the star pig in level 5: gives an extra bird
	int dying;          // 1 = beaten, still on screen for a moment
	int dieTimer;       // counts down, then the pig vanishes
	int alive;          // 1 = draw it , 0 = gone for good
};

struct Egg
{
	double x, y;
	double vy;
	int alive;
};

// ============================================================
//  GLOBAL VARIABLES  (every file can use these)
// ============================================================

struct Bird  birds[MAX_BIRDS];       // birds[0] is the one on the slingshot
int   birdCount = 1;                 // becomes 3 when the blue bird splits

struct Block blocks[MAX_BLOCKS];
int   blockCount = 0;

struct Pig   pigs[MAX_PIGS];
int   pigCount = 0;

// ---- power-ups (LEVEL 5 ONLY) ----
// A star floating in the level. Fly a bird into it and you get one
// extra bird plus some score. Built in loadLevel(5), checked in
// physics.h (moveBird), drawn in render.h (drawGame).
struct PowerUp
{
	int x, y;           // bottom-left corner
	int alive;          // 1 = still there, 0 = already collected
};

struct PowerUp powerUps[MAX_POWERUPS];
int   powerUpCount = 0;

struct Egg   egg;                    // the white bird drops one egg

// ---- the bird queue ----
// currentBird is the one sitting on the slingshot.
// choice[0], choice[1], choice[2] are the next three birds, in the
// order they will be used. When a bird is thrown they all step one
// place forward and ONE new random bird joins the back of the line.
int   currentBird = BIRD_RED;
int   choice[QUEUE_SIZE];

// ---- shuffled bag, so the same bird cannot keep coming up ----
int   birdBag[TOTAL_BIRD_TYPES * 2];
int   bagCount = 0;
int   lastBird = -1;
int   lastBirdRun = 0;

int   birdsLeft = 0;                 // how many shots are left in this level
int   levelDoneTimer = 0;            // counts down on the Level Complete screen
int   reloadTimer = -1;              // counts down to the next bird, -1 = off
int   winTimer = -1;                 // counts down to the popup, -1 = off
int   levelBonus = 0;                // the "birds you did not use" bonus

int   page = PAGE_MENU;              // which screen we are showing
int   level = 1;
int   score = 0;

// ============================================================
//  SAVED SCORES  (the Score button on the homepage)
//  Kept in a small text file, using plain fopen / fprintf / fscanf -
//  the same simple file style as the reference code - so the three
//  numbers below are still there the next time the game is opened.
// ============================================================
#define SCORE_FILE "flyingfury_scores.txt"

int   highestScore = 0;              // the best score ever reached
int   lastScore = 0;                 // the score from the last completed game
int   totalScore = 0;                // every completed game's score, added up

// ------------------------------------------------------------
//  Reads the three saved numbers from the file into the variables
//  above. Called ONCE, at the very start of the program (see
//  main() in iMain.cpp).
//  If the file does not exist yet (the very first time the game is
//  played), fopen() simply returns NULL and the scores stay 0 - that
//  is exactly what a brand new save should look like.
// ------------------------------------------------------------
void loadScores()
{
	FILE *fptr;
	fptr = fopen(SCORE_FILE, "r");

	if (fptr == NULL)
		return;                 // no save file yet

	fscanf(fptr, "%d %d %d", &highestScore, &lastScore, &totalScore);
	fclose(fptr);
}

// ------------------------------------------------------------
//  Writes the three saved numbers back out to the file, so they
//  are remembered the NEXT time the game is opened.
// ------------------------------------------------------------
void saveScores()
{
	FILE *fptr;
	fptr = fopen(SCORE_FILE, "w");

	if (fptr == NULL)
		return;                 // could not open the file

	fprintf(fptr, "%d %d %d", highestScore, lastScore, totalScore);
	fclose(fptr);
}

// ------------------------------------------------------------
//  Called ONCE every time a game finishes - see physics.h, at the
//  two places where the page changes to PAGE_WIN or PAGE_LOSE.
//  Works out the new Highest / Last / Total score and saves them.
// ------------------------------------------------------------
void recordGameScore()
{
	lastScore = score;                        // this game's score
	totalScore = totalScore + score;          // added to the running total

	if (score > highestScore)
		highestScore = score;                 // a new record

	saveScores();
}

int   dragging = 0;                  // 1 while the mouse is pulling the bird
int   mouseX = 0, mouseY = 0;

#endif
