#pragma once

#include "raylib.h"



class Dice3D {
private:
    Vector3 position;      
    Vector3 rotationAxis;  
    float rotationAngle;   
    bool rolling;          
    float rollDuration;    
    float timer;           
    int finalValue;        

public:
    Dice3D(Vector3 pos);
    void StartRoll();
    void Update(float delta);
    void Draw();
    bool IsRolling() const;
    int GetValue() const;
};

