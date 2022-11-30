/*
Projet S1 2022 - Équipe P-24: Memorius
Initialization file with all variables and functions related to setup
@author Marc-Olivier Fecteau, Pier-Olivier Tarte, Thomas-Xavier Guimont, Ethan Beaudoin, Xavier Gervais
@version 1.0 11/26/2022
*/

/******************************** Constants ********************************/

// Define pins for buttons. Button ID based on element in matrix.
#define K11 42
#define K12 46
#define K13 43
#define K21 40
#define K22 41
#define K23 48
#define K31 47
#define K32 45
#define K33 44

// Define pins for buttons' LEDs. Button ID based on element in matrix.
#define K11_LED 24
#define K12_LED 30
#define K13_LED 23
#define K21_LED 29
#define K22_LED 22 
#define K23_LED 27
#define K31_LED 25
#define K32_LED 28
#define K33_LED 26

// Define maximum length of the sequence
#define MAX 30

// Define score thresholds for rewards
const int FIRST_REWARD_SCORE = 5;
const int SECOND_REWARD_SCORE = 10;
const int THIRD_REWARD_SCORE = 15;
const int FINAL_BOSS_SCORE = 20;
bool firstTry = true;  // Second chance token. Voided when calling the function and reset when the game is finished.

// Define LCD dimensions
#define COLS 16 // Number of columns on LCD display
#define ROWS 2  // Number of rows on LCD display

// Define Special Characters ID
#define EYE 0   // ID du caractère "oeil"
#define NORMAL_MOUTH_1L 1   // ID du caractère "bouche_niveau1_gauche"
#define NORMAL_MOUTH_2L 2   // ID du caractère "bouche_niveau2_gauche"
#define NORMAL_MOUTH_3L 3   // ID du caractère "bouche_niveau3_gauche"
#define NORMAL_MOUTH_C 4    // ID du caractère "bouche_centre"
#define NORMAL_MOUTH_1R 5   // ID du caractère "bouche_niveau1_droite"
#define NORMAL_MOUTH_2R 6   // ID du caractère "bouche_niveau1_droite"
#define NORMAL_MOUTH_3R 7   // ID du caractère "bouche_niveau1_droite"

// Define ID for communication codes
#define READ '0'  // Task: read from first Arduino (write isn't used by second Arduino)
#define GAMEOVER '3'  // Signal to find another human
#define HUMAN_FOUND '1' // Signal to start the game
#define FIRST_REWARD_CODE '4' // Code for first reward
#define SECOND_REWARD_CODE '5' // Code for second reward
#define THIRD_REWARD_CODE '6' // Code for third reward
#define FINAL_BOSS_CODE '7' // Code for final boss reward

// Define pins for MP3 module
#define MP3_RX 10 // MP3 module reception pin
#define MP3_TX 11 // MP3 module transmission pin

// Define Folder names for MP3 Module
#define NOTE 1 // Contains sounds for the buttons matrix
#define DANK 2 // Contains dank (memes) sounds
#define R2_D2 3 // Contains R2-D2 noises
#define WIN 4 // Contains sounds/soundtracks for rewards
#define LOSE 5

// Define other constants for MP3 module
#define DEFAULT_VOLUME 10 // Set speaker volume during initialization
#define R2_D2_FILES 6 // Number of files for random R2-D2 noises

/*** Define file ID for each file ***/

// Define for "DANK"
#define ANDROID_NOTIFICATION 1
#define BRUH 2
#define DOLPHIN 3
#define WHAT_THE_DOG_DOIN 4
#define FART 5
#define ROBLOX_DEAD_SOUND 6
#define THE_ROCK_EYEBROW 7
#define TACO_BELL 8
#define WINDOWS_ERROR 9

// Define for "WIN"
#define ROBLOX_CONGRATS 1 // First reward track
#define SUPER_MARIO_BROS_FINISH 2 // Second reward track
#define THIS_IS_EPIC 3 // Third reward track
#define RICK_ROLL 4 // Final boss track