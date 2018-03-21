#include "GameObject.h"
#include "Scene.h"
#include "SquareRenderer.h"
#include "CircleRenderer.h"


GameObject::GameObject(std::shared_ptr<IRenderer> renderer, MPoint2F location) 
	: _rendererPtr(renderer), _location(location) 
{
}

const IRenderer & GameObject::getRenderer()
{
	return *_rendererPtr;
}

const MPoint2F & GameObject::getLocation()
{
	return _location;
}

void GameObject::movement_leftright(double speed, int time, std::pair<int,int> direction)
{

	_location.x = _location.x + (speed*time)*.7   *  direction.first;
	
			//_location.y = _location.y + (speed*time) *.2  * direction.second;
	
}

void GameObject::movement_updown(double speed, int time, std::pair<int, int> direction)
{

	//_location.x = _location.x + (speed*time)*.2   *  direction.first;

	_location.y = _location.y + (speed*time) *.3  * direction.second;

}

void GameObject::movement_sidewise(double speed, int time, std::pair<int, int> direction)
{

	_location.x = _location.x + (speed*time) *.5  *  direction.first;

	_location.y = _location.y + (speed*time) *.5 * direction.second;

}

std::pair<int,int> GameObject::collision()
{
	if (_location.x > 1100 || _location.x <= 50 ) {
		_direction.first = _direction.first * (-1);
	}
	
	if (_location.y < 50   || _location.y >= 520) {
		_direction.second = _direction.second * (-1);
	}

	return _direction;
}







