/*
Projet S1 2022 - Équipe P-24: Memorius
Testing file for final project
@author Marc-Olivier Fecteau, Thomas-Xavier Guimont, Xavier Gervais, Ethan Beaudoin, Pier-Olivier Tarte, Philippe F. Leclerc
@version 1.0 11/20/2022
*/

/******************************** Included librairies/files ********************************/

#include <Arduino.h>
#include <LibRobus.h>
#include <LiquidCrystal_I2C.h>
#include <wire.h>
#include "ComMarketing.h"
#include "Constants.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

/******************************** Debugging/Configuration ********************************/

#define IO_CHECKER_DEBUG
#define INPUT_CHECKER_DEBUG
#define DEV_PROMPTS
//#define COMM_DEBUG
//#define AUDIO_DEBUG
#define SPECIAL_CHARACTERS  // Compile special characters

/******************************** Global variables ********************************/

int score = 0;  // The user's score
unsigned long prevTime = 0; // Countdown timer reset

LiquidCrystal_I2C lcd(0x27, COLS, ROWS); // Create object for LCD display

/******************************** Special characters initialization ********************************/

#ifdef SPECIAL_CHARACTERS
// Default eye
byte eye[8] =
    {
        B11111,
        B10001,
        B10001,
        B10101,
        B10101,
        B10001,
        B10001,
        B11111};

byte bouche_centre[8] =
    {
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111};

byte bouche_niveau1_gauche[8] =
    {
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B11100,
        B00111,
        B00000};

byte bouche_niveau2_gauche[8] =
    {
        B00000,
        B00000,
        B00000,
        B11100,
        B00111,
        B00000,
        B00000,
        B00000};

byte bouche_niveau3_gauche[8] =
    {
        B10000,
        B11100,
        B00111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000};

byte bouche_niveau1_droit[8] =
    {
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00111,
        B11100,
        B00000};

byte bouche_niveau2_droit[8] =
    {
        B00000,
        B00000,
        B00000,
        B00111,
        B11100,
        B00000,
        B00000,
        B00000};

byte bouche_niveau3_droit[8] =
    {
        B00001,
        B00111,
        B11100,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000};
#endif

/******************************** Variables audio module ********************************/

#define NOTE 1
#define DANK 2
#define R2_D2 3
#define WIN 4
#define LOSE 5

SoftwareSerial mySoftwareSerial(10, 11); //Pins used to send commands to module
DFRobotDFPlayerMini myDFPlayer; 


/******************************** Functions declaration ********************************/

// Convert the button number to the corresponding pin on the Arduino
int K2LEDPin(int buttonNumber);

// Convert the button number to the corresponding button pin on the Arduino
int K2Pin(int buttonNumber);

/*** Game functions ***/
void game();
void outputTargetLED(int button); // Briefly turn ON the corresponding button's LED, then turn it OFF.
bool inputChecker(int targetButton);  // Detect user input, identify input and check if input is correct
void timeout(); // Timeout function
void rewardCheck(int score); // Determine the user's reward based on score
void secondChance();  // Offer the user a second chance (10 seconds timeout)
void byeBye();  // Find another user

/*** Debugging/Testing functions ***/
void checkButtonMatrix(); // Check for buttons matrix's I/O configuration

/*** LCD functions ***/
int Longueur_chaine(char texte[]); // Calculer la longueur du message
void PrintLCD(int ligne, char texte[]); // Afficher un message sur une rangée. Ligne ∈ {0, 1}.
void Afficher_yeux_bouche(); // Afficher les yeux et la bouche par défaut

/*** Setup functions ***/
void buttonsPinsInit(); // Initialize pins' state for pins used by buttons matrix
void LCDInit(); // Configure and initialize the LCD display
void audioSetUp();

/*** Audio Module function***/

void playRandomSoundFolder(int NumFolder);

/******************************** Arduino functions ********************************/

void setup()
{
  // Baud rate initalization (for serial monitor prompts)
  Serial.begin(9600);

  // Initialize the buttons matrix
  buttonsPinsInit();

  /*** Initialize special characters ***/

  // Initialize LCD display
  LCDInit();

  // Initialize the MP3 module and speaker

  audioSetUp();
}

void loop()
{

#ifdef COMM_DEBUG
  // Send signal to find human
  comMarketing(GAMEOVER);

  if (comMarketing(READ) == HUMAN_FOUND)  // Signal received from first Arduino to start the game
  {

#ifdef DEV_PROMPTS
    // Show user a signal has been sent
    Serial.println("Signal received.");
#else
    // Clear screen
    lcd.clear();

    // Start game
    game();
#endif

  }
  else 
  {
    int Chance = rand() % 2000;
    if (Chance == 3)
    {
      playRandomSoundFolder(R2_D2);
    }
  }
#endif

#ifdef IO_CHECKER_DEBUG
  // Check for buttons matrix's I/O configuration
  checkButtonMatrix();
#endif

#ifdef INPUT_CHECKER_DEBUG
  int k = 1; // Button to check input for
  while (k < 10)
  {
    if (inputChecker(k))
    {
      k++;
    }
    else
    {
      break;
    }
  }
#endif

}

/******************************** Functions initialization ********************************/

void outputTargetLED(int button)
{
  /* From the button number, the LED of the corresponding button is lit for 500ms, then turned OFF */
  int buttonID = K2LEDPin(button);

  // Briefly turn ON the LED, then turn OFF the LED.
  digitalWrite(buttonID, HIGH);
  delay(500);
  digitalWrite(buttonID, LOW);
  delay(100);
}

int K2LEDPin(int buttonNumber)
{
  /* From the button number ∈ [1,9], output the corresponding LED digital pin on the Arduino */
  switch (buttonNumber)
  {
  case 1:
    return K11_LED;

  case 2:
    return K12_LED;

  case 3:
    return K13_LED;

  case 4:
    return K21_LED;

  case 5:
    return K22_LED;

  case 6:
    return K23_LED;

  case 7:
    return K31_LED;

  case 8:
    return K32_LED;

  case 9:
    return K33_LED;
  }
}

int K2Pin(int buttonNumber)
{
  /* From the button number [1,9], output the corresponding digital pin on the Arduino */
  switch (buttonNumber)
  {
  case 1:
    return K11;

  case 2:
    return K12;

  case 3:
    return K13;

  case 4:
    return K21;

  case 5:
    return K22;

  case 6:
    return K23;

  case 7:
    return K31;

  case 8:
    return K32;

  case 9:
    return K33;
  }
}

void game()
{
  // Reset score
  score = 0;

  /*** Local variables ***/
  int sequence[MAX]; // The sequence generated by the P-RNG

  // Generate a new seed based on program's uptime
  srand(millis());

  // Generate a sequence of numbers ∈ [1, 9]
  for (int i = 0; i < MAX; i++)
  {
    sequence[i] = rand() % 9 + 1;

#ifdef DEV_PROMPTS

    PrintLCD(0, "Initialisation");

#endif
  }

  /* Entering main loop */
  while (true)
  {
    // Clear screen
    lcd.clear();

    /* Output sequence to buttons matrix */
    for (int i = 0; (i <= score) & (i < MAX); i++)
    {
      myDFPlayer.playFolder(DANK, sequence[i]); //À tuner quand on a la plaque
      outputTargetLED(sequence[i]);  
      Serial.print(sequence[i]);
      Serial.print("\t");
    }
    Serial.println();
    delay(500);

    /* Output message to input sequence */
    PrintLCD(0, "Entrez");
    PrintLCD(1, "la sequence:");
    delay(200); // Delay for screen to show message

    // For each element of the level, check if user input is correct
    for (int j = 0; (j <= score) & (j < MAX); j++)
    {
      if (!inputChecker(sequence[j])) // Sequence input error
      {
        //Play losing sound

        myDFPlayer.playFolder(LOSE, 1);
        // Determine the reward
        rewardCheck(score);

        // End game
        return;
      }
    }

    // All inputs were correct: increase score and go to next level.
    score++;
    delay(2000);

/*** Debugging section ***/
#ifdef DEV_PROMPTS
    /* Clear screen and show the input sequence is correct */
    lcd.clear();
    PrintLCD(0, "Correct");
    delay(200); // Delay for screen to show message
#endif
  }
}

void rewardCheck(int score)
{
  /* Show score on screen */
  PrintLCD(0, "Score: ");
  lcd.setCursor(13, 0);
  lcd.print(score);

  if (score >= FINAL_BOSS_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(FINAL_BOSS_CODE);

    comMarketing(READ);

    myDFPlayer.playFolder(WIN, 4);

    // Send signal to find another human
    byeBye();
  }
  else if (score >= THIRD_REWARD_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(THIRD_REWARD_CODE);

    comMarketing(READ);

    myDFPlayer.playFolder(WIN, 3);

    // Send signal to find another human
    byeBye();
  }
  else if (score >= SECOND_REWARD_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(SECOND_REWARD_CODE);

    comMarketing(READ);

    myDFPlayer.playFolder(WIN, 2);

    // Send signal to find another human
    byeBye();

  }
  else if (score >= FIRST_REWARD_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(FIRST_REWARD_CODE);

    comMarketing(READ);

    myDFPlayer.playFolder(WIN, 1);

    // Send signal to find another human
    byeBye();

  }
  else if (firstTry) // If second chance token is available
  {
    // Offer user a second chance
    secondChance();
  }
  else // Bye-Bye now!
  {
    // Reactivate second chance token
    firstTry = true;

    // Find another user
    byeBye();
  }

}

void secondChance()
{

  // Void the second chance token
  firstTry = false;

  // Time given to user to press the middle button (ms)
  unsigned long interval = 10000;

  // Clear the screen
  lcd.clear();

  // Offer a second chance
  PrintLCD(0, "Reessayer?");
  PrintLCD(1, "(Bouton central)");

  // Light up middle bouton's LED
  digitalWrite(K22_LED, HIGH);

tryAgainCheck:
  // Check for timeout
  if (millis() - prevTime <= interval)
  {

    // Check for input
    if (digitalRead(K22))
    {
      // Reset middle button's LED's state
      digitalWrite(K22_LED, LOW);

      // Clear screen
      lcd.clear();

      // Show new game message
      PrintLCD(0, "Round 2: fight!");

      // Start a new game. The token has been voided by this point, so the user can't play indefinitely.
      delay(3000); // Delay before next game starts
      game();
    }
    // TO DO: switch middle button's LED's state
    else // No input
    {
      // Repeatedly check for input until second chance timeout
      goto tryAgainCheck;
    }
  }
  else // Second chance timeout
  {
    // Reset middle button's LED's state
    digitalWrite(K22_LED, LOW);

    // Find another user
    byeBye();
  }
}

void timeout()
{
  // Clear the screen
  lcd.clear();

  // Signal user they took too long to press a button
  PrintLCD(0, "Trop long!");

  if (score > 0) // User has tried to play
  {
    // Check for reward
    rewardCheck(score);
  }
  else // User was bugging Memorius
  {
    // TO DO: madgeRobot function
  }
}

void byeBye()
{
  // Reset second chance token
  firstTry = true;

  // Say "Bye-Bye!" to user
  lcd.clear();
  delay(250);
  PrintLCD(0, "Au revoir!");
}

void checkButtonMatrix()
{
  for (int i = 1; i < 10; i++)
  {
    if (digitalRead(K2Pin(i))) // If input is HIGH, corresponding LED is HIGH
    {
      // Acknowledge input by lighting up pressed button's LED
      digitalWrite(K2LEDPin(i), HIGH);

/*** Debugging section ***/
#ifdef INPUT_CHECKER_DEBUG

        // Show button ID on screen
        lcd.clear();
        lcd.print("Button pressed:");
        lcd.setCursor(15, 0);
        lcd.print(i);
        delay(1000);
        lcd.clear();

#endif

        // Reset pressed button's LED's state
        digitalWrite(K2LEDPin(i), LOW);
      }
    }
  }

bool inputChecker(int targetButton)
  {
    unsigned long interval = 10000; // Time given to user to press a button (ms)

    // Reset timer
    prevTime = millis();

  recheck:
    if (millis() - prevTime > interval) // Start timer for input timeout
    {

#ifdef DEV_PROMPTS
    // Show user took too long to press a button
    Serial.println("Input timeout.");
#else
    // Show user took too long to press a button
    PrintLCD(0, "Trop long!");
#endif

    return false; // End function
  }
  else  // Input timeout not reached
  {
    for (int i = 1; i < 10; i++)  // Go through every button
    {
      if (digitalRead(K2Pin(i))) // Detected an input
      {
        // (Shorter) time given to user to release the button (ms)
        interval = 5000;

        // Reset timer
        prevTime = millis();

        
        while (digitalRead(K2Pin(i))) // Wait for user to release button
        {
          if (millis() - prevTime > interval) // Start timer for release timeout
          {

#ifdef DEV_PROMPTS
            // Show user has pressed the button too long (dev)
            Serial.println("Release timeout.");
#else
            // Show user has pressed the button too long
            lcd.clear();
            PrintLCD(0, "Trop long!");
#endif

            // End function
            return false;
          }
          else  // Release timeout not reached
          {
            // Acknowledge input by lighting up pressed button's LED
            digitalWrite(K2LEDPin(i), HIGH);
          }
        }

        // Reset pressed button's LED's state
        digitalWrite(K2LEDPin(i), LOW);

/*** Debugging section ***/
#ifdef INPUT_CHECKER_DEBUG
        // Show button ID on serial monitor
        Serial.print("Button pressed: ");
        Serial.println(i);
#endif

        // Compare input to target
        if (i != targetButton) // Input is incorrect
        {

#ifdef DEV_PROMPTS
          // Show user the input was incorrect
          Serial.println("Input incorrect.");
#else
          // Show user the input was incorrect
          lcd.clear();
          PrintLCD(0, "Partie terminée");
          delay(2000);  // Delay for user acknowledgement
#endif

          // End function
          return false;
        }
        else // Input is correct
        {
#ifdef DEV_PROMPTS
          // Show user the input was correct
          Serial.print("OK\t");
#endif
          // End function
          return true;
        }
      }
      else if (i == 9) // No input
      {
        // Recheck until there's an input
        goto recheck;
      }
    }
  }
}

void LCDInit()
{

  // Specify the number of columns and rows of the LCD display
  lcd.begin(COLS, ROWS);

  // Initialize the display
  lcd.init();

  // Light up the screen's backlight
  lcd.backlight();

  // Create default eye character
  lcd.createChar(0, eye);

  // Create default mouth characters
  lcd.createChar(1, bouche_centre);
  lcd.createChar(2, bouche_niveau1_gauche);
  lcd.createChar(3, bouche_niveau2_gauche);
  lcd.createChar(4, bouche_niveau3_gauche);
  lcd.createChar(5, bouche_niveau1_droit);
  lcd.createChar(6, bouche_niveau2_droit);
  lcd.createChar(7, bouche_niveau3_droit);

#ifdef DEV_PROMPTS
  Serial.println("Initializing LCD screen ... (May take 3~5 seconds)");
#endif

  delay(3000); // Delay for LCD screen initialization

  // Show normal eye and mouth
  Afficher_yeux_bouche();

  // Place cursor to default position
  lcd.home();
}

int Longueur_chaine(char texte[])
{
  int Longueur_chaine=0;

  while(texte[Longueur_chaine]!='\0')
  {
    Longueur_chaine=Longueur_chaine+1;
  }
  Longueur_chaine=Longueur_chaine-1; //longueur chaine a partir de 0
  return Longueur_chaine;
}

void PrintLCD(int ligne, char texte[])
{
  int Curseur = 0; // Position du curseur

  if (Longueur_chaine(texte) < 16) // Chaîne plus petite que 16 caractères
  {
    // Centrer le message
    Curseur = (16 - (Longueur_chaine(texte))) / 2;
  }
  else // Chaîne 16 caractères ou plus
  {
    Curseur = 0;
  }

  // Placer le curseur
  lcd.setCursor(Curseur, ligne);

  // Afficher le message
  lcd.print(texte);
}

void Afficher_yeux_bouche()
{
  // Yeux
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.write(0);
  lcd.setCursor(9, 0);
  lcd.write(0);

  // Bouche
  lcd.setCursor(4, 1);
  lcd.write(4);
  lcd.setCursor(5, 1);
  lcd.write(3);
  lcd.setCursor(6, 1);
  lcd.write(2);
  lcd.setCursor(7, 1);
  lcd.write(1);
  lcd.setCursor(8, 1);
  lcd.write(1);
  lcd.setCursor(9, 1);
  lcd.write(5);
  lcd.setCursor(10, 1);
  lcd.write(6);
  lcd.setCursor(11, 1);
  lcd.write(7);
}

void buttonsPinsInit()
{
  // Pin initalization for buttons matrix
  pinMode(K11, INPUT);
  pinMode(K12, INPUT);
  pinMode(K13, INPUT);
  pinMode(K21, INPUT);
  pinMode(K22, INPUT);
  pinMode(K23, INPUT);
  pinMode(K31, INPUT);
  pinMode(K32, INPUT);
  pinMode(K33, INPUT);

  // Pin initalization for buttons' LEDs
  pinMode(K11_LED, OUTPUT);
  pinMode(K12_LED, OUTPUT);
  pinMode(K13_LED, OUTPUT);
  pinMode(K21_LED, OUTPUT);
  pinMode(K22_LED, OUTPUT);
  pinMode(K23_LED, OUTPUT);
  pinMode(K31_LED, OUTPUT);
  pinMode(K32_LED, OUTPUT);
  pinMode(K33_LED, OUTPUT);
}

void audioSetUp()
{
  mySoftwareSerial.begin(9600);

Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");
if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(true);
  }
  Serial.println("DFPlayer Mini online.");

  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms

  myDFPlayer.volume(10);  //Set volume value (0~30).

  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);

  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
}

void playRandomSoundFolder(int NumFolder)
{
  int x; 
  x = rand() % 9 + 1;
  myDFPlayer.playFolder(NumFolder, x);
}