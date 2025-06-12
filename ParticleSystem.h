#pragma once
#include "Header.h"
#include "Vector.h"

class ParticleSystem {
private:
    Vector<Particle> particles;
    Vector<Animation> animations;

public:
    void addExplosion(Vector2 pos, Color color);
    void addSplash(Vector2 pos);
    void addAnimation(Vector2 pos, const string& text, Color color, float duration = 2.0f);
    void update(float deltaTime);
    void draw();
};