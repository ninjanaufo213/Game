#ifndef ENEMY_H
#define ENEMY_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp> // For displaying text
#include "Entity.hpp"


// ENEMY class inheriting from Entity
class ENEMY : public Entity
{
private:
    // To store the initial position for respawn
    float initial_x, initial_y;

    // Timer to track respawn after death
    float timer_respawn = 0;

    // Flag to indicate if the enemy is dead
    bool isDead = false;

    // Flag to control visibility (for hiding the enemy after death)
    bool isVisible = true;

    // Flag to ensure sound is played only once on death
    bool soundPlayed = false;

    // Sound-related members
    sf::SoundBuffer buffer; // hold audio data
    sf::Sound sound;        // create Sound object

public:
    // Constructor initialize: animation + position
    ENEMY(AnimationManager &a, Level &lev, int x, int y) : Entity(a, x, y)
    {
        // Save initial position for respawning later
        initial_x = x;
        initial_y = y;

        // Name, speed, health, and initial animation
        option("Enemy", 0.01, 15, "move");

        // Load sound file into buffer
        if (!buffer.loadFromFile("D:/test3/files/siu.wav"))
        {
            // Handle error if the file cannot be loaded
            std::cerr << "Error loading sound file!" << std::endl;
        }
        sound.setBuffer(buffer); // associate loaded sound buffer with sound object
        sound.setVolume(500);    // set volume level
    }

    // Update function to control the enemy's behavior
    void update(float time)
    {
        // If the enemy is dead, track time to respawn
        if (isDead)
        {
            isVisible = false; // Make the enemy invisible
            timer_respawn += time; // Track respawn time

            if (timer_respawn > 3000)
            {
                respawn(); // Respawn after 3 seconds
            }

            return; // Skip the rest of the update logic while dead
        }

        // Update position based on its velocity (speed) and time
        x += dx * time;

        // Increment the timer to track movement direction change
        timer += time;

        // Reverse direction every 3200 units of time (3.2s)
        if (timer > 3200)
        {
            dx *= -1; // Reverse direction
            timer = 0;
        }

        // If health is less than or equal to 0, mark the enemy as dead
        if (Health <= 0 && !isDead)
        {
            anim.set("dead"); // Switch animation to "dead"
            dx = 0;           // Stop movement
            isDead = true;     // Mark enemy as dead
            timer_end = 0;     // Reset death timer

            sound.play();
            
        }

        // Track how long the enemy has been "dead"
        if (isDead)
        {
            timer_end += time;

            if (timer_end > 4000)
            {
                isDead = true;      // Mark enemy as permanently dead
                timer_respawn = 0;  // Start the respawn timer
            }
        }

        // Update the animation based on elapsed time
        anim.tick(time);
    }

    // Respawn the enemy with full health at the initial position
    void respawn()
    {
        Health = 15;        // Reset health
        x = initial_x;      // Reset to initial x position
        y = initial_y;      // Reset to initial y position
        anim.set("move");   // Set animation to "move"
        dx = 0.01;          // Reset speed
        isDead = false;     // Bring the enemy back to life
        isVisible = true;   // Make the enemy visible again
        timer_respawn = 0;  // Reset respawn timer
        timer_end = 0;      // Reset death timer
        soundPlayed = false; // Reset the sound played flag for the next death
    }
};

#endif // ENEMY_H

