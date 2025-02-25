#include "Dice.h"
#include <cmath> // For sine wave

// Constructor: Initialize 3D dice
Dice3D::Dice3D(Vector3 pos) {
    position = pos;
    rotationAxis = { 1, 1, 0 }; // Rotate around diagonal axis
    rotationAngle = 0;
    rolling = false;
    rollDuration = 5.0f;  // 2 seconds rolling time
    timer = 0;
    finalValue = 1;
}

// Start rolling animation
void Dice3D::StartRoll() {
    rolling = true;
    timer = 0;
    rollDuration = 5.0f;
    finalValue = GetRandomValue(1, 6);
}

// Update dice position and rotation
void Dice3D::Update(float delta) {
    if (rolling) {
        timer += delta;

        // Rotate faster at the start, slow down over time
        rotationAngle += 720 * delta * (1.0f - timer / rollDuration);

        // Make the dice bounce using a sine wave
        position.y = 1.0f + sin(timer * 10) * (2.0f - (timer / rollDuration) * 2.0f);

        // Stop after rollDuration
        if (timer >= rollDuration) {
            rolling = false;
            position.y = 1.0f;  // Set final position
            rotationAngle = (finalValue - 1) * 60;  // Align to final face
        }
    }
}

// Draw the 3D dice
void Dice3D::Draw() {
    DrawCube(position, 1.0f,1.0f, 1.0f, BLACK);
    DrawCubeWires(position, 1.0f, 1.0f, 1.0f, BLUE);
}

// Check if dice is rolling
bool Dice3D::IsRolling() const {
    return rolling;
}

// Get the final dice value
int Dice3D::GetValue() const {
    return finalValue;
}
