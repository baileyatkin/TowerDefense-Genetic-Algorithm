#pragma once

#include "Tower.h"

class GameController;
class GameBoard;
class Timer;
class GameState;
class GeneticAlgorithm;

struct TowerInfo
{
	TowerType type;
	int xPos;
	int yPos;
};

class AIController
{
public:
	AIController();
	~AIController();

	void setupBoard();
	int  recordScore();

	void setGameController(GameController* gameController) { m_gameController = gameController; }
	void setGameBoard(GameBoard* gameBoard) { m_gameBoard = gameBoard; }
	void setTimer(Timer* timer) { m_Timer = timer; }
	void setGameState(GameState* gameState) { m_gameState = gameState; }
	void update();
	bool addTower(TowerInfo tower);
	bool addTower(TowerType type, int gridx, int gridy);
	void gameOver();

private:
	GameController* m_gameController;
	GameBoard* m_gameBoard;
	Timer* m_Timer;
	GameState* m_gameState;
	GeneticAlgorithm* m_GA;
	int				GA_iteration;
	const int		GA_maxIteration = 10;

	vector<TowerInfo> towersToAdd = vector<TowerInfo>();

	//int score = 0;
};