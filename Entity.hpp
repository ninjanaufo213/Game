#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Animation.hpp"
#include "level.hpp"

// use all identifiers
using namespace sf;

// song gio
class Entity
{
public:
	float x, y, dx, dy, w, h;
	AnimationManager anim;
	std::vector<Object> obj;
	bool life, dir;
	float timer, timer_end;
	std::string Name;
	int Health;
	
	//anim variable with object A
	//x, y position
	//dx, dy: speed to 0
	//dir: direction (false = left, true = right)
	//timer and timer_end: initialized to 0 track action time later
	Entity(AnimationManager &A, int X, int Y)
    : anim(A), x(X), y(Y), dx(0), dy(0), w(0), h(0), 
	dir(false), life(true), timer(0), timer_end(0), Health(0) {}


	virtual void update(float time) = 0;

	void draw(RenderWindow &window)
	{
		// add y to h to calculate drawing position based on height
		anim.draw(window,x,y+h); // draw at adjusted y position
	}
	
	// checking collisions between objects
	// get bounding rectangle
	FloatRect getRect()
	{
		return FloatRect(x,y,w,h);
	}

	void option(std::string NAME, float SPEED=0, int HEALTH=10, std::string FIRST_ANIM="")
	{
		Name = NAME;
		if (FIRST_ANIM!="") anim.set(FIRST_ANIM); 
		w = anim.getW(); // get width from animation
		h = anim.getH();
		dx = SPEED; // movement speed
		Health = HEALTH;
	}

};


#endif ENTITY_H
