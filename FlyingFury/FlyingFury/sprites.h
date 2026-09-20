//  sprites.h  -  loads all the picture files


#ifndef SPRITES_H
#define SPRITES_H

// ---- one int for every picture ----
int imgBackground;               // level 1 background
int imgLevelBg[6];               // one background per level: [1]..[5]
int imgHomepage;                 // Homepage/homepage.png - the whole start screen
int imgStar;                     // Enemies/star.png - marks the special pig
int imgSlingshot;
int imgBird[TOTAL_BIRD_TYPES];   // red, chuck, bomb, matilda, blue, white, terence
int imgPig[3];                   // pig.png, pig2.png, pig3.png
int imgBlock[3];                 // 0 = post, 1 = beam, 2 = thin post
int imgDot[7];                   // dot1.png ... dot7.png for the aiming line
int imgEgg;                      // Egg.png dropped by the white bird


void loadImages()
{
	imgBackground = iLoadImage("Images//Backgrounds//background.png");


	imgLevelBg[1] = imgBackground;
	imgLevelBg[2] = iLoadImage("Images//Backgrounds//bg2.png");
	imgLevelBg[3] = iLoadImage("Images//Backgrounds//bg3.png");
	imgLevelBg[4] = iLoadImage("Images//Backgrounds//bg4.png");
	imgLevelBg[5] = iLoadImage("Images//Backgrounds//bg5.png");


	imgHomepage = iLoadImage("Images//Homepage//homepage.png");

	// the little star drawn on the special power-up pig in level 5
	imgStar = iLoadImage("Images//Enemies//star.png");
	imgSlingshot  = iLoadImage("Images//Slingshot//slingshot.png");

	imgBird[BIRD_RED]     = iLoadImage("Images//Birds//red.png");
	imgBird[BIRD_CHUCK]   = iLoadImage("Images//Birds//chuck.png");
	imgBird[BIRD_BOMB]    = iLoadImage("Images//Birds//bomb.png");
	imgBird[BIRD_MATILDA] = iLoadImage("Images//Birds//matilda.png");
	imgBird[BIRD_BLUE]    = iLoadImage("Images//Birds//blue.png");
	imgBird[BIRD_WHITE]   = iLoadImage("Images//Birds//white.png");
	imgBird[BIRD_TERENCE] = iLoadImage("Images//Birds//terence.png");

	imgPig[0] = iLoadImage("Images//Enemies//pig.png");
	imgPig[1] = iLoadImage("Images//Enemies//pig2.png");
	imgPig[2] = iLoadImage("Images//Enemies//pig3.png");

	imgBlock[0] = iLoadImage("Images//Blocks//post.png");        // standing plank
	imgBlock[1] = iLoadImage("Images//Blocks//beam.png");        // lying plank
	imgBlock[2] = iLoadImage("Images//Blocks//post_thin.png");   // thin plank

	// the 7 dots of the aiming line
	imgDot[0] = iLoadImage("Images//Dotline//dot1.png");
	imgDot[1] = iLoadImage("Images//Dotline//dot2.png");
	imgDot[2] = iLoadImage("Images//Dotline//dot3.png");
	imgDot[3] = iLoadImage("Images//Dotline//dot4.png");
	imgDot[4] = iLoadImage("Images//Dotline//dot5.png");
	imgDot[5] = iLoadImage("Images//Dotline//dot6.png");
	imgDot[6] = iLoadImage("Images//Dotline//dot7.png");

	imgEgg = iLoadImage("Images//Egg//Egg.png");

	
}

#endif
