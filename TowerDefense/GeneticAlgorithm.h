#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using std::string;
using std::vector;

//32 x 18
#define BOARD_WIDTH_FULL 32
#define BOARD_HEIGHT_FULL 18
#define BOARD_WIDTH 24 // 32 is the full width but after 24 towers cannot attack the track
#define BOARD_HEIGHT 17 // 18 is full width but too low

// number of chroms
#define POP_SIZE 30
// number of bits per chrom
#define CHROM_BITS 10
// number of parents used in crossover
#define CROSSOVER_PARENTS 10

#define LOADING_CHROM false

enum towerBit
{
	empty,
	slammer,
	swinger,
	thrower
};

struct chromosone
{
	short int bitPosX[CHROM_BITS];
	short int bitPosY[CHROM_BITS];
	short int bit[CHROM_BITS];
	int fit;

	bool operator ==(chromosone c)
	{
		for (int i = 0; i < CHROM_BITS; i++)
		{
			if (this->bit[i] != c.bit[i])
			{
				return false;
			}
			if (this->bitPosX[i] != c.bitPosX[i])
			{
				return false;
			}
			if (this->bitPosY[i] != c.bitPosY[i])
			{
				return false;
			}
		}
		return true;
	}
	bool operator!=(chromosone c)
	{
		return !(operator==(c));
	}
};

class GeneticAlgorithm
{
public:
	GeneticAlgorithm();
	~GeneticAlgorithm();
	void Update();

	chromosone* GetPopCurrent() { return popcurrent; }
	chromosone* GetPopNext() { return popnext; }

	void SetCurrentScore(int score);

	int GetCurrentIndex();
private:
	void evpop();

	// order the chroms in popcurrent by fitness
	void PickChroms();
	// select which chroms should be parents
	void Selection();

	// the way parents are selected
	// 0=elitism, 1=steady state, 2=roulette
	const int selectionType = 0;
	void TournamentSelection();

	void RouletteSelection();

	void SteadyStateSelection();
	// the number of chroms to be replaced during steady state selection
	const int steadyStateRemoved = (POP_SIZE - CROSSOVER_PARENTS) - 4;
	void SteadyStateCrossover(vector<int> indexes);

	// crossover parents to the children
	void Crossover();
	// the index of the crossover point. set to -1 to be random.
	const int crossoverPoint = CHROM_BITS / 2;

	// random chance to mutate a bit, changing its tower and position
	bool Mutation();

	// random chance to pick new chroms. creates random bits for the lowest scoring towers. amount defined by NEW_CHROMS
	void PickNewChroms();
	// number of chroms to be randomly generated in picknewchroms()
	const int newChroms = 10;

	// save the current best chroms to a log file
	void LogBestChroms();

	chromosone popcurrent[POP_SIZE];
	chromosone popnext[POP_SIZE];

	// the current index within chromsToTest
	int currentIndex = 0;
	// stores the indexes of chroms that have changed and need testing
	vector<int> chromsToTest = vector<int>();

	// mutating will be true when checking fitness of mutation. needed to avoid checking all chroms.
	bool mutating = false;

	// will be true when just picked new chroms. prevents it from picking new chroms until sorted again
	bool pickedNewChroms = false;

	std::ofstream logFile = std::ofstream("Log.txt", std::ios_base::app);

	chromosone loadChrom;
};

