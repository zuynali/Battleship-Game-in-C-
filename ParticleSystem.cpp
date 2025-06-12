#include "ParticleSystem.h"
#include <stdlib.h> // For rand()

void ParticleSystem::addExplosion(Vector2 pos, Color color) {
    for (int i = 0; i < 15; i++) {
        Particle p;
        p.position = pos;
        p.velocity = {
            (float)(rand() % 200 - 100),
            (float)(rand() % 200 - 100)
        };
        p.color = color;
        p.life = 1.0f;
        p.maxLife = 1.0f;
        p.size = (float)(rand() % 8 + 2);
        particles.push_back(p);
    }
}

void ParticleSystem::addSplash(Vector2 pos) {
    for (int i = 0; i < 8; i++) {
        Particle p;
        p.position = pos;
        p.velocity = {
            (float)(rand() % 100 - 50),
            (float)(rand() % 80 - 20)
        };
        p.color = SKYBLUE;
        p.life = 0.8f;
        p.maxLife = 0.8f;
        p.size = (float)(rand() % 6 + 2);
        particles.push_back(p);
    }
}

void ParticleSystem::addAnimation(Vector2 pos, const string& text, Color color, float duration) {
    Animation anim;
    anim.position = pos;
    anim.text = text; // Uses String's copy constructor
    anim.color = color;
    anim.time = 0;
    anim.duration = duration;
    animations.push_back(anim);
}

void ParticleSystem::update(float deltaTime) {
    // Update particles
    for (int i = 0; i < particles.size();) {
        particles[i].position.x += particles[i].velocity.x * deltaTime;
        particles[i].position.y += particles[i].velocity.y * deltaTime;
        particles[i].velocity.y += 200 * deltaTime; // Gravity
        particles[i].life -= deltaTime;

        if (particles[i].life <= 0) {
            particles.delete_at(i);
        }
        else {
            i++;
        }
    }

    // Update animations
    for (int i = 0; i < animations.size();) {
        animations[i].time += deltaTime;
        if (animations[i].time >= animations[i].duration) {
            animations.delete_at(i);
        }
        else {
            i++;
        }
    }
}

void ParticleSystem::draw() {
    // Draw particles
    for (int i = 0; i < particles.size(); i++) {
        Particle& p = particles[i];
        float alpha = p.life / p.maxLife;
        Color color = p.color;
        color.a = (unsigned char)(255 * alpha);
        DrawCircleV(p.position, p.size, color);
    }

    // Draw animations
    for (int i = 0; i < animations.size(); i++) {
        Animation& anim = animations[i];
        float alpha = 1.0f - (anim.time / anim.duration);
        Color color = anim.color;
        color.a = (unsigned char)(255 * alpha);
        float y_offset = -anim.time * 30; // Float upward

        // Use c_str() for compatibility with Raylib's DrawText
        DrawText(anim.text.c_str(),
            (int)anim.position.x,
            (int)(anim.position.y + y_offset),
            20, color);
    }
}