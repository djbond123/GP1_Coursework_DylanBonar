/*
=================
cBambo.h
- Header file for class definition - SPECIFICATION
- Header file for the Sprite class
=================
*/
#ifndef _CBAMBO_H
#define _CBAMBO_H
#include "cSprite.h"

class cBambo : public cSprite
{
private:
	int mScore;
	eDirection mDirection;
	int id;

public:
	cBambo();
	cBambo(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename); // Constructor
	void update(float deltaTime);					// update the Bambo position based on the translation
	eDirection getDirection();      // Returns the current direction of the sprite
	void setDirection(eDirection theDirection); // Sets the current direction for the sprite.
	int GetId();
	void SetId(int _id);
};
#endif