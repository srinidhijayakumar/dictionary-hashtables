#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

// Function prototype
int compare_words(const void *a, const void *b);

typedef struct Meaning {
    char definition[500];
    struct Meaning *next;
} Meaning;

typedef struct DictEntry {
    char *word;
    Meaning *meanings;
    struct DictEntry *next;
} DictEntry;

typedef struct Dictionary {
    DictEntry *table[TABLE_SIZE];
} Dictionary;

Dictionary *create_dictionary();
unsigned int hash_function(const char *word);
DictEntry *find_entry(DictEntry *entry, const char *word);
void insert_with_meaning(Dictionary *dict, const char *word, const char *meaning);
void search_and_display(Dictionary *dict, const char *word);
void display(Dictionary *dict);
void free_dictionary(Dictionary *dict);
void load_dataset(Dictionary *dict, const char *filename);

int main() {
    // Create a new dictionary
    Dictionary *dict = create_dictionary();

    // Load dataset from a file
    load_dataset(dict, "dataset.txt");

    int choice;
    char word[100];

    do {
        printf("\nMenu:\n");
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
                search_and_display(dict, word);
                break;

            case 2: {
                printf("Enter the word to add: ");
                scanf("%s", word);

                // Check if the word already exists
                DictEntry *entry = find_entry(dict->table[hash_function(word)], word);
                if (entry != NULL) {
                    printf("Word already exists in the dictionary.\n");
                    break;
                }

                // Add new word and meaning
                char new_meaning[500];
                printf("Enter the meaning for %s: ", word);
                getchar();  // Consume the newline character left by the previous scanf
                fgets(new_meaning, sizeof(new_meaning), stdin);
                new_meaning[strcspn(new_meaning, "\n")] = '\0';  // Remove trailing newline
                insert_with_meaning(dict, word, new_meaning);
                printf("Word added successfully.\n");
                break;
            }

            case 3:
                // Display entire dictionary in ascending order
                display(dict);
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

void load_dataset(Dictionary *dict, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char word[100];
    char meaning[500];

    while (fscanf(file, "%s %[^\n]", word, meaning) != EOF) {
        insert_with_meaning(dict, word, meaning);
    }

    fclose(file);
}

// Function to compare two words for sorting
int compare_words(const void *a, const void *b) {
    return strcmp((*(DictEntry **)a)->word, (*(DictEntry **)b)->word);
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

DictEntry *find_entry(DictEntry *entry, const char *word) {
    while (entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void insert_with_meaning(Dictionary *dict, const char *word, const char *meaning) {
    unsigned int index = hash_function(word);

    DictEntry *entry = dict->table[index];
    DictEntry *found_entry = find_entry(entry, word);

    if (found_entry != NULL) {
        // Word already exists, add meaning
        Meaning *new_meaning = (Meaning *)malloc(sizeof(Meaning));
        if (!new_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(new_meaning->definition, meaning);
        new_meaning->next = found_entry->meanings;
        found_entry->meanings = new_meaning;
    } else {
        // Word does not exist, create a new entry
        entry = (DictEntry *)malloc(sizeof(DictEntry));
        if (!entry) {
            fprintf(stderr, "Memory allocation error for dictionary entry.\n");
            exit(EXIT_FAILURE);
        }

        entry->word = strdup(word);
        Meaning *new_meaning = (Meaning *)malloc(sizeof(Meaning));
        if (!new_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(new_meaning->definition, meaning);
        new_meaning->next = NULL;
        entry->meanings = new_meaning;
        entry->next = dict->table[index];
        dict->table[index] = entry;
    }
}

void search_and_display(Dictionary *dict, const char *word) {
    unsigned int index = hash_function(word);
    DictEntry *entry = find_entry(dict->table[index], word);

    if (entry != NULL) {
        printf("%s:\n", entry->word);

        // Display meanings
        Meaning *meaning = entry->meanings;
        while (meaning != NULL) {
            printf("  - %s\n", meaning->definition);
            meaning = meaning->next;
        }
    } else {
        printf("Word not found: %s\n", word);
    }
}

void display(Dictionary *dict) {
    // Collect all entries for sorting
    DictEntry *entries[TABLE_SIZE * 10];  // Assuming a maximum of 10 entries per hash table slot
    int entry_count = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        DictEntry *entry = dict->table[i];
        while (entry != NULL) {
            entries[entry_count++] = entry;
            entry = entry->next;
        }
    }

    // Sort the entries
    qsort(entries, entry_count, sizeof(DictEntry *), compare_words);

    // Display sorted dictionary
    printf("Dictionary Contents (Sorted):\n");
    for (int i = 0; i < entry_count; i++) {
        printf("%s:\n", entries[i]->word);

        // Display meanings
        Meaning *meaning = entries[i]->meanings;
        while (meaning != NULL) {
            printf("  - %s\n", meaning->definition);
            meaning = meaning->next;
        }
    }
}

void free_dictionary(Dictionary *dict) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        DictEntry *entry = dict->table[i];
        while (entry != NULL) {
            DictEntry *next_entry = entry->next;
            Meaning *meaning = entry->meanings;
            while (meaning != NULL) {
                Meaning *next_meaning = meaning->next;
                free(meaning);
                meaning = next_meaning;
            }
            free(entry->word);
            free(entry);
            entry = next_entry;
        }
    }
    free(dict);
}
