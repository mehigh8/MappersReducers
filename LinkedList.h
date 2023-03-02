// Rosu Mihai Cosmin 333CA
#ifndef __LINKED_LIST_H_
#define __LINKED_LIST_H_

typedef struct ll_node_t ll_node_t;
struct ll_node_t
{
    void* data;
    ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

// Functie folosita pentru crearea unei liste.
// data_size - dimensiunea datelor ce vor fi stocate in lista
// return - lista
linked_list_t*
ll_create(unsigned int data_size);

// Functie folosita pentru adaugarea unui element in lista.
// list - lista
// n - pozitia pe care sa fie adaugat elementul
// new_data - elementul ce trebuie adaugat
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data);

// Functie folosita pentru eliminarea unui element din lista.
// list - lista
// n - pozitia elementului ce trebuie eliminat
// return - elementul eliminat
ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n);

// Functie folosita pentru eliberarea memoriei listei.
// list - lista
void
ll_free(linked_list_t* list);

#endif /* __LINKED_LIST_H_ */