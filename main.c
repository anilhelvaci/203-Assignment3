#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Gene structure
typedef struct gene {
    int data;
    struct gene* next;
} GENE;

// Chromosome structure
typedef struct chromosome {
    float rank;
    int fitness;
    GENE* firstGene;
    struct chromosome* next;
} CHROMOSOME;

// Population structure
typedef struct population {
    CHROMOSOME* firstChromosome;
    CHROMOSOME* lastChromosome;
} POPULATION;

// This function is used to  create a gene from a given data
GENE* createGene(int data) {
    // Dynamically allocate space and set the default values
    GENE* newGene = malloc(sizeof(GENE));
    newGene->data = data;
    newGene->next = NULL;
    return newGene;
}

CHROMOSOME* createChromosome(char* geneString) {
    // Dynamically allocate space and set the default values
    CHROMOSOME* newChromosome = malloc(sizeof(CHROMOSOME));
    newChromosome->next = NULL;
    newChromosome->rank = -1;
    newChromosome->fitness = -1;

    // Split the string by ":" and convert the value to int in order to create new gene
    char* token = strtok(geneString, ":");
    newChromosome->firstGene = createGene(atoi(token));
    GENE* lastGene = newChromosome->firstGene;

    token = strtok(NULL, ":");

    // Iterate until the end of the token
    while(token) {
        lastGene->next = createGene(atoi(token));
        lastGene = lastGene->next;
        token = strtok(NULL, ":");
    }

    // Return the newly created chromosome
    return newChromosome;
}

// Calculate the fitness value of a given chromosome
int assignFitness(CHROMOSOME* chromosome) {
    int fitness = 0;

    // Initialize the temp pointer in order to iterate
    GENE* temp = chromosome->firstGene;

    while (temp) {
        // Shift the value one position to the left
        fitness = fitness << 1;
        // Add data
        fitness += temp->data;
        // Continue to new node
        temp = temp->next;
    }

    // Assign the calculated value
    chromosome->fitness = fitness;
    return fitness;
}

// Calculate the rank of the chromosomes from a given population
void assignRanks(POPULATION* population) {
    CHROMOSOME* temp = population->firstChromosome;
    int min = assignFitness(temp); // Assign the first fitness value to the min variable
    int max = min; // Max is equal to min for now
    int fitness = min; // Current fitness is the min for now

    temp = temp->next;

    // Iterate to find max and min fitness values in order to calculate rank
    while (temp) {
        fitness = assignFitness(temp); // Calculate the fitness of next chromosome

        // If smaller than min update as the new min
        if (fitness < min) {
            min = fitness;
        }

        // If greater than max update as the new max
        if (fitness > max) {
            max = fitness;
        }

        temp = temp->next;
    }

    float denominator = max - min;

    temp = population->firstChromosome; // Reassign to head in order to iterate once more

    while (temp) {
        // If denominator is 0 then rank is zero too in order to avoid ZeroDivision error
        if (denominator == 0) {
            temp->rank = 0;
        } else {
            temp->rank = (float) (temp->fitness - min) / denominator; // Perform the formula in the assignment paper
        }
        temp = temp->next;
    }
}

// This is a helper function to perform the swapping operation in the sort function
void swapChromosome(POPULATION* population, CHROMOSOME* prev, CHROMOSOME* current, CHROMOSOME* next) {
    if (prev) {
        prev->next = next; //If prev is not null then it points the next
    } else {
        // If prev is null then current is the first chromosome
        population->firstChromosome = next;
    }
    if (!next->next) {
        population->lastChromosome = current; // If next of the next is null then lastChromosome of population is now the current
    }

    // Swap the remaining nodes
    current->next = next->next;
    next->next = current;
}

// This is an implementation of bubble sort
void sortPopulation(POPULATION* population) {
    // Initialize necessary pointers
    CHROMOSOME* current = population->firstChromosome;
    CHROMOSOME* nextChromosome = current->next;
    CHROMOSOME* prev = NULL;
    CHROMOSOME* last = NULL;

    while (population->firstChromosome != last) {
        current = population->firstChromosome;
        nextChromosome = current->next;
        prev = NULL;

        while (nextChromosome != last) {
            if (current->fitness > nextChromosome->fitness) {
                swapChromosome(population, prev, current, nextChromosome); // If the value of current node is greater then the value of the next node then swap
            }

            // Update pointers
            prev = current;
            current = nextChromosome;
            nextChromosome = nextChromosome->next;
        }
        last = current;
    }
}

// Helper function to perform sorting and assigning ranks
void updatePopulation(POPULATION* population) {
    assignRanks(population);
    sortPopulation(population);
}

// Read the file character by character
char* readAllCharacters(char* filePath) {
    FILE *file;
    file = fopen(filePath, "r"); // Open file
    char* characters = malloc(sizeof(char)); // Dynamically allocate space
    int charCount = 0;
    int c = fgetc(file); // Get the char

    // Iterate
    while (c != EOF) {
        characters[charCount] = c;
        charCount++;
        characters = realloc(characters, sizeof(char) * (charCount + 1));
        c = fgetc(file);
    }

    characters[charCount] = '\0'; // Add end of file character
    fclose(file); // Close the file
    return characters;
}

// Parse the characters line by line
char** charsToLines(char* characters) {
    char** commandLines = malloc(sizeof(char*) * 2); // Allocate space
    char* line = strtok(characters, "\r\n"); // Get the part before the delimeter
    int i = 1;
    commandLines[0] = line;

    // Iterate
    while (line) {
        line = strtok(NULL, "\r\n"); // Get the next part
        commandLines[i] = line;
        i++;
        commandLines = realloc(commandLines, sizeof(char*) * (i + 1)); // Dynamically allocate the space
    }
    return commandLines;
}

void addChromosome(POPULATION* population, CHROMOSOME* chromosome) {
    if (population->lastChromosome) {
        population->lastChromosome->next = chromosome; // If population is not empty add the new chromosome to last
    } else {
        population->firstChromosome = chromosome; // Else add to first
    }

    population->lastChromosome = chromosome;
}

// Function to display a population
void outputPopulation(struct population* population){
    CHROMOSOME* chromosome = population->firstChromosome;
    GENE* gene = NULL;
    while(chromosome){
        gene = chromosome->firstGene->next;
        printf("%d", chromosome->firstGene->data);
        while(gene != NULL){
            printf(":%d", gene->data);
            gene = gene->next;
        }
        printf(" -> %d\n", chromosome->fitness);
        chromosome = chromosome->next;
    }
}

// Function to get the chromosome that is present in a given index
CHROMOSOME* getChromosome(CHROMOSOME* head, int number) {
    CHROMOSOME* temp = head;
    int count = 1;

    while (temp) {
        if (count == number){
            return temp;
        }
        temp = temp->next;
        count++;
    }
    return NULL;
}

// Function perform xover per one selection
void xoverPopulation(int startIndex, int endIndex, char* selections, POPULATION* population) {
    // Extract the selected chromosome location from the given string
    char* token = strtok(selections, ":");
    int firstChr = atoi(token);
    token = strtok(NULL, ":");
    int secondChr = atoi(token);

    // Get the selected chromosomes
    CHROMOSOME* chromosomeOne = getChromosome(population->firstChromosome, firstChr);
    CHROMOSOME* chromosomeTwo = getChromosome(population->firstChromosome, secondChr);

    // Check if chromosomes exists
    if (!chromosomeOne || !chromosomeTwo) {
        printf("Null Chromosome!!!");
        return;
    }

    // Initialize the genes for iteration
    GENE* tempOne = chromosomeOne->firstGene;
    GENE* tempTwo = chromosomeTwo->firstGene;
    int geneCount = 1; // Start counting from 1
    int swapGene = startIndex; // First index position to swap

    while(tempOne && tempTwo) {
        if (geneCount == swapGene && swapGene <= endIndex) {
            // Swap
            int val = tempOne->data;
            tempOne->data = tempTwo->data;
            tempTwo->data = val;
            swapGene++;
        }
        // Move on
        tempOne = tempOne->next;
        tempTwo = tempTwo->next;
        geneCount++;
    }
}

// Function to direct selections per generations to xover per selection
void xoverWrapper(char* indexes, char* selectionLine, POPULATION* population, int popSize) {
    // Extract the start and end positions from a given string
    char* token = strtok(indexes, ":");
    int startIndex = atoi(token);
    token = strtok(NULL, ":");
    int endIndex = atoi(token);

    char* selectionGen[popSize / 2]; // Initialize the selections per generation array
    int count = 0;
    char* selection = strtok(selectionLine, " ");

    while (selection) {
        // Store the selections
        selectionGen[count] = selection;
        count++;
        selection = strtok(NULL, " ");
    }

    int i;
    for (i = 0; i < (popSize / 2); ++i) {
        xoverPopulation(startIndex, endIndex, selectionGen[i], population); // Perform xover per selection
    }
}

// Helper function to perform mutation
int getOpposite(int data) {
    if (data == 1) {
        return 0;
    }
    return 1;
}

// Function to perform mutation
void mutate(int index, POPULATION* population) {
    CHROMOSOME* tempChromosome = population->firstChromosome; // Initialize temp chromosome for iteration

    while (tempChromosome) {
        GENE* tempGene = tempChromosome->firstGene; // Initialize temp gene for iteration
        int geneCount = 1;
        while (tempGene) {
            // Check if the gene is the intended gene to mutate
            if (geneCount == index) {
                tempGene->data = getOpposite(tempGene->data); // Perform the actual mutation
                break;
            }
            // Move on
            geneCount++;
            tempGene = tempGene->next;
        }
        tempChromosome = tempChromosome->next;
    }
}

// Function to initialize the population
void initializePopulation(POPULATION* population, char** popArray, int popSize) {
    int i = 0;
    for (; i < popSize; ++i) {
        addChromosome(population, createChromosome(popArray[i])); // Add the new chromosome
    }
    updatePopulation(population); // Assign ranks, fitness then sort
}

// Function to update the best chromosome
void updateBestChromosome(CHROMOSOME* best, POPULATION* population) {
    // Initialize the values
    best->fitness = population->firstChromosome->fitness;
    best->rank = population->firstChromosome->rank;
    best->firstGene = createGene(population->firstChromosome->firstGene->data);

    // Set necessary pointers
    GENE* tempGene = population->firstChromosome->firstGene->next;
    GENE* lastGene = best->firstGene;

    // Iterate
    while (tempGene) {
        lastGene->next = createGene(tempGene->data); // Create the new gene
        // Move on
        lastGene = lastGene->next;
        tempGene = tempGene->next;
    }
}

// Function to display the best chromosome
void displayBestChromosome(CHROMOSOME* best) {
    printf("Best chromosome found so far: %d", best->firstGene->data);
    GENE* temp = best->firstGene->next;
    while(temp != NULL){
        printf(":%d", temp->data);
        temp = temp->next;
    }
    printf(" -> %d\n", best->fitness);
}

// Check if the new generation has a better chromosome than the best so far
void checkBestChromosome(CHROMOSOME* best, POPULATION* population) {
    if (population->firstChromosome->fitness < best->fitness) {
        updateBestChromosome(best, population);
    }
}

int main(int argc, char *argv[]) {
    // File paths
    char POPULATION_PATH[] = "population";
    char SELECTION_PATH[] = "selection";
    char XOVER_PATH[] = "xover";
    char MUTATION_PATH[] = "mutate";

    // Command line arguments
    int PROB_SIZE = atoi(argv[1]);
    int POP_SIZE = atoi(argv[2]);
    int MAX_GEN = atoi(argv[3]);

    // Load the GA data
    char** selections = charsToLines(readAllCharacters(SELECTION_PATH));
    char** xovers = charsToLines(readAllCharacters(XOVER_PATH));
    char** mutations = charsToLines(readAllCharacters(MUTATION_PATH));

    // Initialize the population
    struct population population;
    population.firstChromosome = NULL;
    population.lastChromosome = NULL;

    // Initialize the best chromosome
    struct chromosome bestChromosome;
    bestChromosome.next = NULL,
    bestChromosome.firstGene = NULL;
    bestChromosome.rank = -1;
    bestChromosome.fitness = -1;

    initializePopulation(&population, charsToLines(readAllCharacters(POPULATION_PATH)), POP_SIZE); // Load the initial population
    updateBestChromosome(&bestChromosome, &population); // Set the best chromosome since this is the initial generation
    // Display operations
    printf("GENERATION: %d\n", 0);
    outputPopulation(&population);
    displayBestChromosome(&bestChromosome);

    // Iterate
    int i ;
    for (i = 0; i < MAX_GEN; ++i) {
        xoverWrapper(xovers[i], selections[i], &population, POP_SIZE); // Perform xover to an entire generation
        mutate(atoi(mutations[i]), &population); // Perform mutation to an entire generation
        updatePopulation(&population); // Update population values according to the new GA operations
        checkBestChromosome(&bestChromosome, &population); // Check the best chromosome
        // Display operations
        printf("GENERATION: %d\n", i + 1);
        outputPopulation(&population);
        displayBestChromosome(&bestChromosome);
    }
    return 0;
}
