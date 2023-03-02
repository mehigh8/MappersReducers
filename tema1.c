// Rosu Mihai Cosmin 333CA
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"

#define MAX_VALUE 2147483647

// Structura folosita pentru a retine fisierele ce trebuie procesate.
// files - caile tuturor fisierelor
// count - numarul total de fisiere
typedef struct files {
	char **files;
	int count;
} files_t;

// Structura folosita pentru a retine date necesare atat mapperilor, cat si reducerilor.
// files - fisierele ce necesita procesare
// current_file - indicele primului fisier ce inca nu a fost procesat
// mappers_lists - lista cu listele mapperilor
// wait_for_mappers - bariera folosita pentru a sincroniza reducerii cu mapperii
// pick_file - mutex folosit cand un mapper preia un fisier
// mappers - numarul de mapperi
// reducers - numarul de reduceri
typedef struct data {
	files_t *files;
	int current_file;
	linked_list_t ***mappers_lists;
	pthread_barrier_t wait_for_mappers;
	pthread_mutex_t pick_file;
	int mappers;
	int reducers;
} data_t;

// Structura folosita pentru a retine detele necesare unui thread.
// data - datele comune
// thread_id - indicele thread-ului
typedef struct thread_data {
	data_t *data;
	int thread_id;
} thread_data_t;

// Functie folosita pentru a retine caile fisierelor ce trebuie procesate.
// files_path - calea fisierului ce contine caile pentru restul fisierelor
// return - structura cu caile fisierelor si numarul lor
files_t *get_files(char *files_path)
{
	FILE *file = fopen(files_path, "r");

	if (!file) {
		fprintf(stderr, "Can't find %s\n", files_path);
		exit(0);
	}

	files_t *files = calloc(1, sizeof(files_t));

	fscanf(file, "%d\n", &files->count);
	files->files = calloc(files->count, sizeof(char *));

	for (int i = 0; i < files->count; i++) {
		char path[256];
		fscanf(file, "%s", path);

		if (path[strlen(path) - 1] == '\n')
			path[strlen(path) - 1] = '\0';

		files->files[i] = calloc(strlen(path) + 1, sizeof(char));
		strcpy(files->files[i], path);
	}

	fclose(file);
	return files;
}

// Functie folosita pentru a determina daca un numar este putere perfecta.
// number - numarul
// power - puterea pentru care se verifica
// return - 1 daca este putere perfecta, 0 altfel
int is_perfect_power(int number, int power)
{
	int left = 1;
	int right = (number + 1) / 2;

	// Folosesc cautare binara pentru a cauta baza, astfel incat baza ridicata
	// la putere sa fie egala cu numarul primit.
	while (left <= right) {
		int mij = (left + right) / 2;
		long long int result = mij;

		// Ridic numarul la putere.
		for (int i = 1; i < power; i++) {
			result *= mij;
			// Daca numarul depaseste valoarea maxima pentru int (chiar daca nu am terminat calculul)
			// inseamna ca este prea mare, deci nu mai continui calculul.
			if (result > MAX_VALUE) {
				break;
			}
		}

		if (result > number) {
			right = mij - 1;
		} else if (result < number) {
			left = mij + 1;
		} else {
			return 1;
		}
	}

	return 0;
}

// Functie folosita de catre thread-urile de tip mapper.
// arg - datele necesare thread-ului (in acest caz ele sunt de tip thread_data_t)
void *mapper_func(void *arg)
{
	// Se preiau datele.
	thread_data_t *thread_data = (thread_data_t *)arg;
	
	data_t *data = thread_data->data;
	int thread_id = thread_data->thread_id;

	// Incepe prelucrarea fisierelor. Se blocheaza mutex-ul pentru a alege un fisier.
	pthread_mutex_lock(&(data->pick_file));
	// Se verifica daca mai sunt fisiere de prelucrat.
	while (data->current_file < data->files->count) {
		// Se alege un fisier, si se incrementeaza current_file, ca celelalte thread-uri mapper
		// sa nu il aleaga pe acelasi.
		int file_id = data->current_file;
		(data->current_file)++;
		// Se deblocheaza mutex-ul, fiindca thread-ul si-a ales fisierul.
		pthread_mutex_unlock(&(data->pick_file));

		// Incepe prelucrarea.
		FILE *file = fopen(data->files->files[file_id], "r");
		if (!file) {
			fprintf(stderr, "Can't find %s\n", data->files->files[file_id]);
			exit(0);
		}

		int count;
		fscanf(file, "%d", &count);

		for (int i = 0; i < count; i++) {
			int current;
			fscanf(file, "%d", &current);
			// Pentru fiecare numar, se verifica daca este putere perfecta pentru fiecare
			// dintre puteri/reduceri.
			for (int j = 0; j < data->reducers; j++) {
				if (is_perfect_power(current, j + 2)) {
					ll_add_nth_node(data->mappers_lists[thread_id][j], 0, &current);
				}
			}
		}

		fclose(file);
		// Dupa ce se termina prelucrarea fisierului, va trebui sa se aleaga alt fisier,
		// deci se blocheaza din nou mutex-ul.
		pthread_mutex_lock(&(data->pick_file));
	}
	// Daca nu mai sunt fisiere se deblocheaza mutex-ul.
	pthread_mutex_unlock(&(data->pick_file));
	// Se asteapta la bariera pentru ca reducerii sa poata incepe.
	pthread_barrier_wait(&(data->wait_for_mappers));

	return NULL;
}

// Functie folosita pentru sortarea numerelor folosind functia qsort de catre reduceri.
// a - primul numar
// b - al doilea numar
// return - diferenta celor doua numere
int number_cmp(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}

// Functie folosita de catre thread-urile de tip reducer.
// arg - datele necesare thread-ului (in acest caz ele sunt de tip thread_data_t)
void *reducer_func(void *arg)
{
	// Se preiau datele.
	thread_data_t *thread_data = (thread_data_t *)arg;
	
	data_t *data = thread_data->data;
	int thread_id = thread_data->thread_id;
	int reducer_id = thread_id - data->mappers;
	
	// Se pregatesc fisierul in care va scrie reducer-ul.
	char path[10];
	sprintf(path, "out%d.txt", reducer_id + 2);
	FILE *file = fopen(path, "w");

	if (!file) {
		fprintf(stderr, "Can't create %s\n", path);
		exit(0);
	}

	// Se asteapta finalizarea mapperilor.
	pthread_barrier_wait(&(data->wait_for_mappers));

	// Incepe prelucrarea. Se calculeaza numarul total de numere ce sunt puteri perfecte.
	int total_entries = 0;
	for (int i = 0; i < data->mappers; i++) {
		total_entries += data->mappers_lists[i][reducer_id]->size;
	}

	int *entries = calloc(total_entries, sizeof(int));
	// Se copiaza toate numerele intr-un array.
	int arr_pos = 0;
	for (int i = 0; i < data->mappers; i++) {
		ll_node_t *curr = data->mappers_lists[i][reducer_id]->head;

		while (curr && curr->next) {
			entries[arr_pos++] = *(int *)curr->data;
			curr = curr->next;
		}
		if (curr) {
			entries[arr_pos++] = *(int *)curr->data;
		}	
	}
	// Array-ul este sortat folosind qsort.
	qsort(entries, total_entries, sizeof(int), number_cmp);

	// Se numara cate numere unice se afla in array.
	int final_count = (total_entries > 0 ? 1 : 0);
	for (int i = 1; i < total_entries; i++) {
		if (entries[i] != entries[i - 1]) {
			final_count++;
		}
	}
	// Se scrie in fisier si apoi este inchis fisierul si eliberata memoria array-ului.
	fprintf(file, "%d", final_count);

	fclose(file);
	free(entries);
	return NULL;
}

// Functie folosita pentru a elibera memoria alocata dinamic pe parcursul programului.
// data - datele folosite de thread-uri
void free_all(data_t *data)
{
	for (int i = 0; i < data->files->count; i++) {
		free(data->files->files[i]);
	}

	free(data->files->files);
	free(data->files);

	for (int i = 0; i < data->mappers; i++) {
		for (int j = 0; j < data->reducers; j++) {
			ll_free(data->mappers_lists[i][j]);
		}
		free(data->mappers_lists[i]);
	}
	
	free(data->mappers_lists);
	free(data);
}

int main(int argc, char **argv)
{
	if (argc < 4) {
		fprintf(stderr, "./tema1 <numar_mapperi> <numar_reduceri> <fisier_intrare>\n");
		return 0;
	}

	int mappers, reducers;
	// Se preiau numarul de mapperi si de reduceri.
	mappers = atoi(argv[1]);
	reducers = atoi(argv[2]);
	// Se preiau caile fisierelor.
	files_t *files = get_files(argv[3]);
	// Se aloca memorie pentru date.
	data_t *data = calloc(1, sizeof(data_t));
	data->files = files;

	data->mappers_lists = calloc(mappers, sizeof(linked_list_t **));
	for (int i = 0; i < mappers; i++) {
		data->mappers_lists[i] = calloc(reducers, sizeof(linked_list_t *));
		for (int j = 0; j < reducers; j++) {
			data->mappers_lists[i][j] = ll_create(sizeof(int));
		}
	}
	// Se creeaza bariera si mutex-ul.
	pthread_barrier_init(&(data->wait_for_mappers), NULL, mappers + reducers);
	pthread_mutex_init(&(data->pick_file), NULL);

	data->mappers = mappers;
	data->reducers = reducers;

	pthread_t threads[mappers + reducers];
	thread_data_t thread_data[mappers + reducers];
	// Se creeaza si lanseaza thread-urile.
	for (int i = 0; i < mappers + reducers; i++) {
		// Se pregatesc datele necesare thread-ului.
		thread_data[i].data = data;
		thread_data[i].thread_id = i;
		// Se verifica daca thread-ul curent este mapper sau reducer.
		if (i < mappers) {
			pthread_create(&threads[i], NULL, mapper_func, &thread_data[i]);
		} else {
			pthread_create(&threads[i], NULL, reducer_func, &thread_data[i]);
		}
	}
	// Se aplica join pe toate thread-urile.
	for (int i = 0; i < mappers + reducers; i++) {
		pthread_join(threads[i], NULL);
	}
	// Se distrug bariera si mutex-ul si se elibereaza memoria.
	pthread_barrier_destroy(&(data->wait_for_mappers));
	pthread_mutex_destroy(&(data->pick_file));
	free_all(data);

	return 0;
}