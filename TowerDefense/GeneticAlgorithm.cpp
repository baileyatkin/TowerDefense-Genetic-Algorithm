#include "GeneticAlgorithm.h"

GeneticAlgorithm::GeneticAlgorithm()
{
#if LOADING_CHROM
	loadChrom.bit[0] = 1;
	loadChrom.bitPosX[0] = 15;
	loadChrom.bitPosY[0] = 5;

	loadChrom.bit[1] = 3;
	loadChrom.bitPosX[1] = 9;
	loadChrom.bitPosY[1] = 10;

	loadChrom.bit[2] = 3;
	loadChrom.bitPosX[2] = 11;
	loadChrom.bitPosY[2] = 6;

	loadChrom.bit[3] = 1;
	loadChrom.bitPosX[3] = 18;
	loadChrom.bitPosY[3] = 10;

	loadChrom.bit[4] = 2;
	loadChrom.bitPosX[4] = 15;
	loadChrom.bitPosY[4] = 1;

	loadChrom.bit[5] = 2;
	loadChrom.bitPosX[5] = 2;
	loadChrom.bitPosY[5] = 1;

	loadChrom.bit[6] = 3;
	loadChrom.bitPosX[6] = 6;
	loadChrom.bitPosY[6] = 9;

	loadChrom.bit[7] = 3;
	loadChrom.bitPosX[7] = 22;
	loadChrom.bitPosY[7] = 14;

	loadChrom.bit[8] = 2;
	loadChrom.bitPosX[8] = 12;
	loadChrom.bitPosY[8] = 9;

	loadChrom.bit[9] = 1;
	loadChrom.bitPosX[9] = 7;
	loadChrom.bitPosY[9] = 7;

	popcurrent[0] = loadChrom;
	popnext[0] = loadChrom;
	currentIndex = 0;
#else
	evpop();
#endif
}

GeneticAlgorithm::~GeneticAlgorithm()
{
	logFile.close();
}

void GeneticAlgorithm::Update()
{
#if LOADING_CHROM
	std::cout << "Score: " << popcurrent[0].fit << std::endl;
#else
	// setting currentindex to -1 shows that it is ready to do GA again, otherwise it is checking fitness of each chrom
	if (currentIndex != -1)
	{
		if (!chromsToTest.empty())
		{
			std::cout << "Index: " << chromsToTest[currentIndex] << ": " << std::flush;
			for (int j = 0; j < CHROM_BITS; j++)
			{
				std::cout << popnext[chromsToTest[currentIndex]].bit[j] << std::flush;
			}
			std::cout << "\nScore: " << popnext[chromsToTest[currentIndex]].fit << std::endl;
		}
		else
		{
			std::cout << "No new chroms to test" << std::endl;
		}

		if (mutating) // if mutating, only want to check fitness of that one chrom, so can change back to -1 after fitness was found
		{
			mutating = false;
			currentIndex = -1;
		}
		else
		{
			//if (popnext[chromsToTest[++currentIndex]].fit != 0) // increase current index by 1
			//{
			//	++currentIndex; // if current chrom fit is not 0, it must have been tested before therefore skip
			//}
			if (++currentIndex >= chromsToTest.size()) // update index. if index greater than max, check for mutation which will change currentindex once finished
			{
				if (rand() % 2 == 1)
				{
					Mutation();
				}
				else
				{
					if (!pickedNewChroms) // has to check if not previously picked new chroms because picknewchroms will cause it to go back to here 
					{
						PickNewChroms();
					}
					else
					{
						pickedNewChroms = false; // reset pickednewchroms to allow it to be used next time
						currentIndex = -1;
					}
				}
			}
		}
	}

	// found fitness of each chroms that needed testing, now to sort and crossover
	if (currentIndex == -1)
	{
		chromsToTest.clear(); // clear chromstotest because they have all been tested
		PickChroms(); // sort popcurrent to have the highest scoring at the front to be used as parents

		std::cout << "\n========================\n";
		for (int i = 0; i < POP_SIZE; i++)
		{
			std::cout << "Chrom " << i << ": ";
			for (int j = 0; j < CHROM_BITS; j++)
			{
				std::cout << popcurrent[i].bit[j] << " ";
			}
			std::cout << std::endl;
			std::cout << "Score: " << popcurrent[i].fit << std::endl;
		}
		std::cout << "========================\n\n";

		Selection();
		//Crossover();
		if (chromsToTest.empty())
		{
			Update(); // redo update if empty, to do mutation/picknewchroms
		}
	}
#endif
}

void GeneticAlgorithm::SetCurrentScore(int score)
{
#if LOADING_CHROM
	popcurrent[0].fit = score;
	popnext[0].fit = score;
#else
	if (currentIndex != -1)
	{
		if (!chromsToTest.empty())
		{
			popnext[chromsToTest[currentIndex]].fit = score;
		}
	}
#endif
}

int GeneticAlgorithm::GetCurrentIndex()
{
#if LOADING_CHROM
	return 0;
#else
	if (!chromsToTest.empty())
	{
		return chromsToTest[currentIndex];
	}
	else
	{
		std::cout << "chromsToTest is empty! Using index 0" << std::endl;
		return 0;
	}
#endif
}

void GeneticAlgorithm::evpop()
{
	// generate chroms for population size
	for (int i = 0; i < POP_SIZE; i++)
	{
		for (int j = 0; j < CHROM_BITS; j++)
		{
			int newPosX, newPosY;
			newPosX = rand() % BOARD_WIDTH;
			newPosY = rand() % BOARD_HEIGHT;
			// check if the generated chrom is not the same as the others
			for (int k = 0; k < POP_SIZE; k++)
			{
				if (k != i)
				{
					while (newPosX == popcurrent[k].bitPosX[j])
					{
						newPosX = rand() % BOARD_WIDTH;
					}
					popcurrent[i].bitPosX[j] = newPosX;

					while (newPosY == popcurrent[k].bitPosY[j])
					{
						newPosY = rand() % BOARD_HEIGHT;
					}
					popcurrent[i].bitPosY[j] = newPosY;
				}
			}

			int newBit = (rand() % towerBit::thrower) + 1;
			popcurrent[i].bit[j] = newBit;
			//random = rand() % BOARD_WIDTH;
			//popcurrent[i].bitPosX[j] = random;
			//random = rand() % BOARD_HEIGHT;
			//popcurrent[i].bitPosY[j] = random;
			//popcurrent[i].fit = 0;
		}

		popnext[i] = popcurrent[i];
		chromsToTest.push_back(i);
	}
}

void GeneticAlgorithm::PickChroms()
{
	for (int i = 0; i < POP_SIZE; i++)
	{
		popcurrent[i] = popnext[i];
	}

	for (int i = 0; i < POP_SIZE; i++)
	{
		for (int j = i + 1; j <= POP_SIZE; j++)
		{
			if (popcurrent[j].fit > popcurrent[i].fit)
			{
				//std::cout << "new chroms picked: " << popcurrent[j].fit << std::endl;
				chromosone temp = popcurrent[j];
				popcurrent[j] = popcurrent[i];
				popcurrent[i] = temp;
			}
		}
	}

	/*std::cout << "\n========================\n";
	for (int i = 0; i < POP_SIZE; i++)
	{
		std::cout << "Chrom " << i << ": ";
		for (int j = 0; j < CHROM_BITS; j++)
		{
			std::cout << popcurrent[i].bit[j] << " ";
		}
		std::cout << std::endl;
		std::cout << "Score: " << popcurrent[i].fit << std::endl;
	}
	std::cout << "========================\n\n";*/

	for (int i = 0; i < POP_SIZE; i++)
	{
		popnext[i] = popcurrent[i]; // update popnext with the sorted chroms
	}
	LogBestChroms();
}

void GeneticAlgorithm::Selection()
{
	switch (selectionType)
	{
	case 0:
		//elitism
		Crossover();
		break;
	case 1:
		//steadystate
		SteadyStateSelection();
		break;
	case 2:
		//roulette
		RouletteSelection();
		Crossover();
		break;
	case 4:
		//tournament
		Crossover();
		break;
	default:
		std::cout << "selectionType out of range! Value was " << selectionType << std::endl;
	}
}

void GeneticAlgorithm::TournamentSelection()
{

}

void GeneticAlgorithm::RouletteSelection()
{
	std::cout << "\nRoulette selection: chosen ";
	// loop to select multiple parents
	for (int i = 0; i < CROSSOVER_PARENTS; i++)
	{
		// find the total sum of all chroms
		int fitnessSum = 0;
		for (int j = 0; j < POP_SIZE; j++)
		{
			fitnessSum += popnext[j].fit;
		}

		// create a random number between 0 and fitnessSum
		int randPoint = rand() % fitnessSum;

		// add up fitness until fitness is greater than randPoint. this is chosen to be parent
		int partialSum = 0;
		int chosenChrom = 0;
		for (int j = 0; j < POP_SIZE; j++)
		{
			partialSum += popnext[j].fit;
			if (partialSum > randPoint)
			{
				chosenChrom = j;
				break;
			}
		}

		// swap i and chosenchrom. i is the position of a parent
		chromosone temp = popnext[chosenChrom];
		popnext[chosenChrom] = popnext[i];
		popnext[i] = temp;
		std::cout << chosenChrom << (i < CROSSOVER_PARENTS - 1 ? "," : "\n\n") << std::flush;
	}
}

void GeneticAlgorithm::SteadyStateSelection()
{
	std::cout << "\nSteady State Selection: chosen ";
	// holds indexes of chroms to be removed
	vector<int> indexes;
	for (int i = 0; i < steadyStateRemoved; i++)
	{
		int childIndex = (rand() % (POP_SIZE - CROSSOVER_PARENTS)) + CROSSOVER_PARENTS;
		while (std::find(indexes.begin(), indexes.end(), childIndex) != indexes.end())
		{
			// indexes contains childindex, so find another
			childIndex = (rand() % (POP_SIZE - CROSSOVER_PARENTS)) + CROSSOVER_PARENTS;
		}
		popnext[childIndex].fit = 0;
		indexes.push_back(childIndex);
		std::cout << childIndex << ((i < (steadyStateRemoved - 1)) ? "," : "\n");
	}
	PickChroms();
	SteadyStateCrossover(indexes);
}

void GeneticAlgorithm::Crossover()
{
	if (POP_SIZE > 1)
	{
		std::cout << "Crossover:" << std::endl;

		int crossPoint = crossoverPoint;
		if (crossoverPoint == -1)
		{
			crossPoint = (rand() % (CHROM_BITS - 1)) + 1;
		}

		// start on parent size to start on children
		for (int i = CROSSOVER_PARENTS; i < POP_SIZE; i++)
		{
			chromosone currentChrom = popnext[i];
			for (int j = 1; j < crossPoint; j++) // crossing bits below the cross point
			{
				// if crossover index greater than 1, take 1. this makes the index different to the index after the cross point
				int index = i % CROSSOVER_PARENTS;
				chromosone crossChrom = popnext[index];
				popnext[i].bit[j] = crossChrom.bit[j];
				popnext[i].bitPosX[j] = crossChrom.bitPosX[j];
				popnext[i].bitPosY[j] = crossChrom.bitPosY[j];
			}
			for (int j = crossPoint; j < CHROM_BITS; j++) // crossing bits above the cross point
			{
				int index = (i + 1) % CROSSOVER_PARENTS;
				chromosone crossChrom = popnext[index];
				popnext[i].bit[j] = crossChrom.bit[j];
				popnext[i].bitPosX[j] = crossChrom.bitPosX[j];
				popnext[i].bitPosY[j] = crossChrom.bitPosY[j];
			}
			if (popnext[i] != currentChrom)
			{
				popnext[i].fit = 0;
				chromsToTest.push_back(i);
			}
		}
		currentIndex = 0;
	}
}

void GeneticAlgorithm::SteadyStateCrossover(vector<int> indexes)
{
	std::cout << "Crossover:" << std::endl;

	int crossPoint = crossoverPoint;
	if (crossoverPoint == -1)
	{
		crossPoint = (rand() % (CHROM_BITS - 1)) + 1;
	}

	// crosses over with remaining chroms
	int size = POP_SIZE - steadyStateRemoved;
	for (int i = 0; i < POP_SIZE; i++)
	{
		chromosone currentChrom = popnext[i];
		for (int j = 1; j < crossPoint; j++) // crossing bits below the cross point
		{
			int index = j % (size > 1 ? size - 1 : size);
			// use popcurrent so that it does not use duplicate chroms
			chromosone crossChrom = popcurrent[index];
			popnext[i].bit[j] = crossChrom.bit[j];
			popnext[i].bitPosX[j] = crossChrom.bitPosX[j];
			popnext[i].bitPosY[j] = crossChrom.bitPosY[j];
		}
		for (int j = crossPoint; j < CHROM_BITS; j++) // crossing bits above the cross point
		{
			int index = j % size;
			// use popcurrent so that it does not use duplicate chroms
			chromosone crossChrom = popcurrent[index];
			popnext[i].bit[j] = crossChrom.bit[j];
			popnext[i].bitPosX[j] = crossChrom.bitPosX[j];
			popnext[i].bitPosY[j] = crossChrom.bitPosY[j];
		}
		if (popnext[i] != currentChrom)
		{
			popnext[i].fit = 0;
			chromsToTest.push_back(i);
		}
	}
	currentIndex = 0;
}

bool GeneticAlgorithm::Mutation()
{
	if ((rand() % 2) == 1 || chromsToTest.empty())
	{
		int bitIndex = rand() % CHROM_BITS;
		int chromIndex = rand() % POP_SIZE;
		// randomise bit
		std::cout << "------------------------\nMutation! Index: " << chromIndex << std::endl;
		for (int i = 0; i < CHROM_BITS; i++)
		{
			std::cout << popnext[chromIndex].bit[i];
		}
		std::cout << " -> ";

		chromosone newChrom = popnext[chromIndex];

		// make new random bit that is not the same
		short newBit = (rand() % towerBit::thrower) + 1;
		while (newBit == popnext[chromIndex].bit[bitIndex])
		{
			newBit = (rand() % towerBit::thrower) + 1;
		}
		newChrom.bit[bitIndex] = newBit;

		// make new random X position
		short int newPosX = rand() % BOARD_WIDTH;
		while (newPosX == popnext[chromIndex].bitPosX[bitIndex])
		{
			newPosX = rand() % BOARD_WIDTH;
		}
		newChrom.bitPosX[bitIndex] = newPosX;

		// make new random Y position
		short int newPosY = rand() % BOARD_HEIGHT;
		while (newPosY == popnext[chromIndex].bitPosY[bitIndex])
		{
			newPosY = rand() % BOARD_HEIGHT;
		}
		newChrom.bitPosY[bitIndex] = newPosY;

		popnext[chromIndex] = newChrom;

		for (int i = 0; i < CHROM_BITS; i++)
		{
			std::cout << popnext[chromIndex].bit[i];
		}
		std::cout << "\n------------------------\n\n";
		//std::cout << "mutation!" << std::endl;
		chromsToTest.push_back(chromIndex);
		currentIndex = chromsToTest.size() - 1;
		mutating = true;
		popnext[chromsToTest[currentIndex]].fit = 0;
		return true;
	}
	else
	{
		currentIndex = -1;
		return false;
	}
}

void GeneticAlgorithm::PickNewChroms()
{
	if (rand() % 2 == 1 || chromsToTest.empty())
	{
		PickChroms(); // sort chroms

		std::cout << "------------------------\nPicked new chroms!\nIndex: ";
		int index = POP_SIZE - 1 - newChroms;
		currentIndex = chromsToTest.size();
		// generate new random chroms that score the lowest
		for (int i = index; i < POP_SIZE; i++)
		{
			std::cout << i << (i == POP_SIZE - 1 ? "\n" : ", ");
			for (int j = 0; j < CHROM_BITS; j++)
			{
				int newBit = (rand() % towerBit::thrower) + 1;
				popnext[i].bit[j] = newBit;
				int newPosX = rand() % BOARD_WIDTH;
				popnext[i].bitPosX[j] = newPosX;
				int newPosY = rand() % BOARD_HEIGHT;
				popnext[i].bitPosY[j] = newPosY;
				popnext[i].fit = 0;
			}
			chromsToTest.push_back(i);
		}
		std::cout << "------------------------\n\n";
		pickedNewChroms = true;
	}
	else
	{
		currentIndex = -1;
	}
}

void GeneticAlgorithm::LogBestChroms()
{
	string newLog = "";
	for (int i = 0; i < CHROM_BITS; i++)
	{
		newLog.append(std::to_string(popcurrent[0].bit[i]) + " | X: " + std::to_string(popcurrent[0].bitPosX[i]) + ", Y: " + std::to_string(popcurrent[0].bitPosY[i]) + "\n");
	}
	newLog.append("Score: " + std::to_string(popcurrent[0].fit) + "\n\n");

	if (logFile && logFile.good())
	{
		logFile << newLog;
	}
	else
	{
		std::cout << "Error opening file!" << std::endl;
	}
	logFile.flush();
}