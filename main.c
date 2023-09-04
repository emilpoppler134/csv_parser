#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ClearScreen printf("\033[H\033[J")

typedef struct Interaction
{
	char source[60];
	char target[60];
	char weight[10];
} Interaction;


// Declaring the data structure of the characters
typedef struct Relationship
{
	char name[60];
	char weight[10];
} Relationship;

typedef struct
{
	Relationship* data;
  int count;
  int capacity;
} dyn_relationship_array;

typedef struct Character
{
	char name[60];
	dyn_relationship_array relationships;
} Character;

typedef struct dyn_character_array
{
  Character* data;
  int count;
  int capacity;
} dyn_character_array;


// Declaring the initialization methods for the dynamic arrays
dyn_character_array init_character_array() {
  dyn_character_array arr;
  arr.data = (Character*)malloc(1 * sizeof(Character));
  arr.count = 0;
  arr.capacity = 1;

  return arr;
}

dyn_relationship_array init_relationship_array() {
  dyn_relationship_array arr;
  arr.data = (Relationship*)malloc(1 * sizeof(Relationship));
  arr.count = 0;
  arr.capacity = 1;

  return arr;
}


// Declaring the push methods for the dynamic arrays
void push_character(dyn_character_array *arr, Character item) {
  if (arr->count == arr->capacity) {
    arr->capacity *= 2;
    arr->data = (Character*)realloc(arr->data, arr->capacity * sizeof(Character));
  }
  arr->data[arr->count] = item;
  arr->count++;
}

void push_relationship(dyn_relationship_array *arr, Relationship item) {
  if (arr->count == arr->capacity) {
    arr->capacity *= 2;
    arr->data = (Relationship*)realloc(arr->data, arr->capacity * sizeof(Relationship));
  }
  arr->data[arr->count] = item;
  arr->count++;
}

// Method that checks if a character already exists in the character array
int array_includes(dyn_character_array arr, char name[60])
{
	for (int i = 0; i < arr.count; i++)
	{
		if (strcmp(arr.data[i].name, name) == 0)
		{
			return i;
		}
	}
	return -1;
}

// Method that parses a line to an interaction
Interaction parseToInteraction(char buffer[256])
{
	Interaction interaction; // Create an interaction

	char row[3][60]; // Declaring a array of strings to split up the values
	
	int index = 0;
	int row_index = 0;
	
	// Loop through all the characters on the line 
	for (int i = 0; i < strlen(buffer) && index < 3; i++) {
		if (buffer[i] == ',') {
			row[index][row_index] = '\0';
			index++; // Switching to the next in the row array
			row_index = 0;
		} else if (buffer[i] != '\n') {
			row[index][row_index] = buffer[i]; // Adding the char to the string
			row_index++;
		}
	}
	
	row[index][row_index] = '\0';

	// Copy the source, target and weight from the row array to the interaction struct
	strcpy(interaction.source, row[0]);
	strcpy(interaction.target, row[1]);
	strcpy(interaction.weight, row[2]);

	return interaction;
}


int main()
{
	// Reading the thrones csv
	FILE *file = fopen("thrones.csv", "r");
	
	if (file == NULL) {
		perror("Error opening file");
		return 1;
	}

	char buffer[256];

	// Create the character array
	dyn_character_array characters = init_character_array();
	
	// Loop through the lines in thrones.csv
	while (fgets(buffer, sizeof(buffer), file)) {
		// Parse the line to an interaction
		Interaction interaction = parseToInteraction(buffer);

		// Createing a relationship and copying from the interaction
		Relationship relationship;
		strcpy(relationship.name, interaction.target);
		strcpy(relationship.weight, interaction.weight);

		// Check if the character name already exists in the character array
		int array_index = array_includes(characters, interaction.source);

		if (array_index == -1) // if not, create a new character
		{
			// Creating a character and copying the source name
			Character character;
			strcpy(character.name, interaction.source);

			// Create a relationship array for the character
			dyn_relationship_array relationships = init_relationship_array();
			push_relationship(&relationships, relationship); // pushing the relationship to the array
			character.relationships = relationships;

			// Pushing the character to the character array
			push_character(&characters, character);
		} else { // else add a relationship to the caracter
			// Pushing the relationship to the relationship array
			push_relationship(&(characters.data[array_index].relationships), relationship);
		}
	}
	// Closeing the file after we are done with it
	fclose(file);	


	// User input
	printf("(L)ist: visa en lista på alla karaktärer\n");
	printf("(R)elationships: visa alla relationer för en viss karaktär\n");
	printf("(Q)uit: avsluta programmet\n");
	
	char choice;
	scanf("%c", &choice);

	switch (choice)
	{
	case 'l':
		for (int i = 0; i < characters.count; i++) {
			printf("%s\n", characters.data[i].name);
		}
		break;
	
	case 'r':
		for (int i = 0; i < characters.count; i++)
		{
			printf("%d - %s\n", i + 1, characters.data[i].name);
		}
		int character_index;
		printf("Vilken karaktär? ");
		scanf("%d", &character_index);
		Character character = characters.data[character_index - 1];

		printf("\n%s relationer\n", character.name);
		
		for (int j = 0; j < character.relationships.count; j++) {
			Relationship relationship = character.relationships.data[j];
			printf("  - Name: %s, weight: %s\n", relationship.name, relationship.weight);
		}
		break;

	default:
		break;
	}
	
	return 0;
}