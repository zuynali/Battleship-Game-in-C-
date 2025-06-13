#pragma once
#include "Header.h"
#include "Vector.h"

#ifndef  PARTICLESYSTEM_H
#define PARICLESYSTEM_H

class ParticleSystem {
private:
    vector<Particle> particles;
    vector<Animation> animations;

public:
    void addExplosion(Vector2 pos, Color color) {
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

    void addSplash(Vector2 pos) {
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

    void addAnimation(Vector2 pos, string text, Color color, float duration = 2.0f) {
        Animation anim;
        anim.position = pos;
        anim.text = text;
        anim.color = color;
        anim.time = 0;
        anim.duration = duration;
        animations.push_back(anim);
    }

    void update(float deltaTime) {
        // Update particles
        for (auto it = particles.begin(); it != particles.end();) {
            it->position.x += it->velocity.x * deltaTime;
            it->position.y += it->velocity.y * deltaTime;
            it->velocity.y += 200 * deltaTime; // Gravity
            it->life -= deltaTime;

            if (it->life <= 0) {
                it = particles.erase(it);
            }
            else {
                ++it;
            }
        }

        // Update animations
        for (auto it = animations.begin(); it != animations.end();) {
            it->time += deltaTime;
            if (it->time >= it->duration) {
                it = animations.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void draw() {
        // Draw particles
        for (const auto& p : particles) {
            float alpha = p.life / p.maxLife;
            Color color = p.color;
            color.a = (unsigned char)(255 * alpha);
            DrawCircleV(p.position, p.size, color);
        }

        // Draw animations
        for (const auto& anim : animations) {
            float alpha = 1.0f - (anim.time / anim.duration);
            Color color = anim.color;
            color.a = (unsigned char)(255 * alpha);
            float y_offset = -anim.time * 30; // Float upward
            DrawText(anim.text.c_str(),
                (int)anim.position.x,
                (int)(anim.position.y + y_offset),
                20, color);
        }
    }
};

#endif // ! PARTICLESYSTUM_H