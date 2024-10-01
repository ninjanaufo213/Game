#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include <SFML/Graphics.hpp>

// J97 MCK
class HealthBar
{
public:
	Texture t; // texture for health bar background
	Sprite s; // sprite display health bar texture
	int max; // maximum health value
	RectangleShape bar; // rectangle represent filled portion health bar

	HealthBar()
	{
		// load texture use as background health bar
		t.loadFromFile("files/images/HealthBar.png");
		
		// set texture to sprite
		s.setTexture(t);
		
		// set inittial color of bar to black
		bar.setFillColor(Color(0,0,0));
		max = 100; // set maximum health value
	}

	void update(int k)
	{
		if (k > 0 && k < max) 
			// set size rectangle based on current health value
			// height bar decreases as health decrease
			bar.setSize( Vector2f(10, (max - k) * 70 / max) );
	}
	
	// draw health bar and background sprite to provided window
   	void draw(RenderWindow &window)
	{
		// get center and size of window's view
		Vector2f center = window.getView().getCenter();
		Vector2f size = window.getView().getSize();
		
		// set position of sprite slightly offset to left of view's center
		s.setPosition(   center.x - size.x/2 - 10, center.y - size.y/2 + 12);
		
		// set position of rectangle (health bar) nexto to sprite
	    bar.setPosition( center.x - size.x/2 + 15, center.y - size.y/2 + 13);

		window.draw(s); // draw sprite
		window.draw(bar); // draw rectangle
	}

};

#endif HEALTHBAR_H
