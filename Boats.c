#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOATS 120
#define NAME_LENGTH 128

// Enum for boat location type
typedef enum {
	SLIP,
	LAND,
	TRAILOR,
	STORAGE
} BoatLocationType;

// Union for location-specific details
typedef union {
	int slipNumber;        // Slip number 1-85
	char bayLetter;        // Bay A-Z
	char trailorTag[16];   // Trailor license tag
	int storageNumber;     // Storage number 1-50
} LocationDetails;

// Structure for storing boat information
typedef struct {
	char name[NAME_LENGTH];     
	int length;              
	BoatLocationType locationType;   
	LocationDetails locationDetails; 
	float amountOwed;          
} Boat;

// Array of boat pointers
Boat* marina[MAX_BOATS];
int numberOfBoats = 0;


// Function to determine boat location type from string
BoatLocationType getLocationType(const char *location) {
	if (strcmp(location, "slip") == 0) {
		return SLIP;
	} else if (strcmp(location, "land") == 0) {
		return LAND;
	} else if (strcmp(location, "trailor") == 0) {
		return TRAILOR;
	} else if (strcmp(location, "storage") == 0) {
		return STORAGE;
	}
	return SLIP; 
}

// Function to sort boats alphabetically by name
void sortMarina() {
    for (int i = 0; i < numberOfBoats - 1; i++) {
        for (int j = 0; j < numberOfBoats - i - 1; j++) {
            if (strcmp(marina[j]->name, marina[j + 1]->name) > 0) {
                // Swap boats
                Boat* temp = marina[j];
                marina[j] = marina[j + 1];
                marina[j + 1] = temp;
            }
        }
    }
}

//function that displays the inventory
void displayInventory() {
	printf("\n");
	for (int i = 0; i < numberOfBoats; i++) {
		if (marina[i] != NULL) {
			printf("%-20s %d' ", marina[i]->name, marina[i]->length);
			switch (marina[i]->locationType) {
			case SLIP:
				printf("   slip   # %2d   ", marina[i]->locationDetails.slipNumber);
				break;
			case LAND:
				printf("   land      %c   ", marina[i]->locationDetails.bayLetter);
				break;
			case TRAILOR:
				printf("trailor %s   ", marina[i]->locationDetails.trailorTag);
				break;
			case STORAGE:
				printf("storage   # %2d   ", marina[i]->locationDetails.storageNumber);
				break;
			}
			printf("Owes $%8.2f\n", marina[i]->amountOwed);
		}
	}
	printf("\n");
}
//function that adds a boat to the system
void addBoat(char* boatData) {
	if (numberOfBoats >= MAX_BOATS) {
		printf("Marina is full!\n");
		return;
	}

	Boat* newBoat = (Boat*)malloc(sizeof(Boat));
	if (!newBoat) {
		printf("Memory allocation failed!\n");
		return;
	}

	char *token = strtok(boatData, ",");
	strncpy(newBoat->name, token, 127);

	token = strtok(NULL, ",");
	newBoat->length = atof(token);

	token = strtok(NULL, ",");
	if (strcmp(token, "slip") == 0) {
		newBoat->locationType = SLIP;
		token = strtok(NULL, ",");
		newBoat->locationDetails.slipNumber = atoi(token);
	} else if (strcmp(token, "land") == 0) {
		newBoat->locationType = LAND;
		token = strtok(NULL, ",");
		newBoat->locationDetails.bayLetter = token[0];
	} else if (strcmp(token, "trailor") == 0) {
		newBoat->locationType = TRAILOR;
		token = strtok(NULL, ",");
		strncpy(newBoat->locationDetails.trailorTag, token, 9);
	} else if (strcmp(token, "storage") == 0) {
		newBoat->locationType = STORAGE;
		token = strtok(NULL, ",");
		newBoat->locationDetails.storageNumber = atoi(token);
	}

	token = strtok(NULL, ",");
	newBoat->amountOwed = atof(token);

	marina[numberOfBoats++] = newBoat;
	sortMarina();
}
// Function to remove a boat by name
void removeBoat(char* boatName) {
	for (int i = 0; i < numberOfBoats; i++) {
		if (marina[i] != NULL && strcasecmp(marina[i]->name, boatName) == 0) {
			printf("%s removed from the Marina\n",marina[i]->name);
			free(marina[i]);
			marina[i] = marina[numberOfBoats - 1];  // Move the last boat to the current position
			marina[numberOfBoats - 1] = NULL;
			numberOfBoats--;
			sortMarina();
			return;
		}
	}
	printf("No boat with that name\n");
}
//function to process payment
void processPayment(char* boatName, float payment) {
	for (int i = 0; i < numberOfBoats; i++) {
		if (marina[i] != NULL && strcasecmp(marina[i]->name, boatName) == 0) {
			if (payment > marina[i]->amountOwed) {
				printf("That is more than the amount owed, $%.2f\n", marina[i]->amountOwed);
			} else {
				marina[i]->amountOwed -= payment;
				printf("Payment processed. New balance: $%.2f\n", marina[i]->amountOwed);
			}
			return;
		}
	}
	printf("No boat with that name\n");
}

// Function to update monthly charges
void updateMonthlyCharges() {
	for (int i = 0; i < numberOfBoats; i++) {
		if (marina[i] != NULL) {
			switch (marina[i]->locationType) {
			case SLIP:
				marina[i]->amountOwed += marina[i]->length * 12.5;
				break;
			case LAND:
				marina[i]->amountOwed += marina[i]->length * 14.0;
				break;
			case TRAILOR:
				marina[i]->amountOwed += marina[i]->length * 25.0;
				break;
			case STORAGE:
				marina[i]->amountOwed += marina[i]->length * 11.2;
				break;
			}
		}
	}
	printf("Monthly charges updated.\n");
}
void scanInput() {
    char option, buffer[256], boatName[128];
    float payment;
    printf("Welcome to the Boat Management System\n-------------------------------------\n");

    do {
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        scanf(" %c", &option);

        switch (option) {
            case 'I':
            case 'i':
                displayInventory();
                break;

            case 'A':
            case 'a':
                printf("Please enter the boat data in CSV format                 : ");
                scanf(" %[^\n]", buffer);  // Reads a line of input (CSV format)
                addBoat(buffer);
                break;

            case 'R':
            case 'r':
                printf("Please enter the boat name                               : ");
                scanf(" %[^\n]", boatName);
                removeBoat(boatName);
                break;

            case 'P':
            case 'p':
                printf("Please enter the boat name                               : ");
                scanf(" %[^\n]", boatName);
                printf("Please enter the amount to be paid                       : ");
                scanf("%f", &payment);
                processPayment(boatName, payment);
                break;

            case 'M':
            case 'm':
                updateMonthlyCharges();
                break;

            case 'X':
            case 'x':
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid option %c\n", option);
        }
    } while (option != 'X' && option != 'x');
}


// Function to load boat data from a CSV file
void loadBoatData(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[256];
	int boatIndex = 0;

	// Read each line of the file
	while (fgets(line, sizeof(line), file)) {
		addBoat(line);
	}

	fclose(file);
}

// Function to save boat data to a CSV file
void saveBoatData(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numberOfBoats; i++) {
        if (marina[i] != NULL) {
            fprintf(file, "%s,%d,", marina[i]->name, marina[i]->length);

            switch (marina[i]->locationType) {
            case SLIP:
                fprintf(file, "slip,%d,", marina[i]->locationDetails.slipNumber);
                break;
            case LAND:
                fprintf(file, "land,%c,", marina[i]->locationDetails.bayLetter);
                break;
            case TRAILOR:
                fprintf(file, "trailor,%s,", marina[i]->locationDetails.trailorTag);
                break;
            case STORAGE:
                fprintf(file, "storage,%d,", marina[i]->locationDetails.storageNumber);
                break;
            }

            fprintf(file, "%.2f\n", marina[i]->amountOwed);
        }
    }

    fclose(file);
    printf("Boat data saved to %s.\n", filename);
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Use BoatData.csv as argument");
		return 1;
	}

	// Load boat data from the provided CSV file
	loadBoatData(argv[1]);
	sortMarina(marina,numberOfBoats);
	
	//let the user interact with the program
	scanInput();
	
	//save the changes to the CSV file
	saveBoatData(argv[1]);


	return 0;
}


