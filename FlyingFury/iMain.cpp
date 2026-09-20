//     main()        -> loads images, opens the window, iStart()
//     iDraw()       -> draws the current page, runs every frame
//     updateGame()  -> moves everything, called by a timer
//     iMouse()      -> menu clicks, shooting, special powers
//     fixedUpdate() -> keyboard keys


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "iGraphics.h"

#include "game.h"        // numbers, structs and variables
#include "sprites.h"     // loads the pictures
#include "levels.h"      // builds the levels
#include "physics.h"     // movement and collision
#include "render.h"      // drawing


int mouseInside(int mx, int my, int x, int y, int w, int h)
{
	if (mx >= x && mx <= x + w && my >= y && my <= y + h) return 1;
	return 0;
}


void iDraw()
{
	iClear();

	if (page == PAGE_MENU)     
		drawMenu();
	else if (page == PAGE_HELP)
		drawHelp();
	else if (page == PAGE_GAME) 
		drawGame();
	else if (page == PAGE_LEVEL_DONE)
	{
		// the finished level stays visible UNDER the popup message
		drawGame();
		drawLevelDone();
	}
	else if (page == PAGE_WIN) 
		drawWin();
	else if (page == PAGE_LOSE) 
		drawLose();
	else if (page == PAGE_SCORE) 
		drawScore();
}


void iMouse(int button, int state, int mx, int my)
{
	// ---------- START PAGE ----------
	if (page == PAGE_MENU)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if (mouseInside(mx, my, BTN_X, BTN_START_Y, BTN_W, BTN_H))
				startNewGame();

			if (mouseInside(mx, my, BTN_X, BTN_SCORE_Y, BTN_W, BTN_H))
				page = PAGE_SCORE;

			if (mouseInside(mx, my, BTN_X, BTN_HELP_Y, BTN_W, BTN_H))
				page = PAGE_HELP;

			if (mouseInside(mx, my, BTN_X, BTN_EXIT_Y, BTN_W, BTN_H))
				exit(0);
		}
		return;
	}

	// ---------- HELP PAGE ----------
	if (page == PAGE_HELP)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if (mouseInside(mx, my, 546, 100, 308, 70))
				page = PAGE_MENU;             // back to the start page
		}
		return;
	}

	// ---------- WIN / GAME OVER PAGE ----------
	if (page == PAGE_WIN || page == PAGE_LOSE)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if (mouseInside(mx, my, 546, 340, 308, 84))
				page = PAGE_MENU;
		}
		return;
	}

	// ---------- SCORE PAGE ----------
	if (page == PAGE_SCORE)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if (mouseInside(mx, my, 546, 330, 308, 60))
				page = PAGE_MENU;             // back to the start page
		}
		return;
	}

	// ---------- GAME PAGE ----------
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// clicked on the bird sitting on the slingshot -> start pulling
		if (birds[0].state == WAITING &&
			mouseInside(mx, my, (int)birds[0].x - 20, (int)birds[0].y - 20,
			            birds[0].size + 40, birds[0].size + 40))
		{
			dragging = 1;
		}
		// clicked while the bird is in the air -> use the special power
		else if (birds[0].state == FLYING)
		{
			usePower();
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (dragging) shootBird();
	}
}


void iMouseMove(int mx, int my)
{
	if (page != PAGE_GAME) 
		return;
	if (!dragging)
		return;
	if (birds[0].state != WAITING) 
		return;

	double dx = mx - BIRD_START_X;
	double dy = my - BIRD_START_Y;
	double dist = sqrt(dx * dx + dy * dy);

	if (dist > MAX_PULL)             // do not allow pulling too far
	{
		dx = dx / dist * MAX_PULL;
		dy = dy / dist * MAX_PULL;
	}

	// The pull is only ever cut down along its OWN direction, above, so
	// the angle you drag is exactly the angle you get. The old line
	// that clamped y to the grass afterwards bent the aim downwards
	// and has been removed - that is why low shots went somewhere else.
	birds[0].x = BIRD_START_X + dx;
	birds[0].y = BIRD_START_Y + dy;
}

void iPassiveMouseMove(int mx, int my)
{
	mouseX = mx;
	mouseY = my;
}


//     SPACE = special power     R = restart level
//     M     = main menu         ESC = exit

void fixedUpdate()
{
	if (isKeyPressed(27))            // 27 is the ESC key
		exit(0);

	if (isKeyPressed(' '))
		usePower();

	// ---- keys 1, 2, 3, 4 put that bird on the slingshot ----
	if (page == PAGE_GAME)
	{
		if (isKeyPressed('1')) selectBird(0);
		if (isKeyPressed('2')) selectBird(1);
		if (isKeyPressed('3')) selectBird(2);
	}

	if (isKeyPressed('r') || isKeyPressed('R'))
	{
		if (page == PAGE_GAME || page == PAGE_LOSE || page == PAGE_WIN)
		{
			if (page != PAGE_GAME) score = 0;
			loadLevel(level);
			page = PAGE_GAME;
		}
	}

	if (isKeyPressed('m') || isKeyPressed('M'))
		page = PAGE_MENU;
}

// ============================================================
//  4. MAIN
// ============================================================
int main()
{
	// background music and the game over sound
	mciSendString("open \"Audios//background.mp3\" alias bgsong", NULL, 0, NULL);
	mciSendString("open \"Audios//gameover.mp3\" alias ggsong", NULL, 0, NULL);
	mciSendString("play bgsong repeat", NULL, 0, NULL);

	// Start the random numbers from a different place every time the
	// game is run. WITHOUT this line rand() always gives exactly the
	// same numbers, so every level handed out the same birds in the
	// same order on every single run.
	srand((unsigned int)time(NULL));

	// Read Highest / Last / Total score from the save file, so the
	// Score page on the homepage shows what was saved last time.
	loadScores();

	// the timer runs the game 50 times per second
	iSetTimer(20, updateGame);

	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Flying Fury");

	loadImages();         
	loadLevel(1);    
	page = PAGE_MENU;

	iStart();
	return 0;
}
