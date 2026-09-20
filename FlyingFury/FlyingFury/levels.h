//  addBlock() and addPig() just put one more item in the array.
//  loadLevel() 


#ifndef LEVELS_H
#define LEVELS_H


//  Adds one block. (x, y) is the BOTTOM-LEFT corner.
//  picture: 0 = post.png (standing), 1 = beam.png (lying), 2 = post_thin.png
//  material: GLASS = light and breaks easily
//            WOOD  = normal
//            STONE = heavy and hard to break

void addBlock(int x, int y, int w, int h, int picture, int material)
{
	if (blockCount >= MAX_BLOCKS) return;

	blocks[blockCount].x = x;
	blocks[blockCount].y = y;
	blocks[blockCount].w = w;
	blocks[blockCount].h = h;
	blocks[blockCount].vx = 0;              // it can be pushed
	blocks[blockCount].vy = 0;              // and it can fall
	blocks[blockCount].angle = 0;           // and it can turn
	blocks[blockCount].spin = 0;
	blocks[blockCount].impactVX = 0;
	blocks[blockCount].impactVY = 0;
	blocks[blockCount].hurtCool = 0;
	blocks[blockCount].broken = 0;
	blocks[blockCount].breakTimer = 0;
	blocks[blockCount].flash = 0;
	blocks[blockCount].picture = picture;
	blocks[blockCount].material = material; // WOOD, STONE or GLASS
	blocks[blockCount].maxHealth = materialHealth(material);
	blocks[blockCount].health = blocks[blockCount].maxHealth;
	blocks[blockCount].alive = 1;
	blockCount++;
}


//  Adds one pig. (x, y) is the BOTTOM-LEFT corner.
//  picture: 0 = pig.png (weak) , 1 = pig2.png (normal) , 2 = pig3.png (helmet)
//  health : how much damage it can take before it disappears

// Adds one power-up star. Only level 5 calls this.
void addPowerUp(int x, int y)
{
	if (powerUpCount >= MAX_POWERUPS) return;

	powerUps[powerUpCount].x = x;
	powerUps[powerUpCount].y = y;
	powerUps[powerUpCount].alive = 1;
	powerUpCount++;
}

void addPig(int x, int y, int size, int picture, int health)
{
	if (pigCount >= MAX_PIGS) return;

	pigs[pigCount].x = x;
	pigs[pigCount].y = y;
	pigs[pigCount].size = size;
	pigs[pigCount].vx = 0;                   // a pig can fall
	pigs[pigCount].vy = 0;
	pigs[pigCount].picture = picture;
	pigs[pigCount].maxHealth = health;       // pig3 wears a helmet, so it is tough
	pigs[pigCount].health = health;
	pigs[pigCount].hurtCool = 0;
	pigs[pigCount].special = 0;      // only level 5 turns this on
	pigs[pigCount].dying = 0;
	pigs[pigCount].dieTimer = 0;
	pigs[pigCount].alive = 1;
	pigCount++;
}


//  Puts the next bird of the queue on the slingshot.

void putBirdOnSlingshot()
{
	birdCount = 1;
	egg.alive = 0;
	dragging = 0;

	birds[0].x = BIRD_START_X;
	birds[0].y = BIRD_START_Y;
	birds[0].vx = 0;
	birds[0].vy = 0;
	reloadTimer = -1;
	birds[0].powerUsed = 0;
	birds[0].hitCool = 0;
	birds[0].restCount = 0;
	birds[0].lifeTime = 0;
	birds[0].boostFrames = 0;
	birds[0].turnFrames = 0;
	birds[0].turnPush = 0;
	birds[0].vx = 0;
	birds[0].vy = 0;

	if (birdsLeft > 0)
	{
		birds[0].type = currentBird;
		birds[0].state = WAITING;
	}
	else
	{
		birds[0].type = BIRD_RED;
		birds[0].state = DONE;          // no shots left
	}

	// Terence is drawn much bigger than the others
	if (birds[0].type == BIRD_TERENCE) birds[0].size = TERENCE_SIZE;
	else                               birds[0].size = BIRD_SIZE;

	birds[0].mass = birdMass(birds[0].type);   // Terence pushes much harder
}


//  Picks ONE random bird from the list this level is allowed to
//  use. Called once for every new bird that joins the queue.
// Writes this level's allowed bird types into out[] and returns how
// many there are.
int levelBirdList(int out[])
{
	if (level == 1)
	{
		out[0] = BIRD_RED;
		out[1] = BIRD_CHUCK;
		out[2] = BIRD_BLUE;
		out[3] = BIRD_WHITE;
		return 4;                 // 4 kinds of bird can appear in level 1
	}

	if (level == 2)
	{
		out[0] = BIRD_RED;
		out[1] = BIRD_CHUCK;
		out[2] = BIRD_BOMB;
		out[3] = BIRD_WHITE;
		out[4] = BIRD_MATILDA;
		return 5;                 // 5 kinds of bird can appear in level 2
	}

	if (level == 4)
	{
		out[0] = BIRD_RED;
		out[1] = BIRD_CHUCK;
		out[2] = BIRD_BOMB;
		out[3] = BIRD_BLUE;
		out[4] = BIRD_WHITE;
		out[5] = BIRD_TERENCE;
		return 6;                 // 6 kinds of bird can appear in level 4
	}

	// levels 3 and 5 use every bird type
	out[0] = BIRD_RED;
	out[1] = BIRD_CHUCK;
	out[2] = BIRD_BOMB;
	out[3] = BIRD_MATILDA;
	out[4] = BIRD_BLUE;
	out[5] = BIRD_WHITE;
	out[6] = BIRD_TERENCE;
	return 7;
}

// Fills the bag with one of every allowed bird and shuffles it.
void refillBag()
{
	int list[TOTAL_BIRD_TYPES];
	int n = levelBirdList(list);

	bagCount = 0;
	for (int i = 0; i < n; i++)
	{
		birdBag[bagCount] = list[i];
		bagCount++;
	}

	// shuffle: swap every card with a random one further along
	for (int i = bagCount - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);
		int keep = birdBag[i];
		birdBag[i] = birdBag[j];
		birdBag[j] = keep;
	}
}

// Empties the bag, so a restart never continues an old shuffle.
void resetBag()
{
	bagCount = 0;
	lastBird = -1;
	lastBirdRun = 0;
	refillBag();
}

// Takes the next bird out of the bag. Because the bag holds ONE of
// each type, a bird cannot come back until every other bird has been
// used, and it can never appear more than twice in a row.
int randomBirdForLevel()
{
	if (bagCount <= 0) refillBag();

	int pick = birdBag[bagCount - 1];

	// already had this bird twice running? take a different card instead
	if (pick == lastBird && lastBirdRun >= 2)
	{
		for (int i = bagCount - 2; i >= 0; i--)
		{
			if (birdBag[i] != lastBird)
			{
				birdBag[bagCount - 1] = birdBag[i];
				birdBag[i] = pick;
				pick = birdBag[bagCount - 1];
				break;
			}
		}
	}

	bagCount--;

	if (pick == lastBird)
	{
		lastBirdRun++;
	}
	else
	{
		lastBird = pick;
		lastBirdRun = 1;
	}

	return pick;
}


//  Fills the whole queue at the start of a level: one bird for the
//  slingshot and three more waiting in line behind it.


void fillQueue()
{
	resetBag();                       // a fresh shuffle for this level
	currentBird = randomBirdForLevel();

	for (int i = 0; i < QUEUE_SIZE; i++)
		choice[i] = randomBirdForLevel();

	putBirdOnSlingshot();
}


//  A bird has been used up, so the whole line steps forward:
//
//     slingshot <- choice[0] <- choice[1] <- choice[2] <- new bird
//
//  Nothing is shuffled and nothing is re-rolled, so the order the
//  player can see is exactly the order the birds will arrive in.
//  Only ONE new random bird joins, at the very back.
void advanceQueue()
{
	currentBird = choice[0];                          // first in line goes

	for (int i = 0; i < QUEUE_SIZE - 1; i++)
		choice[i] = choice[i + 1];                    // the rest move up

	choice[QUEUE_SIZE - 1] = randomBirdForLevel();    // one new one at the back

	putBirdOnSlingshot();
}

//  The player pressed 1, 2 or 3: that waiting bird trades places
//  with the one on the slingshot. The other two do not move.
void selectBird(int k)
{
	if (k < 0 || k >= QUEUE_SIZE) return;
	if (birds[0].state != WAITING) return;   // not while it is flying
	if (dragging) return;                    // not while aiming

	int keep = currentBird;
	currentBird = choice[k];
	choice[k] = keep;

	putBirdOnSlingshot();
}

//  Builds one level.
void loadLevel(int n)
{
	level = n;
	winTimer = -1;
	reloadTimer = -1;
	blockCount = 0;
	pigCount = 0;
	powerUpCount = 0;      // levels 1-4 have no power-ups at all
	egg.alive = 0;

	if (n == 1)
	{
		// ---- LEVEL 1 : one small hut. Easy and open. ----
		birdsLeft = 8;                                    // level 1: 2 more than before
		fillQueue();

		addBlock(880, GROUND_Y, 12, 133, 0, WOOD);
		addBlock(1030, GROUND_Y, 12, 133, 0, WOOD);
		addBlock(866, GROUND_Y + 133, 190, 17, 1, GLASS);   // glass roof

		addPig(940, GROUND_Y, 77, 0, 1);                    // in the hut
		addPig(950, GROUND_Y + 150, 70, 1, 2);              // on the roof
		addPig(1140, GROUND_Y, 70, 0, 2);                   // out in the open
	}
	else if (n == 2)
	{
		// ---- LEVEL 2 : two rooms and an upper floor ----
		birdsLeft = 10;                                    // level 2: 2 more than before
		fillQueue();

		addBlock(850, GROUND_Y, 12, 140, 0, WOOD);
		addBlock(975, GROUND_Y, 12, 140, 0, WOOD);
		addBlock(1100, GROUND_Y, 12, 140, 0, WOOD);
		addBlock(836, GROUND_Y + 140, 276, 17, 1, WOOD);    // first floor

		addBlock(890, GROUND_Y + 157, 12, 120, 0, GLASS);
		addBlock(1040, GROUND_Y + 157, 12, 120, 0, GLASS);
		addBlock(876, GROUND_Y + 277, 176, 17, 1, WOOD);    // roof

		addPig(890, GROUND_Y, 77, 0, 2);                    // ground room 1
		addPig(1015, GROUND_Y, 77, 1, 2);                   // ground room 2
		addPig(940, GROUND_Y + 157, 87, 2, 3);              // upper room, helmet
		addPig(940, GROUND_Y + 294, 70, 0, 2);              // on the roof
	}
	else if (n == 3)
	{
		// ---- LEVEL 3 : three storeys, stone base, pigs walled in ----
		birdsLeft = 12;                                    // level 3: 2 more than before
		fillQueue();

		addBlock(840, GROUND_Y, 12, 154, 0, STONE);
		addBlock(970, GROUND_Y, 12, 154, 0, STONE);
		addBlock(1100, GROUND_Y, 12, 154, 0, STONE);
		addBlock(826, GROUND_Y + 154, 288, 17, 1, WOOD);    // first floor

		addBlock(880, GROUND_Y + 171, 12, 130, 0, STONE);   // stone walls now
		addBlock(1060, GROUND_Y + 171, 12, 130, 0, STONE);
		addBlock(866, GROUND_Y + 301, 208, 17, 1, WOOD);    // second floor

		addBlock(920, GROUND_Y + 318, 12, 98, 0, WOOD);
		addBlock(1020, GROUND_Y + 318, 12, 98, 0, WOOD);
		addBlock(906, GROUND_Y + 416, 128, 17, 1, GLASS);   // top floor

		addPig(880, GROUND_Y, 77, 0, 4);                    // ground room 1
		addPig(1010, GROUND_Y, 84, 2, 6);                   // ground room 2, helmet
		addPig(950, GROUND_Y + 171, 77, 1, 4);              // middle room
		addPig(945, GROUND_Y + 318, 70, 0, 4);              // top room
		addPig(945, GROUND_Y + 433, 70, 1, 3);              // on the roof
	}
	else if (n == 4)
	{
		// ---- LEVEL 4 : a fort - two stone towers joined by a bridge ----
		birdsLeft = 14;                                    // level 4: 2 more than before
		fillQueue();

		// left tower, two storeys
		addBlock(830, GROUND_Y, 12, 140, 0, STONE);
		addBlock(940, GROUND_Y, 12, 140, 0, STONE);
		addBlock(816, GROUND_Y + 140, 160, 17, 1, STONE);
		addBlock(856, GROUND_Y + 157, 12, 110, 0, WOOD);
		addBlock(936, GROUND_Y + 157, 12, 110, 0, WOOD);
		addBlock(842, GROUND_Y + 267, 120, 17, 1, WOOD);

		// right tower, taller
		addBlock(1060, GROUND_Y, 12, 190, 0, STONE);
		addBlock(1180, GROUND_Y, 12, 190, 0, STONE);
		addBlock(1046, GROUND_Y + 190, 160, 17, 1, STONE);
		addBlock(1086, GROUND_Y + 207, 12, 110, 0, WOOD);
		addBlock(1166, GROUND_Y + 207, 12, 110, 0, WOOD);
		addBlock(1072, GROUND_Y + 317, 120, 17, 1, WOOD);

		// the bridge joining the two towers, standing on its own legs
		addBlock(990, GROUND_Y, 12, 140, 0, WOOD);
		addBlock(1040, GROUND_Y, 12, 140, 0, WOOD);
		addBlock(970, GROUND_Y + 140, 96, 17, 1, WOOD);

		addPig(870, GROUND_Y, 77, 0, 4);                    // left tower, ground
		addPig(880, GROUND_Y + 157, 70, 1, 4);              // left tower, upper
		addPig(1100, GROUND_Y, 84, 2, 7);                   // right tower, helmet
		addPig(1110, GROUND_Y + 207, 70, 1, 4);             // right tower, upper
		addPig(1110, GROUND_Y + 334, 70, 0, 4);             // on the right roof
	}
	else
	{
		// ---- LEVEL 5 : the fortress. Four storeys, all stone. ----
		birdsLeft = 16;                                    // level 5: 2 more than before
		fillQueue();

		addBlock(840, GROUND_Y, 12, 150, 0, STONE);
		addBlock(960, GROUND_Y, 12, 150, 0, STONE);
		addBlock(1080, GROUND_Y, 12, 150, 0, STONE);
		addBlock(1200, GROUND_Y, 12, 150, 0, STONE);
		addBlock(826, GROUND_Y + 150, 388, 17, 1, STONE);   // first floor

		addBlock(880, GROUND_Y + 167, 12, 130, 0, STONE);
		addBlock(1000, GROUND_Y + 167, 12, 130, 0, STONE);
		addBlock(1120, GROUND_Y + 167, 12, 130, 0, STONE);
		addBlock(866, GROUND_Y + 297, 268, 17, 1, STONE);   // second floor

		addBlock(920, GROUND_Y + 314, 12, 110, 0, WOOD);
		addBlock(1060, GROUND_Y + 314, 12, 110, 0, WOOD);
		addBlock(906, GROUND_Y + 424, 168, 17, 1, WOOD);    // third floor

		addBlock(960, GROUND_Y + 441, 12, 90, 0, WOOD);
		addBlock(1020, GROUND_Y + 441, 12, 90, 0, WOOD);
		addBlock(946, GROUND_Y + 531, 98, 17, 1, GLASS);    // the very top

		addPig(880, GROUND_Y, 77, 0, 4);                    // ground room 1
		addPig(1000, GROUND_Y, 84, 2, 6);                   // ground room 2, helmet
		addPig(1120, GROUND_Y, 77, 0, 4);                   // ground room 3
		addPig(920, GROUND_Y + 167, 77, 1, 4);              // second floor room
		addPig(1040, GROUND_Y + 167, 84, 2, 5);             // second floor, helmet
		addPig(960, GROUND_Y + 314, 70, 1, 5);              // third floor room
		addPig(965, GROUND_Y + 441, 70, 0, 5);              // the top room
		addPig(965, GROUND_Y + 548, 70, 1, 4);              // right on the very top

		// ---- LEVEL 5 ONLY: the power-up stars ----
		// One tucked inside the base with the pigs, one floating in the
		// air on the way in, and one beside the base.
		addPowerUp(1150, GROUND_Y + 167);                   // inside the base
		addPowerUp(640, GROUND_Y + 430);                    // in the air
		addPowerUp(1290, GROUND_Y);                         // beside the base

		// ---- LEVEL 5 ONLY: the special star pig ----
		// pigs[0] is the plain pig on the left of the ground floor. A
		// plain pig is used on purpose: the helmet pig already has a
		// star painted on its helmet, so the marker would not show up.
		// Popping it gives one extra bird (see damagePig in physics.h).
		pigs[0].special = 1;
	}

	// SAFETY: a level must never have blocks but no pig, otherwise it
	// could never be won. If that ever happened, put one pig on the
	// ground in front of the structure.
	if (pigCount == 0)
		addPig(924, GROUND_Y, 77, 0, 1);
}


//Starts a completely new game from level 1.
void startNewGame()
{
	level = 1;
	score = 0;
	loadLevel(level);
	page = PAGE_GAME;
}

#endif
