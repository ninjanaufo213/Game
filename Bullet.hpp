#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"
#include <SFML/Audio.hpp>

// Bullet class inherits from Entity class, extends function of Entity
// represent a bullet object, handling movement and interactions with solid objects
// use an AnimationManager for animations and updates the bullet's state over time
class Bullet : public Entity
{
private:
    // Sound-related members
    sf::SoundBuffer buffer; // Holds audio data for the explosion sound
    sf::Sound sound;        // Creates a Sound object to play the sound

    // Flag to ensure the sound is played only once upon collision
    bool soundPlayed = false;

public:
    // Level &lev: reference to game level, contains objects like walls, bullet can collide with
    // x, y: position
    // bool dir: direction of the bullet
    // Entity(a, x, y) initializes base class Entity with animation manager and position
    Bullet(AnimationManager &a, Level &lev, int x, int y, bool dir) : Entity(a, x, y)
    {
        // "Bullet" name of entity
        // 0.3 speed
        // 10 damage
        // "move" current animation status
        option("Bullet", 0.3, 10, "move");

        // Direction handling for the bullet
        // dir == true, bullet moves to left, horizontal velocity dx = -0.3
        // dir == false, bullet moves to right
        if (dir)
            dx = -0.3;

        // Retrieves a list of "solid" objects from the game level that the bullet may collide with
        obj = lev.GetObjects("solid");

        // Load sound file into buffer (explosion sound)
        if (!buffer.loadFromFile("D:/test3/files/ban.wav"))
        {
            // Handle error if the sound file cannot be loaded
            std::cerr << "Error loading sound file!" << std::endl;
        }
        sound.setBuffer(buffer); // Associate the loaded sound buffer with the sound object
        sound.setVolume(70);    // Set volume level (adjust as needed)
    }

    // Update bullet's position and state over time
    void update(float time)
    {
        // Moves the bullet
        x += dx * time;

        // Check collisions between the bullet and solid objects
        // getRect() gets the rectangle of the bullet's bounding box, obj[i].rect is the bounding box of a solid object
        // Bullet -> solid object -> Health = 0 -> bullet destruction
        for (int i = 0; i < obj.size(); i++)
            if (getRect().intersects(obj[i].rect))
            {
                Health = 0;
            }

        // If Health <= 0 (bullet hit something), play explosion sound and animation
        if (Health <= 0)
        {
            anim.set("explode"); // Switch animation to "explode"
            dx = 0;              // Stop bullet's movement

            // Play explosion sound only once
            if (!soundPlayed)
            {
                sound.play();    // Play the sound once
                soundPlayed = true; // Ensure the sound is not played again
            }

            // If the explosion animation has finished, remove the bullet from the game
            if (!anim.isPlaying())
                life = false; // Bullet will be destroyed (removed)
        }

        // Update the animation frame based on elapsed time (time)
        anim.tick(time);
    }
};

#endif // BULLET_H

