#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct Meaning {
    char tamil_definition[500];
    char english_definition[500];
    struct Meaning *next;
} Meaning;

typedef struct DictEntry {
    char *tamil_word;
    char *english_word;
    Meaning *meanings;
    struct DictEntry *next;
} DictEntry;

typedef struct Dictionary {
    DictEntry *table[TABLE_SIZE];
} Dictionary;

// Function prototypes
Dictionary *create_dictionary();
unsigned int hash_function(const char *word);
DictEntry *find_entry(DictEntry *entry, const char *word, int lang);
void insert_with_meaning(Dictionary *dict, const char *tamil_word, const char *english_word, const char *tamil_meaning, const char *english_meaning);
void search_and_display(Dictionary *dict);
void display(Dictionary *dict);
void free_dictionary(Dictionary *dict);
void load_dataset(Dictionary *dict, const char *filename);
int compare_words(const void *a, const void *b);

int main() {
    // Create a new dictionary
    Dictionary *dict = create_dictionary();

    // Load dataset from a file
    load_dataset(dict, "dataset.txt");

    int choice;
    char word[100];
    int lang_choice;

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
                printf("Choose language for search:\n");
                printf("1. Tamil\n");
                printf("2. English\n");
                printf("Enter your choice: ");
                scanf("%d", &lang_choice);

                printf("Enter the word to search: ");
                scanf("%s", word);

                search_and_display(dict, word, lang_choice);
                break;

            case 2: {
                printf("Choose language for adding a word:\n");
                printf("1. Tamil\n");
                printf("2. English\n");
                printf("Enter your choice: ");
                scanf("%d", &lang_choice);

                printf("Enter the word to add: ");
                scanf("%s", word);

                // Check if the word already exists
                DictEntry *entry = find_entry(dict->table[hash_function(word)], word, lang_choice);
                if (entry != NULL) {
                    printf("Word already exists in the dictionary.\n");
                    break;
                }

                // Add new word and meaning
                char new_tamil_meaning[500];
                char new_english_meaning[500];

                printf("Enter the meaning in Tamil: ");
                getchar();  // Consume the newline character left by the previous scanf
                fgets(new_tamil_meaning, sizeof(new_tamil_meaning), stdin);
                new_tamil_meaning[strcspn(new_tamil_meaning, "\n")] = '\0';  // Remove trailing newline

                printf("Enter the meaning in English: ");
                fgets(new_english_meaning, sizeof(new_english_meaning), stdin);
                new_english_meaning[strcspn(new_english_meaning, "\n")] = '\0';  // Remove trailing newline

                insert_with_meaning(dict, word, word, new_tamil_meaning, new_english_meaning);
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

// Function to compare two words for sorting
int compare_words(const void *a, const void *b) {
    return strcmp((*(DictEntry **)a)->tamil_word, (*(DictEntry **)b)->tamil_word);
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

DictEntry *find_entry(DictEntry *entry, const char *word, int lang) {
    while (entry != NULL) {
        if (lang == 1 && strcmp(entry->tamil_word, word) == 0) {
            return entry;
        } else if (lang == 2 && strcmp(entry->english_word, word) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void insert_with_meaning(Dictionary *dict, const char *tamil_word, const char *english_word, const char *tamil_meaning, const char *english_meaning) {
    unsigned int index = hash_function(tamil_word);

    DictEntry *entry = dict->table[index];
    DictEntry *found_entry = find_entry(entry, tamil_word, 1);

    if (found_entry != NULL) {
        // Word already exists, add meaning
        Meaning *new_tamil_meaning = (Meaning *)malloc(sizeof(Meaning));
        Meaning *new_english_meaning = (Meaning *)malloc(sizeof(Meaning));

        if (!new_tamil_meaning || !new_english_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }

        strcpy(new_tamil_meaning->tamil_definition, tamil_meaning);
        new_tamil_meaning->next = found_entry->meanings;
        found_entry->meanings = new_tamil_meaning;

        strcpy(new_english_meaning->english_definition, english_meaning);
        new_english_meaning->next = found_entry->meanings;
        found_entry->meanings = new_english_meaning;
    } else {
        // Word does not exist, create a new entry
        entry = (DictEntry *)malloc(sizeof(DictEntry));
        if (!entry) {
            fprintf(stderr, "Memory allocation error for dictionary entry.\n");
            exit(EXIT_FAILURE);
        }

        entry->tamil_word = strdup(tamil_word);
        entry->english_word = strdup(english_word);

        Meaning *new_tamil_meaning = (Meaning *)malloc(sizeof(Meaning));
        Meaning *new_english_meaning = (Meaning *)malloc(sizeof(Meaning));

        if (!new_tamil_meaning || !new_english_meaning) {
            fprintf(stderr, "Memory allocation error for meaning.\n");
            exit(EXIT_FAILURE);
        }

        strcpy(new_tamil_meaning->tamil_definition, tamil_meaning);
        new_tamil_meaning->next = NULL;
        entry->meanings = new_tamil_meaning;

        strcpy(new_english_meaning->english_definition, english_meaning);
        new_english_meaning->next = NULL;
        entry->meanings = new_english_meaning;

        entry->next = dict->table[index];
        dict->table[index] = entry;
    }
}

void search_and_display(Dictionary *dict, const char *word, int lang) {
    unsigned int index = hash_function(word);
    DictEntry *entry = find_entry(dict->table[index], word, lang);

    if (entry != NULL) {
        if (lang == 1) {
            printf("Tamil: %s\n", entry->tamil_word);
            printf("Meanings:\n");
            // Display Tamil meanings
            Meaning *tamil_meaning = entry->meanings;
            while (tamil_meaning != NULL) {
                printf("  - %s\n", tamil_meaning->tamil_definition);
                tamil_meaning = tamil_meaning->next;
            }
        } else {
            printf("English: %s\n", entry->english_word);
            printf("Meanings:\n");
            // Display English meanings
            Meaning *english_meaning = entry->meanings;
            while (english_meaning != NULL) {
                printf("  - %s\n", english_meaning->english_definition);
                english_meaning = english_meaning->next;
            }
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
        printf("Tamil: %s, English: %s\n", entries[i]->tamil_word, entries[i]->english_word);

        // Display Tamil meanings
        printf("Meanings in Tamil:\n");
        Meaning *tamil_meaning = entries[i]->meanings;
        while (tamil_meaning != NULL) {
            printf("  - %s\n", tamil_meaning->tamil_definition);
            tamil_meaning = tamil_meaning->next;
        }

        // Display English meanings
        printf("Meanings in English:\n");
        Meaning *english_meaning = entries[i]->meanings;
        while (english_meaning != NULL) {
            printf("  - %s\n", english_meaning->english_definition);
            english_meaning = english_meaning->next;
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
            free(entry->tamil_word);
            free(entry->english_word);
            free(entry);
            entry = next_entry;
        }
    }
    free(dict);
}

void load_dataset(Dictionary *dict, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char tamil_word[100];
    char english_word[100];
    char tamil_meaning[500];
    char english_meaning[500];

    while (fscanf(file, "%s %s %[^\n] %[^\n]", tamil_word, english_word, tamil_meaning, english_meaning) != EOF) {
        insert_with_meaning(dict, tamil_word, english_word, tamil_meaning, english_meaning);
    }

    fclose(file);
}

