#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int SIZE_X = 20;
const int SIZE_Y = 20;

const int BIRTH = 3;
const int DEATH = 4;


const int FILL = 40;

void print_map(int map[][SIZE_Y]);

int get_neighbors(int x, int y, int map[][SIZE_Y]);

void gen_map(int map[SIZE_X][SIZE_Y]);

void copy_map(int map[SIZE_X][SIZE_Y], int copy[][SIZE_Y]);

void smooth(int map[][SIZE_Y]);

int main(int argc, char **argv)
{
	clock_t start, end;
	unsigned int seed = 100;

	srand(seed);

	int map[SIZE_X][SIZE_Y];
	start = clock();
	
	gen_map(map);

	print_map(map);

	
	printf("map loc = %d ", &map);
	printf("\n\n");

	
	for(int i = 0; i < 10; i++){
		smooth(map);
	}
	print_map(map);
	end = clock();

	printf("time = %d %d\n", (end-start), CLOCKS_PER_SEC);
			//, (double) (start-end)/(double) CLOCKS_PER_SEC);
}

void print_map(int map[][SIZE_Y]){

	for( int y = 0; y < SIZE_Y; y++){
		for( int x = 0; x < SIZE_X; x++){
			printf(" %d ", map[x][y]);	
			//printf(" %d", get_neighbors(x,y,map));
		}
		printf("\n");
	}
}

int get_neighbors(int xLoc, int yLoc, int map[][SIZE_Y]){
	
	int count = 0;


	for(int x = xLoc-1; x <= xLoc+1; x++){
		for(int y = yLoc-1; y <= yLoc+1; y++){
			if ( x >= 0 && y >= 0 && x < SIZE_X && y < SIZE_Y ){
				if( map[x][y] == 0){
					if(x != xLoc || y != yLoc)
						count += 1;
				}
			} else
				count += 1;
		}
	}
	return count;

}

void smooth(int map[][SIZE_Y]){
	
	int copy[SIZE_X][SIZE_Y];
	copy_map(map, copy);
	

	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
			int nCount = get_neighbors(x,y, map);
			if( map[x][y] == 0 ){
				if (nCount < DEATH)
					copy[x][y] = -1;
			}if( map[x][y] == -1){
				if (nCount > BIRTH)
					copy[x][y] = 0;
			}
		}
	}

	copy_map(copy, map);
}

void copy_map(int map[SIZE_X][SIZE_Y], int copy[][SIZE_Y]){
	
	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
			copy[x][y] = map[x][y];	
		}
	}
	return;
}

void gen_map(int map[SIZE_X][SIZE_Y] ){

	for( int y = 0; y < SIZE_Y; y++){
		for(int x = 0; x < SIZE_X; x++){
			if( rand() % 100 > FILL)
				map[x][y] = 0;
			else
				map[x][y] = -1;
		}
	}
	
	return;
}

