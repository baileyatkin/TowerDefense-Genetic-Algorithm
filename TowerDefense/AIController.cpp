#include "AIController.h"
#include "Timer.h"
#include "GameState.h"
#include <iostream>
#include <Windows.h>
#include "GeneticAlgorithm.h"

#include "GameController.h"

using namespace std;

AIController::AIController()
{
	m_gameController = nullptr;
	m_gameBoard = nullptr;
	m_Timer = nullptr;
	m_gameState = nullptr;

	m_GA = new GeneticAlgorithm();
	GA_iteration = 0;
}

AIController::~AIController()
{

}

void AIController::gameOver()
{
	//m_GA->SetCurrentScore(m_gameState->getScore());

	// output chrom
	/*cout << "Chrom: ";
	for (int i = 0; i < CHROM_BITS; i++)
	{
		cout << m_GA->GetPopNext()[m_GA->GetCurrentIndex()].bit[i] << " ";
	}
	cout << endl;*/
	// output score
	//cout << "Score: " << m_gameState->getScore() << endl;
	// reset score
	m_gameState->setScore(0);
	// clear towers
	towersToAdd.clear();
	m_GA->Update();
	GA_iteration++;
}

void AIController::update()
{
	if (m_Timer == nullptr)
		return;

	// HINT
	// a second has elapsed - your GA manager (GA Code) may decide to do something at this time...
	static double elapsedSeconds = 0;
	double seconds = floor(m_Timer->elapsedSeconds());
	if (seconds > elapsedSeconds)
	{
		elapsedSeconds = seconds;

	}

	//GAManager::Instance()->Update(m_Timer->elapsedSeconds());

	if (!towersToAdd.empty()) // if haven't placed all GA towers
	{
		for (int i = 0; i < towersToAdd.size(); i++)
		{
			if (addTower(towersToAdd[i])) // if the tower was successfully added, remove from vector
			{
				//cout << "Add tower: " << (int)towersToAdd[i].type << ", X: " << towersToAdd[i].xPos << ", Y: " << towersToAdd[i].yPos << endl;
				swap(towersToAdd[i], towersToAdd.back());
				towersToAdd.pop_back();
			}
		}
	}

	// this might be useful? Monsters killed
	static int monstersKilled = 0;

	if (m_gameState->getMonsterEliminated() > monstersKilled)
	{
		monstersKilled = m_gameState->getMonsterEliminated();
	}

	recordScore();
}

bool AIController::addTower(TowerInfo tower)
{
	return m_gameBoard->addTower(tower.type, tower.xPos, tower.yPos);
}

bool AIController::addTower(TowerType type, int gridx, int gridy)
{
	// grid position can be from 0,0 to 25,17
	/*
	enum class TowerType {
	empty, slammer, swinger, thrower };
	*/

	bool towerAdded = m_gameBoard->addTower(type, gridx, gridy);
	return towerAdded;
	// NOTE towerAdded might be false if the tower can't be placed in that position, is there isn't enough funds
}

void AIController::setupBoard()
{
	m_Timer->start();

	chromosone GAPopulation = m_GA->GetPopNext()[m_GA->GetCurrentIndex()];
	for (int i = 0; i < CHROM_BITS; i++)
	{
		/*TowerType t = (TowerType)GAPopulation[i].bit[j];
		int xPos = GAPopulation[i].bitPosX[j];
		int yPos = GAPopulation[i].bitPosY[j];*/
		TowerInfo t;
		t.type = (TowerType)GAPopulation.bit[i];
		t.xPos = GAPopulation.bitPosX[i];
		t.yPos = GAPopulation.bitPosY[i];

		if (addTower(t))
		{
			//cout << "Add tower: " << (int)t.type << ", X: " << t.xPos << ", Y: " << t.yPos << endl;
		}
		else
		{
			//cout << "Unable to add tower: " << (int)t.type << ", X: " << t.xPos << ", Y: " << t.yPos << endl;
			towersToAdd.push_back(t);
		}
	}
}

int AIController::recordScore()
{
	int currentWave = m_gameState->getCurrentWave();
	int killCount = m_gameState->getMonsterEliminated();
	currentWave *= 10; // living longer is good
	int score = currentWave + killCount;

	m_gameState->setScore(score);
	m_GA->SetCurrentScore(score);

	return score;
}