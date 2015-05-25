fndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include "State.h"

// Initialize the character state
void State_CharacterController_Initialize(State* s, const float velocity, const float angularVelocity, float maxVel, float shootSpeed);

// Free members in the character state
void State_CharacterController_Free(State* s);

// Update the render manager, lets the character update position and movement.
void State_CharacterController_Update(GObject* GO, State* state);

// Camera Rotation
void State_CharacterController_Rotate(GObject* GO, State* state);

// Camera Translation
void State_CharacterController_Translate(GObject* GO, State* state);

void State_CharacterController_ShootBullet(GObject* GO, State* state);
#endif
