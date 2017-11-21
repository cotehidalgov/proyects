#include "../watcher/watcher.h"
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <gmp.h>

#define max(i, j) ((i) > (j) ? (i) : (j))

typedef enum {UP, DOWN,	LEFT, RIGHT} MoveType;

FILE* output_file;
int width;
int height;
int n_buttons = 0;
int n_blocks = 0;
CellType** puzzle;
int* blocks;
bool** state;
mpz_t key;
int M;
char* hash_string;
int valid_positions = 0;
char** steps;
HashTable* ht;

void open_file(char* file_name);
int pos_to_n(int row, int col);
void n_to_pos(int n, int* i, int* j);
const char* move_to_str(MoveType move);
void hash_state(mpz_t key);
void iddfs();
bool dfs(int limit);
bool posible(int row, int col, MoveType move);
void move_block(int row, int col, MoveType move, int block_index);
void unmove_block(int row, int col, int block_index);
bool is_solved();
void write_output(int limit);
void clear_memory();


void print_tablero(){
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			switch (puzzle[i][j]) {
				case ICE:
					if (state[i][j]) printf("B ");
					else printf("_ ");
					break;
				case ANTI_ICE:
				 	printf("# ");
					break;
				case WALL:
				  printf("X ");
					break;
				case BUTTON:
					if (state[i][j]) printf("B ");
					else printf("o ");
					break;
			}
		}
		printf("\n");
	}
}


int main(int argc, char *argv[])
{
	open_file(argv[1]);
	output_file = fopen(argv[2], "w");

	ht = create_table();

	iddfs();

	clear_memory();

	// const char* template = "iteracion_%d.png";
	//
	// /* Abrimos la interfaz grafica*/
	// watcher_open(width, height);
	//
	// for(int row = 0; row < height; row++)
	// {
	// 	for(int col = 0; col < width; col++)
	// 	{
	// 		watcher_set_cell_type(row, col, puzzle[row][col]);
	// 	}
	// }
	//
	// /* Indicamos la posicion del bloque de hielo */
	// watcher_cell_ice_block(ice_block_row, ice_block_col);
	//
	// watcher_snapshot("estado_inicial.png");
	//
	// int iteracion = 0;
	//
	// /* Movemos el bloque hacia arriba hasta que choque con algo */
	// while(puzzle[ice_block_row - 1][ice_block_col] != ANTI_ICE && puzzle[ice_block_row - 1][ice_block_col] != WALL)
	// {
	// 	sleep(1);
	// 	/* El bloque de hielo ya no esta ahí */
	// 	watcher_cell_empty_cell(ice_block_row, ice_block_col);
	// 	ice_block_row--;
	// 	/* Lo dibujamos en su nueva posicion */
	// 	watcher_cell_ice_block(ice_block_row, ice_block_col);
	//
	// 	char filename[64];
	// 	sprintf(filename, template, iteracion++);
	//
	// 	watcher_snapshot(filename);
	// }
	//
	// for(int row = 0; row < height; row++)
	// {
	// 	for(int col = 0; col < width; col++)
	// 	{
	// 		if(puzzle[row][col] == BUTTON)
	// 		{
	// 			if(ice_block_col != col || ice_block_row != row)
	// 			{
	// 				printf("Puzzle no resuelto :(\n");
	// 			}
	// 			else
	// 			{
	// 				printf("Puzzle resuelto :D!!\n");
	// 			}
	// 		}
	// 	}
	// }
	//
  // watcher_close();
	//
	// for(int row = 0; row < height; row++)
	// {
	// 	free(puzzle[row]);
	// }
	// free(puzzle);
	//
  return 0;
}

void open_file(char* file_name){
	FILE* file = fopen(file_name, "r");

	fscanf(file, "%i", &width) 																			? : exit(8);
	fscanf(file, "%i", &height) 																		? : exit(8);

	M = max(width, height);
	char buf;

	puzzle = malloc(sizeof(CellType*) * height);
	state = malloc(sizeof(bool*) * height);
	blocks = malloc(sizeof(int) * height * width);

  for(int row = 0; row < height; row++){
		puzzle[row] = malloc(sizeof(CellType) * width);
		state[row] = calloc(sizeof(bool), width);

	 	for(int col = 0; col < width; col++){
			fscanf(file, "%c", &buf)																		? : exit(8);
			if (buf == ' ' || buf == '\n') fscanf(file, "%c", &buf)			? : exit(8);
			switch (buf) {
				case '#':
					puzzle[row][col] = ANTI_ICE;
					break;
				case 'o':
					puzzle[row][col] = BUTTON;
					n_buttons ++;
					valid_positions++;
					break;
				case 'H':
					blocks[n_blocks++] = pos_to_n(row, col);
					state[row][col] = true;
				case '_':
					puzzle[row][col] = ICE;
					valid_positions++;
					break;
				case 'X':
					puzzle[row][col] = WALL;
					break;
			}
		}
 	}
	hash_string = malloc(sizeof(char) * valid_positions + 1);
	hash_string[valid_positions] = 0;
	fclose(file);
	mpz_init(key);
}

int pos_to_n(int row, int col){
	return row * M + col;
}

void n_to_pos(int n, int* i, int* j){
	*i = n/M;
	*j = n % M;
}

const char* move_to_str(MoveType move){
	switch (move) {
		case UP:
			return "U";
		case DOWN:
			return "D";
		case LEFT:
			return "L";
		case RIGHT:
			return "R";
	}
}

void hash_state(mpz_t key){ // Editará hash_string para el estado actual (dependiendo de la posicion de los bloques)
	int cont = 0;
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			if (puzzle[i][j] == ICE || puzzle[i][j] == BUTTON){
				if (state[i][j]){
					hash_string[cont] = '1';
				}
				else{
					hash_string[cont] = '0';
				}
				cont++;
			}
		}
	}
	mpz_set_str(key, hash_string, 2);
}

bool is_solved(){
	int cont = 0;
	int row, col;
	for (size_t i = 0; i < n_blocks; i++) {
		n_to_pos(blocks[i], &row, &col); // Ahora row y col tendrán la posicion actual del i-esimo bloque
		if (puzzle[row][col] == BUTTON) cont++;
	}
	return cont == n_buttons;
}

bool posible(int row, int col, MoveType move){
	if (move == UP){
		return (row + 1 < height && row - 1 >= 0 &&
			!state[row+1][col] && puzzle[row+1][col] != WALL &&
			(puzzle[row-1][col] == ICE || puzzle[row-1][col] == BUTTON) && !state[row-1][col]);
	}
	else if (move == DOWN){
		return (row + 1 < height && row - 1 >= 0 &&
			!state[row-1][col] && puzzle[row-1][col] != WALL &&
			(puzzle[row+1][col] == ICE || puzzle[row+1][col] == BUTTON) && !state[row+1][col]);
	}
	else if (move == RIGHT){
		return (col + 1 < width && col - 1 >= 0 &&
			!state[row][col-1] && puzzle[row][col-1] != WALL &&
		  (puzzle[row][col+1] == ICE || puzzle[row][col+1] == BUTTON) && !state[row][col+1]);
	}
	else if (move == LEFT){
		return (col + 1 < width && col - 1 >= 0 &&
			!state[row][col+1] && puzzle[row][col+1] != WALL &&
		  (puzzle[row][col-1] == ICE || puzzle[row][col-1] == BUTTON) && !state[row][col-1]);
	}
	return false;
}

void move_block(int row, int col, MoveType move, int block_index){
	if (move == UP){
		for (int i = row-1; i >= 0; i--) {
			if (puzzle[i][col] == WALL || puzzle[i][col] == ANTI_ICE || state[i][col]){
				state[row][col] = false;
				state[i+1][col] = true;
				blocks[block_index] = pos_to_n(i+1, col);
				break;
			}
		}
	}
	else if (move == DOWN){
		for (int i = row+1; i < height; i++) {
			if (puzzle[i][row] == WALL || puzzle[i][col] == ANTI_ICE || state[i][col]){
				state[row][col] = false;
				state[i-1][col] = true;
				blocks[block_index] = pos_to_n(i-1, col);
				break;
			}
		}
	}
	else if (move == RIGHT){
		for (int i = col+1; i < width; i++) {
			if (puzzle[row][i] == WALL || puzzle[row][i] == ANTI_ICE || state[row][i]){
				state[row][col] = false;
				state[row][i-1] = true;
				blocks[block_index] = pos_to_n(row, i-1);
				break;
			}
		}
	}
	else if (move == LEFT){
		for (int i = col-1; i >= 0; i--) {
			if (puzzle[row][i] == WALL || puzzle[row][i] == ANTI_ICE || state[row][i]){
				state[row][col] = false;
				state[row][i+1] = true;
				blocks[block_index] = pos_to_n(row, i+1);
				break;
			}
		}
	}
}

void unmove_block(int row, int col, int block_index){
	int i, j;
	n_to_pos(blocks[block_index], &i, &j);
	blocks[block_index] = pos_to_n(row, col);
	state[i][j] = false;
	state[row][col] = true;
}

bool dfs(int limit){
	if (limit == 0) return is_solved();

	else{
		int prev_limit;
		hash_state(key);
		if (hashtable_find(ht, key, &prev_limit) && prev_limit >= limit){ // Este estado ya existe Significa que ese estado no llegue al objetivo en muchos mas pasos
			return false;
		}
		else{
			hashtable_insert(ht, key, limit);
		}

		int row;
		int col;
		for (size_t block_index = 0; block_index < n_blocks; block_index++) { // Posibles bloques a mover
			n_to_pos(blocks[block_index], &row, &col);

			for (size_t move = 0; move < 4; move++) { // Posibles direcciones para ese bloque
				if (posible(row, col, move)){
					move_block(row, col, move, block_index);
					if (dfs(limit-1)) {
						steps[limit-1] = malloc(sizeof(char)*18);
						sprintf(steps[limit-1], "push %i %i %s", row, col, move_to_str(move));
						return true;
					}
					unmove_block(row, col, block_index);
				}
			}
		}
		return false;
	}
}


void iddfs(){
	int limit = 0;
	while (true) {
		steps = malloc(sizeof(char*) * limit);
		if (dfs(limit++)) break; // return limit, limit++
		free(steps);
	}
	write_output(limit);
}

void write_output(int limit){
	fprintf(output_file, "%d\n", limit-1);
	for (int i = limit-2; i >= 0; i--) {
		fprintf(output_file, "%s\n", steps[i]);
		free(steps[i]);
	}
	free(steps);
	printf("Puzzle resuelto :D\n");
	fclose(output_file);
}

void clear_memory(){
	for (size_t i = 0; i < height; i++) {
		free(puzzle[i]);
		free(state[i]);
	}

	free(puzzle);
	free(state);
	free(blocks);
	free(hash_string);
	mpz_clear(key);
	destroy_table(ht);
}
