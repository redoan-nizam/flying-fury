
//  render.h  -  everything that is drawn on the screen

#ifndef RENDER_H
#define RENDER_H

char textBuffer[100];      // used by sprintf_s for the score text


#define BTN_X       550
#define BTN_W       284
#define BTN_H       72
#define BTN_START_Y 433
#define BTN_SCORE_Y 336
#define BTN_HELP_Y  235
#define BTN_EXIT_Y  138

// ------------------------------------------------------------
//  Draws one button: a box with a word inside it.
// ------------------------------------------------------------
//  A button. If the mouse is on top of it, it lights up.
//  (mouseX and mouseY are updated in iPassiveMouseMove.)
void drawButton(int x, int y, int w, int h, char *label)
{
	int hot;
	if (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h)
		hot = 1;
	else
		hot = 0;

	// a dark shadow under the button
	iSetColor(20, 25, 35);
	iFilledRectangle(x + 4, y - 4, w, h);

	if (hot) iSetColor(255, 205, 60);       // lit up
	else     iSetColor(225, 60, 45);        // Angry Birds red

	iFilledRectangle(x, y, w, h);

	iSetColor(255, 255, 255);               // white border
	iRectangle(x, y, w, h);
	iRectangle(x + 3, y + 3, w - 6, h - 6);

	if (hot) iSetColor(40, 30, 10);
	else     iSetColor(255, 255, 255);

	iText(x + w / 2 - (int)strlen(label) * 6, y + h / 2 - 8,
	      label, GLUT_BITMAP_TIMES_ROMAN_24);
}

// ------------------------------------------------------------
//  START PAGE : plain sky blue background + 3 buttons
// ------------------------------------------------------------
void drawMenu()
{
	// The homepage is simply the picture, stretched over the window.
	// Nothing is drawn on top, so it looks exactly like the file.
	// The clicking is handled in iMain.cpp -> iMouse().
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgHomepage);
}

// ------------------------------------------------------------
//  HELP PAGE : how to play
// ------------------------------------------------------------
void drawHelp()
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgBackground);
	iSetColor(25, 32, 48);
	iFilledRectangle(84, 84, SCREEN_WIDTH - 168, SCREEN_HEIGHT - 168);
	iSetColor(255, 205, 60);
	iRectangle(84, 84, SCREEN_WIDTH - 168, SCREEN_HEIGHT - 168);

	iSetColor(255, 205, 60);
	iText(600, 720, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(235, 240, 250);
	iText(251, 644, "GOAL : kill every pig before the birds run out.", GLUT_BITMAP_HELVETICA_18);
	iText(251, 588, "1. Hold the LEFT MOUSE BUTTON on the bird.", GLUT_BITMAP_HELVETICA_18);
	iText(251, 546, "2. Drag backwards. Further back = more power.", GLUT_BITMAP_HELVETICA_18);
	iText(251, 503, "3. Release the button to shoot.", GLUT_BITMAP_HELVETICA_18);
	iText(251, 461, "4. Click again in the air to use the special power.", GLUT_BITMAP_HELVETICA_18);
	iText(251, 420, "Press 1, 2, 3 or 4 to pick one of the 4 offered birds.", GLUT_BITMAP_HELVETICA_18);

	iText(251, 371, "BIRDS (power = how much damage it does) :", GLUT_BITMAP_HELVETICA_18);
	iText(280, 333, "Red     power 1 - only breaks weak blocks", GLUT_BITMAP_HELVETICA_18);
	iText(280, 299, "Chuck   power 2 - click to fly faster (then power 3)", GLUT_BITMAP_HELVETICA_18);
	iText(280, 266, "Bomb    power 3 - click to explode", GLUT_BITMAP_HELVETICA_18);
	iText(280, 232, "Matilda power 1 - click to turn around", GLUT_BITMAP_HELVETICA_18);
	iText(280, 198, "Blue    power 1 - click to become 3 birds", GLUT_BITMAP_HELVETICA_18);
	iText(280, 165, "White   power 1 - click to drop an egg (egg power 2)", GLUT_BITMAP_HELVETICA_18);
	iText(200,  94, "Terence power 4 - big and heavy, breaks anything", GLUT_BITMAP_HELVETICA_18);

	drawButton(546, 100, 308, 70, "BACK");
}

// ------------------------------------------------------------
//  SCORE PAGE : shows the three saved numbers
// ------------------------------------------------------------
void drawScore()
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgBackground);
	iSetColor(25, 32, 48);
	iFilledRectangle(400, 300, 600, 300);
	iSetColor(255, 205, 60);
	iRectangle(400, 300, 600, 300);

	iSetColor(255, 205, 60);
	iText(560, 550, "SCORES", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(235, 240, 250);
	sprintf_s(textBuffer, "HIGHEST SCORE : %d", highestScore);
	iText(480, 495, textBuffer, GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf_s(textBuffer, "LAST SCORE    : %d", lastScore);
	iText(480, 455, textBuffer, GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf_s(textBuffer, "TOTAL SCORE   : %d", totalScore);
	iText(480, 415, textBuffer, GLUT_BITMAP_TIMES_ROMAN_24);

	drawButton(546, 330, 308, 60, "BACK");
}

// ------------------------------------------------------------
//  GAME PAGE
// ------------------------------------------------------------
void drawGame()
{
	// 1. the background picture for THIS level, stretched over the
	//    window. Level 1 uses background.png, levels 2-5 use bg2..bg5.
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgLevelBg[level]);

	// 2. the blocks. iRotate turns the picture around the middle of
	//    the block, so a falling plank really looks like it is turning.
	for (int i = 0; i < blockCount; i++)
	{
		if (!blocks[i].alive) continue;

		double cx = blocks[i].x + blocks[i].w / 2.0;
		double cy = blocks[i].y + blocks[i].h / 2.0;

		iRotate(cx, cy, blocks[i].angle);
		iShowImage((int)blocks[i].x, (int)blocks[i].y,
		           (int)blocks[i].w, (int)blocks[i].h, imgBlock[blocks[i].picture]);
		iUnRotate();

		// the break effect, shown for every destroyed block
		if (blocks[i].flash > 0)
		{
			double g = blocks[i].flash * 26.0;

			// the green channel brightens as the flash starts, but it
			// must never go above 255 (the brightest a color can be)
			double greenValue = 200 + g;
			if (greenValue > 255) greenValue = 255;

			iSetColor(255, greenValue, 120);
			iFilledRectangle(cx - blockHalfW(i), cy - blockHalfH(i),
			                 blockW(i), blockH(i));
			blocks[i].flash--;
		}
	}

	// 3. the pigs (only the ones that are still alive)
	for (int i = 0; i < pigCount; i++)
	{
		if (!pigs[i].alive) continue;

		// A beaten pig keeps its NORMAL size and simply stays on screen
		// for a moment before it disappears.
		iShowImage((int)pigs[i].x, (int)pigs[i].y,
		           pigs[i].size, pigs[i].size, imgPig[pigs[i].picture]);

		// LEVEL 5 ONLY: a small star on the TOP-RIGHT corner of the pig
		// marks it as the special power-up pig. Popping it gives one
		// extra bird (handled in damagePig() in physics.h).
		if (pigs[i].special)
			iShowImage((int)pigs[i].x + pigs[i].size - 26,
			           (int)pigs[i].y + pigs[i].size - 26,
			           30, 30, imgStar);
	}

	// 3b. the power-up stars (level 5 only - the other levels have none)
	for (int i = 0; i < powerUpCount; i++)
	{
		if (powerUps[i].alive)
			iShowImage(powerUps[i].x, powerUps[i].y,
			           POWERUP_SIZE, POWERUP_SIZE, imgStar);
	}

	// 4. the slingshot
	iShowImage(SLING_X, GROUND_Y - 20, SLING_W, SLING_H, imgSlingshot);

	// 5. the birds
	for (int i = 0; i < birdCount; i++)
	{
		if (birds[i].state != DONE)
			iShowImage((int)birds[i].x, (int)birds[i].y,
			           birds[i].size, birds[i].size, imgBird[birds[i].type]);
	}

	// 6. the egg of the white bird (Images//Egg//Egg.png)
	if (egg.alive)
		iShowImage((int)egg.x, (int)egg.y, EGG_SIZE, EGG_SIZE + 4, imgEgg);

	// 7. the birds waiting their turn, on the LEFT side.
	//    Just the pictures - no box, no card, no white background and
	//    no numbers. The bird you have chosen is simply drawn bigger.
	//    choice[0] is the bird that comes next, so it is drawn at the
	//    front (lowest) and a little bigger than the two behind it.
	for (int i = 0; i < QUEUE_SIZE; i++)
	{
		int qy = 240 + i * 95;               // a column up the left edge

		if (i == 0)
			iShowImage(10, qy - 8, 84, 84, imgBird[choice[i]]);  // next up
		else
			iShowImage(22, qy, 62, 62, imgBird[choice[i]]);
	}

	// 8. the dotted aiming line, made only from the dot pictures
	if (dragging && birds[0].state == WAITING)
	{
		double px = birds[0].x + birds[0].size / 2.0;   // start at the bird
		double py = birds[0].y + birds[0].size / 2.0;
		double vx = (BIRD_START_X - birds[0].x) * POWER;   // same speed the
		double vy = (BIRD_START_Y - birds[0].y) * POWER;   // shot will have

		for (int d = 0; d < 14; d++)
		{
			// move forward a few steps, exactly like the flying bird does
			for (int step = 0; step < 5; step++)
			{
				vy = vy - GRAVITY;
				px = px + vx;
				py = py + vy;
			}
			if (py < GROUND_Y || px > SCREEN_WIDTH) break;

			// dot1.png ... dot7.png used again and again
			iShowImage((int)px - 13, (int)py - 10, 25, 20, imgDot[d % 7]);
		}
	}

	// 9. the score and the number of birds left
	//    (dark bars first, so white text can be read on the bright sky)
	iSetColor(30, 40, 60);
	iFilledRectangle(0, SCREEN_HEIGHT - 56, SCREEN_WIDTH, 56);
	iFilledRectangle(0, 0, SCREEN_WIDTH, 46);

	iSetColor(255, 255, 255);
	sprintf_s(textBuffer, "LEVEL %d   SCORE : %d   BIRD : %s (power %d)",
	          level, score, birdName(birds[0].type), birdPower(birds[0].type));
	iText(28, SCREEN_HEIGHT - 40, textBuffer, GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf_s(textBuffer, "BIRDS LEFT : %d", birdsLeft);
	iText(SCREEN_WIDTH - 330, SCREEN_HEIGHT - 40, textBuffer, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(235, 240, 250);
	iText(28, 16, "Keys 1-3 swap a waiting bird in. Drag & release to shoot. Click in the air = power. R = restart, M = menu",
	      GLUT_BITMAP_HELVETICA_18);
}

// ------------------------------------------------------------
//  LEVEL COMPLETE MESSAGE
//  Drawn on top of the finished level for about 2 seconds. The bar
//  fills up while the next level is loading.
// ------------------------------------------------------------
void drawLevelDone()
{
	// how far through the message we are: 0.0 at the start, 1.0 at the end
	double done = 1.0 - (double)levelDoneTimer / (double)LEVEL_DONE_FRAMES;

	// the panel drops down into place during the first few frames
	int slide = 0;
	if (done < 0.18) slide = (int)((0.18 - done) * 900);

	int panelY = 300 - slide;

	iSetColor(20, 26, 40);
	iFilledRectangle(400, panelY, 600, 250);
	iSetColor(255, 205, 60);
	iRectangle(400, panelY, 600, 250);
	iRectangle(405, panelY + 5, 590, 240);

	// first the news, then a moment later the loading line
	iSetColor(255, 205, 60);
	iText(470, panelY + 195, "ALL ENEMIES DESTROYED !", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(235, 240, 250);
	sprintf_s(textBuffer, "Level %d cleared      Score : %d", level, score);
	iText(500, panelY + 155, textBuffer, GLUT_BITMAP_HELVETICA_18);

	if (done > 0.35)                       // appears after a short pause
	{
		sprintf_s(textBuffer, "Loading Level %d ...", level + 1);
		iText(560, panelY + 85, textBuffer, GLUT_BITMAP_HELVETICA_18);
	}

	// the loading bar
	iSetColor(60, 70, 90);
	iFilledRectangle(450, panelY + 40, 500, 24);
	iSetColor(255, 205, 60);
	iFilledRectangle(450, panelY + 40, (int)(500 * done), 24);
	iSetColor(255, 255, 255);
	iRectangle(450, panelY + 40, 500, 24);
}

// ------------------------------------------------------------
//  WIN PAGE
// ------------------------------------------------------------
void drawWin()
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgBackground);
	iSetColor(25, 32, 48);
	iFilledRectangle(400, 300, 600, 300);
	iSetColor(255, 205, 60);
	iRectangle(400, 300, 600, 300);
	iText(600, 520, "YOU WIN !", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(235, 240, 250);
	sprintf_s(textBuffer, "FINAL SCORE : %d", score);
	iText(574, 489, textBuffer, GLUT_BITMAP_TIMES_ROMAN_24);

	drawButton(546, 340, 308, 84, "MENU");
}

// ------------------------------------------------------------
//  GAME OVER PAGE
// ------------------------------------------------------------
void drawLose()
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgBackground);
	iSetColor(25, 32, 48);
	iFilledRectangle(400, 300, 600, 300);
	iSetColor(255, 205, 60);
	iRectangle(400, 300, 600, 300);
	iText(581, 560, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(235, 240, 250);
	iText(503, 489, "You ran out of birds. Press R to try again.", GLUT_BITMAP_HELVETICA_18);

	drawButton(546, 340, 308, 84, "MENU");
}

#endif
