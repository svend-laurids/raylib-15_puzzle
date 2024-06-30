#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
float Clip(float value, float min, float max);
typedef struct STRUCT_BOARD
{
	//CONSTRUCTOR
	int gridwidth; //Width of the drawn image in pixels 
	int gridheight; //Height of the drawn image in pixels
	int gridcolumns; //Number of columns, which the image is split into
	int gridrows; //Number of grids, which the image is split into
	//INITIALIZED UPON CONSTRUCTION
	int gridsize; //gridwidth*gridheight
	int tilewidth; //Width of indivdiual tile
	int tileheight; //Height of individual tile
	int emptytileindex; //Index of the empty tile, SYMBOLIZED IN THE GRID ARRAY BY VALUE 0!
	int* grid; //The index of the grid determines the position of the tiles themselves which are the values
}Board;
Board* InitBoard(float gridwidth, float gridheight, int gridcolumns, int gridrows); //CONSTRUCTOR
void PRINTBOARD(Board* board) // FOR DEVELOPMENT
{
	for(int i = 0; i < board->gridsize; i++)
	{
		printf("board[%i] : %i\n", i, board->grid[i]);
	};
};
void FreeBoard(Board* board);
int GetGridColumnFromIndex(Board* board, int tileindex); //Actually a mod function
int GetGridRowFromIndex(Board* board, int tileindex); //Actually a div function
int GetGridIndex(Board* board, int tilecolumn, int tilerow);
int GetGridIndexFromPixels(Board* board, Vector2 coordinate);
void EmptyTileSwitch(Board* board, int targetindex);
typedef enum ENUM_DIRECTION
{
	NONE = 0,
	UP,
	RIGHT,
	DOWN,
	LEFT,
} Direction;
Direction GetTileIndexFromDirection(Board* board, Direction direction); 
Direction EmptyTileDirection(Board* board, int tileindex); 
void MoveEmptyTileInDirection(Board* board, Direction direction); 
Direction RandomDirection();
Direction RandomMove(Board* board);
typedef struct STRUCT_SCRAMBLESEQUENCE
{
	int length;
	int currentdirection;
	Direction* direction;
} Scramblesequence;
//Scramblesequence* CreateScrambleSequence(Board* board, int repetitions);
Scramblesequence* BoardScramble(Board* board, int repetitions);
Vector2 TileMovingOffset(Board* board, Direction direction, Vector2 initialposition, Vector2 currentposition);
Vector2 TileMovingOffsetAnimation(Board* board, Direction direction, float tileoffsetpercentage);
typedef struct STRUCT_BOARDIMAGE
{
	Image image;
	Texture texture;
	int imagetilewidth;
	int imagetileheight;
} Boardimage;
Boardimage* InitBoardimage(Board* board, char* imagesrc);
void FreeBoardimage(Boardimage* boardimage);
typedef struct STRUCT_CONTROLS
{
	int selectedgridindex;	
	Vector2 mouseposition;
	Vector2 mouseclickposition;
	Direction emptytiledirection;
}Controls;
typedef enum ENUM_SCREEN
{
	MENU,
	SCRAMBLE,
	GAME,
	ENDING,	
} Screen;
typedef struct STRUCT_ANIMATOR
{
	int framesperaction; 
	float animationspeedhorizontal; //Value between 0 and 1vb
	float animationspeedvertical;
	float offset;
	int currentframe;
} Animator;
Animator InitAnimator(Board* board, int framesperaction);
void DrawTile(Board* board, Boardimage* boardimage, int index, Vector2 offset);
int main()
{
	Board* board = InitBoard(800, 600, 4, 4);
	InitWindow(board->gridwidth, board->gridheight, "test");
	Boardimage* boardimage = InitBoardimage(board, "assets/image.png");

	Vector2 ZERO_VECTOR = (Vector2){ .x = 0.0, .y = 0.0 };

	Scramblesequence* scramblesequence = BoardScramble(board, 50);

	Board* board2 = InitBoard(800, 600, 4, 4);
	
	Animator animator = InitAnimator(board, 5);

	Controls controls;
	Screen screen = SCRAMBLE;

	SetTargetFPS(60);

	while(!WindowShouldClose())
	{

		//INPUT
		switch(screen)
		{
			case MENU:
			{
				
			} break;
			case SCRAMBLE:
			{
				
				controls.selectedgridindex = GetTileIndexFromDirection(board2, scramblesequence->direction[scramblesequence->currentdirection]);

				if(animator.framesperaction-animator.currentframe != 0)
				{

					if(scramblesequence->direction[scramblesequence->currentdirection] == UP || scramblesequence->direction[scramblesequence->currentdirection] == DOWN)
					{
						animator.offset += animator.animationspeedvertical;
					}	
					if(scramblesequence->direction[scramblesequence->currentdirection] == RIGHT || scramblesequence->direction[scramblesequence->currentdirection] == LEFT)
					{
						animator.offset += animator.animationspeedhorizontal;
					}	
					animator.currentframe++;

				}
				else
				{

					MoveEmptyTileInDirection(board2, scramblesequence->direction[scramblesequence->currentdirection]);
					scramblesequence->currentdirection++;
					animator.currentframe = 0;
					animator.offset = 0;

				}

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { screen = GAME; };
				if(scramblesequence->currentdirection == scramblesequence->length) { screen = GAME; };

			} break;
			case GAME:
			{

				controls.mouseposition = GetMousePosition();

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{

					controls.mouseclickposition = GetMousePosition();
					controls.selectedgridindex = GetGridIndexFromPixels(board, controls.mouseclickposition);
					controls.emptytiledirection = EmptyTileDirection(board, controls.selectedgridindex);

				}
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
				{

					int mousereleasegridindex = GetGridIndexFromPixels(board, controls.mouseposition);
					if(controls.emptytiledirection != NONE && mousereleasegridindex == board->emptytileindex)
					{
						EmptyTileSwitch(board, controls.selectedgridindex);
					};

					controls.selectedgridindex = -1;
				}
			} break;
			case ENDING:
			{

			} break;
			default: break;
		}
		//DRAWING

		ClearBackground(RAYWHITE);
		BeginDrawing();

		switch(screen)
		{
			case MENU:
			{

			} break;
			case SCRAMBLE:
			{


				for(int i = 0; i < board2->gridsize; i++)
				{
					if(i == board2->emptytileindex)
					{
						continue;
					}
					else if(i == controls.selectedgridindex)
					{

						Direction direction = scramblesequence->direction[scramblesequence->currentdirection];
						Vector2 offset = TileMovingOffsetAnimation(board2, direction, animator.offset);
						DrawTile(board2, boardimage, i, offset);
						continue;
					}
					else
					{

						DrawTile(board2, boardimage, i, ZERO_VECTOR);		
					}
				};

			} break;
			case GAME:
			{

				for(int i = 0; i < board->gridsize; i++)
				{
					if(i == board->emptytileindex)
					{
						continue;
					}
					else if(i == controls.selectedgridindex)
					{

						Vector2 offset = TileMovingOffset(board, controls.emptytiledirection, controls.mouseclickposition, controls.mouseposition);
						DrawTile(board, boardimage, i, offset);
						continue;
					}
					else
					{

						DrawTile(board, boardimage, i, ZERO_VECTOR);		
					}
				};

			} break;
			case ENDING:
			{

			} break;
			default: break;
		}

		EndDrawing();

	}
	
	FreeBoard(board);
	FreeBoardimage(boardimage);
	CloseWindow();

	return 0;
}
float Clip(float value, float min, float max)
{
	if(value < min) { return min; }
	else if(value > max) { return max; }
	else { return value; };
};
Board* InitBoard(float gridwidth, float gridheight, int gridcolumns, int gridrows) //CONSTRUCTOR
{
	Board* board = (Board*)(malloc(sizeof(int)*8 + sizeof(Rectangle) + sizeof(int*)));
	board->gridwidth = gridwidth;
	board->gridheight = gridheight;
	board->gridcolumns = gridcolumns;
	board->gridrows = gridrows;
	board->tilewidth = gridwidth/gridcolumns;
	board->tileheight = gridheight/gridrows;
	board->gridsize = gridcolumns*gridrows;
	board->emptytileindex = 0;
	board->grid = (int*)(malloc(sizeof(int)*board->gridsize));
	for(int i = 0; i < board->gridsize; i++) { board->grid[i] = i; };
	return board;
};
void FreeBoard(Board* board)
{
	free(board->grid);
	free(board);
};
int GetGridColumnFromIndex(Board* board, int tileindex) //Actually a mod function
{
	int i = 0;
	int remainder = tileindex;
	while(remainder-board->gridcolumns >= 0) { remainder -= board->gridcolumns; };
	return remainder; 
}; 
int GetGridRowFromIndex(Board* board, int tileindex) //Actually a div function
{
	int row = 0;
	while(board->gridcolumns*(row+1)-tileindex <= 0) { row++; };
	return row;
};
int GetGridIndex(Board* board, int tilecolumn, int tilerow)
{
	return (tilecolumn + tilerow*board->gridcolumns);
};
int GetGridIndexFromPixels(Board* board, Vector2 coordinate)
{
	int column = 0;
	int row = 0;
	while(board->tilewidth*(column+1)-coordinate.x < 0) { column++; };
	while(board->tileheight*(row+1)-coordinate.y < 0) { row++; };
	return GetGridIndex(board, column, row);
};
void EmptyTileSwitch(Board* board, int targetindex)
{
	board->grid[board->emptytileindex] = board->grid[targetindex];
	board->grid[targetindex] = 0;
	board->emptytileindex = targetindex;
};
Direction GetTileIndexFromDirection(Board* board, Direction direction)
{
	switch(direction)
	{
		case UP: return board->emptytileindex-board->gridcolumns;			
		case RIGHT: return board->emptytileindex+1;			
		case DOWN: return board->emptytileindex+board->gridcolumns;			
		case LEFT: return board->emptytileindex-1;			
		default: return NONE;
	}
};
Direction EmptyTileDirection(Board* board, int targetindex)
{ 
	int up = targetindex-board->gridcolumns;
	int right = targetindex+1;
	int down = targetindex+board->gridcolumns;
	int left = targetindex-1;
	if(board->emptytileindex == up) { return UP; }
	else if(board->emptytileindex == right) { return RIGHT; }
	else if(board->emptytileindex == down) { return DOWN; }
	else if(board->emptytileindex == left) { return LEFT; }
	else { return NONE; };
};
void MoveEmptyTileInDirection(Board* board, Direction direction)
{
	switch(direction)
	{
		case UP:
		{
			EmptyTileSwitch(board, board->emptytileindex-board->gridcolumns);
		} break;
		case RIGHT:
		{
			EmptyTileSwitch(board, board->emptytileindex+1);
		} break;
		case DOWN:
		{
			EmptyTileSwitch(board, board->emptytileindex+board->gridcolumns);
		} break;
		case LEFT:
		{
			EmptyTileSwitch(board, board->emptytileindex-1);
		} break;
		default: break;
	}
}
Direction RandomMove(Board* board)
{
	Direction randomdirection = (Direction)((rand() % 4) + 1);
	switch(randomdirection)
	{
		case UP:
		{
			if(GetGridRowFromIndex(board, board->emptytileindex)-1 >= 0)
			{
				EmptyTileSwitch(board, board->emptytileindex-board->gridcolumns);
				return UP;
			}
			else
			{
				EmptyTileSwitch(board, board->emptytileindex+board->gridcolumns);
				return DOWN;
			}
		} break;
		case RIGHT:
		{
			if(GetGridColumnFromIndex(board, board->emptytileindex)+1 < board->gridcolumns)
			{
				EmptyTileSwitch(board, board->emptytileindex+1);
				return RIGHT;
			}
			else
			{
				EmptyTileSwitch(board, board->emptytileindex-1);
				return LEFT;
			}
		} break;
		case DOWN:
		{ 
			if(GetGridRowFromIndex(board, board->emptytileindex)+1 < board->gridrows)
			{
				EmptyTileSwitch(board, board->emptytileindex+board->gridcolumns);
				return DOWN;
			}
			else
			{
				EmptyTileSwitch(board, board->emptytileindex-board->gridcolumns);
				return UP;
			}
		} break;
		case LEFT:
		{
			if(GetGridColumnFromIndex(board, board->emptytileindex)-1 >= 0)
			{
				EmptyTileSwitch(board, board->emptytileindex-1);
				return LEFT;
			}
			else
			{
				EmptyTileSwitch(board, board->emptytileindex+1);
				return RIGHT;
			}
		} break;
		default: break;
	}
	return NONE;
};
Scramblesequence* BoardScramble(Board* board, int repetitions)
{
	Scramblesequence* scramblesequence = (Scramblesequence*)malloc(sizeof(int)*2 + sizeof(Direction*)*repetitions);	
	scramblesequence->currentdirection = 0;
	scramblesequence->length = repetitions;
	scramblesequence->direction = (Direction*)malloc(sizeof(Direction)*repetitions);
	for(int i = 0; i < repetitions; i++)
	{
		scramblesequence->direction[i] = RandomMove(board);
	}
	return scramblesequence;
};
Vector2 TileMovingOffset(Board* board, Direction direction, Vector2 initialposition, Vector2 currentposition)
{
	Vector2 offset;
	switch(direction)
	{
		case UP:
		{
			float x = 0.0;
			float y = Clip(initialposition.y-currentposition.y, 0.0, (float)(board->tileheight));
			offset = (Vector2){ .x = x, .y = y };
		} break;
		case RIGHT:
		{
			float x = Clip(initialposition.x-currentposition.x, -(float)(board->tilewidth), 0.0);
			float y = 0.0;
			offset = (Vector2){ .x = x, .y = y };
		} break;
		case DOWN:
		{
			float x = 0.0;
			float y = Clip(initialposition.y-currentposition.y, -(float)board->tileheight, 0.0);
			offset = (Vector2){ .x = x, .y = y };
		} break;
		case LEFT:
		{
			float x = Clip(initialposition.x-currentposition.x, 0.0, (float)board->tilewidth);
			float y = 0.0;
			offset = (Vector2){ .x = x, .y = y };
		} break;
		default:
		{
		 	offset = (Vector2){ .x = 0.0, .y = 0.0 };
		} break;
	}
	return offset;
};
Vector2 TileMovingOffsetAnimation(Board* board, Direction direction, float f_offset)
{
	Vector2 offset;
	switch(direction)
	{
		case UP:
		{
			float x = 0.0;
			float y = -f_offset;
			offset = (Vector2){ .x = x, .y = y };
		} break;
		case RIGHT:
		{
			float x = f_offset;
			float y = 0.0;
			offset = (Vector2){ .x = x, .y = y };
		} break;
		case DOWN:
		{
			float x = 0.0;
			float y = f_offset;
			offset = (Vector2){ .x = x, .y = y };
		} break;
		case LEFT:
		{
			float x = -f_offset;
			float y = 0.0;
			offset = (Vector2){ .x = x, .y = y };
		} break;
		default:
		{
		 	offset = (Vector2){ .x = 0.0, .y = 0.0 };
		} break;
	}
	return offset;
}
Boardimage* InitBoardimage(Board* board, char* imagesrc)
{

	Boardimage* boardimage = (Boardimage*)(malloc(sizeof(Image) + sizeof(Texture) + sizeof(int)*2));
	boardimage->image = LoadImage(imagesrc);
	boardimage->texture = LoadTextureFromImage(boardimage->image);
	boardimage->imagetilewidth = boardimage->image.width/board->gridcolumns;
	boardimage->imagetileheight = boardimage->image.height/board->gridrows;
	return boardimage;
};
void FreeBoardimage(Boardimage* boardimage)
{
	UnloadImage(boardimage->image);
	free(boardimage);
};
Animator InitAnimator(Board* board, int framesperaction)
{
	Animator animator;
	animator.framesperaction = framesperaction;
	animator.animationspeedvertical = board->tileheight/framesperaction;	
	animator.animationspeedhorizontal = board->tilewidth/framesperaction;
	animator.offset = 0.0;
	animator.currentframe = 0;
	return animator;
}
void DrawTile(Board* board, Boardimage* boardimage, int index, Vector2 offset)
{
	int tilecolumn = GetGridColumnFromIndex(board, index);
	int tilerow = GetGridRowFromIndex(board, index);
	int tilex = tilecolumn*board->tilewidth;
	int tiley = tilerow*board->tileheight;
	Rectangle tilerectangle = (Rectangle){ .x = tilex, .y = tiley, .width = board->tilewidth, .height = board->tileheight };

	int imagecolumn = GetGridColumnFromIndex(board, board->grid[index]);
	int imagerow = GetGridRowFromIndex(board, board->grid[index]);
	int imagex = imagecolumn*boardimage->imagetilewidth;
	int imagey = imagerow*boardimage->imagetileheight;
	Rectangle imagerectangle = (Rectangle){ .x = imagex, .y = imagey, .width = boardimage->imagetilewidth, .height = boardimage->imagetileheight };

	DrawTexturePro(boardimage->texture, imagerectangle, tilerectangle, offset, 0.0, WHITE); 
};
