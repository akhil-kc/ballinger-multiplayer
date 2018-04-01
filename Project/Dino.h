#ifndef _CG_DINO_H_
#define _CG_DINO_H_

#include "Model.h"
#include "Vertex.h"
#include "Vector.h"
#include "cBicho.h"
#include "Timer.h"
#include "cCamera.h"

#define PLAYER_SPEED  0.008f
#define PLAYER_JUMP_SPEED  0.4f
#define FRICTION   0.05f
#define ELASTICITY   0.5f //porcentaje de rango [0.0, 1.0]
#define MAX_MOVEMENT 0.4f

static const char	DINO_BODY_FILE[] = "../models/dino/body.sgf",
DINO_EYES_FILE[] = "../models/dino/eyes.sgf",
DINO_ARM_FILE[] = "../models/dino/arm.sgf",
DINO_LEG_FILE[] = "../models/dino/leg.sgf";

static const char	DINO_TEXTURE[] = "../textures/dinoskin.rgb";

static const BoundingBox DINO_TEXTURE_BOX(Vertex3(0, 0, 0), Vertex3(5, 7, 10));

// The dinosaur :)
class Dino : public cBicho
{
public:
	Dino();
	void draw(/*cTerrain *Terrain, cCamera *Camera*/);
	virtual ~Dino(void);
	void SetFade(bool b);
	// Set position, direction and phase (in walking)
	//void setPosition(Vertex3 pos);
	//void setDirection(Vector3 dir);
	void setPhase(float phase); // phase is 0..1 of the walking cycle
	void init();
	void rotate(float angle);
	Timer _timer;
	Timer & getTimer();
	void setAngle(float angle);
	float _ang;
	bool isMove;
	bool isJump;
	bool isRight;
private:
	// Rotate around a given point
	static void rotateAround(float cx, float cy, float cz,		// The point to rotate around
		float ax, float ay, float az,		// The rotation vector
		float angle);						// The rotation angle

											// Smoothly interpolates between s and e at point (v/d)
	static float interpolate(float s, float e, float v, float d);

	// Get the height of the leg at a given phase
	static float getLegHeight(float phase);

	// The models of the dinosaur
	Model	_body;
	Model	_eyes;
	Model	_arm;
	Model	_leg;

	float		_phase;			// Phase of the walking cycle
								//Vertex3		_position;
								//Vector3		_direction;
	bool fade;

};

#endif
