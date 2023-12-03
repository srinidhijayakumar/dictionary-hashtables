#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

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

void display(Dictionary *dict) {
    printf("Dictionary Contents:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        DictEntry *entry = dict->table[i];
        while (entry != NULL) {
            printf("%s:\n", entry->word);

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

// Function to read dataset from a file and insert into the dictionary
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

int main() {
    // Create a new dictionary
    Dictionary *dict = create_dictionary();

    // Load dataset from a file
    load_dataset(dict, "dataset.txt");

    // Display the dictionary
    display(dict);

    // Search for a word
    char search_word[100];
    printf("Enter a word to search: ");
    scanf("%s", search_word);

    search_and_display(dict, search_word);

    // Free dictionary
    free_dictionary(dict);

    return 0;
}
