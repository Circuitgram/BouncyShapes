#include "Scene.h"
#include "iostream"
#include "GameObject.h"

void Scene::insertGameObject(std::unique_ptr<GameObject>& object)
{
	_gameObjects.push_back(std::move(object));
}

void Scene::Update(double deltaTime)
{
	for (int i = 0; i < _gameObjects.size();i++) {
		if(i < 3)  _gameObjects.at(i)->movement_leftright(0.5*i+.1, deltaTime, _gameObjects.at(i)->collision());
		else if(i < 6) _gameObjects.at(i)->movement_updown(0.3*i + .1, deltaTime, _gameObjects.at(i)->collision());
		else if (i < _gameObjects.size())_gameObjects.at(i)->movement_sidewise(0.1*i + .1, deltaTime, _gameObjects.at(i)->collision());
	}
}
void Scene::Draw(ID2D1HwndRenderTarget & target)
{
	
	for (auto& object : _gameObjects)
	{
		auto current = object->getLocation();
		const auto& renderer = object->getRenderer();
		renderer.Render(target, current);
	}
}
