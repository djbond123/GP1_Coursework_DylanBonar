/*
=================
cBa.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cBambo.h"

/*
===========================
* Return Id assigned to object
===========================
*/
int cBambo::GetId() {
	return cBambo::id;
}

/*
===========================
* Set the id
===========================
*/
void cBambo::SetId(int _id) {
	cBambo::id = _id;
}
/*
=================
- Data constructor initializes the cBalloon to the data passed to 
- the constructor from the paramater sPosition.
- Is always called, thus ensures all sprite objects are in a consistent state.
=================
*/
cBambo::cBambo(): cSprite() 			// Default constructor
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );
	/* generate secret number: */
	mScore = (rand() % 10 + 1)*10;
	mDirection = static_cast<eDirection>(rand() % 2);
}
cBambo::cBambo(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename): cSprite(sPosition,pd3dDevice,theFilename)  // Constructor
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );
	/* generate secret number: */
	mScore = (rand() % 10 + 1)*10;
	mDirection = static_cast<eDirection>(rand() % 2);
}
/*
=================
- update the position etc of the sprite.
=================
*/
void cBambo::update(float deltaTime) 					// update the balloons position based on the translation
{
	D3DXVECTOR2 tempV2;
	D3DXVECTOR3 tempV3;
	tempV2 = cBambo::getSpritePos2D() + (cBambo::getTranslation())*deltaTime;
	tempV3 = D3DXVECTOR3(tempV2.x,tempV2.y,0.0f);
	cBambo::setSpritePos(tempV3);
	cSprite::update();
}
/*
=================
- Returns the current direction of the sprite.
=================
*/
eDirection cBambo::getDirection()       // Rturns the current direction of the sprite
{
	return cBambo::mDirection;
}
/*
=================
- Sets the current direction for the sprite.
=================
*/
void cBambo::setDirection(eDirection theDirection)  // Sets the current direction for the sprite.
{
	cBambo::mDirection = theDirection;
}
