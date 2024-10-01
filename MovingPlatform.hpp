#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "Entity.hpp"

// inheriting form Entity
class MovingPlatform: public Entity
{
public:
	// constructor initialize MovingPlatform object
	MovingPlatform(AnimationManager &a, Level &lev,int x,int y):Entity(a,x,y)
	{
		// setup parameters: name, speed, no direction (0), "move" animation 
		option("MovingPlatform", 0.05, 0, "move");
	}

	void update(float time)
	{
		// move platform horizontally based on current speed(dx) and elapsed time
		x += dx * time;
		timer += time;
		
		// time > 6000, reverse direction and reset timer
		if (timer > 6000) {
			dx *= -1;
			timer = 0;
		}
		
		// update platform's animation based on elapsed time
		anim.tick(time);
	}

};

#endif MOVINGPLATFORM_H
