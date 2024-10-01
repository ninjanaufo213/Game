#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "level.hpp"
#include "Animation.hpp"
#include "Entity.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "MovingPlatform.hpp"
#include "HealthBar.hpp"
#include <SFML/Audio.hpp>

using namespace sf;

// Dom dom Hoa hai duong
void RunGame()
{	
	SoundBuffer buffer; // hold audio data
	buffer.loadFromFile("D:/test3/files/nhachay.wav"); // load sound file into buffer
	Sound sound; // create Sound object use buffer to play sound
	sound.setBuffer(buffer); // associate loaded sound buffer with sound object
	sound.setLoop(true); // run to die 
	sound.setVolume(65); // medium rare
	sound.play(); // play --> bat nhac hay and chillll
	
	// Create game window
	RenderWindow window(VideoMode(450, 280), "Game do hoa mot nguoi");

	View view( FloatRect(0, 0, 450, 280) ); // set up camera view

	Level lvl; // Create level object
	lvl.LoadFromFile("files/Level1.tmx"); // Load level from file
	
	// Load textures (ket cau)
	Texture enemy_t, moveplatform_t, megaman_t, bullet_t, bg;
	bg.loadFromFile("files/images/bg.png");
	enemy_t.loadFromFile("files/images/enemy.png");
	moveplatform_t.loadFromFile("files/images/movingPlatform.png");
	megaman_t.loadFromFile("files/images/megaman.png");
	bullet_t.loadFromFile("files/images/bullet.png");

	// set up animations for player and entities
	AnimationManager anim;
	anim.loadFromXML("files/anim_megaman.xml",megaman_t);
	anim.animList["jump"].loop = 0;

	AnimationManager anim2;
	
	// name animation, texture file bullet, (7, 10) = (x, y), (8, 8) = (w, h), number of frames for "move"(a single frame), aniamtion speed; 
	anim2.create("move",bullet_t,7,10,8,8,1,0);
	
	// 29 offset to skip between frames on texture, false => set animation not loop only play once and stop
	anim2.create("explode",bullet_t,27,7,18,18,4,0.01, 29,false);

	AnimationManager anim3;
	anim3.create("move",enemy_t,0,0,16,16,2,0.002,18); // 18 offset
	anim3.create("dead",enemy_t,58,0,16,16,1,0); // 1 number of frames, 0 animation speed not change frame

	AnimationManager anim4;
	anim4.create("move",moveplatform_t,0,0,95,22,1,0);

    Sprite background(bg);
    background.setOrigin(bg.getSize().x/2,bg.getSize().y/2);
	
	// manage all game entities
	// iterator "it" is declared to traverse the list of entities
	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;
	
	// find all objects of type "enemy" from current level and store them in a vector
	std::vector<Object> e = lvl.GetObjects("enemy");
	
	// loop through enemy objects
	for (int i = 0; i < e.size(); i++)
		// create new ENEMY instance with its animation, level context (boi canh), start position
		// add newly created ENEMY to entities list for management
		entities.push_back(new ENEMY(anim3, lvl, e[i].rect.left, e[i].rect.top) );

	e = lvl.GetObjects("MovingPlatform");
	for (int i=0;i < e.size();i++)
		entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top) );

	Object pl = lvl.GetObject("player");
	PLAYER Mario(anim, lvl, pl.rect.left, pl.rect.top);
	
	// created to manage player's health display
	HealthBar healthBar;
	
	// manage time in game
	Clock clock;

	// keep running while game window is open
	while (window.isOpen())
	{
		// gets time elapsed since the last frame in microseconds
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart(); // clock is restarted to track time for next frame

		time = time / 600;  // time scale down, get an appropriate unit for animation and movement speed

		if (time > 40) time = 40; // avoid excessive time between frames (maintain smooth frame rate)

		Event event;
		// loop checks for any pending events (like key presses or closing window)
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			
			// press Space, new bullet object is created and added to the entities list
			// position at Mario.x + 18 and Mario.y + 18
			// direction == Mario direction
			if (event.type == Event::KeyPressed)
				if (event.key.code==Keyboard::Space)
		        	entities.push_back(new Bullet(anim2,lvl,Mario.x+18,Mario.y+18,Mario.dir) );
		}


		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) Mario.key["L"] = true;
		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) Mario.key["R"] = true;
		if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) Mario.key["Up"] = true;
		if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) Mario.key["Down"] = true;
		if (Keyboard::isKeyPressed(Keyboard::Space)) Mario.key["Space"] = true;
		


		for(it=entities.begin();it!=entities.end();)
		{
			Entity *b = *it;
			b->update(time); // update current entity (position, state,...)
			
			// if entity is dead (life == false), erase form list and free memory
			if (b->life==false)	{ it  = entities.erase(it); delete b;}
			else it++; // still alive, move to next one
		}


		Mario.update(time); // update Mario's state based on time(position, animation)
		healthBar.update(Mario.Health); // update health bar based on Mario's current health

		// enemy collision handing
		for(it=entities.begin();it!=entities.end();it++)
		{
			//1. Enemy collision
			if ((*it)->Name=="Enemy")
			{
				Entity *enemy = *it;

				if (enemy->Health<=0) continue; // skip enemies that have no health

				if  (Mario.getRect().intersects( enemy->getRect() )) { // if Mario collides with enemy
				    if (Mario.dy>0) { // Mario is falling onto the enemy
						enemy->dx=0; // stop enemy movement
						Mario.dy=-0.2; // make Mario jump slightly after defeating the enemy
						enemy->Health=0; // enemy is defeated
					}
				    else if (!Mario.hit) { // Mario is hit by enemy
						Mario.Health-=5; // lose health
						Mario.hit=true; // set a flag to indicate Mario was hit
						// Push Mario slightly to the left or right depending on his direction
				        if (Mario.dir) Mario.x+=10; 
						else Mario.x-=10;
					}
				}
				
				// check for bullet collision with enemy
				for (std::list<Entity*>::iterator it2=entities.begin(); it2!=entities.end(); it2++)
				{
					Entity *bullet = *it2;
					// if it's a bullet and still avtive
					if (bullet->Name == "Bullet")
						if ( bullet->Health > 0)
							if  (bullet->getRect().intersects( enemy->getRect() ) )
								// bullet is destroyed
								// enemy takes damage
							 { bullet->Health = 0; enemy->Health -= 5;}
				}
			}

			//2. Moving Platform collision
			if ((*it)->Name=="MovingPlatform")
			{
			    Entity *movPlat = *it;
				if  (Mario.getRect().intersects( movPlat->getRect() ) )
				   if (Mario.dy > 0) // if Mario is falling
					  if (Mario.y + Mario.h < movPlat->y + movPlat->h) // Mario is on top of platform
					  {
					  	  Mario.y = movPlat->y - Mario.h + 12; // position Mario on top of platform 
						  Mario.x += movPlat->dx * time; // move Mario horizontally with flatform
						  Mario.dy = 0; // stop Mario's downward movement
						  Mario.STATE=PLAYER::stay; // set Mario's state to "stay"
					  }
			}

			
		}


		// rendering game scene, including background, player(Mario), entities(enemies, platforms) and health bar
		view.setCenter( Mario.x,Mario.y); // set view's center to Mario's current position
		window.setView(view); // apply updated view to window
		
		// background is drawn behind all other objects
		background.setPosition(view.getCenter()); // position background relative to camera's center, creating illusion that background moves with camera
		window.draw(background); // draw background on the screen
		
		// lvl object contains the map and environment layout
		lvl.Draw(window); // draw level's static components (terrain, obstacles)
		
		// iterates over all the enities in the entities list and draw each one
		for(it=entities.begin();it!=entities.end();it++)
			(*it)->draw(window); // draw each enity(enemies, moving platforms)

		Mario.draw(window); // draw Mario
		healthBar.draw(window);
		
		// final step that makes all rendered elements appear on the screen
		window.display(); // display rendered frame on the screen
	}


}
// end game het phim Toan xai tech Toan nhech
#endif GAME_H

