#pragma once

#include "raylib.h"



class Dice3D {
private:
    Vector3 position;      // Dice position
    Vector3 rotationAxis;  // Rotation axis
    float rotationAngle;   // Rotation angle
    bool rolling;          // Is rolling?
    float rollDuration;    // Rolling time
    float timer;           // Timer for animation
    int finalValue;        // Final dice value

public:
    Dice3D(Vector3 pos);
    void StartRoll();
    void Update(float delta);
    void Draw();
    bool IsRolling() const;
    int GetValue() const;
};

