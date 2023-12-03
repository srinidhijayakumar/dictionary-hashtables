#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

// Structure to store meanings
typedef struct Meaning {
    char *definition;
    struct Meaning *next;
} Meaning;

// Structure for dictionary entry
typedef struct DictEntry {
    char *word;
    Meaning *meanings;
    struct DictEntry *next;
} DictEntry;

// Structure for the dictionary itself
typedef struct Dictionary {
    DictEntry *table[TABLE_SIZE];
} Dictionary;

// Function to calculate hash value for a word
unsigned int hash_function(const char *word) {
    unsigned int hash = 0;
    while (*word) {
        hash = (hash * 31) + *word;
        word++;
    }
    return hash % TABLE_SIZE;
}

// Function to find an entry in the dictionary
DictEntry *find_entry(DictEntry *entry, const char *word) {
    while (entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

// Function to create a new dictionary
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

// Function to free the dictionary and associated memory
void free_dictionary(Dictionary *dict) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        DictEntry *entry = dict->table[i];
        while (entry != NULL) {
            DictEntry *next_entry = entry->next;

            // Free word
            free(entry->word);

            // Free meanings
            Meaning *meaning = entry->meanings;
            while (meaning != NULL) {
                Meaning *next_meaning = meaning->next;
                free(meaning->definition);
                free(meaning);
                meaning = next_meaning;
            }

            // Free entry
            free(entry);

            entry = next_entry;
        }
    }
    free(dict);
}

// Function to insert a word with its meaning into the dictionary
void insert_with_meaning(Dictionary *dict, const char *word, const char *meaning) {
    unsigned int index = hash_function(word);

    // Check if the word already exists in the dictionary
    DictEntry *found_entry = find_entry(dict->table[index], word);

    if (found_entry != NULL) {
        // Word found, add new meaning
        // Create a new meaning node
        Meaning *new_meaning = (Meaning *)malloc(sizeof(Meaning));
        if (!new_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }
        new_meaning->definition = strdup(meaning);
        new_meaning->next = found_entry->meanings;
        found_entry->meanings = new_meaning;
    } else {
        // Word not found, create a new entry
        DictEntry *new_entry = (DictEntry *)malloc(sizeof(DictEntry));
        if (!new_entry) {
            fprintf(stderr, "Memory allocation error for dictionary entry.\n");
            exit(EXIT_FAILURE);
        }
        new_entry->word = strdup(word);

        // Create a new meaning node
        Meaning *new_meaning = (Meaning *)malloc(sizeof(Meaning));
        if (!new_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }
        new_meaning->definition = strdup(meaning);
        new_meaning->next = NULL;

        // Update entry with meaning
        new_entry->meanings = new_meaning;

        // Insert at the beginning of the linked list
        new_entry->next = dict->table[index];
        dict->table[index] = new_entry;
    }
}

// Function to display the dictionary with meanings
void display_with_meaning(Dictionary *dict) {
    printf("Dictionary Contents:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        DictEntry *entry = dict->table[i];
        while (entry != NULL) {
            printf("%s\n", entry->word);

            // Display meanings
            Meaning *meaning = entry->meanings;
            while (meaning != NULL) {
                printf("  - %s\n", meaning->definition);
                meaning = meaning->next;
            }

            entry = entry->next;
        }
    }
}

int main() {
    // Create a new dictionary
    Dictionary *dict = create_dictionary();

    // Example usage with meanings
    insert_with_meaning(dict, "hello", "Used as a greeting");
    insert_with_meaning(dict, "world", "The earth, together with all of its countries and peoples");
    insert_with_meaning(dict, "hello", "An expression of goodwill or friendliness");

    // Display the dictionary contents with meanings
    display_with_meaning(dict);

    // Free dictionary
    free_dictionary(dict);

    return 0;
}