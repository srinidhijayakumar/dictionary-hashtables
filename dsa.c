#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct Meaning {
    char definition[500];
    struct Meaning *next;
} Meaning;

typedef struct Translation {
    char word[100];
    Meaning *meanings;
    struct Translation *next;
} Translation;

typedef struct Dictionary {
    Translation *table[TABLE_SIZE];
} Dictionary;

// Function prototypes
Dictionary *create_dictionary();
unsigned int hash_function(const char *word);
Translation *find_translation(Translation *translation, const char *word);
void insert_with_meaning(Dictionary *dict, const char *word, const char *meaning, const char *language);
void search_and_display(Dictionary *dict, const char *word, const char *language);
void display(Dictionary *dict, int language_choice);
void free_dictionary(Dictionary *dict);
void load_dataset(Dictionary *dict, const char *filename, const char *language);
int compare_translations(const void *a, const void *b);
int get_language_choice();
int is_typing_in_chosen_language(const char *word, const char *language);

int main() {
    // Create a new dictionary
    Dictionary *dict = create_dictionary();

    // Get user's language choice
    int language_choice = get_language_choice();

    // Load dataset from a file for the chosen language
    load_dataset(dict, (language_choice == 1) ? "english_dataset.txt" : "tamil_dataset.txt", 
                 (language_choice == 1) ? "english" : "tamil");

    int choice;
    char word[100];

    do {
        // Display menu based on the language choice
        printf("\nMenu (%s):\n", (language_choice == 1) ? "English" : "Tamil");
        printf("1. Search for a word\n");
        printf("2. Add a word\n");
        printf("3. Display entire dictionary in ascending order\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter the word to search: ");
                scanf("%s", word);
                if (is_typing_in_chosen_language(word, (language_choice == 1) ? "english" : "tamil")) {
                    search_and_display(dict, word, (language_choice == 1) ? "english" : "tamil");
                } else {
                    printf("You are typing in the wrong language. Please type in %s.\n",
                           (language_choice == 1) ? "English" : "Tamil");
                }
                break;

            case 2: {
                printf("Enter the word to add: ");
                scanf("%s", word);
                if (is_typing_in_chosen_language(word, (language_choice == 1) ? "english" : "tamil")) {
                    // Check if the word already exists
                    Translation *translation = find_translation(dict->table[hash_function(word)], word);
                    if (translation != NULL) {
                        printf("Word already exists in the dictionary.\n");
                        break;
                    }

                    // Add new word and meaning
                    char new_meaning[500];
                    printf("Enter the meaning for %s: ", word);
                    getchar();  // Consume the newline character left by the previous scanf
                    fgets(new_meaning, sizeof(new_meaning), stdin);
                    new_meaning[strcspn(new_meaning, "\n")] = '\0';  // Remove trailing newline
                    insert_with_meaning(dict, word, new_meaning, (language_choice == 1) ? "english" : "tamil");
                    printf("Word added successfully.\n");
                } else {
                    printf("You are typing in the wrong language. Please type in %s.\n",
                           (language_choice == 1) ? "English" : "Tamil");
                }
                break;
            }

            case 3:
                // Display entire dictionary in ascending order
                display(dict, language_choice);
                break;

            case 4:
                // Exit
                break;

            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
        }

    } while (choice != 4);

    // Free dictionary
    free_dictionary(dict);

    return 0;
}

// Function to compare two translations for sorting
int compare_translations(const void *a, const void *b) {
    return strcmp((*(Translation **)a)->word, (*(Translation **)b)->word);
}

// Function to get the language choice from the user
int get_language_choice() {
    int language_choice;
    do {
        printf("Select Language:\n");
        printf("1. English\n");
        printf("2. Tamil\n");
        printf("Enter your choice: ");
        scanf("%d", &language_choice);
    } while (language_choice != 1 && language_choice != 2);

    return language_choice;
}

Dictionary *create_dictionary() {
    Dictionary *dict = (Dictionary *)malloc(sizeof(Dictionary));
    if (!dict) {
        fprintf(stderr, "Memory allocation error for dictionary.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        dict->table[i] = NULL;
    }

    return dict;
}

unsigned int hash_function(const char *word) {
    unsigned int hash = 0;
    while (*word) {
        hash = (hash * 31) + *word;
        word++;
    }
    return hash % TABLE_SIZE;
}

Translation *find_translation(Translation *translation, const char *word) {
    while (translation != NULL) {
        if (strcmp(translation->word, word) == 0) {
            return translation;
        }
        translation = translation->next;
    }
    return NULL;
}

void insert_with_meaning(Dictionary *dict, const char *word, const char *meaning, const char *language) {
    unsigned int index = hash_function(word);

    Translation *translation = dict->table[index];
    Translation *found_translation = find_translation(translation, word);

    if (found_translation != NULL) {
        // Word already exists, add meaning
        Meaning *new_meaning = (Meaning *)malloc(sizeof(Meaning));
        if (!new_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(new_meaning->definition, meaning);
        new_meaning->next = found_translation->meanings;
        found_translation->meanings = new_meaning;
    } else {
        // Word does not exist, create a new translation
        translation = (Translation *)malloc(sizeof(Translation));
        if (!translation) {
            fprintf(stderr, "Memory allocation error for translation.\n");
            exit(EXIT_FAILURE);
        }

        strcpy(translation->word, word);
        Meaning *new_meaning = (Meaning *)malloc(sizeof(Meaning));
        if (!new_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(new_meaning->definition, meaning);
        new_meaning->next = NULL;
        translation->meanings = new_meaning;
        translation->next = dict->table[index];
        dict->table[index] = translation;
    }
}

void search_and_display(Dictionary *dict, const char *word, const char *language) {
    unsigned int index = hash_function(word);
    Translation *translation = find_translation(dict->table[index], word);

    if (translation != NULL) {
        printf("%s (%s):\n", translation->word, language);

        // Display meanings
        Meaning *meaning = translation->meanings;
        while (meaning != NULL) {
            printf("  - %s\n", meaning->definition);
            meaning = meaning->next;
        }
    } else {
        printf("Word not found: %s\n", word);
    }
}

void display(Dictionary *dict, int language_choice) {
    // Collect all translations for sorting
    Translation *translations[TABLE_SIZE * 10];  // Assuming a maximum of 10 translations per hash table slot
    int translation_count = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        Translation *translation = dict->table[i];
        while (translation != NULL) {
            translations[translation_count++] = translation;
            translation = translation->next;
        }
    }

    // Sort the translations
    qsort(translations, translation_count, sizeof(Translation *), compare_translations);

    // Display sorted dictionary
    printf("Dictionary Contents (Sorted):\n");
    for (int i = 0; i < translation_count; i++) {
        printf("%s (%s):\n", translations[i]->word, (language_choice == 1) ? "english" : "tamil");

        // Display meanings
        Meaning *meaning = translations[i]->meanings;
        while (meaning != NULL) {
            printf("  - %s\n", meaning->definition);
            meaning = meaning->next;
        }
    }
}

void free_dictionary(Dictionary *dict) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Translation *translation = dict->table[i];
        while (translation != NULL) {
            Translation *next_translation = translation->next;
            Meaning *meaning = translation->meanings;
            while (meaning != NULL) {
                Meaning *next_meaning = meaning->next;
                free(meaning);
                meaning = next_meaning;
            }
            free(translation);
            translation = next_translation;
        }
    }
    free(dict);
}

void load_dataset(Dictionary *dict, const char *filename, const char *language) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char word[100];
    char meaning[500];

    while (fscanf(file, "%s %[^\n]", word, meaning) != EOF) {
        insert_with_meaning(dict, word, meaning, language);
    }

    fclose(file);
}

int is_typing_in_chosen_language(const char *word, const char *language) {
    // Check if the entered word is in the chosen language
    return (strstr(word, language) != NULL);
}
