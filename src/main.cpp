#include <iostream>
#include <raylib.h>
#define ROWS 20
#define COLS 20
#define CELLSIZE 60

bool lost = false;
bool won = false;
bool gameOver = false;

int revealedCells = 0;
float startTime, endTime;

class Game{
    int cellSide, cellx, celly;

public:
    bool isRevealed;
    bool isFlagged;
    bool hasMine;
    int nearbyMines;

    Game(){
        cellSide = CELLSIZE;
        cellx = -1;
        celly = -1;
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

void placeMines(int minesToPlace, int rows, int columns){
    while (minesToPlace > 0){
        int i = GetRandomValue(0, rows - 1);
        int j = GetRandomValue(0, columns - 1);
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


void displayTime(float start){
    int end = endTime;
    int minute = (end-start) / 60;
    int second = static_cast<int>(end-start) % 60;
    DrawText(TextFormat("TIME: %d min %d sec", minute, second), 10, GetScreenHeight() - 30, 20, RAYWHITE);
}


void restart(int rows, int columns){
    int minesNo =  0.1*(rows*columns) + 2; // same as mines to place

    // unreveal, unmine and unflag all cells
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            cell[i][j].isFlagged = false;
            cell[i][j].isRevealed = false;
            cell[i][j].hasMine = false;
        }
    }
    placeMines(minesNo, rows, columns);
    countMines();
}





int main() {
    // set minimum window size initially
    int screenWidth = 540;
    int screenHeight = 540;

    int rows;
    int columns;
    int indexi, indexj;
    int minesToPlace;
    int flaggedMines = 0;
    bool difficultyChosen = false;

    char loseMessage[] = "YOU LOST";
    char winMessage[] = "YOU WON!";
    char restartText[] = "Press 'R' to restart";


    InitWindow(screenWidth, screenHeight, "Mines"); // initialize window
    SetTargetFPS(60);


    // main game loop
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        if(!difficultyChosen){
            DrawRectangle(0,0, screenWidth, screenHeight, BLACK);
            Vector2 mouse = GetMousePosition();
            char chooseDiff[] = "Choose a difficulty";
            char diff99[] = "9x9";
            char diff1616[] = "16x16";
            Rectangle button99 = {270, 240, 204, 54};
            Rectangle button1616 = {270, 340, 204, 54};

            DrawText(chooseDiff, screenWidth/2 - MeasureText(chooseDiff, 40)/2, 100, 40, RAYWHITE);

            DrawRectangle(screenWidth/2 - 100, 240, 204, 54, RAYWHITE);
            DrawRectangle(screenWidth/2 - 97, 242, 200, 50, LIGHTGRAY);
            DrawText(diff99, screenHeight/2 - MeasureText(diff99, 30)/2, 250, 30, DARKGRAY);

            DrawRectangle(screenWidth/2 - 100, 340, 204, 54, RAYWHITE);
            DrawRectangle(screenWidth/2 - 97, 342, 200, 50, LIGHTGRAY);
            DrawText(diff1616, screenHeight/2 - MeasureText(diff1616, 30)/2, 350, 30, DARKGRAY);

            if(CheckCollisionPointRec(mouse, button99) && IsMouseButtonPressed(0)){
                difficultyChosen = true;
                rows = 9;
                columns = 9;
                screenWidth = 9*CELLSIZE;
                screenHeight = 9*CELLSIZE;
                minesToPlace = 0.1*(rows*columns) + 2; 
                startTime = GetTime();

                placeMines(minesToPlace, rows, columns);
                countMines();
                SetWindowSize(screenWidth, screenHeight);
            }

            if(CheckCollisionPointRec(mouse, button1616) && IsMouseButtonPressed(0)){
                difficultyChosen = true;
                rows = 16;
                columns = 16;
                screenWidth = 16*CELLSIZE;
                screenHeight = 16*CELLSIZE;
                minesToPlace = 0.1*(rows*columns) + 2;  
                startTime = GetTime();

                placeMines(minesToPlace, rows, columns);
                countMines();
                SetWindowSize(screenWidth, screenHeight);
            }

        }
        else if(difficultyChosen){
            // flag cell on right click
            if(!gameOver && IsMouseButtonPressed(1)){
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
            if(!gameOver && IsMouseButtonPressed(0)){
                indexi = GetMouseX() / CELLSIZE;
                indexj = GetMouseY() / CELLSIZE;
                if (!cell[indexi][indexj].isFlagged){
                    if (!cell[indexi][indexj].hasMine){
                        revealEmptyCells(indexi, indexj);
                    } else {  // lose condition
                        cell[indexi][indexj].revealCell();
                        lost = true;
                        endTime = GetTime();
                    }
                }
                
                revealedCells++;
            }

            // restart on pressing R on end screen
            if(gameOver && IsKeyPressed(KEY_R)){
                restart(rows, columns);

                startTime = GetTime();
                won = false;
                lost = false;
                gameOver = false;
            }

            // draw cells
            for (int i = 0; i < rows; i++){
                for (int j = 0; j < columns; j++){
                    cell[i][j].Draw(i, j);
                }
            }

            if(lost && !won){ // if lost (clicked a mine)
                gameOver = true;
                DrawRectangle(0,0,screenWidth,screenHeight, Fade(DARKGRAY, 0.8f));
                DrawText(loseMessage, screenWidth/2 - MeasureText(loseMessage, 80)/2, screenHeight/3, 80, RED);
                DrawText(restartText, screenWidth/2 - MeasureText(restartText, 25)/2, screenHeight/3 + 80, 25, RAYWHITE);
                
                revealAll();
                displayTime(startTime);
            }


            if(flaggedMines == minesToPlace){ // win condition
                won = true;
                endTime = GetTime();
                flaggedMines++; // change flagged mines so it only triggers once
            }

            if(won){ // if won (flagged all mines)
                gameOver = true;
                DrawRectangle(0,0,screenWidth,screenHeight, Fade(DARKGRAY, 0.8f));
                DrawText(winMessage, screenWidth/2 - MeasureText(winMessage, 80)/2, screenHeight/3, 80, GREEN);
                DrawText(restartText, screenWidth/2 - MeasureText(restartText, 25)/2, screenHeight/3 + 80, 25, RAYWHITE);
                revealAll();
                displayTime(startTime);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
