#include <iostream>
#include <raylib.h>

#define ROWS 9
#define COLS 9
#define CELLSIZE 60

bool lost = false;

char loseMessage[] = "YOU LOST";
char winMessage[] = "YOU WON!";
int revealedCells = 0;

class Game{
    int cellSide, cellx, celly;

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

    void revealCell(){
        isRevealed = true;

        if (hasMine){ // lose condition
            lost = true;
        }
    }

    void Draw(int x, int y){
        cellx = x;
        celly = y;
        DrawRectangleLines(cellx * cellSide, celly * cellSide, cellSide, cellSide, WHITE);

        if (isRevealed){
            if (hasMine){
                DrawRectangle(cellx * cellSide + 5, celly * cellSide + 5, cellSide - 10, cellSide - 10, RED);
            } 
            else{
                DrawRectangle(cellx * cellSide + 5, celly * cellSide + 5, cellSide - 10, cellSide - 10, LIGHTGRAY);
                if (nearbyMines != 0) {
                    DrawText(TextFormat("%d", nearbyMines), cellx * cellSide + 10, celly * cellSide + 10, 40, DARKGRAY);
                }
            }

        } 
        else if(isFlagged){
            DrawRectangle(cellx * cellSide + 5, celly * cellSide + 5, cellSide - 10, cellSide - 10, BLUE);
        }
    }
};

Game cell[ROWS][COLS];

void revealEmptyCells(int i, int j){
    // return if cell out of range
    if (i < 0 || i >= ROWS || j < 0 || j >= COLS || cell[i][j].isRevealed || cell[i][j].hasMine) {
        return;
    }
    cell[i][j].revealCell();
    revealedCells++;

    if (cell[i][j].nearbyMines == 0){
        for (int m = -1; m <= 1; m++) {
            for (int n = -1; n <= 1; n++){
                if (m != 0 || n != 0) {
                    revealEmptyCells(i + m, j + n);
                }
            }
        }
    }
}

void countMines(){
    for (int i = 0; i < ROWS; i++){
        for (int j = 0; j < COLS; j++){
            if (!cell[i][j].hasMine){
                int mineCount = 0;
                for (int m = -1; m <= 1; m++){
                    for (int n = -1; n <= 1; n++){
                        int ni = i + n;
                        int mj = j + m;
                        if (ni >= 0 && ni < ROWS && mj >= 0 && mj < COLS && cell[ni][mj].hasMine){
                            mineCount++;
                        }
                    }
                }
                cell[i][j].nearbyMines = mineCount;
            }
        }
    }
}

void placeMines(int minesToPlace){
    while (minesToPlace > 0){
        int i = GetRandomValue(0, ROWS - 1);
        int j = GetRandomValue(0, COLS - 1);
        if (!cell[i][j].hasMine){
            cell[i][j].hasMine = true;
            minesToPlace--;
        }
    }
}

void revealAll(){
    for (int i = 0; i < ROWS; i++){
        for (int j = 0; j < COLS; j++){
            cell[i][j].revealCell();
        }
    }  
}


int main() {
    const int screenWidth = 540;
    const int screenHeight = 540;

    int indexi, indexj;
    int minesToPlace = 10;
    int flaggedMines = 0;


    InitWindow(screenWidth, screenHeight, "Mines");
    SetTargetFPS(60);

    // Place mines and count nearby mines
    placeMines(minesToPlace);
    countMines();

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        // flag cell on right click
        if (IsMouseButtonPressed(1)){
            indexi = GetMouseX() / CELLSIZE;
            indexj = GetMouseY() / CELLSIZE;
            if (!cell[indexi][indexj].isRevealed){
                cell[indexi][indexj].toggleFlag();
            }

            if(cell[indexi][indexj].hasMine){
                if(cell[indexi][indexj].isFlagged){
                    flaggedMines++;
                }
                else if(!cell[indexi][indexj].isFlagged){
                    flaggedMines--;
                }
                
            }
        }

        // reveal cell on left click
        if (IsMouseButtonPressed(0)){
            indexi = GetMouseX() / CELLSIZE;
            indexj = GetMouseY() / CELLSIZE;
            if (!cell[indexi][indexj].isFlagged){
                if (!cell[indexi][indexj].hasMine){
                    revealEmptyCells(indexi, indexj);
                } else {
                    cell[indexi][indexj].revealCell();
                    lost = true;
                }
            }
            
            revealedCells++;
        }

        // draw 9x9 cells
        for (int i = 0; i < ROWS; i++){
            for (int j = 0; j < COLS; j++){
                cell[i][j].Draw(i, j);
            }
        }

        if(lost){
            DrawRectangle(0,0,screenWidth,screenHeight, Fade(DARKGRAY, 0.8f));
            DrawText(loseMessage, screenWidth/2 - MeasureText(loseMessage, 80)/2, screenHeight/3, 80, RED);
            revealAll();
        }

        if(flaggedMines == minesToPlace){
            DrawRectangle(0,0,screenWidth,screenHeight, Fade(DARKGRAY, 0.8f));
            DrawText(winMessage, screenWidth/2 - MeasureText(winMessage, 80)/2, screenHeight/3, 80, GREEN);
            revealAll();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
