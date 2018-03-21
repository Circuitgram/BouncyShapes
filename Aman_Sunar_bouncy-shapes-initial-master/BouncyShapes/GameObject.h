#pragma once

#include <memory>
#include <vector>
#include "GeometryPrimitives.h"
#include "IRenderer.h"

class GameObject {
public:
	GameObject(std::shared_ptr<IRenderer> renderer, MPoint2F location);
	const IRenderer& getRenderer();
	const MPoint2F& getLocation();
	void GameObject::movement_leftright(double speed, int time,std::pair<int,int> direction);
	void GameObject::movement_updown(double speed, int time, std::pair<int, int> direction);
	void GameObject::movement_sidewise(double speed, int time, std::pair<int, int> direction);
	std::pair<int,int>GameObject::collision();

private:
	
	std::shared_ptr<IRenderer> _rendererPtr;
	MPoint2F _location;
	std::pair<int,int> _direction = std::make_pair(1,1);
	int _i = 0;
};