/*
 * entity.cpp
 *
 *  Created on: 11/05/2013
 *      Author: felipe
 */

#include "entity.hpp"
#include <cmath>

using Physics::convertToPixels;
using Physics::Body;
using fgeal::Rectangle;

Entity::Entity(Animation* anim, Body *b) :
animation(anim),
body(b)
{}

Entity::~Entity()
{
	delete animation;
	delete body;
}

void Entity::draw(const Rectangle& visibleArea)
{
	float offx = 0, offy = 0;
	offx += -visibleArea.x;
	offy += -visibleArea.y;

	offx += (convertToPixels(body->getWidth()) - animation->current().width * animation->current().scale.x)/2;
	offy +=  convertToPixels(body->getHeight()) - animation->current().height * animation->current().scale.y;

	if(not body->isRotationFixed())
		animation->current().angle = -body->getAngle();
	else
		animation->current().angle = 0;

	animation->draw(convertToPixels(body->getX()) + offx, convertToPixels(body->getY()) + offy);
}
