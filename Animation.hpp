#ifndef ANIMATION_H
#define ANIMATION_H
//use SFML library for render and TinyXML for read animation XML files
#include "TinyXML/tinyxml.h"
#include <SFML/Graphics.hpp>


using namespace sf;

//dom dom
//handle the frames of an animation and update, both regular and flipped frame
class Animation
{
public:
	
	//vectors holding rectangles defining sprite sub-areas
	//frames_flip for mirrored or flipped animations
	std::vector<IntRect> frames, frames_flip;
	//currentFrame keeps track current frame
	//speed control speed animation
	//loop animation repeat when it reaches the end
	//flip indicates animation mirrored horizontally
	//isPlaying animation is currently active
	float currentFrame, speed;
	bool loop, flip, isPlaying;   
	Sprite sprite; //SFML Sprite object represent the animated image
	
	//constructor: initializes default values
	Animation()
	{
		currentFrame = 0;
		isPlaying=true;
		flip=false;
		loop=true;
	}
	
	//updates current frame based on elapsed time (time) and speed animation
	void tick(float time)
	{
		//not play -> stop
		if (!isPlaying) return;

		currentFrame += speed * time;
		
		//currentFrame exceeds total frames (loop is true), currentFrame minus frames.size();
		if (currentFrame > frames.size()) 
		{ 
			currentFrame -= frames.size();
			
			//loop is false -> stop animation
			if (!loop) 
			{
				isPlaying=false; return;
			}
		}
		
		//sprite is updated with appropriate texture rectangle (frame) based on flip is true or false
		int i = currentFrame;
		sprite.setTextureRect( frames[i] );
		if (flip) sprite.setTextureRect( frames_flip[i] );
	}

};

//thien ly oi
//class manages multiple animations and handles switching between them
//loading animations form XML, controlling playback
class AnimationManager
{

public:
	
	//currentAnim name current animation being played
	//animList: map stores animations
    std::string currentAnim;
	std::map<std::string, Animation> animList;
	
	//constructor is empty
	AnimationManager()
	{}
	
	//destructor clears animList to free up memory
	~AnimationManager()
	{ 
		animList.clear();
	}

	//creates a new animationm, take name, texture, parameters like position and size each frame in texture, the number of frames, speed, whether it should loop
	//poplates the frames and frames_flip vectors with IntRect ojbects
	void create(std::string name, Texture &texture, int x, int y, int w, int h, int count, float speed, int step=0, bool Loop=true)
	{
		//creates an instance Animation a, hold data new animation
		Animation a;
		a.speed = speed;
		a.loop = Loop;
		a.sprite.setTexture(texture); //assigns a texture (an image) to sprite in the animation, texture contains all the frames
		
		//set origin sprite to (0, h)
		//0: x-origin left edge sprite
		//h: y-origin bottom sprite
		a.sprite.setOrigin(0,h);
		
		//generating texture rectangles (InRect) for each frame and adding to animation
		for (int i=0;i<count;i++)
		{
			//IntRect(x + i * step, y, w, h) creates texture for each frame,define texture corresponds current frame
			// x + i * step: x-coordinate top-left corner frame
			// y: y-coordinate top-left corner
			// w and h: width and height frame, determine the size of each frame
			a.frames.push_back( IntRect(x+i*step, y, w, h)  );
			a.frames_flip.push_back( IntRect(x+i*step+w, y, -w, h)  );
		}
		
		
		animList[name] = a;
		currentAnim = name; //set name newly created animation, make it -> currently active in animation manager
	}

	//loads aniamtion data from XML file using TinyXML library
	//file analysis, take animation name(title), delay(speed), frame definitions(cut)
	//add these frames to the frames and frames_flip vectors
	//aniamation data is stored in the animList map
	void loadFromXML(std::string fileName,Texture &t)
	{
		//working with TinyXML: c_str() converts C++ string to a C-style string
		TiXmlDocument animFile(fileName.c_str());
		
		//load the XML data from the file into the animFile object
		animFile.LoadFile();
		
		//root element is stored in the pointer head
		//all animation data inside this <sprites> tag
		TiXmlElement *head;
		head = animFile.FirstChildElement("sprites");
		
		//FirstChildElement("animation") finds first within <sprites> that is an <animation> tag
		//this tag contains the data for a single animation
		TiXmlElement *animElement;
		animElement = head->FirstChildElement("animation");
		
		//iterates through all the <aniamation> elements inside <sprites> tag
		while(animElement)
		{
			Animation anim;
			//title attribute from <animation> tag is retrieved and stored in the currentAnim string
			currentAnim = animElement->Attribute("title");
			int delay = atoi(animElement->Attribute("delay"));
			anim.speed = 1.0/delay; anim.sprite.setTexture(t);
			
			//<cut> describes frame dimensions and position
			TiXmlElement *cut;
			cut = animElement->FirstChildElement("cut");
			while (cut)
			{
				int x = atoi(cut->Attribute("x"));
				int y = atoi(cut->Attribute("y"));
				int w = atoi(cut->Attribute("w"));
				int h = atoi(cut->Attribute("h"));

				anim.frames.push_back( IntRect(x,y,w,h) );
				anim.frames_flip.push_back( IntRect(x+w,y,-w,h)  );
				
				//moves to the next <cut> element in the XML(if any)
				cut = cut->NextSiblingElement("cut");
			}
			
			//make bottom-left corner sprite -> origin point for transformations
			anim.sprite.setOrigin(0,anim.frames[0].height);
			
			animList[currentAnim] = anim;
			
			//moves to next <animation> element int XML(if any)
			animElement = animElement->NextSiblingElement("animation");
		}
	}

	void set(std::string name)
	{
		currentAnim = name; //set current animation to one specified by name
		animList[currentAnim].flip=0; //reset flip status
	}
	
	//draw current animation's sprite to given window at (x, y)
	//set sprite's position to(x, y), render in SFML RenderWindow
	void draw(RenderWindow &window,int x=0, int y=0)
	{
		animList[currentAnim].sprite.setPosition(x,y); //set sprite position
		window.draw( animList[currentAnim].sprite ); //draw sprite on window
	}

	void flip(bool b=1) {animList[currentAnim].flip = b;} //flip sprite based on boolean flag

	void tick(float time)	 {animList[currentAnim].tick(time);} //update frames based on elaped time

	void pause() {animList[currentAnim].isPlaying=false;} //isPlaying = false -> prevent it updating or playing

	void play()  {animList[currentAnim].isPlaying=true;}

	void play(std::string name)  {animList[name].isPlaying=true;} //playing specified aniamtion

	bool isPlaying()  {return animList[currentAnim].isPlaying;} //check current animation is playing
	
	//return height first frame of current animation
	float getH()  {return animList[currentAnim].frames[0].height;}

	float getW() {return animList[currentAnim].frames[0].width;}

};

#endif ANIMATION_H
