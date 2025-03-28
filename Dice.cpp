#include "Dice.h"
#include <cmath> // For sine wave

/// <summary>
/// This is the constructor of the dice
/// </summary>
/// <param name="pos"></param>
Dice3D::Dice3D(Vector3 pos) {
    position = pos;
    rotationAxis = { 1, 1, 0 }; 
    rotationAngle = 0;
    rolling = false;
    rollDuration = 5.0f;  // rolling time
    timer = 0;
    finalValue = 1;
}

/// <summary>
/// This function is to start the roll of the dice and the animation
/// </summary>
void Dice3D::StartRoll() {
    rolling = true;
    timer = 0;
    rollDuration = 5.0f;
    finalValue = GetRandomValue(1, 6);
}


/// <summary>
/// This function is to update the dice
/// </summary>
/// <param name="delta"></param>
void Dice3D::Update(float delta) {
    if (rolling) {
        timer += delta;

        
		rotationAngle += 720 * delta * (1.0f - timer / rollDuration);// its starting to slow down

        
        position.y = 1.0f + sin(timer * 10) * 
            (2.0f - (timer / rollDuration) * 2.0f);// Make the dice bounce using a sin wave

        // Stop after rollDuration
        if (timer >= rollDuration) {
            rolling = false;
            position.y = 1.0f;  // Set final position
            rotationAngle = (finalValue - 1) * 60;  // Align to final face
        }
    }
}


/// <summary>
/// This function is to draw the dice
/// </summary>
void Dice3D::Draw() {
    DrawCube(position, 1.0f,1.0f, 1.0f, BLACK);
    DrawCubeWires(position, 1.0f, 1.0f, 1.0f, BLUE);
}


/// <summary>
/// This function is to check if the dice is rolling
/// </summary>
/// <returns></returns>
bool Dice3D::IsRolling() const {
    return rolling;
}


/// <summary>
/// This function is to get the value of the dice
/// </summary>
/// <returns></returns>
int Dice3D::GetValue() const {
    return finalValue;
}
