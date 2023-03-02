// Rosu Mihai Cosmin 333CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"

linked_list_t*
ll_create(unsigned int data_size)
{
    linked_list_t *ll = malloc(sizeof(linked_list_t));
    if (!ll) {
        fprintf(stderr, "%s\n", "Malloc failed");
        exit(0);
    }

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;
    return ll;
}

void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    if (!list) {
        return;
    }

    // Se aloca memorie pentru noul element si se copiaza datele in nod.
	ll_node_t *new = malloc(sizeof(ll_node_t));
	if(!new) {
        fprintf(stderr, "%s\n", "Malloc failed");
    }
    new->next = NULL;
    new->data = malloc(list->data_size);
    if(!(new->data)) {
        fprintf(stderr, "%s\n", "Malloc failed");
    }
    memcpy(new->data, new_data, list->data_size);

    if (n > list->size)
    	n = list->size;
    // Se verifica daca elementul trebuie pus pe prima pozitie.
    if (n == 0) {
    	ll_node_t *aux = list->head;
    	new->next = aux;
    	list->head = new;
    // Se verifica daca elementul trebuie pus pe ultima pozitie.
    } else if (n == list->size) {
    	ll_node_t *aux = list->head;

    	while (aux->next != NULL)
    		aux = aux->next;
    	aux->next = new;
    // Altfel, inseamna ca pozitia se afla undeva la mijlocul listei.
    } else {
    	unsigned int i = 1;
    	ll_node_t *prev, *curr;
    	prev = list->head;
    	curr = prev->next;

    	while (i < n) {
    		i++;
    		prev = prev->next;
    		curr = curr->next;
    	}
    	new->next = curr;
    	prev->next = new;
    }

    list->size++;
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    if (!list)
        return NULL;

    if (list->head == NULL) {
    	fprintf(stderr, "There are no nodes to remove.");
    	return NULL;
    }

    if (n >= list->size)
    	n = list->size - 1;

    // Se verifica daca elementul se afla pe prima pozitie.
    if (n == 0){
    	ll_node_t *aux = list->head;
    	list->head = aux->next;
    	list->size--;
    	return aux;
    }
    // Altfel trebuie cautat in lista.
    else {
    	unsigned int i = 1;
    	ll_node_t *prev, *curr;
    	prev = list->head;
    	curr = prev->next;

    	while (i < n){
    		i++;
    		prev = prev->next;
    		curr = curr->next;
    	}
    	prev->next = curr->next;
    	list->size--;
    	return curr;
    }
}

void
ll_free(linked_list_t* list)
{
    if (list == NULL) {
        return;
    }

    while (list->size > 0) {
        ll_node_t *curr = ll_remove_nth_node(list, 0);
        free(curr->data);
        free(curr);
    }

    free(list);
}