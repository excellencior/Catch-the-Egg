# include "iGraphics.h"
# include <bits/stdc++.h>
# include <windows.h>
# include <mmsystem.h>

int basket_len = 120;
#define basket_width 94
#define egg_len 40
#define egg_width 50
#define chicken_len 128
#define chicken_width 166
#define perk_len 40
#define perk_width 50
/// Perks Identity
#define perk_larger_basket 0
#define perk_extra_time 1
#define perk_slow_down_egg_fall 2
#define perk_smaller_basket 3
#define perk_speed_up_egg_fall 4

/// Home button 60x60

#define screen_length 1360
#define screen_width 760

/// Pages
#define menuPage 1
#define levelPage 2
#define instructionsPage 6
#define gamePage 3
#define seehighscorePage 4
#define showhighScore 7
#define savenamePage 5
#define errorPage 8
#define loadingPage 9
#define demoPage 10

/// Levels and save
#define level_1 1
#define level_2 2
#define level_3 3
int level = 0;

void eggt();
void eggfall();
void egg();
void reset_perk_position();
void show_scores();
void save_score_and_name();
void reset_play_screen();
void saveGameState();
void loadGameState();
void score_to_string();
void check_for_game();
void set_game_to_null();
void set_game_to_value();

int page = loadingPage;

char instruction[10][100] = {
        "Choose your LEVELS from NEW GAME menu",
        "Egg Values : Golden -> 10 , Blue -> 5 , Normal -> 1 , Poop -> -10",
        "There are certain perks / damages : Big Basket, Small Basket",
        "Faster Egg Fall, Slower Egg Fall, Extra Time",
        "When the timer ends the Player needs to input his/her NAME",
        "You can see your high score from the HIGH SCORE option",
        "Player can RESUME his game from the LAST PLAYED STATE"
};
int showInstruction = 0;

int chicken_x = 680, chicken_y = 600, basket_x = 680, chicken_dis = 10; // the change rate & the position of the chicken
int basket_1 = 1; // If basket_1 == 0 it means the basket is larger

int resume = 0; /* Play/Pause indicator*/
char timer[20] = "Time : 00 : 00";
int timer_second = 60;
int click = 0; /* Mouse click indicator */
int egg_x = chicken_x, egg_y = chicken_y+40, egg_dy = 15, egg_dx = 3; // Position of the egg
int  wind_duration = 30; // Wind will blow after every 2s for 10s

int check_game = -1, load_dx = 0;
int demo[10] = {1}, idx = 0;

int egg_catch[10] = {1}; // Indicator for catching the egg and egg number >> 0(normal), 1(blue), 2(golden), 3(poop)
char score[20] = "Score : 000000"; // size : 14, last index = 13
int add_score = 0;

char egg_color[20][100] = {
        "normal_egg.bmp",
        "normal_egg.bmp",
        "normal_egg.bmp",
        "blue_egg.bmp",
        "blue_egg.bmp",
        "golden_egg.bmp",
        "chicken_poop.bmp",
        "chicken_poop.bmp"
    };
int song = 1; int idx_egg_type =0;
int start_egg_2 = 0; // It helps to fall the 2nd egg after the 1st egg reaches the half distance

struct _perk
{
    int perk_x = rand()%screen_length, perk_y = screen_width;
    int show = 0;
} perk[5];

int perk_load_timer = 10, perkNum = -1, perkType = -1; // perkType is for finding which is which perk
char perk_type[6][40] = {
            "perk_larger_basket.bmp",
            "perk_extra_time.bmp",
            "perk_slow_down_egg_fall.bmp",
            "perk_smaller_basket.bmp",
            "perk_speed_up_egg_fall.bmp"
        };
int perk_timer = 0; char perk_remaining_timer[] = "010"; // Time remaining of the activated perk

struct _egg
{
    int egg_x = chicken_x,
    egg_y = chicken_y + 40,
    egg_i = 0;
} e[2];

///         -------------------------------------- Function Section   -------------------------------------------
void score_to_string()
{
    score[13] = 48 + (add_score%10);
    score[12] = 48 + ((add_score/10)%10);
    score[11] = 48 + ((add_score/100)%10);
    score[10] = 48 + ((add_score/1000)%10);
}

void egg_cnt() /// For showing the score
{
    if (e[idx_egg_type].egg_i >= 0 && e[idx_egg_type].egg_i <= 2) add_score++;
    else if (e[idx_egg_type].egg_i >= 3 && e[idx_egg_type].egg_i <= 4) add_score+=5;
    else if (e[idx_egg_type].egg_i == 5) add_score+=10;
    else add_score-=10;
    score_to_string();
    if (add_score < 0) { score[13] = '0'; score[12] = '0'; score[11] = '0'; score[10] = '0'; add_score = 0; }
    if (e[idx_egg_type].egg_i > 5) PlaySound("poop.wav", NULL, SND_ASYNC);
    else PlaySound("egg_caught.wav", NULL, SND_ASYNC);
}

void reset_perk_position()
{
    perk[perkNum].perk_y = screen_width;
    perk[perkNum].perk_x = 100 + (rand() % (screen_length-200)); // Perk will stay between 100 and screen_length-100
}

// ---------------------------------------Saving the players name and score
// ---------------------------------------- saving the player name
char playerName[100] = "";
int name_len = 0;
char name[3][10][100];

struct _saveScore
{
    char name[100];
    int score;
} saveScore[11];

//  -------------------------------------------- High Score Saving -------------------------------------------

void save_score_and_name()
{
    FILE* fp = NULL;
    int i=0;
    if (level == level_1) fp = fopen("save_score_level_1.txt", "r");
    else if (level == level_2) fp = fopen("save_score_level_2.txt", "r");
    else if (level == level_3) fp = fopen("save_score_level_3.txt", "r");

    for (i=0; i<10; i++){
        fscanf(fp, "%s %d", &saveScore[i].name, &saveScore[i].score);
    }
    strcpy(saveScore[i].name, playerName);
    for (int len = strlen(playerName); len>=0; len--){
        playerName[len] = '\0';
    }
    saveScore[i].score = add_score;

    struct _saveScore temp;

    for (int j=0; j<i; j++){
        for (int k=0; k<i-j; k++){
            if (saveScore[k].score < saveScore[k+1].score){
                temp = saveScore[k];
                saveScore[k] = saveScore[k+1];
                saveScore[k+1] = temp;
            }
        }
    }

    fclose(fp);

    if (level == level_1) fp = fopen("save_score_level_1.txt", "w");
    else if (level == level_2) fp = fopen("save_score_level_2.txt", "w");
    else if (level == level_3) fp = fopen("save_score_level_3.txt", "w");

    for (i=0; i<10; i++){
        fprintf(fp, "%s %d\n", saveScore[i].name, saveScore[i].score);
    }
    fclose(fp);
    add_score = 0; // The score becomes zero after saving it
}

void show_scores()
{
    FILE* fp = NULL;
    if (level == level_1) fp = fopen("save_score_level_1.txt", "r");
    else if (level == level_2) fp = fopen("save_score_level_2.txt", "r");
    else if (level == level_3) fp = fopen("save_score_level_3.txt", "r");

    for (int i=0; i<10; i++){
        fgets(name[level-1][i], 100, fp);
        }
    fclose(fp);
    page = showhighScore;
}
// -----------------------------------------------------------------------------------------------------------------

void reset_play_screen()
{
    chicken_x = 680;
    basket_x = 680;
    // ------------------
    e[0] = {chicken_x, chicken_y+40}; // Resetting the position of the egg
    e[1] = {chicken_x, chicken_y+40};
    start_egg_2 = 0;
    // ------------------
    score[13] = '0';  // Resetting the score board 000000
    score[12] = '0';
    score[11] = '0';
    score[10] = '0';
    //--------------------
    reset_perk_position();
    // ------------------
    timer_second = 60;
    resume = 0;
    strcpy(playerName, "");
}

void saveGameState()
{
    FILE* fp = fopen("saved_game.txt", "w");

    fprintf(fp, "%s\n", timer);

    fprintf(fp, "%d %d %d %d %d %d\n", basket_len, level, chicken_x, basket_x, basket_1, resume);

    fprintf(fp, "%d %d %d %d %d\n", timer_second, click, egg_dy, egg_dx, wind_duration);

    for (int i=0; i<=7; i++) fprintf(fp, "%d ", egg_catch[i]); fprintf(fp, "\n");

    fprintf(fp, "%d %d %d %d\n", add_score, song, idx_egg_type, start_egg_2);

    for (int i=0; i<5; i++) fprintf(fp, "%d %d %d ", perk[i].perk_x, perk[i].perk_y, perk[i].show); fprintf(fp, "\n");

    fprintf(fp, "%d %d %d %d %s\n", perk_load_timer, perkNum, perkType, perk_timer, perk_remaining_timer);

    for (int i=0; i<2; i++) fprintf(fp, "%d %d %d ", e[i].egg_x, e[i].egg_y, e[i].egg_i); fprintf(fp, "\n");

    fclose(fp);
}

void loadGameState()
{
    FILE* fp = fopen("saved_game.txt", "r");

    fgets(timer, 20, fp);

    fscanf(fp, "%d %d %d %d %d %d", &basket_len, &level, &chicken_x, &basket_x, &basket_1, &resume);

    fscanf(fp, "%d %d %d %d %d", &timer_second, &click, &egg_dy, &egg_dx, &wind_duration);

    for (int i=0; i<=7; i++) fscanf(fp, "%d", &egg_catch[i]);

    fscanf(fp, "%d %d %d %d", &add_score, &song, &idx_egg_type, &start_egg_2);

    for (int i=0; i<5; i++) fscanf(fp, "%d %d %d", &perk[i].perk_x, &perk[i].perk_y, &perk[i].show);

    fscanf(fp, "%d %d %d %d %s", &perk_load_timer, &perkNum, &perkType, &perk_timer, &perk_remaining_timer);

    for (int i=0; i<2; i++) fscanf(fp, "%d %d %d", &e[i].egg_x, &e[i].egg_y, &e[i].egg_i);

    score_to_string();

    fclose(fp);
}

void set_game_to_value()
{
    FILE* check = fopen("check_previous_game.txt", "w");
    fprintf(check, "%d", 1);
    fclose(check);
}

void check_for_game()
{
    FILE* check = fopen("check_previous_game.txt", "r");
    fscanf(check, "%d", &check_game);
    fclose(check);
}

void set_game_to_null()
{
    FILE* fp = fopen("saved_game.txt", "w");
    fclose(fp);
    FILE* check = fopen("check_previous_game.txt", "w");
    fprintf(check, "%d", 0);
    fclose(check);
}

// ------------------------------------------------------------------------------------------------------------------

void iDraw()
{
    iClear();
    if (page == menuPage) {
        iShowBMP(0, 0, "menu_scr.bmp"); // The menu screen
        iShowBMP2(100, 580, "game_title.bmp", 0x00ffffff);
        iShowBMP2(1080, 400, "menuButtons\\newgameButton.bmp", 0x00ffffff);
        iShowBMP2(1080, 300, "menuButtons\\resumeButton.bmp", 0x00ffffff);
        iShowBMP2(1080, 200, "menuButtons\\highscoreButton.bmp", 0x00ffffff);
        iShowBMP2(1080, 100, "menuButtons\\exitButton.bmp", 0x00ffffff);
        iShowBMP2(80, 100,  "menuButtons\\instructionsButton.bmp", 0x00ffffff);
        if (song) { PlaySound("song.wav", NULL, SND_ASYNC); song = 0; } // Playing song in the main menu
        iSetColor(255, 255, 255);
        if (showInstruction)
        {
            for (int i=0, ins_y = 200; i<6; i++, ins_y -= 30){
            iText(300, ins_y, instruction[i], GLUT_BITMAP_TIMES_ROMAN_24);
            }
            iSetColor(0, 150, 150); iFilledRectangle(80, 40, 200, 40);
            iSetColor(255, 255, 255); iText(85, 50, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }
    else if (page == gamePage) { // the game screen

    iShowBMP(0, 0, "farm.bmp");
    if (resume == 0) iText(640, 380, "Tap the basket to start", GLUT_BITMAP_HELVETICA_18);

        /// There should be a option to select different duration

    iSetColor(255, 0, 0);
    iText(20, 560, timer, GLUT_BITMAP_TIMES_ROMAN_24); // Timer clock

    iShowBMP2(e[0].egg_x, e[0].egg_y, egg_color[e[0].egg_i], 0x00ffffff);  // Rendering the egg in the screen with random egg_color
    if (start_egg_2) { iShowBMP2(e[1].egg_x, e[1].egg_y, egg_color[e[1].egg_i], 0x00ffffff); }

    if (e[idx_egg_type = 0].egg_y < 55) {
        if (e[0].egg_x >= basket_x && e[0].egg_x+egg_len <= basket_x+basket_len) egg_cnt(); // Counting the score after the egg is in the basket
        else if (e[idx_egg_type].egg_i <= 5) iShowBMP2(e[idx_egg_type].egg_x, 50, "broken_egg.bmp", 0x00ffffff);
    }
    else if (e[idx_egg_type = 1].egg_y < 55) {
        if (e[1].egg_x >= basket_x && e[1].egg_x+egg_len <= basket_x+basket_len) egg_cnt(); // Counting the score after the egg is in the basket
        else if (e[idx_egg_type].egg_i <= 5) iShowBMP2(e[idx_egg_type].egg_x, 50, "broken_egg.bmp", 0x00ffffff);
    }

    /// Perk Handling
    if (perk[perkNum].show)
        iShowBMP2(perk[perkNum].perk_x, perk[perkNum].perk_y, perk_type[perkNum], 0x00ffffff);

    if (perk[perkNum].perk_y <= 55){
        if (perk[perkNum].perk_x >= basket_x &&
         perk[perkNum].perk_x+perk_len <= basket_x+basket_len)
        {
            PlaySound("perk_caught.wav", NULL, SND_ASYNC);
            if (perkNum == perk_larger_basket) {
                basket_len = 180;
                perkType = perk_larger_basket; // perkType
            }
            else if (perkNum == perk_extra_time){
                perkType = perk_extra_time;
            }
            else if (perkNum == perk_slow_down_egg_fall) {
                perkType = perk_slow_down_egg_fall;
                egg_dy = 10;
            }
            else if (perkNum == perk_smaller_basket) {
                perkType = perk_smaller_basket;
                basket_len = 80;
            }
            else if (perkNum == perk_speed_up_egg_fall) {
                perkType = perk_speed_up_egg_fall;
                egg_dy = 20;
            }
            perk_timer = 10;
        }
    }
    if (perk_timer > 0){
        iSetColor(0, 255, 100);
        iText(20, 490, perk_remaining_timer, GLUT_BITMAP_TIMES_ROMAN_24);
    }
    /// <<<---
    if (perkType == perk_larger_basket && perk_timer > 0) iShowBMP2(basket_x, 40, "basket_2.bmp", 0x00ffffff); /// The larger basket
    else if (perkType == perk_smaller_basket && perk_timer > 0) iShowBMP2(basket_x, 40, "basket_3.bmp", 0x00ffffff);
    else iShowBMP2(basket_x, 40, "basket_1.bmp", 0x00ffffff); /// the Basket 120 x 94

    iSetColor(255, 0, 0);
    iText(20, 510, score, GLUT_BITMAP_TIMES_ROMAN_24); // Score on the screen

    iSetColor(0, 128, 0); iFilledRectangle(0, 600, 1360, 5); // Bamboo
//    iShowBMP2(0, 550, "bash.bmp", 0x00ffffff);
    iShowBMP2(chicken_x, chicken_y, "chicken2.bmp", 0x00ffffff); // The CHICKEN

    if (resume) iShowBMP2(1300, 540, "Pause_button.bmp", 0x00ffffff);
    else iShowBMP2(1300, 540, "resume_button.bmp", 0x00ffffff);

    iShowBMP2(1280, 460, "home_button.bmp", 0x00ffffff);
    //iRectangle(1190, 490, 150, 30); iText(1200, 500, "Back to main menu"); // Back to main button in the play screen
    song = 1; // Game Screen > Menu : The song will start to play

    /// ----------------------------- Prompt for saving score
    }
    else if (page == levelPage){ /// Prompt for new game screen
        iShowBMP(0, 0, "menu_scr.bmp");
        iSetColor(0, 150, 150); iFilledRectangle(420, 700, 560, 40);
        iSetColor(255, 255, 0); iText(600, 710, "PLAY THE GAME", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(420, 500, 140, 40);
        iSetColor(255, 255, 255); iText(445, 510, "LEVEL 1", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(0, 150, 150); iFilledRectangle(620, 500, 180, 40);
        iSetColor(255, 255, 0); iText(645, 510, "3 MINUTES", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(420, 400, 140, 40);
        iSetColor(255, 255, 255); iText(445, 410, "LEVEL 2", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(0, 150, 150); iFilledRectangle(620, 400, 180, 40);
        iSetColor(255, 255, 0); iText(645, 410, "2 MINUTES", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(420, 300, 140, 40);
        iSetColor(255, 255, 255); iText(445, 310, "LEVEL 3", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(0, 150, 150); iFilledRectangle(620, 300, 180, 40);
        iSetColor(255, 255, 0); iText(645, 310, "1 MINUTE", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(620, 200, 180, 40);  // Going back to the main menu
        iSetColor(255, 255, 0); iText(645, 210, "GO BACK", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    /// Need to show prompt for saving name

    else if (page == seehighscorePage){
        iShowBMP(0, 0, "menu_scr.bmp");
        iSetColor(0, 150, 150); iFilledRectangle(420, 700, 560, 40);
        iSetColor(255, 255, 0); iText(600, 710, "See High Score", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(420, 500, 140, 40);
        iSetColor(255, 255, 255); iText(445, 510, "LEVEL 1", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(0, 150, 150); iFilledRectangle(620, 500, 200, 40);
        iSetColor(255, 255, 0); iText(645, 510, "See High Scores", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(420, 400, 140, 40);
        iSetColor(255, 255, 255); iText(445, 410, "LEVEL 2", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(0, 150, 150); iFilledRectangle(620, 400, 200, 40);
        iSetColor(255, 255, 0); iText(645, 410, "See High Scores", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(420, 300, 140, 40);
        iSetColor(255, 255, 255); iText(445, 310, "LEVEL 3", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(0, 150, 150); iFilledRectangle(620, 300, 200, 40);
        iSetColor(255, 255, 0); iText(645, 310, "See High Scores", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(620, 200, 180, 40);  // Going back to the main menu
        iSetColor(255, 255, 0); iText(645, 210, "GO BACK", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    else if (page == showhighScore){
        iShowBMP(0, 0, "menu_scr.bmp");

        iSetColor(0, 150, 150); iFilledRectangle(0, 600, 1360, 40);
        iSetColor(255, 255, 255);
        if (level == level_1) iText(625, 610, "LEVEL 1", GLUT_BITMAP_TIMES_ROMAN_24);
        else if (level == level_2) iText(625, 610, "LEVEL 2", GLUT_BITMAP_TIMES_ROMAN_24);
        else if (level == level_3) iText(625, 610, "LEVEL 3", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(450, 90, 450, 510);
        iSetColor(255, 255, 255); iText(580, 560, "Names and Scores", GLUT_BITMAP_TIMES_ROMAN_24);
        for (int i=0, text_y = 530; i<10; i++, text_y -= 30){
            iText(600, text_y, name[level-1][i], GLUT_BITMAP_TIMES_ROMAN_24);
        }
        iSetColor(255, 150, 150); iFilledRectangle(450, 50, 450, 40);
        iSetColor(255, 0, 0); iText(580, 60, "GO BACK", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    else if (page == savenamePage){
        iShowBMP(0, 0, "farm.bmp");

        iShowBMP2(450, 650, "game_over2.bmp", 0x00ffffff);
        iSetColor(128, 128, 0); iFilledRectangle(0, 600, 1360, 5); // Bamboo
        iSetColor(128, 128, 0); iFilledRectangle(0, 200, 1360, 5); // Bamboo

        iSetColor(128, 128, 0); iFilledRectangle(0, 550, 1360, 40); // Bamboo
        iSetColor(255, 255, 255); iText(600, 560, "Your", GLUT_BITMAP_TIMES_ROMAN_24);
                                  iText(660, 560, score, GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 255, 255);
        if (level == level_1) iText(600, 400, "LEVEL 1", GLUT_BITMAP_TIMES_ROMAN_24);
        else if (level == level_2) iText(625, 610, "LEVEL 2", GLUT_BITMAP_TIMES_ROMAN_24);
        else if (level == level_3) iText(625, 610, "LEVEL 3", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 255, 0); iFilledCircle(410, 515, 10);
        iSetColor(255, 0, 0); iText(425, 510, "Your name must be within 100 letters!", GLUT_BITMAP_HELVETICA_18);

        iSetColor(0, 150, 150); iFilledRectangle(400, 400, 480, 40);  // Box for saving the player name
        iSetColor(255, 255, 0); iText(425, 410, "Enter your name : ", GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(255, 255, 255); iText(625, 410, playerName, GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(0, 150, 150); iFilledRectangle(400, 300, 280, 40);  // Going back to the main menu
        iSetColor(255, 255, 0); iText(425, 310, "Back To Main Menu", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else if (page == errorPage)
    {
            if (check_game == 0) {
            iShowBMP(0, 0, "farm.bmp");
            iSetColor(0, 150, 150); iFilledRectangle(490, 500, 400, 60);
            iSetColor(255, 255, 255); iText(610, 530, "No saved game available", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(610, 510, "Please Play a new game", GLUT_BITMAP_TIMES_ROMAN_24);
            iSetColor(0, 150, 150); iFilledRectangle(620, 200, 180, 40);  // Going back to the main menu
            iSetColor(255, 255, 0); iText(645, 210, "GO BACK", GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }
    else if (page == loadingPage)
    {
        iShowBMP(0, 0, "menu_scr.bmp");
        iShowBMP2(100, 580, "game_title.bmp", 0x00ffffff);
        iSetColor(0, 150, 150); iFilledRectangle(500, 490, 450, 40);
        iSetColor(255, 255, 255); iText(520, 500, "CATCH THE EGG! IT'S SUPER EASY", GLUT_BITMAP_TIMES_ROMAN_24);
        iRectangle(490, 200, 400, 60);
        iSetColor(255, 150, 0); iFilledRectangle(490, 200, load_dx, 60);
        iSetColor(0, 150, 150); iFilledRectangle(80, 100, 270, 40);
        iSetColor(255, 255, 255); iText(85, 110, "LOADING.....", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else if (page == demoPage){
        iShowBMP(0, 0, "menu_scr.bmp");
        iSetColor(0, 128, 0); iFilledRectangle(0, 700, 1360, 40); // Bamboo
        iSetColor(255, 255, 255); iText(650, 710, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
        for (int i=300; i<=1100; i+=100) iCircle(i, 240, 10);
        if (demo[0]) { iSetColor(0, 0, 255); iFilledCircle(300, 240, 10); iShowBMP(340, 260, "demo1.bmp"); }
        else if (demo[1]) { iSetColor(0, 0, 255); iFilledCircle(400, 240, 10); iShowBMP(340, 260, "demo2.bmp"); }
        else if (demo[2]) { iSetColor(0, 0, 255); iFilledCircle(500, 240, 10); iShowBMP(340, 260, "demo3.bmp"); }
        else if (demo[3]) { iSetColor(0, 0, 255); iFilledCircle(600, 240, 10); iShowBMP(340, 260, "demo4.bmp"); }
        else if (demo[4]) { iSetColor(0, 0, 255); iFilledCircle(700, 240, 10); iShowBMP(340, 260, "demo5.bmp"); }
        else if (demo[5]) { iSetColor(0, 0, 255); iFilledCircle(800, 240, 10); iShowBMP(340, 260, "demo6.bmp"); }
        else if (demo[6]) { iSetColor(0, 0, 255); iFilledCircle(900, 240, 10); iShowBMP(340, 260, "demo7.bmp"); }
        else if (demo[7]) { iSetColor(0, 0, 255); iFilledCircle(1000, 240, 10); iShowBMP(340, 260, "demo8.bmp"); }
        else if (demo[8]) { iSetColor(0, 0, 255); iFilledCircle(1100, 240, 10); iShowBMP(340, 260, "demo9.bmp"); }
        iSetColor(0, 150, 150); iFilledRectangle(120, 140, 180, 40);  // Going back to the main menu
        iSetColor(255, 255, 0); iText(145, 150, "GO BACK", GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

void iMouseMove(int mx, int my)
{
    if (click && mx>(basket_len/2) && mx<1360-(basket_len)/2) basket_x = mx-(basket_len / 2);
}

void iMouse(int button, int state, int mx, int my)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (page == menuPage){  // The menu screen buttons
        if (mx>=1080 && mx<=1270 && my>=310 && my<=370) {
                check_for_game();
                if (check_game == 1) {
                        loadGameState();
                        page = gamePage;
                }
                else page = errorPage;
            }// The resume button in the menu screen
        if (mx>=1080 && mx<=1270 && my>=100 && my<=170) { saveGameState(); exit(0); }
        if (mx>=1080 && mx<=1270 && my>=400 && my<=460) page = levelPage; // Menu > Level Selection
        if (mx>=1080 && mx<=1270 && my>=200 && my<=260) page = seehighscorePage;
        if (mx>=80 && mx<= 280 && my>=100 && my<=170) showInstruction = showInstruction ? 0 : 1;
        if (mx>=80 && mx<=280 && my>=40 && my<=100) page = demoPage;
        }

        /// New Game command > PlayScreen
        if (page == levelPage){  // Choosing different levels
        if (mx>=620 && mx<=800 && my>=500 && my<= 540) { timer_second = 3*60 - 1; timer[8] = '3'; page = gamePage; level = level_1; }
        if (mx>=620 && mx<=800 && my>=400 && my<= 440) { timer_second = 2*60 - 1; timer[8] = '2'; page = gamePage; level = level_2; }
        if (mx>=620 && mx<=800 && my>=300 && my<= 340) { timer_second = 1*60 - 1; timer[8] = '1'; page = gamePage; level = level_3; }
        //if (timer[8] != '0') { timer[12] = '0'; timer[13] = '0'; }
        if (mx>=620 && mx<=800 && my>=200 && my<=240) page = menuPage;  // Level Selection > Menu
        if (page == gamePage){
              chicken_x = 680;
              basket_x = 680;
              // ------------------
              e[0] = {chicken_x, chicken_y+40}; // Resetting the position of the egg
              e[1] = {chicken_x, chicken_y+40};
              start_egg_2 = 0;
              // ------------------ resetting timer
              timer[12] = '0';
              timer[13] = '0';
              // ------------------
              score[13] = '0';  // Resetting the score board 000000
              score[12] = '0';
              score[11] = '0';
              score[10] = '0';
              add_score = 0; // resetting the score bar
              // ------------------
              reset_perk_position();
              // ------------------
              perkType = -1;
              // ------------------
            }
        }

        if (page == gamePage){  // Function of the play pause button on the play screen
        if (mx>=1300 && mx<=1340 && my>=540 && my<=580) resume = resume ? 0 : 1; // Pause and Resume
        if (mx>=1280 && mx<=1340 && my>460 && my<=520) {
                page = menuPage;
                resume = 0;
                saveGameState();
                set_game_to_value();
            } // Play screen > Menu screen

        if (mx>=basket_x && mx<=basket_x+basket_len && my>=40 && my<=130) {
                resume = 1; click = 1; PlaySound("click_basket.wav", NULL, SND_ASYNC);
        }
        else click = 0; // Clicking on the basket
        }

        if (page == seehighscorePage){
            if (mx>=620 && mx<=820 && my>=500 && my<= 540) { level = level_1; show_scores(); }
            if (mx>=620 && mx<=820 && my>=400 && my<= 440) { level = level_2; show_scores(); }
            if (mx>=620 && mx<=820 && my>=300 && my<= 340) { level = level_3; show_scores(); }

            if (mx>=620 && mx<=800 && my>=200 && my<=240) page = menuPage;  // High scores page > Menu
        }

        if (page == savenamePage){
            if (mx>=400 && mx<=680 && my>=300 && my<=340)
            {
              page = menuPage; // save name Page -> menuPage
              reset_play_screen();
              set_game_to_null();
              strcpy(playerName, "");
            }
        }

        if (page == showhighScore){
            if (mx >= 450 && mx <= 900 && my >= 60 && my <= 100) page = seehighscorePage;
        }
        if (page == errorPage){
            if (mx >= 620 && mx <= 800 && my >= 200 && my <= 240) page = menuPage;
        }
        if (page == demoPage){
            if (mx >= 120 && mx <= 300 && my >= 140 && my <= 200) page = menuPage;
            idx = 0;
        }

    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {

    }
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    if (key == 'q'){
        saveGameState();
        exit(0);
    }
    if (page == savenamePage){
        set_game_to_null();
        if (key == '\r') { save_score_and_name(); page = menuPage; name_len = 0; reset_play_screen(); }// Save score function writes the score in a file
                            // reset_play_screen() -> resets the variables of the play screen
        if (key == '\b'){
            name_len--;
            playerName[name_len] = '\0';
            if (name_len <= 0) name_len = 0;
        }
        else{
            playerName[name_len] = key;
            name_len++;
        }
    }
//    if (key == 'a') saveGameState();
}

void iSpecialKeyboard(unsigned char key)
{

    if(key == GLUT_KEY_END)
    {
        exit(0);
    }
    if (key == GLUT_KEY_RIGHT && basket_x<1240 && resume) basket_x += 40;
    if (key == GLUT_KEY_LEFT && basket_x>0 && resume && basket_x>0) basket_x -= 40;
    if (basket_x < 0) basket_x = 10;
    if (page == demoPage){
        if (key == GLUT_KEY_RIGHT) {
            for (int i=0; i<9; i++) {
                demo[i] = 0;
            }
            if (idx <= 7) demo[++idx] = 1;
            demo[idx] = 1;
        }
        if (key == GLUT_KEY_LEFT) {
            for (int i=0; i<9; i++) {
                demo[i] = 0;
            }
            if (idx >= 1) demo[--idx] = 1;
            demo[idx] = 1;
        }
    }
}

void chicken_move()
{
    if (resume){
    if (page == gamePage) chicken_x += chicken_dis;
    if (chicken_x>=1160 || chicken_x <= 100 || rand()%50==11) chicken_dis *= -1; // random movement must be included
    }
}
void eggfall1()
{
    if (resume){

    if (wind_duration < 10 && e[0].egg_x < screen_length-50) e[0].egg_x += egg_dx; /// adding wind effect

    if (e[0].egg_y>50) e[0].egg_y -= egg_dy;
    else {
        for (int i=0; i<=7; i++) egg_catch[i] = 0;
        e[0].egg_i = rand()%8;
        egg_catch[e[0].egg_i] = 1;
        e[0].egg_y = chicken_y; e[0].egg_x = chicken_x+40;
        }
    }
}
void eggfall2()
{
    if (e[0].egg_y <= 380 && !start_egg_2) { start_egg_2 = 1; e[1] = {chicken_x, chicken_y+40}; }
    if (resume && start_egg_2){

    if (wind_duration < 10 && e[1].egg_x < screen_length-50) e[1].egg_x += egg_dx;

    if (e[1].egg_y>50) e[1].egg_y -= egg_dy;
    else {
        for (int i=0; i<=7; i++) egg_catch[i] = 0;
        e[1].egg_i = rand()%8;
        egg_catch[e[1].egg_i] = 1;
        e[1].egg_y = chicken_y; e[1].egg_x = chicken_x+40;
    }
    }
}
 /// There can a option that with the decay of time the egg falling time decreases
void second()
{
    if (resume && page == gamePage){

    if (timer_second >= 0){
    timer[8] = 48 + (timer_second / 60);
    timer[12] = 48 + ((timer_second % 60) / 10);
    timer[13] = 48 + (timer_second % 60) % 10;
    timer_second--;
    }
    else { page = savenamePage; PlaySound("game_over.wav", NULL, SND_ASYNC); }

        { // Timer section for perks
            if (perk_timer > 0){
            perk_timer--; // It states the duration of the perk
            perk_remaining_timer[2] = 48+(perk_timer % 10);
            perk_remaining_timer[1] = 48+((perk_timer / 10) % 10);
            perk_remaining_timer[0] = 48+((perk_timer / 100) % 10);
            if ((perkType == perk_larger_basket || perkType == perk_smaller_basket)
                 && perk_timer == 0) basket_len = 120;
            if (perkType == perk_extra_time && perk_timer == 9) timer_second += 10;
            if ((perkType == perk_slow_down_egg_fall || perkType == perk_speed_up_egg_fall)
                 && perk_timer == 0) egg_dy = 15;
            }
        }

        if (perk_load_timer > 0) perk_load_timer--;

        if (wind_duration >= 0) wind_duration--;
        else wind_duration = 30;
    }
}

void perkfall()
{
    if (resume && perk_load_timer <= 0){
    perk[perkNum].perk_y -= 8;

    if (perk[perkNum].perk_y < 45) { // 45 is less than the basket catch of the perk
        reset_perk_position();

    for (int i=0; i<3; i++) perk[i].show = 0; // Setting all perk to invisible
    perk_load_timer = 10 + (rand() % 10); // Loading perk after random time , minimum delay is 10 and max is 19
    perkNum = rand() % 5; // Generating random perk
    perk[perkNum].show = 1; // Setting the perk as visible
        }
    }
    if (page == loadingPage){
        load_dx += 10;
        if (load_dx >= 400) page = menuPage;
    }
}


int main()
{
    iSetTimer(5, perkfall);
    iSetTimer(5, eggfall1);
    iSetTimer(15, eggfall2);
    iSetTimer(5, chicken_move);
    iSetTimer(1000, second);
    iInitialize(1360, 760, "Catch The Egg");
    return 0;
}
