#include "raylib.h"
#include <cstdio>
#include <cstdlib>
using namespace std;

int highScore = 0;
int score;

//Button size values
const int buttonHeight = 50;
const int buttonWidth = 200;

//stars on start screen values
const int numOfStars = 200;
Rectangle Stars[numOfStars];
double lastMoveTime[numOfStars];
float starSpeed[numOfStars];

//Player Ship Values
const float shipBodyWidth = 20;
const float shipBodyHeight = 45;
const float shipWingWidth = 25;
const float shipWingHeight = 15;
const float shipFlairWidth = 10;
const float shipFlairHeight = 13;
const float shipGunWidth = 8;
const float shipGunHeight = 6;
float shipBodyXPos;
float shipLeftWingXPos;
float shipRightWingXPos;
float shipBodyYPos;
float shipWingYPos;
float shipLeftFlairXPos;
float shipRightFlairXPos;
float shipFlairYPos;
float shipGunXPos;
float shipGunYPos;

//values for the lives
int lives = 3;
Rectangle heart1;
Rectangle heart1TopLeft;
Rectangle heart1TopRight;
Rectangle heart1Bottom;
Rectangle heart1BottomBottom;
Rectangle heart1Sides;
Rectangle heart2;
Rectangle heart2TopLeft;
Rectangle heart2TopRight;
Rectangle heart2Bottom;
Rectangle heart2BottomBottom;
Rectangle heart2Sides;
Rectangle heart3;
Rectangle heart3TopLeft;
Rectangle heart3TopRight;
Rectangle heart3Bottom;
Rectangle heart3BottomBottom;
Rectangle heart3Sides;

//values on how much a ship should move every frame
const float shipMoveValue = 3;

//values for the rocket projectiles
int rocketsFired = 0;
const int numberOfRockets = 200;
Rectangle rockets[numberOfRockets];
const float rocketWidth = 4;
const float rocketHeight = 12;
const float rocketMoveValue = 4;
int timeLastFired = 0;

//value for which wave you are on
int wave = 1;
int currentWave3SpawnIndex = 0;

//value for all of the enemy ships
//for now enemies are just going to be very basic rectangles so that the game is playable
const float enemyMoveSlowValue = 2;
const float enemyMoveFastValue = 5;
const float swooperYValue = -10;
float swooperLeftX;
float swooperRightX;
const int numberOfEnemies = 501;
int enemyMoveDownCount[numberOfEnemies];
Rectangle enemies[numberOfEnemies];
bool goingRight[numberOfEnemies];
bool onScreenBefore[numberOfEnemies];
const float enemyBodyWidth = 15;
const float enemyBodyHeight = 15;
int enemiesDeployed;
int enemiesKilled;
float swooperTargetX[numberOfEnemies];
float swooperYMoveValue;
float swooperXMoveValue[numberOfEnemies];
bool swoopingDown[numberOfEnemies];
bool staging[numberOfEnemies];
int stageCount[numberOfEnemies];
enum enemyType{DEAD, SLOW, FAST, SWOOP};
enemyType enemyShipType[numberOfEnemies];
const int enemyMoveDownValue = 30;

//values for when the player was last hit so that there is a grace period
int timeLastHit = 0;


void startMenu();
void playGame();
void gameOver();

void startMenu()
{
    int frameCount = 0; //how many times the loop has run essentially, 
                        //used for blinking text bc I think it looks cool

    while(!WindowShouldClose())
    {
        frameCount++;
        
        BeginDrawing();
        ClearBackground(BLACK); //Black Background bc we are in space

        //calculates where the title should go depending on the size of the window
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        int textWidth = MeasureText("GALAGA", 30);
        int posX = (screenWidth - textWidth) / 2; //calculates middle of the screen
        int posY = screenHeight / 4;// top quarter of the screen bc stuff will go below

        //Displays the title of the game and blinks it every 0.5 seconds
        if ((frameCount / 30) % 2 == 0) //60 frames per second = 30 frames every 0.5 seconds
        {
            DrawText("GALAGA", posX, posY, 30, WHITE);
        }


        //sets the values for the high score to be displayed under the title
        //aswell as sets the high score to display the right number
        char highScoreText[50];
        sprintf(highScoreText, "HIGH SCORE: %d", highScore);

        int scorePosX = ((screenWidth - MeasureText(highScoreText, 20)) / 2);
        int scorePosY = posY + 50; //dirrectly under the title
        //displays high score text
        DrawText(highScoreText, scorePosX, scorePosY, 20, WHITE);



        //time to display buttons that do things
        //first, set where the buttons should be
        float buttonPosX = (GetScreenWidth() - buttonWidth) / 2.0f; //middle
        float buttonPosY = (GetScreenHeight() - buttonHeight) / 2.0f; //middle

        //first button goes middle middle
        Rectangle startButton = {buttonPosX, buttonPosY, (float)buttonWidth, (float)buttonHeight};

        DrawRectangleRec(startButton, DARKGRAY);

        //next, update the Y position so button 2 is right under startButton
        buttonPosY = buttonPosY + (buttonPosY / 4.0f);

        Rectangle fullscreenButton = {buttonPosX, buttonPosY, (float)buttonWidth, (float)buttonHeight};

        //second button goes right under the first
        DrawRectangleRec(fullscreenButton, DARKGRAY);

        //Time for the buttons to have text in them
        //first button is for start and we want the text centered
        const char* label = "START";
        int fontSize = 20;
        textWidth = MeasureText(label, fontSize);

        //Now to display the centered text in the button
        DrawText(label, startButton.x + (startButton.width - textWidth) / 2, startButton.y + (startButton.height - fontSize) / 2, fontSize, WHITE);

        //now for the next button, we just change the label and font size
        label = "TOGGLE";
        fontSize = 20;
        textWidth = MeasureText(label, fontSize);

        //now to display the TOGGLE part of TOGGLE FULLSCREEN
        DrawText(label, fullscreenButton.x + (fullscreenButton.width - textWidth) / 2, fullscreenButton.y + (fullscreenButton.height - fontSize) / 4, fontSize, WHITE);

        label = "FULLSCREEN";
        fontSize = 20;
        textWidth = MeasureText(label, fontSize);

        //now to display the TOGGLE part of TOGGLE FULLSCREEN
        DrawText(label, fullscreenButton.x + (fullscreenButton.width - textWidth) / 2, fullscreenButton.y + (fullscreenButton.height - fontSize) / 4 * 3, fontSize, WHITE);


        //Now that the buttons are displayed, lets make them do something when clicked
        if (CheckCollisionPointRec(GetMousePosition(), startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            EndDrawing();
            playGame();
        }

        if (CheckCollisionPointRec(GetMousePosition(), fullscreenButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            ToggleFullscreen();
        }


        //I want the background to be a little more interesting so I'm going to add some squares that go across the screen the make it look like stars
        if(frameCount == 1)
        {
            for(int i = 0; i < numOfStars; i++)
            {
                int x = GetRandomValue(1, GetScreenWidth());
                int y = GetRandomValue(1, GetScreenHeight());
                
                Stars[i] = {(float)x, (float)y, 5, 5};
                
                if(!(CheckCollisionRecs(Stars[i], fullscreenButton) | CheckCollisionRecs(Stars[i], startButton)))
                {
                    DrawRectangleRec(Stars[i], WHITE);
                }

                starSpeed[i] = GetRandomValue(1, 7) / 100.0f; //gives values between 0.01 and 0.07 to determine star speed

                lastMoveTime[i] = GetTime(); //sets the time the stars last moved to the time they were created
                
            }
        }
        else
        {
            double currentTime = GetTime();

            for(int j = 0; j < numOfStars; j++)
            {
                if(currentTime - lastMoveTime[j] >= starSpeed[j])
                {
                    lastMoveTime[j] = currentTime;

                    Stars[j].y += 1;

                    if(Stars[j].y >= GetScreenHeight())
                    {
                        Stars[j].y = 0;
                    }
                    
                }

                if(!(CheckCollisionRecs(Stars[j], fullscreenButton) | CheckCollisionRecs(Stars[j], startButton)))
                {
                    DrawRectangleRec(Stars[j], WHITE);
                }
                
            }
        }


        EndDrawing();
    }

    CloseWindow();
    exit(0);

}

void playGame()
{
    int frameCount = 0;
    score = 0;
    enemiesDeployed = 0;
    enemiesKilled = 0;
    timeLastFired = 0;

    while(!WindowShouldClose())
    {
        ++frameCount; //add 1 to frameCount for every frame that happens, 60 fps

        BeginDrawing();

        ClearBackground(BLACK);


        char whatWave[50];
        sprintf(whatWave, "WAVE %d", wave);

        int fontSize = 20;
        int textWidth = MeasureText(whatWave, fontSize);

        //This displays the Wave 1 text for only a short time before the wave actually starts
        if ((frameCount / 20) % 2 == 0 && frameCount <= 119) //60 frames per second = 20 frames per 1/3 of a second, 3 flashes per second
        {
            DrawText(whatWave, (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 4, fontSize, WHITE);
        }

        //Now, we want our ship to appear, and we want this ship to appear even during the wave 1 text
        
        Rectangle shipBody = {shipBodyXPos, shipBodyYPos, shipBodyWidth, shipBodyHeight};
        Rectangle shipLeftWing = {shipLeftWingXPos, shipWingYPos, shipWingWidth, shipWingHeight};
        Rectangle shipRightWing = {shipRightWingXPos, shipWingYPos, shipWingWidth, shipWingHeight};
        Rectangle shipLeftFlair = {shipLeftFlairXPos, shipFlairYPos, shipFlairWidth, shipFlairHeight};
        Rectangle shipRightFlair = {shipRightFlairXPos, shipFlairYPos, shipFlairWidth, shipFlairHeight};
        Rectangle shipGun = {shipGunXPos, shipGunYPos, shipGunWidth, shipGunHeight};

        DrawRectangleRec(shipBody, WHITE);
        DrawRectangleRec(shipLeftWing, WHITE);
        DrawRectangleRec(shipRightWing, WHITE);
        DrawRectangleRec(shipLeftFlair, WHITE);
        DrawRectangleRec(shipRightFlair, WHITE);
        DrawRectangleRec(shipGun, WHITE);

        //These 2 if statements make the players ship move left or right on either arrow key useage or WASD usage
        if((IsKeyDown(KEY_A) | IsKeyDown(KEY_LEFT)) && shipLeftFlairXPos > 0)
        {
            shipBodyXPos = shipBodyXPos - shipMoveValue;
            shipLeftWingXPos = shipLeftWingXPos - shipMoveValue;
            shipRightWingXPos = shipRightWingXPos - shipMoveValue;
            shipLeftFlairXPos = shipLeftFlairXPos - shipMoveValue;
            shipRightFlairXPos = shipRightFlairXPos - shipMoveValue;
            shipGunXPos = shipGunXPos - shipMoveValue;
        }

        if((IsKeyDown(KEY_D) | IsKeyDown(KEY_RIGHT)) && (shipRightFlairXPos + shipFlairWidth) < GetScreenWidth())
        {
            shipBodyXPos = shipBodyXPos + shipMoveValue;
            shipLeftWingXPos = shipLeftWingXPos + shipMoveValue;
            shipRightWingXPos = shipRightWingXPos + shipMoveValue;
            shipLeftFlairXPos = shipLeftFlairXPos + shipMoveValue;
            shipRightFlairXPos = shipRightFlairXPos + shipMoveValue;
            shipGunXPos = shipGunXPos + shipMoveValue;
        }

        //This if statement toggles fullscreen on F key
        if(IsKeyPressed(KEY_F))
        {
            ToggleFullscreen();
        }

        //This if statement is going to fire a rocket from the ships gun
        if(IsKeyPressed(KEY_SPACE) | IsKeyPressed(KEY_W) | IsKeyPressed(KEY_UP))
        {
            if((frameCount > (timeLastFired + 12)) | (frameCount == 0 && timeLastFired == 0))
            {
                rocketsFired++;
                timeLastFired = frameCount;

                rockets[rocketsFired - 1] = {shipGunXPos + (rocketWidth / 2), shipGunYPos - (rocketHeight / 2), rocketWidth, rocketHeight};
            }
        }

        //This loop is going to print every rocket that should be on the screen and makes it move
        for(int i = 0; i < numberOfRockets; i++)
        {
            if(rockets[i].width != 1)
            {
                DrawRectangleRec(rockets[i], BLUE);

                rockets[i].y = rockets[i].y - rocketMoveValue;

                if(rockets[i].y == 0)
                {
                    rockets[i] = {0, 0, 1, 1};
                }
            }
        }

        if(rocketsFired == numberOfRockets - 1)
        {
            rocketsFired = 0;
        }

        //Display some hearts in the bottom right corner to display how many lives the player has left
        const char* label = "LIVES : ";

        DrawText(label, 5, GetScreenHeight() - 25, 20, GRAY);

        if(lives >= 1)
        {
            DrawRectangleRec(heart1, RED);
            DrawRectangleRec(heart1TopLeft, RED);
            DrawRectangleRec(heart1TopRight, RED);
            DrawRectangleRec(heart1Bottom, RED);
            DrawRectangleRec(heart1BottomBottom, RED);
            DrawRectangleRec(heart1Sides, RED);
        }

        if(lives >= 2)
        {
            DrawRectangleRec(heart2, RED);
            DrawRectangleRec(heart2TopLeft, RED);
            DrawRectangleRec(heart2TopRight, RED);
            DrawRectangleRec(heart2Bottom, RED);
            DrawRectangleRec(heart2BottomBottom, RED);
            DrawRectangleRec(heart2Sides, RED);
        }

        if(lives == 3)
        {
            DrawRectangleRec(heart3, RED);
            DrawRectangleRec(heart3TopLeft, RED);
            DrawRectangleRec(heart3TopRight, RED);
            DrawRectangleRec(heart3Bottom, RED);
            DrawRectangleRec(heart3BottomBottom, RED);
            DrawRectangleRec(heart3Sides, RED);
        }

        //NOW TO MAKE ENEMIES!!!!!
        //What enemies you face depends on the wave
        if(wave == 1)
        {
            if(frameCount == 120) //This is the first frame of the wave
            {
                //first we need to initialize the location of all of the enemies
                //first wave is easy so lets make 10 enemies appear at a time all in a row and the next row appears when that row goes down
                for(int i = 0; i < 10; i++)
                {
                    enemiesDeployed++;

                    if(i == 0)
                    {
                        enemies[i] = {-20, 25, enemyBodyWidth, enemyBodyHeight};
                        enemyShipType[i] = SLOW;
                    }
                    else
                    {
                        enemies[i] = {enemies[i-1].x - 40, 25, enemyBodyWidth, enemyBodyHeight};
                        enemyShipType[i] = SLOW;
                    }
                }
            }
            else if(frameCount > 120)
            {
                //release next enemies every 10 seconds unit 5 waves have been released
                for(int n = 1; n < 5; n++)
                {
                    if(frameCount == (60 * 10 * n + 120)) //60fps * 10 seconds until release + 120 frames since that was first enemies
                    {
                        for(int i = 10 * n; i < ((10 * n) + 10); i++)
                        {
                            if(i == 10 * n)
                            {
                                enemiesDeployed++;
                                
                                enemies[i] = {-20, 25, enemyBodyWidth, enemyBodyHeight};
                                enemyShipType[i] = SLOW;
                            }
                            else
                            {
                                enemiesDeployed++;
                                
                                enemies[i] = {enemies[i-1].x - 40, 25, enemyBodyWidth, enemyBodyHeight};
                                enemyShipType[i] = SLOW;
                            }
                        }
                    }
                }

                //moves all enemies
                for(int i = 0; i < numberOfEnemies; i++)
                {
                    if(enemies[i].height != -1)
                    {
                        if(enemies[i].x >= GetScreenWidth() - enemyBodyWidth) //Go down when on right wall
                        {
                            enemies[i].y = enemies[i].y + enemyMoveSlowValue;
                            enemyMoveDownCount[i]++;

                            if(enemyMoveDownCount[i] == enemyMoveDownValue)
                            {
                                enemyMoveDownCount[i] = 0;

                                enemies[i].x = enemies[i].x - enemyMoveSlowValue;

                                goingRight[i] = false;

                                onScreenBefore[i] = true;
                            }
                        }
                        else if(enemies[i].x <= 0 && onScreenBefore[i] == true) //Go down when on left wall
                        {
                            enemies[i].y = enemies[i].y + enemyMoveSlowValue;
                            enemyMoveDownCount[i]++;

                            if(enemyMoveDownCount[i] == enemyMoveDownValue)
                            {
                                enemyMoveDownCount[i] = 0;

                                enemies[i].x = enemies[i].x + enemyMoveSlowValue;

                                goingRight[i] = true;
                            }
                        }
                        else if(goingRight[i] == true) //go right
                        {
                            enemies[i].x = enemies[i].x + enemyMoveSlowValue;

                        }
                        else //go left
                        {
                            enemies[i].x = enemies[i].x - enemyMoveSlowValue;
                        }

                        DrawRectangleRec(enemies[i], PURPLE);
                    }
                }
            }

            if(enemiesDeployed == 50 && enemiesKilled == enemiesDeployed)
            {
                //YOU WON THIS ROUND

                //initializes values back to zero
                frameCount = 0;
                enemiesDeployed = 0;
                enemiesKilled = 0;
                timeLastFired = 0;

                //starts next wave
                wave++;
            }
        }
        else if(wave == 2)
        {
            if(frameCount == 120)
            {
                for(int i = 0; i < 1; i++)
                {
                    enemiesDeployed++;

                    enemies[i] = {swooperRightX, swooperYValue, enemyBodyWidth, enemyBodyHeight};
                    enemyShipType[i] = SWOOP;
                }
            }
            else if(frameCount > 120)
            {
                //release next enemy every 5 seconds until 10 waves have been released
                for(int i = 1; i < 10; i++)
                {
                    if(frameCount == (120 + (60 * 5 * i)))//120 because thats the start + 60fps * 5sec * wave number (every 5 seconds)
                    {
                        enemiesDeployed++;
                        
                        enemies[i] = {swooperRightX,swooperYValue, enemyBodyWidth, enemyBodyHeight};
                        enemyShipType[i] = SWOOP;
                    }
                }

                //move the enemies
                //these enemies are going to be the "SWOOPERS" so they need to figure out where the player is and target them
                for(int i = 0; i < enemiesDeployed; i++)
                {
                    if(enemies[i].x == swooperRightX && enemies[i].y == swooperYValue) //if the enemy has been initialized but not moved yet
                    {
                        swooperTargetX[i] = shipBodyXPos;

                        swooperXMoveValue[i] = (swooperTargetX[i] - enemies[i].x) / 120.0f;

                        swoopingDown[i] = true;

                        enemies[i].x = enemies[i].x - swooperXMoveValue[i];
                        enemies[i].y = enemies[i].y + swooperYMoveValue; 
                    }
                    else if(swoopingDown[i] == true) //if the enemy has started to move but not reached its target
                    {
                        enemies[i].x = enemies[i].x + swooperXMoveValue[i];
                        enemies[i].y = enemies[i].y + swooperYMoveValue;

                        if(enemies[i].y >= shipBodyYPos)
                        {
                            swoopingDown[i] = false; //when the enemy reaches its target, stop going down
                        }
                    }
                    else if(swoopingDown[i] == false && staging[i] == false)
                    {
                        //return to staging area

                        enemies[i].x = enemies[i].x + swooperXMoveValue[i];
                        enemies[i].y = enemies[i].y - swooperYMoveValue;

                        if(enemies[i].y <= GetScreenHeight() / 8)
                        {
                            staging[i] = true;
                        }
                    }
                    else if(staging[i] == true)
                    {
                        //If the enemy is in staging mode then I want it to just move left to right until it swoops again
                        stageCount[i]++;

                        if(stageCount[i] == 1)
                        {
                            if(GetRandomValue(0,1) == 0)
                            {
                                goingRight[i] = false;
                            }
                            else
                            {
                                goingRight[i] = true;
                            }
                        }

                        if(goingRight[i] == true)
                        {
                            enemies[i].x = enemies[i].x + enemyMoveSlowValue;

                            if(enemies[i].x >= GetScreenWidth() - enemyBodyWidth)
                            {
                                goingRight[i] = false;
                            }
                        }
                        else
                        {
                            enemies[i].x = enemies[i].x - enemyMoveSlowValue;

                            if(enemies[i].x <= 0)
                            {
                                goingRight[i] = true;
                            }
                        }

                        if(stageCount[i] > 120) // at least 2 seconds of staging before it can maybe swoop
                        {
                            if(GetRandomValue(1,5) > 3) //20% chance to swoop
                            {
                                swooperTargetX[i] = shipBodyXPos;

                                swooperXMoveValue[i] = (swooperTargetX[i] - enemies[i].x) / 120.0f;

                                swoopingDown[i] = true;
                                staging[i] = false;
                                stageCount[i] = 0;

                                enemies[i].x = enemies[i].x - swooperXMoveValue[i];
                                enemies[i].y = enemies[i].y + swooperYMoveValue; 
                            }
                        }
                    }

                    for(int i = 0; i < numberOfEnemies; i++)
                    {
                        if(enemyShipType[i] != DEAD)
                        {
                            DrawRectangleRec(enemies[i], BLUE);
                        }
                    }
                }
            }

            if(enemiesDeployed == 10 && enemiesKilled == enemiesDeployed)
            {
                //YOU WON THIS ROUND

                //initializes values back to zero
                frameCount = 0;
                enemiesDeployed = 0;
                enemiesKilled = 0;
                timeLastFired = 0;

                //starts next wave
                wave++;
            }
        }
        else if(wave == 3)
        {
            if(frameCount <= 119)
            {
                currentWave3SpawnIndex = 0;
                
                for(int i = 0; i < numberOfEnemies; i++)
                {
                    enemies[i] = {-1, -1, -1, -1};
                    enemyMoveDownCount[i] = 0;
                    onScreenBefore[i] = false;
                    goingRight[i] = true;
                    enemyShipType[i] = DEAD;
                    swooperTargetX[i] = -1;
                    swoopingDown[i] = false;
                    staging[i] = false;
                    stageCount[i] = 0;
                    swooperXMoveValue[i] = 0;
                }
            }
            else if(frameCount >= 120)
            {
                //all types of enemies have been introduced
                //now you play until you die

                //release next enemy every 3 seconds until 100 waves have been released
                    if(frameCount == (120 + (60 * 2 * currentWave3SpawnIndex)))//120 because thats the start + 60fps * 3sec * wave spawn index (every 3 seconds)
                    {
                        //determine what type of wave it's going to be
                        int type = GetRandomValue(1,4);

                        if(type == 1)
                        {
                            //slow
                            for(int j = enemiesDeployed; j < enemiesDeployed + 5; j++)
                            {
                                if(j == enemiesDeployed)
                                {
                                    enemies[j] = {-20 , 25, enemyBodyWidth, enemyBodyHeight};
                                    enemyShipType[j] = SLOW;

                                    enemyMoveDownCount[j] = 0;
                                    onScreenBefore[j] = false;
                                    goingRight[j] = true;
                                }
                                else
                                {
                                    enemies[j] = {enemies[j - 1].x - 40, 25, enemyBodyWidth, enemyBodyHeight};
                                    enemyShipType[j] = SLOW;

                                    enemyMoveDownCount[j] = 0;
                                    onScreenBefore[j] = false;
                                    goingRight[j] = true;
                                }
                            }

                            enemiesDeployed = enemiesDeployed + 5;
                        }
                        else if(type == 2)
                        {
                            //fast
                            for(int j = enemiesDeployed; j < enemiesDeployed + 5; j++)
                            {
                                if(j == enemiesDeployed)
                                {
                                    enemies[j] = {-20 , 25, enemyBodyWidth, enemyBodyHeight};
                                    enemyShipType[j] = FAST;

                                    enemyMoveDownCount[j] = 0;
                                    onScreenBefore[j] = false;
                                    goingRight[j] = true;
                                }
                                else
                                {
                                    enemies[j] = {enemies[j - 1].x - 40, 25, enemyBodyWidth, enemyBodyHeight};
                                    enemyShipType[j] = FAST;

                                    enemyMoveDownCount[j] = 0;
                                    onScreenBefore[j] = false;
                                    goingRight[j] = true;
                                }
                            }

                            enemiesDeployed = enemiesDeployed + 5;
                        }
                        else if(type >= 3)
                        {
                            //swoop
                            int rightOrLeft = GetRandomValue(0,1);

                            if(rightOrLeft == 0)
                            {
                                enemies[enemiesDeployed] = {swooperLeftX, swooperYValue, enemyBodyWidth, enemyBodyHeight};
                            }
                            else
                            {
                                enemies[enemiesDeployed] = {swooperRightX, swooperYValue, enemyBodyWidth, enemyBodyHeight};
                            }

                            enemyShipType[enemiesDeployed] = SWOOP;
                            enemiesDeployed++;
                        }

                        currentWave3SpawnIndex++;
                    }

                //make enemies move
                for(int i = 0; i < enemiesDeployed; i++)
                {
                    if(enemyShipType[i] == SLOW)
                    {
                        if(enemies[i].height != -1)
                        {
                            if(enemies[i].x >= GetScreenWidth() - enemyBodyWidth) //Go down when on right wall
                            {
                                enemies[i].y = enemies[i].y + enemyMoveSlowValue;
                                enemyMoveDownCount[i]++;

                                if(enemyMoveDownCount[i] == enemyMoveDownValue)
                                {
                                    enemyMoveDownCount[i] = 0;

                                    enemies[i].x = enemies[i].x - enemyMoveSlowValue;

                                    goingRight[i] = false;

                                    onScreenBefore[i] = true;
                                }
                            }
                            else if(enemies[i].x <= 0 && onScreenBefore[i] == true) //Go down when on left wall
                            {
                                enemies[i].y = enemies[i].y + enemyMoveSlowValue;
                                enemyMoveDownCount[i]++;

                                if(enemyMoveDownCount[i] == enemyMoveDownValue)
                                {
                                    enemyMoveDownCount[i] = 0;

                                    enemies[i].x = enemies[i].x + enemyMoveSlowValue;

                                    goingRight[i] = true;
                                }
                            }
                            else if(goingRight[i] == true) //go right
                            {
                                enemies[i].x = enemies[i].x + enemyMoveSlowValue;

                            }
                            else //go left
                            {
                                enemies[i].x = enemies[i].x - enemyMoveSlowValue;
                            }
                        }
                    }
                    else if(enemyShipType[i] == FAST)
                    {
                        if(enemies[i].height != -1)
                        {
                            if(enemies[i].x >= GetScreenWidth() - enemyBodyWidth) //Go down when on right wall
                            {
                                enemies[i].y = enemies[i].y + enemyMoveSlowValue;
                                enemyMoveDownCount[i]++;

                                if(enemyMoveDownCount[i] == enemyMoveDownValue)
                                {
                                    enemyMoveDownCount[i] = 0;

                                    enemies[i].x = enemies[i].x - enemyMoveFastValue;

                                    goingRight[i] = false;

                                    onScreenBefore[i] = true;
                                }
                            }
                            else if(enemies[i].x <= 0 && onScreenBefore[i] == true) //Go down when on left wall
                            {
                                enemies[i].y = enemies[i].y + enemyMoveSlowValue;
                                enemyMoveDownCount[i]++;

                                if(enemyMoveDownCount[i] == enemyMoveDownValue)
                                {
                                    enemyMoveDownCount[i] = 0;

                                    enemies[i].x = enemies[i].x + enemyMoveFastValue;

                                    goingRight[i] = true;
                                }
                            }
                            else if(goingRight[i] == true) //go right
                            {
                                enemies[i].x = enemies[i].x + enemyMoveFastValue;

                            }
                            else //go left
                            {
                                enemies[i].x = enemies[i].x - enemyMoveFastValue;
                            }
                        }
                    }
                    else if(enemyShipType[i] == SWOOP)
                    {
                        if(((enemies[i].x == swooperRightX) | (enemies[i].x == swooperLeftX)) && enemies[i].y == swooperYValue) //if the enemy has been initialized but not moved yet
                        {
                            swooperTargetX[i] = shipBodyXPos;

                            swooperXMoveValue[i] = (swooperTargetX[i] - enemies[i].x) / 120.0f;

                            swoopingDown[i] = true;

                            enemies[i].x = enemies[i].x - swooperXMoveValue[i];
                            enemies[i].y = enemies[i].y + swooperYMoveValue; 
                        }
                        else if(swoopingDown[i] == true) //if the enemy has started to move but not reached its target
                        {
                            enemies[i].x = enemies[i].x + swooperXMoveValue[i];
                            enemies[i].y = enemies[i].y + swooperYMoveValue;

                            if(enemies[i].y >= shipBodyYPos)
                            {
                                swoopingDown[i] = false; //when the enemy reaches its target, stop going down
                            }
                        }
                        else if(swoopingDown[i] == false && staging[i] == false)
                        {
                            //return to staging area

                            enemies[i].x = enemies[i].x + swooperXMoveValue[i];
                            enemies[i].y = enemies[i].y - swooperYMoveValue;

                            if(enemies[i].y <= GetScreenHeight() / 8)
                            {
                                staging[i] = true;
                            }
                        }
                        else if(staging[i] == true)
                        {
                            //If the enemy is in staging mode then I want it to just move left to right until it swoops again
                            stageCount[i]++;

                            if(stageCount[i] == 1)
                            {
                                if(GetRandomValue(0,1) == 0)
                                {
                                    goingRight[i] = false;
                                }
                                else
                                {
                                    goingRight[i] = true;
                                }
                            }

                            if(goingRight[i] == true)
                            {
                                enemies[i].x = enemies[i].x + enemyMoveSlowValue;

                                if(enemies[i].x >= GetScreenWidth() - enemyBodyWidth)
                                {
                                    goingRight[i] = false;
                                }
                            }
                            else
                            {
                                enemies[i].x = enemies[i].x - enemyMoveSlowValue;

                                if(enemies[i].x <= 0)
                                {
                                    goingRight[i] = true;
                                }
                            }

                            if(stageCount[i] > 120) // at least 2 seconds of staging before it can maybe swoop
                            {
                                if(GetRandomValue(1,5) > 3) //20% chance to swoop
                                {
                                    swooperTargetX[i] = shipBodyXPos;

                                    swooperXMoveValue[i] = (swooperTargetX[i] - enemies[i].x) / 120.0f;

                                    swoopingDown[i] = true;
                                    staging[i] = false;
                                    stageCount[i] = 0;

                                    enemies[i].x = enemies[i].x - swooperXMoveValue[i];
                                    enemies[i].y = enemies[i].y + swooperYMoveValue; 
                                }
                            }
                        }
                    }
                }

                //print the enemy if it isn't dead
                for(int i = 0; i < numberOfEnemies; i++)
                {
                    if(enemyShipType[i] != DEAD)
                    {
                        if(enemyShipType[i] == SLOW)
                        {
                            DrawRectangleRec(enemies[i], PURPLE);
                        }
                        else if(enemyShipType[i] == FAST)
                        {
                            DrawRectangleRec(enemies[i], PINK);
                        }
                        else if(enemyShipType[i] == SWOOP)
                        {
                            DrawRectangleRec(enemies[i], BLUE);
                        }
                    }
                }
            }
        }
        

        

        //Now we check to see if a rocket hits an enemy
        for(int i = 0; i < numberOfRockets; i++)
        {
            for(int j = 0; j < numberOfEnemies; j++)
            {
                if(enemyShipType[j] != DEAD)
                {
                    if(CheckCollisionRecs(rockets[i], enemies[j]))
                    {
                        enemies[j].x = -1;
                        enemies[j].y = -1;
                        enemies[j].width = -1;
                        enemies[j].height = -1;

                        if(enemyShipType[j] == SLOW)
                        {
                            score += 10;
                        }
                        else if(enemyShipType[j] == FAST)
                        {
                            score += 20;
                        }
                        else if(enemyShipType[j] == SWOOP)
                        {
                            score += 30;
                        }

                        rockets[i] = {0, 0, 1 ,1};

                        
                        enemyShipType[j] = DEAD;
                        enemiesKilled++;
                    }
                }
            }
        }

        //If an enemy hits the player then they need to lose a life
        for(int i = 0; i < numberOfEnemies; i++)
        {
            if(enemyShipType[i] != DEAD && (CheckCollisionRecs(enemies[i], shipGun) | CheckCollisionRecs(enemies[i], shipBody) | CheckCollisionRecs(enemies[i], shipLeftWing) | CheckCollisionRecs(enemies[i], shipRightWing)))
            {
                if(frameCount + 90 > timeLastHit) //Grace period so you don't lose 3 lives immediately, only takes a life if it's been over 1.5 seconds since last hit
                {
                    timeLastHit = frameCount;

                    lives--;
                }
                
                enemiesKilled++;

                enemies[i] = {-1, -1, -1, -1};
                enemyShipType[i] = DEAD;

                if(lives == 0)
                {
                    if(score > highScore)
                    {
                        highScore = score;
                    }
                    
                    EndDrawing();
                    gameOver();
                }
            }
        }

        //displays score during round
        char whatScore[50];
        sprintf(whatScore, "SCORE: %d", score);

        fontSize = 20;
        textWidth = MeasureText(whatScore, fontSize);

        if ((frameCount / 10) % 2 == 0) //60 frames per second = 10 frames per 1/6 of a second, 6 flashes per second
        {
            DrawText(whatScore, GetScreenWidth() - textWidth, GetScreenHeight() - 25, fontSize, GRAY);
        }

        EndDrawing();
    }

    CloseWindow(); //if the program reaches this then it is because the user wanted to quit the program
    exit(0);
}

//Displays the game over screen
void gameOver()
{
    int frameCount = 0;

    //reset values used during the game
    lives = 3;
    timeLastHit = 0;
    wave = 1;
    
    for(int i = 0; i < numberOfEnemies; i++)
    {
        enemies[i] = {-1, -1, -1, -1};
        enemyMoveDownCount[i] = 0;
        onScreenBefore[i] = false;
        goingRight[i] = true;
        enemyShipType[i] = DEAD;
        swooperTargetX[i] = -1;
        swoopingDown[i] = false;
        staging[i] = false;
        stageCount[i] = 0;
    }

    for(int i = 0; i < numberOfRockets; i++)
    {
        rockets[i] = {0, 0, 1, 1};
    }

    while(!WindowShouldClose())
    {
        frameCount++;
        
        BeginDrawing();

        ClearBackground(BLACK);

        //display game over text
        const char* label = "GAME OVER";
        int fontSize = 30;
        int textWidth = MeasureText(label, fontSize);
        if((frameCount / 30) % 2 == 0)
        {
            DrawText(label, (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2, fontSize, WHITE);
        }
        
        //display continue text
        label = "PRESS ENTER TO CONTINUE";
        fontSize = 15;
        textWidth = MeasureText(label, fontSize);
        if((frameCount / 30) % 2 == 0)
        {
            DrawText(label, (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 + 30, fontSize, GRAY);
        }

        //once enter is pressed, go back to the start menu
        if(IsKeyPressed(KEY_ENTER))
        {
            EndDrawing();
            startMenu();
        }

        EndDrawing();
    }
    
    CloseWindow();
    exit(0);
}


int main(void)
{
    //starts the window up and sets game to 60fps
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Galaga");
    SetTargetFPS(60);


    //initialize values for where the ship should go when it starts
    shipBodyXPos = (GetScreenWidth() - shipBodyWidth) / 2;
    shipLeftWingXPos = shipBodyXPos - shipWingWidth;
    shipRightWingXPos = shipBodyXPos + shipBodyWidth;
    shipBodyYPos = (GetScreenHeight() - shipBodyHeight) / 12.0f * 11.0f;
    shipWingYPos = shipBodyYPos + (shipBodyHeight / 2);
    shipLeftFlairXPos = shipLeftWingXPos - (shipFlairWidth / 2);
    shipRightFlairXPos = shipRightWingXPos + shipWingWidth - (shipFlairWidth / 2);
    shipFlairYPos = shipWingYPos + shipWingHeight;
    shipGunXPos = shipBodyXPos + (shipBodyWidth / 2) - (shipGunWidth / 2);
    shipGunYPos = shipBodyYPos - shipGunHeight;

    //initializes the rocket array so that the code knows none of the rockets are in use
    for(int i = 0; i < numberOfRockets; i++)
    {
        rockets[i] = {0, 0, 1, 1};
    }

    //initializes the size and placement for the hearts
    //{x pos, y pos, width, height
    heart1 = {(float)MeasureText("LIVES : ",20) + 5, (float)GetScreenHeight() - 25,8,10};
    heart1TopLeft = {heart1.x, heart1.y - 2, heart1.width / 4 + 1, 3};
    heart1TopRight = {heart1.x + (heart1.width - heart1TopLeft.width), heart1TopLeft.y, heart1TopLeft.width, heart1TopLeft.height};
    heart1Bottom = {heart1.x + (heart1.width / 8),heart1.y + heart1.height, heart1TopLeft.width * 2, 2};
    heart1BottomBottom = {heart1Bottom.x + 1, heart1Bottom.y + 2, heart1TopLeft.width + 1, 2};
    heart1Sides = {heart1.x - 1, heart1.y + 2, heart1.width + 2, heart1.height - 2};

    heart2 = {heart1.x + 15, heart1.y, heart1.width, heart1.height};
    heart2TopLeft = {heart2.x, heart2.y - 2, heart2.width / 4 + 1, 3};
    heart2TopRight = {heart2.x + (heart2.width - heart2TopLeft.width), heart2TopLeft.y, heart2TopLeft.width, heart2TopLeft.height};
    heart2Bottom = {heart2.x + (heart2.width / 8),heart2.y + heart2.height, heart2TopLeft.width * 2, 2};
    heart2BottomBottom = {heart2Bottom.x + 1, heart2Bottom.y + 2, heart2TopLeft.width + 1, 2};
    heart2Sides = {heart2.x - 1, heart2.y + 2, heart2.width + 2, heart2.height - 2};

    heart3 = {heart2.x + 15, heart1.y, heart1.width, heart1.height};
    heart3TopLeft = {heart3.x, heart3.y - 2, heart3.width / 4 + 1, 3};
    heart3TopRight = {heart3.x + (heart3.width - heart3TopLeft.width), heart3TopLeft.y, heart3TopLeft.width, heart3TopLeft.height};
    heart3Bottom = {heart3.x + (heart3.width / 8),heart3.y + heart3.height, heart3TopLeft.width * 2, 2};
    heart3BottomBottom = {heart3Bottom.x + 1, heart3Bottom.y + 2, heart3TopLeft.width + 1, 2};
    heart3Sides = {heart3.x - 1, heart3.y + 2, heart3.width + 2, heart3.height - 2};

    //initialize enemies so that the code knows which ones are in use
    for(int i = 0; i < numberOfEnemies; i++)
    {
        enemies[i] = {-1, -1, -1, -1};
        enemyMoveDownCount[i] = 0;
        onScreenBefore[i] = false;
        goingRight[i] = true;
        enemyShipType[i] = DEAD;
        swooperTargetX[i] = -1;
        swoopingDown[i] = false;
        staging[i] = false;
        stageCount[i] = 0;
    }

    //sets the start X location and Y move value for the swoopers since it depends on screen size
    swooperLeftX = GetScreenWidth() / 4;
    swooperRightX = GetScreenWidth() / 4 * 3;
    swooperYMoveValue = (shipFlairYPos - swooperYValue) / 120.0f; // divide by 120 bc 60fps * 2 sec = 120 frames

    //displays start menu
    startMenu();
    
    return 0;
}