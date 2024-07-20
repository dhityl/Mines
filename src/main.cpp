
#include <iostream>
#include <raylib.h>


#define ROWS 9
#define COLS 9
#define CELLSIZE 60

class Game{
    int cellSide;

    public:
    bool isRevealed;
    bool isFlagged;
    bool hasMine;
    int nearbyMines;

    Game(){
        cellSide = CELLSIZE;
        nearbyMines = 0;
        isRevealed = false;
        isFlagged = false;
        hasMine = false;
    }

    void toggleFlag(){
        isFlagged = !isFlagged;
    }


    void cellRevealed(){
        isRevealed = true;

        
        if(hasMine){ // lose condition
            
        }
    }

    void Draw(int cellx, int celly){
        DrawRectangleLines(cellx*cellSide,celly*cellSide, cellSide, cellSide, WHITE);


        if(isRevealed){
            if(hasMine){
                DrawRectangle(cellx*cellSide +5,celly*cellSide +5, cellSide -10, cellSide -10, RED);
            }


            else{
            DrawRectangle(cellx*cellSide +5,celly*cellSide +5, cellSide -10, cellSide -10, LIGHTGRAY);
            if(nearbyMines!=0){
                DrawText(TextFormat("%d", nearbyMines), cellx*cellSide + 10, celly*cellSide +10, 40, DARKGRAY);
            }
            
            }

        }

        else if(isFlagged){
            DrawRectangle(cellx*cellSide +5,celly*cellSide +5, cellSide -10, cellSide -10, BLUE);
        }

    }
};



int main()
{
    const int screenWidth = 540;
    const int screenHeight = 540;

    Game cell[ROWS][COLS];
    int minesToPlace = 10;
    int indexi, indexj;

    InitWindow(screenWidth, screenHeight, "Mines");
    SetTargetFPS(60);




    while(!WindowShouldClose()){
        BeginDrawing();

        
        // generate mines
        while (minesToPlace > 0){
            int i = rand() % ROWS;
            int j = rand() % COLS;
            if(!cell[i][j].hasMine){
                cell[i][j].hasMine = true;
                minesToPlace--;
            }
        }

        // flag cell on right click
        if(IsMouseButtonPressed(1)){
            indexi = GetMouseX()/60;
            indexj = GetMouseY()/60;
            if(!cell[indexi][indexj].isRevealed){
                cell[indexi][indexj].toggleFlag();
            }
        }

        // reveal cell on left click
        if(IsMouseButtonPressed(0)){
            indexi = GetMouseX()/60;
            indexj = GetMouseY()/60;
            if(!cell[indexi][indexj].isFlagged){
                cell[indexi][indexj].cellRevealed();
                cell[indexi][indexj].nearbyMines = 0 ;
            
                // count mines around the cell
                int mineCount = 0;
                if(!cell[indexi][indexj].hasMine){

                    for(int m = -1; m <= 1; m++){
                        for(int n = -1; n <= 1; n++){

                            int ni = indexi + n;
                            int mj = indexj + m;

                            if(ni < ROWS && ni >= 0 && mj < COLS && mj >= 0){    
                                if(cell[ni][mj].hasMine){
                                    mineCount++;
                                }
                            }
                        }
                    }
                    cell[indexi][indexj].nearbyMines = mineCount;
                }

            }

        }



        // draw 9x9 cells
        for(int i = 0; i < ROWS; i++){
            for(int j = 0; j < COLS; j++){
                cell[i][j].Draw(i, j);
            }
        }


        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

