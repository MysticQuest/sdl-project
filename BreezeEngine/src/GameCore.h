#pragma once

#include <SDL.h>
#include "MySDLWindow.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/TransformSystem.h"
#include "ECS/Systems/InputSystem.h"
#include "ECS/Systems/CollisionSystem.h"
#include "MyMath.h"
#include <functional>


class GameCore
{
public:
	GameCore(MySDLWindow& sdlWindow, int frame_rate);
	void InitializeGame();
	~GameCore();

	void Render(MySDLWindow& sdlWindow, const std::vector<Entity>& entities);
	void PollEvents();
	void Update(int deltaTime);
	void Run(MySDLWindow& sdlWindow);
	void GameOver();
	void RestartGame();
	void RegisterObserver(const std::function<void(int)>& callback);
	ECSManager ecs;

private:
	std::vector<Entity> entities;
	int m_frame_rate = 0;
	MySDLWindow& m_sdlWindow;
	bool isRunning = true;
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	bool gameOver = false;
	SDL_Texture* gameOverTexture = nullptr;

	std::function<void(int)> observer;


	std::unique_ptr<RenderSystem> renderSystem;
	std::unique_ptr<TransformSystem> transformSystem;
	std::unique_ptr<InputSystem> inputSystem;
	std::unique_ptr<CollisionSystem> collisionSystem;
};