typedef struct Node {
    int number; // number corresponding to count for head
    struct Node *next;
    struct Node *prev;
} Node;

void init_head(Node** head);

// Return 1 if list is empty and 0 else
int is_empty(Node** head);

// Add an element at start of the list
void insert_first(int number, Node** head);

// Remove an element from the list
void remove_element(Node** head, Node* element);

// Display all elements of the list
void display_list(Node** head);