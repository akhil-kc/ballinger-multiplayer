#include "cDino.h"
#include "util.h"
float fyaw = 0.0, fx = 0.0, fy = 0.0, fz = 0.0;
Dino::Dino()
{
	fade = true;
}
void Dino::init(bool isPlayer2)
{
	if (isPlayer2) {
		_body.loadSGF(DINO_RED_BODY_FILE);
		_eyes.loadSGF(DINO_EYES_FILE);
		_arm.loadSGF(DINO_RED_ARM_FILE);
		_leg.loadSGF(DINO_RED_LEG_FILE);
	}
	else {
		_body.loadSGF(DINO_BODY_FILE);
		_eyes.loadSGF(DINO_EYES_FILE);
		_arm.loadSGF(DINO_ARM_FILE);
		_leg.loadSGF(DINO_LEG_FILE);
	}
	

	// Load the texture
	CGTexture *pTex = new CGTexture(DINO_TEXTURE);
	if (pTex->loadTextureFile())
	{
		_body.setTexture(pTex, DINO_TEXTURE_BOX);
		_arm.setTexture(pTex, DINO_TEXTURE_BOX);
		_leg.setTexture(pTex, DINO_TEXTURE_BOX);
	}

	_phase = 0.0;
	isJump = false;
}
Dino::~Dino(void)
{
}
void Dino::SetFade(bool b)
{
	fade = b;
}
void Dino::setAngle(float angle)
{
	_ang = angle;
}
void Dino::draw(cTerrain *Terrain, cCamera *Camera)
{
	glPushMatrix();
	float x = GetX();
	float y;
	if (abs(Camera->GetY() - Terrain->GetHeight(GetX(), GetZ())) > 5) {
		y = Camera->GetY() - 2;
	} else {
		y = Terrain->GetHeight(GetX(), GetZ()) + 0.5f;
	}
	float z = GetZ();

	Camera->GetDirectionVector(fx, fy, fz);
	glTranslatef(x, y, z);
	float ang = rad2deg(atan2(-fz, fx));
	glRotatef(ang, 0.0, 1.0, 0.0);

	// Move z-axis to the middle of the dino
	glTranslatef(0, 0, -1.5);

	// Calculate some cycle values
	float	cosVal = cos(_phase * 2 * PI); // cosine function
	float   sinVal = sin(_phase * 2 * PI); // sine function
	float	cosVal2 = cos((_phase + 0.5) * 2 * PI); // second cosine function

													
	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.5f);
	_body.draw(true);
	glPopMatrix();

	glScalef(0.2f, 0.2f, 0.5f);
	glTranslatef(0, 0, -0.1);
	_eyes.draw(false);
	glTranslatef(0, 0, 0.1);

	// Draw one leg	
	glPushMatrix();
	float up = getLegHeight(_phase);
	//printf("leg height: %f\n", up);
	glScalef(0.6f, 0.6f, 1.0f);
	glTranslatef(0, up, 3);
	if (isMove)
		rotateAround(10, 5, 0, 0, 0, 1, cosVal * -55);
	_leg.draw(true);
	glPopMatrix();

	// Draw other leg
	glPushMatrix();
	float q = fmod(_phase + 0.5, 1.0);
	up = getLegHeight(q);
	glScalef(0.6f, 0.6f, 1.0f);
	glTranslatef(0, up, -1.5);
	if (isMove)
		rotateAround(10, 5, 0, 0, 0, 1, cosVal2 * -55);
	_leg.draw(true);
	glPopMatrix();

	// Draw one arm
	glPushMatrix();
	glTranslatef(0, 0, -0.6);
	if (isMove)
		rotateAround(10, 10, 0, 0, 0, 1, cosVal2 * 20);
	_arm.draw(true);
	glPopMatrix();

	// Draw other arm
	glPushMatrix();
	glTranslatef(0, 0, 2.7);
	if (isMove)
		rotateAround(10, 10, 0, 0, 0, 1, cosVal2 * -20);
	_arm.draw(true);
	glPopMatrix();

	glPopMatrix();
}


void Dino::rotateAround(float cx, float cy, float cz,
	float ax, float ay, float az,
	float angle)
{

	glTranslatef(cx, cy, cz);
	glRotatef(angle, ax, ay, az);
	glTranslatef(-cx, -cy, -cz);
}

void Dino::rotate(float angle)
{
	glPushMatrix();
	glTranslatef(-GetX(), -GetY(), -GetZ());
	glRotatef(angle, 0, 1, 0);
	glTranslatef(GetX(), GetY(), GetZ());
	glPopMatrix();
}

float Dino::interpolate(float s, float e, float v, float d)
{
	// Interpolates sort of smoothly
	v = v / d;

	v = v * 3.14;
	v = v - 3.14 / 2;
	v = sin(v);
	v += 1.0;
	v /= 2.0;
	return v * (e - s) + s;
}

float Dino::getLegHeight(float phase)
{
	float up = 0;
	float top = 1.5;		// top position of the leg
	float center = 1;		// center position of the leg
	float bottom = 0;		// lowest position of the leg
	float step = 0.30;		// position when taking a step up

	if (phase < 0.25)
	{
		up = interpolate(center, top, phase, 0.25);
	}
	else if (phase < 0.5)
	{
		up = interpolate(top, bottom, phase - 0.25, 0.25);
	}
	else if (phase < 0.75)
	{
		up = interpolate(bottom, step, phase - 0.5, 0.25);
	}
	else //if (phase < 1.0)
	{
		up = interpolate(step, center, phase - 0.75, 0.25);
	}
	return up;
}

void Dino::setPhase(float phase)
{
	_phase = phase;
}

Timer & Dino::getTimer()
{
	return _timer;
}
