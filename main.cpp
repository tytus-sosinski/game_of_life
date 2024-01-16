#include <iostream>
#include <ncursesw/curses.h>
#include <cmath>
#include <string>
#include <stdlib.h>
#include <time.h>


const int pop_size = 64;

class coord{
	public:
		int y,x;
};

class pop{
	public:
		int current,total;
};

void ncurses_init_colors(){

	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_GREEN);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
}

void ncurses_config(){
	ncurses_init_colors();
	keypad(stdscr, TRUE);
	timeout(50);
	noecho();
}

void timeout_settings(){
	timeout(-1);
	move(1,2);printw("Choose interval between generations:");
	move(2,2);printw("1 = 50ms");
	move(3,2);printw("2 = 100ms");
	move(4,2);printw("3 = 150ms");
	move(5,2);printw("4 = 200ms");
	move(6,2);printw("5 = 250ms");
	move(7,2);printw("6 = 300ms");
	move(8,2);printw("7 = 350ms");
	move(9,2);printw("8 = 400ms");
	move(10,2);printw("9 = 450ms");
	move(11,2);printw("0 = 500ms");
	move(12,2);printw("waiting for input...");
	int input = getch();
	if(input == 48)
		timeout(500);
	else
		timeout((input-48)*50);
	clear();
		
}

void print_cursor(coord coord){
	move(coord.y+1, coord.x*2+2);
	attron(COLOR_PAIR(1));
	printw("<>");
	attroff(COLOR_PAIR(1));
}

void print_border(){
	attron(COLOR_PAIR(3));
	move(0,0);printw("//");
	for(int i=0;i<pop_size;++i)
		printw("==");
	printw("\\\\");
	
	for(int i=0;i<pop_size;++i){
		move(i+1,0);printw("||");
		move(i+1,pop_size*2+2);printw("||");
	}
	
	move(pop_size+1,0);printw("\\\\");
	for(int i=0;i<pop_size;++i)
		printw("==");
	printw("//");
	attroff(COLOR_PAIR(3));
}

void print_statistics(int population[pop_size][pop_size], int& gen_number, pop& pop){
	int num = 123;

	
	move(1,6+2*pop_size);printw("Current simulation number: ");printw("%d",gen_number);
	move(2,6+2*pop_size);printw("Current population size  : ");printw("%d",pop.current);
	move(3,6+2*pop_size);printw("Total population         : ");printw("%d",pop.total);
	move(5,6+2*pop_size);printw("press \'z\' to open instructions...");
}

void print_instructions(){
	timeout(-1);
	move(1,2);printw("Instructions:");
	move(2,2);printw("input == \'q\' : change state of a cell");
	move(3,2);printw("input == \'a\' : begin simulation");
	move(4,2);printw("input != \'a\' : stop simulation");
	move(5,2);printw("input == UP/DOWN/LEFT/RIGHT : move around the board");
	move(6,2);printw("input == \'b\' : generate random board");
	move(7,2);printw("input == \'z\' : display this screen");
	move(8,2);printw("press any key to set interval...");
	getch();
	clear();
	timeout_settings();

}

void print_blueprint(int population[pop_size][pop_size], int& gen_number, pop& pop){
	srand(time(NULL));
	gen_number = 1;
	pop.current = 0;
	pop.total = 0;
	
	for(int i=0;i<pop_size;++i){
		for(int j=0;j<pop_size;++j){
			int x = rand() % 2;
			if(x==1){
				population[i][j] = 1;
				pop.current++;
				pop.total++;
			}
		}
	}

	
}

void print_population(int population[pop_size][pop_size]){
	for(int i=0;i<pop_size;++i){
		for(int j=0;j<pop_size;++j){
			move(i+1,j*2+2);
			if(population[i][j]==1){
				attron(COLOR_PAIR(2));printw("xx");attroff(COLOR_PAIR(2));
			}
		}
	}
}

int get_neighbors(int population[pop_size][pop_size], int y, int x){
	int value = 0;
	for(int i=-1;i<2;++i){
		for(int j=-1;j<2;++j){
			if(population[y+i][x+j]==1&&y+i>=0&&x+j>=0&&y+i<=pop_size-1&&x+j<=pop_size-1){
					value++;
			}
		}
	}
	if(population[y][x]==1)
		return value-1;
	else
		return value;
}

void change_population(coord coord, int population[pop_size][pop_size], char last_character, pop& pop){
	if(last_character=='q'){
		int cell = population[coord.y][coord.x];
		if(cell==1){
			population[coord.y][coord.x]=0;
			pop.current--;
		}
		else{
			population[coord.y][coord.x]=1;	
			pop.current++;
			pop.total++;		
		}

	}
	
	if(last_character=='a'){
		int temp[pop_size][pop_size] = {};
		for(int i=0;i<pop_size;++i)
			for(int j=0;j<pop_size;++j)
				temp[i][j]=population[i][j];
				
		for(int i=0;i<pop_size;++i){
			for(int j=0;j<pop_size;++j){
				
				int value = get_neighbors(population, i, j);
				int status = population[i][j];
				
				if(status==1&&(value<2||value>3)){
					temp[i][j]=0;
					pop.current--;
				}
				else if(status==0&&value==3){
					temp[i][j]=1;
					pop.current++;
					pop.total++;
				}
	
			}
		}
		for(int i=0;i<pop_size;++i)
			for(int j=0;j<pop_size;++j)
				population[i][j]=temp[i][j];
		refresh();		
	}
}

void print_board(coord coord, char last_character, int population[pop_size][pop_size], int& gen_number, pop& pop){
	clear();
	if(last_character=='z')
		print_instructions();
	if(last_character=='b')
		print_blueprint(population, gen_number, pop);
	print_border();
	print_population(population);
	print_statistics(population, gen_number, pop);
	print_cursor(coord);
	change_population(coord, population, last_character, pop);
	if(last_character=='a')
		gen_number++;
	move(0,0);
	refresh();
}

int main(void) {

	WINDOW * mainwin = initscr();
	ncurses_config();
	char last_character;
	int population[pop_size][pop_size] = {};
	coord coord;
	coord.y = 0;
	coord.x = 0;
	int gen_number = 1;
	pop pop;
	pop.current = 0;
	pop.total = 0;
	while(1==1){
		if(last_character!='a')
			last_character=' ';
	
		int input = getch();
		if(input != ERR)
			if(input == KEY_UP && coord.y != 0)
				--coord.y;
			else if(input == KEY_DOWN && coord.y != pop_size-1)
				++coord.y;
			else if(input == KEY_LEFT && coord.x != 0)
				--coord.x;
			else if(input == KEY_RIGHT && coord.x != pop_size-1)
				++coord.x;
			else
				last_character = input;
		
		print_board(coord, last_character, population, gen_number, pop);		
	}
	

	delwin(mainwin);
	endwin();
	refresh();
	return EXIT_SUCCESS;
}
