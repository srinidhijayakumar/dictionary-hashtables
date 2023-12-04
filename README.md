# Dictionary Implementation in C

This C program implements a simple dictionary that allows users to store, search, and display words and their meanings. The dictionary uses a hash table data structure to efficiently manage word entries.

## Features

- **Search:** Users can search for a word in the dictionary and view its meanings.
- **Addition:** New words can be added to the dictionary along with their meanings.
- **Display:** The entire dictionary can be displayed in ascending order.

## Getting Started

### Prerequisites

- Ensure you have a C compiler installed on your system.

### Usage

1. Clone the repository:

    ```bash
    git clone https://github.com/yourusername/dictionary-c.git
    ```

2. Compile the program:

    ```bash
    cd dsa-c
    gcc -o dsa dsa.c
    ```

3. Run the executable:

    ```bash
    ./dsa
    ```

4. Follow the on-screen instructions to interact with the dictionary.

## Implementation Details

- **Hash Function:** The dictionary uses a basic hash function that calculates the hash value by multiplying the ASCII values of characters and applying modulo.

- **Collision Resolution:** The collision resolution methodology used is separate chaining. In case of a collision, a linked list is employed to handle multiple entries in the same hash bucket.

## Performance

- **Time Complexity:**
  - Search: O(1) on average (considering a good hash function and minimal collisions).
  - Addition: O(1) on average (considering a good hash function and minimal collisions).
  - Display: O(NlogN) for the entire dictionary, where N is the number of entries.

- **Space Complexity:** O(N + M), where N is the number of unique words, and M is the total number of meanings stored.

## File Structure

- `dsa.c`: Contains the main C code for the dictionary implementation.
- `english_dataset.txt`: Sample dataset for the English language.
- `tamil_dataset.txt`: Sample dataset for the Tamil language.
