
//  physics.h  -  movement, real collisions, powers, win and lose

#ifndef PHYSICS_H
#define PHYSICS_H


//  Do two rectangles overlap?  Returns 1 for yes, 0 for no.
int boxesTouch(double ax, double ay, double aw, double ah,
               double bx, double by, double bw, double bh)
{
	if (ax + aw < bx) return 0;      // A is completely left of B
	if (bx + bw < ax) return 0;      // A is completely right of B
	if (ay + ah < by) return 0;      // A is completely below B
	if (by + bh < ay) return 0;      // A is completely above B
	return 1;                        // they overlap
}

// How deep two rectangles overlap, on x and on y.
double overlapX(double ax, double aw, double bx, double bw)
{
	// the RIGHT edge of the overlap is whichever rectangle ENDS first
	double right;
	if (ax + aw < bx + bw) right = ax + aw;
	else                   right = bx + bw;

	// the LEFT edge of the overlap is whichever rectangle STARTS later
	double left;
	if (ax > bx) left = ax;
	else         left = bx;

	return right - left;
}
double overlapY(double ay, double ah, double by, double bh)
{
	// the TOP edge of the overlap is whichever rectangle ENDS first
	double top;
	if (ay + ah < by + bh) top = ay + ah;
	else                   top = by + bh;

	// the BOTTOM edge of the overlap is whichever rectangle STARTS later
	double bottom;
	if (ay > by) bottom = ay;
	else         bottom = by;

	return top - bottom;
}

// ============================================================
//  THE BOX A TURNED BLOCK REALLY TAKES UP
//
//  A block is stored as x, y, w, h, but once it has tilted it no
//  longer fills that upright box. A plank lying on its side is WIDE
//  and SHORT, not tall and thin. These little functions work out the
//  box the turned block actually covers, and every collision below
//  uses them. This is what makes a toppled plank lie down properly
//  instead of still behaving as if it were standing up.
// ============================================================
// The CENTRE point of a block: halfway across it on x, and halfway
// up it on y. Used everywhere below, so it is worked out here once.
double blockCX(int i)
{
	double centreX = blocks[i].x + blocks[i].w / 2.0;
	return centreX;
}
double blockCY(int i)
{
	double centreY = blocks[i].y + blocks[i].h / 2.0;
	return centreY;
}

double blockHalfW(int i)
{
	double angleInRadians = blocks[i].angle * PI / 180.0;

	// When a block is turned, part of its WIDTH and part of its HEIGHT
	// both add to how wide it looks now. Adding the two parts together
	// gives the true half-width of the turned block.
	double widthPart  = fabs(cos(angleInRadians)) * blocks[i].w / 2.0;
	double heightPart = fabs(sin(angleInRadians)) * blocks[i].h / 2.0;
	double halfWidth = widthPart + heightPart;

	// a long beam tilting must not grow so wide it swallows the whole
	// stack next to it, so its growth is capped
	double maxHalfWidth = blocks[i].w / 2.0 * CFG.extentGrowMax;
	if (halfWidth > maxHalfWidth) halfWidth = maxHalfWidth;

	return halfWidth;
}
double blockHalfH(int i)
{
	double angleInRadians = blocks[i].angle * PI / 180.0;

	double widthPart  = fabs(sin(angleInRadians)) * blocks[i].w / 2.0;
	double heightPart = fabs(cos(angleInRadians)) * blocks[i].h / 2.0;
	double halfHeight = widthPart + heightPart;

	double maxHalfHeight = blocks[i].h / 2.0 * CFG.extentGrowMax;
	if (halfHeight > maxHalfHeight) halfHeight = maxHalfHeight;

	return halfHeight;
}

// The turned block's left edge, bottom edge, full width and full
// height - all worked out from the centre point and the half sizes
// above.
double blockLeft(int i)
{
	double leftEdge = blockCX(i) - blockHalfW(i);
	return leftEdge;
}
double blockBottom(int i)
{
	double bottomEdge = blockCY(i) - blockHalfH(i);
	return bottomEdge;
}
double blockW(int i)
{
	double fullWidth = blockHalfW(i) * 2.0;
	return fullWidth;
}
double blockH(int i)
{
	double fullHeight = blockHalfH(i) * 2.0;
	return fullHeight;
}


double birdHitSize(int i)
{
	double hitSize = birds[i].size * BIRD_HIT_PART;
	return hitSize;
}
double birdHitX(int i)
{
	double emptySpace = birds[i].size - birdHitSize(i);
	double hitX = birds[i].x + emptySpace / 2.0;
	return hitX;
}
double birdHitY(int i)
{
	double emptySpace = birds[i].size - birdHitSize(i);
	double hitY = birds[i].y + emptySpace / 2.0;
	return hitY;
}

double pigHitSize(int p)
{
	double hitSize = pigs[p].size * PIG_HIT_PART;
	return hitSize;
}
double pigHitX(int p)
{
	double emptySpace = pigs[p].size - pigHitSize(p);
	double hitX = pigs[p].x + emptySpace / 2.0;
	return hitX;
}
double pigHitY(int p)
{
	double emptySpace = pigs[p].size - pigHitSize(p);
	double hitY = pigs[p].y + emptySpace / 2.0;
	return hitY;
}

// Is anything still holding this block up? Used to wake a sleeping
// block the moment whatever it was standing on disappears, so nothing
// can ever be left floating in the air.
int blockIsSupported(int i)
{
	// on the grass. Both the turned hull AND the block's own bottom
	// edge are checked: a tilted block lies flat on the ground while
	// its turned hull reads a few pixels higher, and testing only the
	// hull made it look unsupported so it never stopped twitching.
	if (blockBottom(i) <= GROUND_Y + 2.0) return 1;
	if (blocks[i].y <= GROUND_Y + 2.0) return 1;

	for (int j = 0; j < blockCount; j++)
	{
		if (j == i || !blocks[j].alive || blocks[j].broken) continue;

		double top = blockBottom(j) + blockH(j);
		if (top > blockBottom(i) + 12.0) continue;       // not underneath
		if (top < blockBottom(i) - 8.0) continue;        // too far below
		if (overlapX(blockLeft(i), blockW(i), blockLeft(j), blockW(j)) > 0)
			return 1;
	}

	// a block can also be resting on a PIG - without this it was
	// reported as floating and shaken awake for ever
	for (int p = 0; p < pigCount; p++)
	{
		if (!pigs[p].alive) continue;
		double top = pigs[p].y + pigs[p].size;
		if (top > blockBottom(i) + 12.0) continue;
		if (top < blockBottom(i) - 8.0) continue;
		if (overlapX(blockLeft(i), blockW(i), pigs[p].x, pigs[p].size) > 0)
			return 1;
	}
	return 0;
}


int pigIsSupported(int p)
{
	if (pigs[p].y <= GROUND_Y + 2.0) return 1;

	for (int b = 0; b < blockCount; b++)
	{
		if (!blocks[b].alive || blocks[b].broken) continue;
		double top = blockBottom(b) + blockH(b);
		if (top > pigs[p].y + 12.0) continue;
		if (top < pigs[p].y - 8.0) continue;
		if (overlapX(pigs[p].x, pigs[p].size, blockLeft(b), blockW(b)) > 0)
			return 1;
	}
	return 0;
}


//Are all the pigs dead?

int allPigsDead()
{
	for (int i = 0; i < pigCount; i++)
		if (pigs[i].alive && !pigs[i].dying) return 0;   // beaten counts as gone
	return 1;
}


void destroyBlock(int b)
{
	if (!blocks[b].alive || blocks[b].broken) return;

	blocks[b].broken = 1;
	blocks[b].breakTimer = BLOCK_FALL_FRAMES;
	blocks[b].health = 0;

	score += SCORE_PER_BLOCK;                       // ALWAYS scored
	blocks[b].flash = BLOCK_FLASH_FRAMES;           // ALWAYS shows the effect
}

void damageBlock(int b, int dmg)
{
	if (!blocks[b].alive || blocks[b].broken || dmg <= 0) return;

	blocks[b].health = blocks[b].health - dmg;

	if (blocks[b].health <= 0)
	{
		destroyBlock(b);



		for (int k = 0; k < blockCount; k++)
		{
			if (k == b || !blocks[k].alive || blocks[k].broken) continue;

			// is it sitting right on top of the broken one?
			double gap = blockBottom(k) - (blockBottom(b) + blockH(b));
			if (gap < -6 || gap > 10) continue;
			if (overlapX(blockLeft(k), blockW(k), blockLeft(b), blockW(b)) <= 0) continue;

			// tip away from the middle of the block that just broke
			double side = blockCX(k) - blockCX(b);
			if (side >= 0) side = 1;
			else            side = -1;

			blocks[k].vx = blocks[k].vx + side * 1.6;
			blocks[k].spin = blocks[k].spin - side * 2.2;
		}
	}
}

void damagePig(int p, int dmg)
{
	if (!pigs[p].alive || pigs[p].dying || dmg <= 0) return;

	pigs[p].health = pigs[p].health - dmg;

	if (pigs[p].health <= 0)
	{
		// It is beaten NOW - the level already counts it - but it
		// stays on screen for about 2 seconds so you can see it react
		// and pop, instead of blinking out the instant it is touched.
		pigs[p].dying = 1;
		pigs[p].dieTimer = PIG_DIE_FRAMES;
		score += SCORE_PER_PIG;      // a pig is a pig, however it died

		// LEVEL 5 ONLY: the pig wearing the star gives one extra bird
		// and some extra score. levels.h sets special = 1 on it.
		if (pigs[p].special)
		{
			birdsLeft++;
			score += SCORE_SPECIAL_PIG;
			pigs[p].special = 0;     // it can only be collected once
		}
	}
}


void knockPig(int p, double hitSpeed, double weight)
{
	if (!pigs[p].alive || pigs[p].dying) return;
	if (pigs[p].hurtCool > 0) return;            // already hurt a moment ago
	if (hitSpeed <= PIG_HURT_SPEED) return;      // too gentle to hurt

	int dmg = (int)((hitSpeed - PIG_HURT_SPEED) * weight / PIG_HURT_DIV);

	if (dmg > 0)
	{
		damagePig(p, dmg);
		pigs[p].hurtCool = PIG_HURT_COOL;
	}
}

// ------------------------------------------------------------
//  An explosion: everything close by is damaged AND thrown away.
// ------------------------------------------------------------
void blastAt(double bx, double by, double reach, int damage)
{
	for (int i = 0; i < blockCount; i++)
	{
		if (!blocks[i].alive) continue;

		double cx = blockCX(i);
		double cy = blockCY(i);
		double dx = cx - bx;
		double dy = cy - by;
		double dist2 = dx * dx + dy * dy;

		if (dist2 < reach * reach)
		{
			double dist = sqrt(dist2);
			if (dist < 1) dist = 1;

			// the blast throws the block outwards and makes it spin
			double force = 19.6 * (1.0 - dist / reach) / materialMass(blocks[i].material);
			blocks[i].vx = blocks[i].vx + (dx / dist) * force;
			blocks[i].vy = blocks[i].vy + (dy / dist) * force;
			blocks[i].spin = blocks[i].spin + (dx / dist) * force * 0.43;

			damageBlock(i, damage);
		}
	}

	for (int i = 0; i < pigCount; i++)
	{
		if (!pigs[i].alive) continue;

		double cx = pigs[i].x + pigs[i].size / 2.0;
		double cy = pigs[i].y + pigs[i].size / 2.0;
		double dx = cx - bx;
		double dy = cy - by;
		double dist2 = dx * dx + dy * dy;

		if (dist2 < reach * reach)
		{
			double dist = sqrt(dist2);
			if (dist < 1) dist = 1;
			double force = 14.0 * (1.0 - dist / reach);
			pigs[i].vx = pigs[i].vx + (dx / dist) * force;
			pigs[i].vy = pigs[i].vy + (dy / dist) * force;

			damagePig(i, damage);      // an explosion always hurts
		}
	}
}



int damageOf(int i)
{
	int power = birdPower(birds[i].type);

	if (birds[i].type == BIRD_CHUCK && birds[i].powerUsed)
		power = power + 1;          // a faster bird hits harder

	return power;
}


void usePower()
{
	if (page != PAGE_GAME) return;
	if (birds[0].state != FLYING) return;     // never on the slingshot
	if (birds[0].powerUsed) return;           // only once per bird

	birds[0].powerUsed = 1;

	if (birds[0].type == BIRD_CHUCK)
	{
		// yellow bird: speeds up over the next few frames, so you can
		// actually see him accelerate instead of jumping instantly
		birds[0].boostFrames = 6;      // 1.12 six times is about double
	}
	else if (birds[0].type == BIRD_BOMB)
	{
		// black bird: explodes where it is
		blastAt(birds[0].x + birds[0].size / 2.0,
		        birds[0].y + birds[0].size / 2.0, 182, 4);
		birds[0].state = DONE;
	}
	else if (birds[0].type == BIRD_MATILDA)
	{
		// green bird: swings round like a boomerang. Her sideways speed
		// is changed a little every frame for 12 frames, so the turn is
		// a smooth curve instead of a sudden flip.
		birds[0].turnFrames = 12;
		birds[0].turnPush = -birds[0].vx * 2.0 / 12.0;
		birds[0].vy = birds[0].vy * 0.5 + 5.6;
	}
	else if (birds[0].type == BIRD_WHITE)
	{
		// white bird: drops an egg and is pushed upward
		egg.x = birds[0].x + birds[0].size / 2.0 - EGG_SIZE / 2.0;
		egg.y = birds[0].y - EGG_SIZE;
		egg.vy = -4.2;
		egg.alive = 1;
		birds[0].vy = birds[0].vy + 12.6;
	}
	else if (birds[0].type == BIRD_BLUE)
	{
		// blue bird: becomes 3 birds going in 3 directions
		birdCount = 3;
		for (int i = 1; i < 3; i++)
		{
			birds[i] = birds[0];             // copy everything
			birds[i].size = BIRD_SIZE - 17;
			birds[i].hitCool = 0;            // each one may hit on its own
			birds[i].restCount = 0;
			birds[i].lifeTime = 0;
			birds[i].boostFrames = 0;
			birds[i].turnFrames = 0;
		}
		birds[0].size = BIRD_SIZE - 17;
		birds[1].vy = birds[1].vy + 4.2;     // one goes higher
		birds[2].vy = birds[2].vy - 4.2;     // one goes lower
	}

}


void shootBird()
{
	double dx = BIRD_START_X - birds[0].x;    // how far it was pulled back
	double dy = BIRD_START_Y - birds[0].y;

	dragging = 0;

	if (dx * dx + dy * dy < 25)               // only a true tap is ignored
	{
		birds[0].x = BIRD_START_X;
		birds[0].y = BIRD_START_Y;
		return;
	}

	birds[0].vx = dx * POWER;
	birds[0].vy = dy * POWER;
	birds[0].state = FLYING;
	birds[0].hitCool = 0;
	birdsLeft--;                              // one shot is used up
}


void birdHitsBlock(int i, int b)
{
	if (blocks[b].broken) return;              // already smashed, just debris

	double s  = birdHitSize(i);                // the real hitting box,
	double hx = birdHitX(i);                   // smaller than the picture
	double hy = birdHitY(i);

	double ox = overlapX(hx, s, blockLeft(b), blockW(b));
	double oy = overlapY(hy, s, blockBottom(b), blockH(b));
	if (ox <= 0 || oy <= 0) return;            // they do not really touch

	// ---- 1. which way would the bird be pushed out? ----
	double nx = 0, ny = 0;
	if (ox < oy)
	{
		if (hx + s / 2.0 < blockCX(b)) nx = -1;
		else                           nx = 1;
	}
	else
	{
		if (hy + s / 2.0 < blockCY(b)) ny = -1;
		else                           ny = 1;
	}

	// ---- 2. how hard was the hit? ----
	double impact = -(birds[i].vx * nx + birds[i].vy * ny);
	if (impact < 0) impact = 0;

	// ---- 3. damage FIRST, so we know if the block survives ----
	//      Each block has its own short cooldown, so smashing a plank
	//      does not stop the same bird hurting the pig behind it.
	if (blocks[b].hurtCool == 0)
	{
		int dmg = (int)(damageOf(i) * impact / 5.6);
		damageBlock(b, dmg);            // a gentle touch does 0 damage
		blocks[b].hurtCool = 8;         // one touch = one hit
	}

	// ---- 4. the block broke: the bird SMASHES THROUGH it ----
	//      It only slows down a little. Bouncing back off a block it
	//      has just shattered is what made a strong shot feel useless.
	if (blocks[b].broken || !blocks[b].alive)
	{
		birds[i].vx = birds[i].vx * 0.55;   // smashing through costs speed
		birds[i].vy = birds[i].vy * 0.55;
		return;
	}

	// ---- 5. the block held: push the bird out and bounce it off ----
	if (nx < 0) birds[i].x = birds[i].x - ox;
	if (nx > 0) birds[i].x = birds[i].x + ox;
	if (ny < 0) birds[i].y = birds[i].y - oy;
	if (ny > 0) birds[i].y = birds[i].y + oy;

	if (impact < 1.5)                          // barely touching: settle
	{
		if (nx != 0) birds[i].vx = 0;
		else         birds[i].vy = 0;
		birds[i].vx = birds[i].vx * FRICTION;
	}
	else if (nx != 0)
	{
		birds[i].vx = -birds[i].vx * BOUNCE;
		birds[i].vy = birds[i].vy * FRICTION;
	}
	else
	{
		birds[i].vy = -birds[i].vy * BOUNCE;
		birds[i].vx = birds[i].vx * FRICTION;
	}

	// ---- 6. the block is pushed away and starts to turn ----
	double push = impact * birds[i].mass / materialMass(blocks[b].material);
	blocks[b].vx = blocks[b].vx - nx * push * 0.30;
	blocks[b].vy = blocks[b].vy - ny * push * 0.30;

	if (nx != 0)
	{
		double offY = (hy + s / 2.0) - blockCY(b);
		blocks[b].spin = blocks[b].spin - nx * offY * push * 0.011;
	}
	else
	{
		double offX = (hx + s / 2.0) - blockCX(b);
		blocks[b].spin = blocks[b].spin + ny * offX * push * 0.011;
	}
}

// ============================================================
//  BIRD  <->  PIG      exactly the same idea
// ============================================================
void birdHitsPig(int i, int p)
{
	double s  = birdHitSize(i);
	double hx = birdHitX(i);
	double hy = birdHitY(i);
	double ps = pigHitSize(p);

	double ox = overlapX(hx, s, pigHitX(p), ps);
	double oy = overlapY(hy, s, pigHitY(p), ps);
	if (ox <= 0 || oy <= 0) return;            // a near miss really misses now

	double nx = 0, ny = 0;
	if (ox < oy)
	{
		if (hx + s / 2.0 < pigHitX(p) + ps / 2.0)
		{
			birds[i].x = birds[i].x - ox;
			nx = -1;
		}
		else
		{
			birds[i].x = birds[i].x + ox;
			nx = 1;
		}
	}
	else
	{
		if (hy + s / 2.0 < pigHitY(p) + ps / 2.0)
		{
			birds[i].y = birds[i].y - oy;
			ny = -1;
		}
		else
		{
			birds[i].y = birds[i].y + oy;
			ny = 1;
		}
	}

	double impact = -(birds[i].vx * nx + birds[i].vy * ny);
	if (impact < 0) impact = 0;

	// the pig is knocked away
	pigs[p].vx = pigs[p].vx - nx * impact * 0.35 * birds[i].mass;
	pigs[p].vy = pigs[p].vy - ny * impact * 0.35 * birds[i].mass;

	// A DIRECT BIRD HIT MUST ALWAYS COUNT.
	// pigs[p].hurtCool exists so that one falling block does not hurt a
	// pig once per frame. But it was also blocking the bird: if a block
	// had brushed the pig a moment earlier, a full power hit did NOTHING
	// at all. The bird has its own cooldown, so we clear the pig's one
	// and let the hit through.
	if (birds[i].hitCool == 0)
	{
		pigs[p].hurtCool = 0;

		// the bird's own power counts as extra weight behind the hit.
		// knockPig already keeps its own cooldown for each pig.
		knockPig(p, impact, birds[i].mass * damageOf(i) * 0.7);

		// A DIRECT HIT ALWAYS COUNTS. knockPig rounds down, so a slower
		// hit from a light bird could work out at 0 damage and the pig
		// walked away from a dead-on strike. Every real contact above
		// pigMinHitSpeed now takes at least one point off.
		if (impact > CFG.pigMinHitSpeed && !pigs[p].dying)
			damagePig(p, 1);

		birds[i].hitCool = 6;
	}

	// if that finished the pig off, the bird carries straight on
	if (!pigs[p].alive)
	{
		birds[i].vx = birds[i].vx * 0.75;
		birds[i].vy = birds[i].vy * 0.75;
		return;
	}

	if (nx != 0) birds[i].vx = -birds[i].vx * BOUNCE;
	else         birds[i].vy = -birds[i].vy * BOUNCE;
}


void collectPowerUps(int i)
{
	for (int k = 0; k < powerUpCount; k++)
	{
		if (!powerUps[k].alive) continue;

		if (boxesTouch(birds[i].x, birds[i].y, birds[i].size, birds[i].size,
		               powerUps[k].x, powerUps[k].y,
		               POWERUP_SIZE, POWERUP_SIZE))
		{
			powerUps[k].alive = 0;      // collected, gone for good
			birdsLeft++;                // the extra bird
			score += SCORE_POWERUP;
		}
	}
}

// ------------------------------------------------------------
//  Moves one bird. The movement is cut into PHYSICS_STEPS small
//  steps, so a fast bird cannot pass through a thin plank.
// ------------------------------------------------------------
void moveBird(int i)
{
	if (birds[i].state != FLYING) return;

	if (birds[i].hitCool > 0) birds[i].hitCool--;

	// Chuck getting faster
	if (birds[i].boostFrames > 0)
	{
		birds[i].vx = birds[i].vx * 1.12;
		birds[i].vy = birds[i].vy * 1.12;
		birds[i].boostFrames--;
	}

	// Matilda swinging round
	if (birds[i].turnFrames > 0)
	{
		birds[i].vx = birds[i].vx + birds[i].turnPush;
		birds[i].turnFrames--;
	}


	int hitGround = 0;

	// where the bird was at the start of this frame, so we can see how
	// far it REALLY travelled. Its speed alone is not enough: a bird
	// sitting on a block is pushed back up every step, so its speed
	// never quite reaches zero even though it is not going anywhere.
	double wasX = birds[i].x;
	double wasY = birds[i].y;

	// The faster the bird is going, the more pieces the frame is cut
	// into, so the bird never moves more than about 4 pixels at a time
	// and can never skip over a thin plank, however hard it was fired.
	double howFast = sqrt(birds[i].vx * birds[i].vx + birds[i].vy * birds[i].vy);
	int steps = PHYSICS_STEPS + (int)(howFast / 4.0);
	if (steps > 60) steps = 60;

	for (int step = 0; step < steps; step++)
	{
		birds[i].vy = birds[i].vy - GRAVITY / steps;
		birds[i].x = birds[i].x + birds[i].vx / steps;
		birds[i].y = birds[i].y + birds[i].vy / steps;

		for (int b = 0; b < blockCount; b++)
			if (blocks[b].alive) birdHitsBlock(i, b);

		for (int p = 0; p < pigCount; p++)
			if (pigs[p].alive) birdHitsPig(i, p);

		collectPowerUps(i);          // level 5 stars


		// the ground
		if (birds[i].y < GROUND_Y)
		{
			birds[i].y = GROUND_Y;

			if (!hitGround)                  // only the first touch counts
			{
				if (birds[i].vy > -1.5) birds[i].vy = 0;      // settle
				else                    birds[i].vy = -birds[i].vy * BOUNCE;
				birds[i].vx = birds[i].vx * 0.94;   // rolls on, does not stop dead
				hitGround = 1;
			}
			else if (birds[i].vy < 0)
			{
				birds[i].vy = 0;
			}
		}
	}

	// ---- when is the shot over? ----
	// It is NOT enough to test "is it on the ground", because a bird

	birds[i].lifeTime++;

	double movedX = birds[i].x - wasX;
	double movedY = birds[i].y - wasY;
	double moved  = sqrt(movedX * movedX + movedY * movedY);

	if (moved < REST_MOVE) birds[i].restCount++;
	else                    birds[i].restCount = 0;

	if (birds[i].x > SCREEN_WIDTH || birds[i].x < -birds[i].size ||
		birds[i].restCount > 40 ||          // it has stopped somewhere
		birds[i].lifeTime > 900)            // safety net: never fly for ever
	{
		birds[i].state = DONE;
	}
}


//  BLOCKS : gravity, turning, landing
void moveBlocks()
{
	for (int i = 0; i < blockCount; i++)
	{
		if (!blocks[i].alive) continue;

		if (blocks[i].hurtCool > 0) blocks[i].hurtCool--;

		// NO SLEEPING. Every living block is integrated every step, so
		// gravity can never be skipped for a block whose support has
		// just been destroyed. Stacks are held still by the contact
		// solver and friction instead of by a sleep flag.

		blocks[i].vy = blocks[i].vy - GRAVITY;      // everything falls

		// speed cap: nothing may move far enough in one step to skip a block
		if (blocks[i].vy < -CFG.maxSpeed) blocks[i].vy = -CFG.maxSpeed;
		if (blocks[i].vy >  CFG.maxSpeed) blocks[i].vy =  CFG.maxSpeed;
		if (blocks[i].vx < -CFG.maxSpeed) blocks[i].vx = -CFG.maxSpeed;
		if (blocks[i].vx >  CFG.maxSpeed) blocks[i].vx =  CFG.maxSpeed;

		blocks[i].x = blocks[i].x + blocks[i].vx;
		blocks[i].y = blocks[i].y + blocks[i].vy;

		blocks[i].angle = blocks[i].angle + blocks[i].spin;
		blocks[i].spin = blocks[i].spin * CFG.angDamp;
		blocks[i].vx = blocks[i].vx * CFG.linDamp;

		// Remember how fast it is really moving THIS frame. The ground
		// (and other blocks) may stop it a few lines below, and without
		// this the crushing speed would already be 0 by the time the
		// pig is checked - so a falling block would never hurt anything.
		blocks[i].impactVX = blocks[i].vx;
		blocks[i].impactVY = blocks[i].vy;

		// keep the angle in a sensible range
		if (blocks[i].angle > 360) blocks[i].angle -= 360;
		if (blocks[i].angle < -360) blocks[i].angle += 360;

		// landing on the grass. The TURNED box is used, so a plank that
		// has tipped over rests on its long side, not on its old corner.
		if (blockBottom(i) < GROUND_Y || blocks[i].y <= GROUND_Y + 2.0)
		{
			if (blockBottom(i) < GROUND_Y)
				blocks[i].y = blocks[i].y + (GROUND_Y - blockBottom(i));

			// damage scales with the FORCE of the landing: mass * speed
			// relative impact speed * mass, straight into the same
			// damage pipeline the bird uses
			double hitForce = materialMass(blocks[i].material) * (-blocks[i].vy);
			if (hitForce > CFG.breakForce)
				damageBlock(i, (int)(hitForce / CFG.breakForce));

			// bounce and friction come from the material
			if (blocks[i].vy < 0)
				blocks[i].vy = -blocks[i].vy * materialBounce(blocks[i].material);
			if (blocks[i].vy < 1.0 && blocks[i].vy > 0) blocks[i].vy = 0;
			blocks[i].vx = blocks[i].vx * materialFriction(blocks[i].material);

			// A block lying at an angle on the ground is not stable, so
			// gravity keeps turning it until it lies flat. We pull the
			// angle towards the nearest quarter turn (0, 90, 180...),
			// which is how a real plank settles on its side.
			double flat = blocks[i].angle;
			double turns = flat / 90.0;
			if (turns >= 0) turns = (double)((int)(turns + 0.5));
			else            turns = (double)((int)(turns - 0.5));
			double target = turns * 90.0;

			blocks[i].spin = blocks[i].spin + (target - blocks[i].angle) * FLAT_PULL;
			blocks[i].spin = blocks[i].spin * 0.86;

			if (blocks[i].spin < 0.05 && blocks[i].spin > -0.05)
				blocks[i].angle = blocks[i].angle
				                + (target - blocks[i].angle) * 0.25;

		}

		// count the smashed piece down and then remove it
		if (blocks[i].broken)
		{
			blocks[i].breakTimer--;
			if (blocks[i].breakTimer <= 0) blocks[i].alive = 0;
		}

		// the side walls
		if (blockLeft(i) < 0)
		{
			blocks[i].x = blocks[i].x - blockLeft(i);
			blocks[i].vx = 0;
		}
		if (blockLeft(i) + blockW(i) > SCREEN_WIDTH)
		{ blocks[i].x = blocks[i].x - (blockLeft(i) + blockW(i) - SCREEN_WIDTH);
		  blocks[i].vx = 0; }

		// SLEEPING: a block that has been almost still for a while is put
		// to sleep, which is what stops a finished stack jittering for ever
		// ABS: how fast the block is moving, ignoring the direction
		double lin = blocks[i].vx;
		if (lin < 0) lin = -lin;

		double dwn = blocks[i].vy;
		if (dwn < 0) dwn = -dwn;

		double ang = blocks[i].spin;
		if (ang < 0) ang = -ang;

		// a block that is barely moving AND really supported has its
		// last wobble removed, so a finished stack is still
		if (lin < CFG.sleepLinear && dwn < CFG.sleepLinear
		    && ang < CFG.sleepAngular && blockIsSupported(i))
		{
			blocks[i].vx = 0; blocks[i].spin = 0;
			if (blocks[i].vy > -CFG.sleepLinear) blocks[i].vy = 0;
		}
	}
}


//  Stops blocks from sinking into each other, so a tower can
//  stand up and then collapse when a leg is knocked out.
void separateBlocks()
{
	for (int pass = 0; pass < 2; pass++)
	{
		int firstPass;
		if (pass == 0) firstPass = 1;
		else            firstPass = 0;
		for (int i = 0; i < blockCount; i++)
		{
			if (!blocks[i].alive || blocks[i].broken) continue;

			for (int j = i + 1; j < blockCount; j++)
			{
				if (!blocks[j].alive) continue;
				// a smashed piece is pushed aside by a standing block,
				// but it can never hold that block up
				if (blocks[j].broken)
				{
					double bx = overlapX(blockLeft(i), blockW(i), blockLeft(j), blockW(j));
					double by = overlapY(blockBottom(i), blockH(i), blockBottom(j), blockH(j));
					if (bx > 0 && by > 0)
					{
						if (by < bx)
						{
							if (blocks[j].y > blocks[i].y) blocks[j].y = blocks[j].y + by;
							else                            blocks[j].y = blocks[j].y - by;
							blocks[j].vy = blocks[j].vy * 0.4;
						}
						else
						{
							if (blocks[j].x > blocks[i].x) blocks[j].x = blocks[j].x + bx;
							else                            blocks[j].x = blocks[j].x - bx;
							blocks[j].vx = blocks[j].vx * 0.4;
						}
					}
					continue;
				}

				double ox = overlapX(blockLeft(i), blockW(i), blockLeft(j), blockW(j));
				double oy = overlapY(blockBottom(i), blockH(i), blockBottom(j), blockH(j));
				if (ox <= 0 || oy <= 0) continue;

				if (oy < ox)
				{
					// one is standing on the other: lift the upper one up
					int up = i, down = j;
					if (blockCY(j) > blockCY(i))
					{
						up = j;
						down = i;
					}

					// a heavy block dropping on another one damages it
					// Fix only a SLICE of the overlap, and ignore a tiny
					// one. Shoving the whole penetration out in one step
					// is what threw the stack into the air.
					double fix = oy - CFG.correctSlop;
					if (fix < 0) fix = 0;
					blocks[up].y = blocks[up].y + fix * CFG.correctPercent;

					// IMPULSE: the struck block reacts in proportion to the
					// mass and speed coming into it, instead of the faller
					// simply stopping dead on top of it.
					double mu = materialMass(blocks[up].material);
					double md = materialMass(blocks[down].material);
					double rel = blocks[up].vy - blocks[down].vy;

					// Only a REAL landing gets an impulse. A block that is
					// simply sitting there still has a tiny downward speed
					// from gravity every step, and treating that as an
					// impact shook every tower to pieces.
					if (rel < -CFG.impactMin && firstPass)
					{
						double e = materialBounce(blocks[up].material);
						if (materialBounce(blocks[down].material) < e)
							e = materialBounce(blocks[down].material);

						// the standard "impulse" formula: how hard the two
						// blocks must be pushed apart so they bounce apart
						// realistically instead of just stopping dead
						double pushAmount = -(1.0 + e) * rel;
						double massFactor  = 1.0 / mu + 1.0 / md;
						double j = pushAmount / massFactor;

						blocks[up].vy   = blocks[up].vy   + j / mu;
						blocks[down].vy = blocks[down].vy - j / md;

						// TORQUE: the load lands off to one side, so the
						// block underneath is turned, not just pushed down
						double lever = blockCX(up) - blockCX(down);
						blocks[down].spin = blocks[down].spin
						                  - lever * j * CFG.torqueScale / md;
						blocks[up].spin   = blocks[up].spin
						                  + lever * j * CFG.torqueScale * 0.5 / mu;

						// and the landing damages the struck block by force
						double landForce = mu * (-rel);
						if (landForce > CFG.breakForce)
							damageBlock(down, (int)(landForce / CFG.breakForce));
					}
					else
					{
						// Resting contact: stop them sinking into each
						// other. The downward speed is only cancelled if
						// the block REALLY has something under it - the
						// padded box of a tilted neighbour used to hold
						// blocks up in mid air.
						if (blocks[up].vy < 0 && blockIsSupported(up))
							blocks[up].vy = 0;
						if (blocks[down].vy > 0) blocks[down].vy = 0;
						blocks[up].spin = blocks[up].spin * 0.8;
					}

					// FORCE TRANSFER. This only happens when the upper
					// block is actually MOVING. A stack that is just
					// standing there must be left completely alone,
					// otherwise the towers slowly shake themselves down.
					double moving = blocks[up].vx;
					if (moving < 0) moving = -moving;
					double dropping = blocks[up].vy;
					if (dropping < 0) dropping = -dropping;

					if (moving > 1.0 || dropping > 1.0)
					{
						double share = materialMass(blocks[up].material)
						             / (materialMass(blocks[down].material) * 4.0);
						if (share > 0.5) share = 0.5;
						blocks[down].vx = blocks[down].vx + blocks[up].vx * share;
						blocks[up].vx   = blocks[up].vx * (1.0 - share);

						// a block landing off to one side leans the one
						// below it over
						double lean = blockCX(up) - blockCX(down);
						blocks[down].spin = blocks[down].spin - lean * 0.0006;
					}
				}
				else
				{
					// side by side: push them apart
					double fixX = ox - CFG.correctSlop;
					if (fixX < 0) fixX = 0;
					double half = fixX * CFG.correctPercent / 2.0;
					double keepI = blocks[i].vx, keepJ = blocks[j].vx;
					if (blockCX(i) < blockCX(j))
					{
						blocks[i].x = blocks[i].x - half;
						blocks[j].x = blocks[j].x + half;
					}
					else
					{
						blocks[i].x = blocks[i].x + half;
						blocks[j].x = blocks[j].x - half;
					}

					// a shove passes sideways through the stack, but only
					// if something was really moving
					if (keepI > 1.0 || keepI < -1.0 || keepJ > 1.0 || keepJ < -1.0)
					{
						blocks[i].vx = keepI * 0.4 + keepJ * 0.4;
						blocks[j].vx = keepJ * 0.4 + keepI * 0.4;
					}
					else
					{
						blocks[i].vx = keepI * 0.5;
						blocks[j].vx = keepJ * 0.5;
					}
				}
			}
		}
	}
}


//  Pigs standing on blocks, and blocks falling onto pigs.
void pigsAndBlocks()
{
	for (int i = 0; i < pigCount; i++)
	{
		if (!pigs[i].alive) continue;

		for (int b = 0; b < blockCount; b++)
		{
			if (!blocks[b].alive) continue;      // a falling piece still counts

			double ps = pigs[i].size;          // full size: the pig must
			double px = pigs[i].x;             // stand ON the block, not
			double py = pigs[i].y;             // sink into it

			double ox = overlapX(px, ps, blockLeft(b), blockW(b));
			double oy = overlapY(py, ps, blockBottom(b), blockH(b));
			if (ox <= 0 || oy <= 0) continue;

			// HOW HARD did the block and the pig come together?
	
			double rvx = blocks[b].impactVX - pigs[i].vx;
			double rvy = blocks[b].impactVY - pigs[i].vy;
			double hitSpeed = sqrt(rvx * rvx + rvy * rvy);

			knockPig(i, hitSpeed, materialMass(blocks[b].material));

			if (!pigs[i].alive) break;

			if (blocks[b].broken) continue;       // debris cannot hold a pig up

			if (oy < ox)
			{
				if (py + ps / 2.0 > blockCY(b))
				{
					pigs[i].y = pigs[i].y + oy;       // the pig stands on the block
					if (pigs[i].vy < 0 && pigIsSupported(i)) pigs[i].vy = 0;
				}
				else
				{
					// the BLOCK is on top, so the pig holds it up. Without
					// this the block would keep sinking through the pig,
					// getting faster and faster, and would kill even a pig
					// it was only resting on.
					blocks[b].y = blocks[b].y + oy;
					if (blocks[b].vy < 0) blocks[b].vy = 0;
					blocks[b].spin = blocks[b].spin * 0.8;
				}
			}
			else
			{
				if (px + ps / 2.0 < blockCX(b))
					pigs[i].x = pigs[i].x - ox;       // pushed sideways
				else
					pigs[i].x = pigs[i].x + ox;
				pigs[i].vx = pigs[i].vx * 0.5;
			}
		}
	}
}



void movePigs()
{
	for (int i = 0; i < pigCount; i++)
	{
		if (!pigs[i].alive) continue;

		if (pigs[i].hurtCool > 0) pigs[i].hurtCool--;
		if (pigs[i].dying) continue;          // it is popping, leave it alone

		pigs[i].vy = pigs[i].vy - GRAVITY;

		// A pig thrown by a blast can move further in one step than a
		// plank is thick and used to shoot straight through it. The
		// move is cut into small pieces, pushing the pig out of the
		// blocks after each piece.
		double fast = sqrt(pigs[i].vx * pigs[i].vx + pigs[i].vy * pigs[i].vy);
		int pigSteps = 1 + (int)(fast / 4.0);
		if (pigSteps > 20) pigSteps = 20;

		for (int st = 0; st < pigSteps; st++)
		{
			pigs[i].x = pigs[i].x + pigs[i].vx / pigSteps;
			pigs[i].y = pigs[i].y + pigs[i].vy / pigSteps;
			pigsAndBlocks();
		}
		pigs[i].vx = pigs[i].vx * 0.98;

		// landing on the grass
		if (pigs[i].y < GROUND_Y)
		{
			pigs[i].y = GROUND_Y;

			knockPig(i, -pigs[i].vy, 1.0);        // a long fall hurts too

			if (pigs[i].vy < 0) pigs[i].vy = 0;
			pigs[i].vx = pigs[i].vx * FRICTION;
		}

		if (pigs[i].x < 0)
		{
			pigs[i].x = 0;
			pigs[i].vx = 0;
		}
		if (pigs[i].x + pigs[i].size > SCREEN_WIDTH)
		{
			pigs[i].x = SCREEN_WIDTH - pigs[i].size;
			pigs[i].vx = 0;
		}
	}
}


void keepAboveGround()
{
	for (int i = 0; i < blockCount; i++)
	{
		if (!blocks[i].alive) continue;
		if (blocks[i].y < GROUND_Y)
		{
			blocks[i].y = GROUND_Y;
			if (blocks[i].vy < 0) blocks[i].vy = 0;
		}
	}
	for (int i = 0; i < pigCount; i++)
	{
		if (!pigs[i].alive) continue;
		if (pigs[i].y < GROUND_Y)
		{
			pigs[i].y = GROUND_Y;
			if (pigs[i].vy < 0) pigs[i].vy = 0;
		}
	}
}

// ------------------------------------------------------------
//  Moves the egg dropped by the white bird.
// ------------------------------------------------------------
void moveEgg()
{
	if (!egg.alive) return;

	egg.vy = egg.vy - GRAVITY;
	egg.y = egg.y + egg.vy;

	if (egg.y < GROUND_Y)
	{
		blastAt(egg.x, egg.y, 126, 3);
		egg.alive = 0;
		return;
	}

	for (int b = 0; b < blockCount; b++)
	{
		if (!blocks[b].alive) continue;
		if (boxesTouch(egg.x, egg.y, EGG_SIZE, EGG_SIZE,
		               blocks[b].x, blocks[b].y, blocks[b].w, blocks[b].h))
		{
			blastAt(egg.x, egg.y, 126, 3);
			egg.alive = 0;
			return;
		}
	}

	for (int p = 0; p < pigCount; p++)
	{
		if (!pigs[p].alive) continue;
		if (boxesTouch(egg.x, egg.y, EGG_SIZE, EGG_SIZE,
		               pigs[p].x, pigs[p].y, pigs[p].size, pigs[p].size))
		{
			blastAt(egg.x, egg.y, 126, 3);
			egg.alive = 0;
			return;
		}
	}
}


void updateDyingPigs()
{
	for (int i = 0; i < pigCount; i++)
	{
		if (!pigs[i].alive || !pigs[i].dying) continue;

		pigs[i].dieTimer--;
		if (pigs[i].dieTimer <= 0) pigs[i].alive = 0;
	}
}


//  THE GAME LOOP. A timer calls this again and again.

void stepWorld()
{
	for (int i = 0; i < birdCount; i++) moveBird(i);

	moveBlocks();
	separateBlocks();
	movePigs();
	pigsAndBlocks();
	keepAboveGround();
	moveEgg();
}

void updateGame()
{
	updateDyingPigs();          // always, whatever page we are on

	// ---- the short "Level Complete" message ----

	if (page == PAGE_LEVEL_DONE)
	{
		levelDoneTimer--;
		if (levelDoneTimer <= 0)
		{
			level++;
			loadLevel(level);
			page = PAGE_GAME;
		}
		return;
	}

	if (page != PAGE_GAME) return;


	unsigned long now = (unsigned long)clock();
	double dt = CFG.fixedStep;

	if (physLastTick != 0)
	{
		double real = (double)(now - physLastTick) / (double)CLOCKS_PER_SEC;
		if (real > CFG.fixedStep * 3.0 && real < CFG.maxFrame)
			dt = real;                       // we fell behind, catch up
	}
	physLastTick = now;

	physAccum = physAccum + dt;

	int guard = 0;
	while (physAccum >= CFG.fixedStep && guard < 2)
	{
		stepWorld();
		physAccum = physAccum - CFG.fixedStep;
		guard++;
	}
	if (physAccum > CFG.fixedStep) physAccum = 0;   // never build up a backlog

	// ---- WIN: no pigs left ----
	if (allPigsDead() && winTimer < 0 && page == PAGE_GAME)
		winTimer = (int)(CFG.levelDoneDelay / CFG.fixedStep);   // fires once

	if (winTimer > 0) winTimer--;      // the world keeps running meanwhile

	if (allPigsDead() && winTimer == 0)
	{
		winTimer = -1;
		// BONUS: every bird you did not need is worth a lot. This is
		// what makes finishing a level with few birds score highest.
		levelBonus = birdsLeft * SCORE_PER_UNUSED_BIRD;
		score += levelBonus;
		birdsLeft = 0;                           // so it cannot be added twice

		if (level < TOTAL_LEVELS)
		{
			page = PAGE_LEVEL_DONE;              // stop the game and show it
			levelDoneTimer = LEVEL_DONE_FRAMES;  // about 2 seconds
		}
		else
		{
			// the whole game is finished - save the Highest/Last/Total score
			recordGameScore();
			page = PAGE_WIN;
		}
		return;
	}

	// ---- is the shot over? ----
	int stillFlying = 0;
	for (int i = 0; i < birdCount; i++)
		if (birds[i].state == FLYING) stillFlying = 1;

	if (stillFlying || egg.alive) return;

	// RELOAD: the run is over, so start one single timer. We do NOT
	// wait for the debris to settle any more.
	if (birds[0].state == DONE)
	{
		if (reloadTimer < 0)
			reloadTimer = (int)(CFG.reloadDelay / CFG.fixedStep);

		if (reloadTimer > 0)
		{
			reloadTimer--;
			return;              // still waiting - come back next frame
		}

		reloadTimer = -1;

		// PIGS ARE CHECKED FIRST. If the last bird killed the last pig
		// it is a WIN, and that win is already on its way in, so it
		// must not be turned into a loss just because no birds remain.
		if (allPigsDead() || winTimer >= 0)
			return;

		if (birdsLeft > 0) advanceQueue();            // the line steps forward
		else
		{
			// the game ended in a loss - still save the score reached so far
			recordGameScore();
			page = PAGE_LOSE;
		}
	}
}

#endif
