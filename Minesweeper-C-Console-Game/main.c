// C Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// Windows Support
#ifdef _WIN32
	#include <windows.h>
	bool windows_console_init(void){
    	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    	if (handle == INVALID_HANDLE_VALUE){
        	return false;
    	}
    	DWORD mode = 0;
    	if (!GetConsoleMode(handle, &mode)){
        	return false;
    	}
    	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    	if (!SetConsoleMode(handle, mode)){
        	return false;
    	}
    	return true;
	}
#endif

// My Library
#include "ulist.h"

// Compile-Time Define (BOARD CONFIG)
#define BOARD_COL 10
#define BOARD_ROW 10
#define BOARD_SIZE (BOARD_COL * BOARD_ROW)

// Compile-Time Define (Terminal/Console Color)
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_RESET "\033[0m"


// Board Render Function
void board_render(const char board_state[BOARD_COL][BOARD_ROW], const bool board_visibility[BOARD_COL][BOARD_ROW], const int8_t emoji_state){
	static char emoji0[] = "😀";
	static char emoji1[] = "🤔";
	static char emoji2[] = "🤕";
	printf("\n\n\t%s+------------+-------------+------------+\n", COLOR_MAGENTA);
	switch (emoji_state){
		case 0: 
			printf("\t|            | %sMINESWEEPER%s |     %s     |\n", COLOR_BLUE, COLOR_MAGENTA, emoji0);
			break;
		case 1:
			printf("\t|            | %sMINESWEEPER%s |     %s     |\n", COLOR_BLUE, COLOR_MAGENTA, emoji1);
			break;
		case 2:
			printf("\t|            | %sMINESWEEPER%s |     %s     |\n", COLOR_BLUE, COLOR_MAGENTA, emoji2);
			break;
	}    	
	printf("\t+------------+-------------+------------+\n");
	printf("%s", COLOR_YELLOW);
	for (size_t i = 0; i < BOARD_COL; i++){
		putchar('\t');
		for (size_t j = 0; j < BOARD_ROW; j++){
			printf("+---");
		}
		printf("+\n\t");
		for (size_t j = 0; j < BOARD_ROW; j++){
			if (board_visibility[i][j] == true){
				printf("| %s%c%s ", (board_state[i][j] == '*') ? COLOR_RED : COLOR_BLUE, board_state[i][j], COLOR_YELLOW);
			}
			else{
				printf("|   ");
			}
		}
		printf("|\n");
	}
	putchar('\t');
	for (size_t j = 0; j < BOARD_ROW; j++){
		printf("+---");
	}
	printf("+%s\n", COLOR_RESET);
}

// Get Input Function
void get_input(int* input, const bool cycle_check){
	if (cycle_check == false) printf("\t\t     %sPlayer input : %s", COLOR_MAGENTA, COLOR_BLUE);
	else printf("\t\t     %s:: Player input : %s", COLOR_MAGENTA, COLOR_BLUE);
	scanf("%d", input);
}

// Check Player Input Function 
bool check_input(const int input){
	if (input >= 1 && input <= BOARD_SIZE){
		return true;
	}
	return false;
}

// Input Value to Matrix Index Value
matrix_t convert_input(const int input){
	matrix_t __mtx__;
	__mtx__.col = (size_t) input / BOARD_COL;
	__mtx__.row = (size_t) input % BOARD_COL;
	return __mtx__;
}

// Matrix Index Check Function
bool check_index(const bool board_visibility[BOARD_COL][BOARD_ROW], const matrix_t matrix_index){
	if (board_visibility[matrix_index.col][matrix_index.row] == false){
		return true;
	}
	return false;
}

// Zone Check Function
bool check_safe_zone(bool board_visibility[BOARD_COL][BOARD_ROW], const unsigned int board_bomb_mapping[BOARD_COL], const size_t col){
	if (board_bomb_mapping[col] == 0){
		for (size_t i = 0; i < BOARD_ROW; i++){
			board_visibility[col][i] = true;
		}
		return true;
	}
	return false;
}

// Game Over Check Function
bool check_gameover(const char board_state[BOARD_COL][BOARD_ROW], const matrix_t matrix_index){
	if (board_state[matrix_index.col][matrix_index.row] == '*'){
		return true;
	}
	return false;
}

// Game Win Check Function
bool check_win(const bool board_visibility[BOARD_COL][BOARD_ROW], const ulist_t* const board_space_mapping){
	for (size_t i = 0; i < board_space_mapping->iterator; i++){
		matrix_t index = board_space_mapping->buffer[i];
		if (board_visibility[index.col][index.row] == false){
			return false;
		}
	}
	return true;
}

// Clear Display Function
void clear_display(void){
	#ifdef __linux__
		system("clear");
	#elif _WIN32
		system("cls");
	#endif
}

// Main C Function
int main(void){
	#ifdef _WIN32
		if (windows_console_init() == false) return 1;
	#endif
	clear_display();
	srand(time(NULL));

	// Heap Allocate
	ulist_t* board_space_mapping = ulist_create(1);
	if (!board_space_mapping) return 1;

	// Stack Allocate / Game Logical Matrix Data Structures
	char board_state[BOARD_COL][BOARD_ROW];
	bool board_visibility[BOARD_COL][BOARD_ROW];
	unsigned int board_bomb_mapping[BOARD_COL];
	int8_t emoji_state = 1;
	int input;

	// Game Logical Loop
	for (size_t i = 0; i < BOARD_COL; i++){
		board_bomb_mapping[i] = 0;
		for (size_t j = 0; j < BOARD_ROW; j++){
			board_state[i][j] = (char) (rand() % BOARD_COL) + '0';
			if (board_state[i][j] == '0'){
				++board_bomb_mapping[i];
				board_state[i][j] = '*';
			}
			else{
				board_state[i][j] = ' ';
				matrix_t space_index = {.col=i, .row=j};
				ulist_insert(board_space_mapping, space_index);
			}
			board_visibility[i][j] = false;
		}
	}

	// Board Configure Loop
	for (size_t i = 0; i < board_space_mapping->iterator; i++){
		matrix_t index = board_space_mapping->buffer[i]; 
		board_state[index.col][index.row] = (char) board_bomb_mapping[index.col] + '0';
	}
	
	// Game Loop
	for (size_t i = 0; i < BOARD_SIZE; i++){
		board_render(board_state, board_visibility, emoji_state);
		get_input(&input, false);	
		while (check_input(input) == false){
			get_input(&input, true);
			if (check_input(input) == true){
				break;
			}
		}
		matrix_t matrix_index = convert_input(input - 1);
		while (check_index(board_visibility, matrix_index) == false){
			get_input(&input, true);
			while (check_input(input) == false){
				get_input(&input, true);
				if (check_input(input) == true){
					break;
				}
			}
			matrix_index = convert_input(input - 1);
			if (check_index(board_visibility, matrix_index) == true){
				break;
			}
		}
		if (check_safe_zone(board_visibility, board_bomb_mapping, matrix_index.col) == true){
			emoji_state = 0;
			clear_display();
			continue;
		}
		board_visibility[matrix_index.col][matrix_index.row] = true;
		if (check_gameover(board_state, matrix_index) == true){
			emoji_state = 2;
			clear_display();
			board_render(board_state, board_visibility, emoji_state);
			printf("\t%s#############>%s [GAME OVER] %s<#############%s\n\n\n", COLOR_YELLOW, COLOR_RED, COLOR_YELLOW, COLOR_RESET);
			break;
		}
		if (check_win(board_visibility, board_space_mapping) == true){
			emoji_state = 0;
			clear_display();
			board_render(board_state, board_visibility, emoji_state);
			printf("\t%s################>%s [WIN] %s<################%s\n\n\n", COLOR_YELLOW, COLOR_GREEN, COLOR_YELLOW, COLOR_RESET);			
			break;
		}
		clear_display();	
	}
	
	ulist_delete(board_space_mapping);
	return 0;
}
