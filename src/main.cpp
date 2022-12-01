/*
Projet S1 2022 - Équipe P-24: Memorius
Testing file for final project
@author Marc-Olivier Fecteau, Thomas-Xavier Guimont, Xavier Gervais, Ethan Beaudoin, Pier-Olivier Tarte, Philippe F. Leclerc
@version 1.0 11/20/2022
*/

/******************************** Included librairies/files ********************************/

#include <Arduino.h>
#include <LibRobus.h>
#include <LiquidCrystal_I2C.h> // LCD functions
#include <wire.h> // LCD functions
#include <SoftwareSerial.h> // MP3 functions
#include <DFRobotDFPlayerMini.h> // MP3 module library
#include "ComMarketing.h" // Function for basic communication protocol between two Arduinos
#include "Constants.h" // Defines

/******************************** Debugging/Configuration ********************************/

//#define IO_QUICK_CHECK
//#define IO_CHECKER_DEBUG
//#define DEV_PROMPTS
//#define COMM_DEBUG
//#define AUDIO_DEBUG
#define SPECIAL_CHARACTERS // Compile special characters (mostly here so we can collapse the section)
#define GAME // Run game in loop

/******************************** Global variables ********************************/

int score = 0;  // The user's score
unsigned long prevTime = 0; // Countdown timer reset
unsigned long blinkPrevTime = 0; // Countdown timer for the blinkingLED

LiquidCrystal_I2C lcd(0x27, COLS, ROWS); // Create object for LCD display

SoftwareSerial mySoftwareSerial(MP3_RX, MP3_TX); // RX/TX pins for MP3 module
DFRobotDFPlayerMini myDFPlayer; // TO DO: understand what this function does

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

/******************************** Functions declaration ********************************/

// Convert the button number to the corresponding pin on the Arduino
int ButtonToLEDPin(int buttonNumber);

// Convert the button number to the corresponding button pin on the Arduino
int ButtonToPin(int buttonNumber);

/*** Game functions ***/
void game();
bool wannaPlay(); // Prompts the user to start a game (10 seconds timeout)
void outputTargetLED(int button); // Briefly turn ON the corresponding button's LED, then turn it OFF.
void blinkingLED(int buttonNumber); // Periodically switch an LED's state (500 ms)
int inputChecker(int targetButton);  // Detect user input, identify input and check if input is correct
bool timeout(); // Timeout function
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
void audioSetup();

/*** Audio Module function ***/
void playRandomSoundFolder(int NumFolder);


/******************************** Arduino functions ********************************/

void setup()
{
  // Baud rate initalization (for serial monitor prompts)
  Serial.begin(9600);

  // Initialize the buttons matrix
  buttonsPinsInit();

  // Initialize LCD display
  LCDInit();

  // Initialize the MP3 module
  audioSetup();

#ifdef DEV_PROMPTS
  // For user acknowledgment that setup is completed
  Serial.println("Setup completed: Test can start.");
#endif

}

void loop()
{
Afficher_yeux_bouche();
#ifdef GAME
  // Send signal to find human
  comMarketing(GAMEOVER);

  if (comMarketing(READ) == HUMAN_FOUND)  // Signal received from first Arduino to start the game
  {
    // Clear screen
    lcd.clear();
    delay(250);

    // Greet the user
    PrintLCD(0, "Bonjour");

    // Ask user if he wants to play
    if (wannaPlay())
    {
      // Turn off blinking LED
      digitalWrite(ButtonToLEDPin(5), LOW);
      game();
    }
    else
    {
      byeBye();
    }
  }
  else 
  {
    playRandomSoundFolder(R2_D2);
  }
#endif

#ifdef IO_QUICK_CHECK
  // Check for buttons matrix's I/O configuration
  checkButtonMatrix();
#endif

#ifdef IO_CHECKER_DEBUG
  int buttonNumber = 1; // Button to check input for

  // Check for input on buttons matrix
  while (buttonNumber < 10)
  {
    if (inputChecker(buttonNumber))
    {
      buttonNumber++;
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
  int buttonID = ButtonToLEDPin(button);

  // Briefly turn ON the LED, then turn OFF the LED.
  digitalWrite(buttonID, HIGH);
  delay(500);
  digitalWrite(buttonID, LOW);
  delay(100);
}

int ButtonToLEDPin(int buttonNumber)
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

int ButtonToPin(int buttonNumber)
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
  //delay before the game starts
  delay(1000);

  // Reset score
  score = 0;

  int sequence[MAX]; // The sequence generated by the P-RNG

  // Generate a new seed based on program's uptime
  srand(millis());

  // Generate a sequence of numbers ∈ [1, 9]
  for (int i = 0; i < MAX; i++)
  {
    sequence[i] = rand() % 9 + 1;

#ifdef DEV_PROMPTS

    PrintLCD(0, "Sequence generated.");

#endif

  }

  /* Entering main loop */
  while (true)
  {
    // Clear screen
    lcd.clear();
    delay(10);

    /* Output sequence to buttons matrix */
    for (int i = 0; (i <= score) & (i < MAX); i++)
    {
      myDFPlayer.playFolder(NOTE, sequence[i]);
      outputTargetLED(sequence[i]);

#ifdef DEV_PROMPTSsecondchance
      Serial.print(sequence[i]);
      Serial.print("\t");
#endif

    }
    //Serial.println();

    /* Output message to input sequence */
    PrintLCD(0, "Entrez");
    PrintLCD(1, "la sequence:");
    //delay(100); // Delay for screen to show message

    if (score < MAX)
    {
      int j = 0; // Sequence increment

      while (j <= score)
      {
        switch (inputChecker(sequence[j]))
        {
        case 0:
          j++;
        case 1:
          rewardCheck(score);

          // End game
          return;
        case 2:
          byeBye();

          // End game
          return;
        }
      }

      // All inputs were correct: Increase score and go to next level.
      score++;
    }
    else // User has beaten the game
    {
      rewardCheck(MAX);
    }

    // Show user the input sequence was correct
    //lcd.clear();
    //PrintLCD(0, "Correct");

    // Delay between two levels
    delay(400);
  }
}

void rewardCheck(int score)
{
  // Show score on screen
  PrintLCD(0, "Score: ");
  lcd.setCursor(13, 0);
  lcd.print(score);

  if (score >= FINAL_BOSS_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(FINAL_BOSS_CODE);

     // Play final boss soundtrack
    myDFPlayer.playFolder(WIN, 4);

    comMarketing(READ);

    // Send signal to find another human
    byeBye();
  }
  else if (score >= THIRD_REWARD_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(THIRD_REWARD_CODE);

    // Play third reward soundtrack
    myDFPlayer.playFolder(WIN, 3);

    comMarketing(READ);

    // Send signal to find another human
    byeBye();
  }
  else if (score >= SECOND_REWARD_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(SECOND_REWARD_CODE);

    // Play second reward soundtrack
    myDFPlayer.playFolder(WIN, 2);

    comMarketing(READ);

    // Send signal to find another human
    byeBye();
  }
  else if (score >= FIRST_REWARD_SCORE)
  {
    // Send signal to first Arduino
    comMarketing(FIRST_REWARD_CODE);

    // Play first reward soundtrack
    myDFPlayer.playFolder(WIN, 1);

    comMarketing(READ);

    // Send signal to find another human
    byeBye();
  }
  else if (firstTry) // If second chance token is available
  {
    myDFPlayer.playFolder(LOSE, 1);
    delay(1500);
    // Offer user a second chance
    secondChance();
  }
  else // Bye-Bye now!
  {
    // Find another user
    byeBye();
  }
}

void secondChance()
{

  // Void the second chance token
  firstTry = false;

  // Time given to user to press the middle button (ms)
  unsigned long secondChanceTimeoutInterval = 10000;

  // Clear the screen
  lcd.clear();

  // Offer a second chance
  PrintLCD(0, "Reessayer?");
  PrintLCD(1, "(Bouton central)");

  // Light up middle bouton's LED
  blinkingLED(5);

tryAgainCheck:
  // Check for timeout
  if (millis() - prevTime <= secondChanceTimeoutInterval)
  {
    blinkingLED(5);

    // Check for input
    if (digitalRead(K22))
    {
      // Reset middle button's LED's state
      digitalWrite(K22_LED, LOW);

      // Clear screen
      lcd.clear();

      // Show new game message
      PrintLCD(0, "Deuxieme chance!");

      // Start a new game. The token has been voided by this point, so the user can't play indefinitely.
      delay(3000); // Delay before next game starts
      game();
    }
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

bool timeout()
{
  // Clear the screen
  lcd.clear();

  // Signal user they took too long to press a button
  PrintLCD(0, "Trop long!");

  if (score > 0) // User has tried to play
  {
    // Check for reward
    return false;
  }
  else // J-P is trying to fuck up the robot
  {
    return true;
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
    if (digitalRead(ButtonToPin(i))) // If input is HIGH, corresponding LED is HIGH
    {
      // Acknowledge input by lighting up pressed button's LED
      digitalWrite(ButtonToLEDPin(i), HIGH);

/*** Debugging section ***/
#ifdef IO_QUICK_CHECK

      // Show button ID on screen
      lcd.clear();
      lcd.print("Button pressed:");
      lcd.setCursor(15, 0);
      lcd.print(i);
      delay(1000);
      lcd.clear();

#endif

      // Reset pressed button's LED's state
      digitalWrite(ButtonToLEDPin(i), LOW);
    }
  }
}

int inputChecker(int targetButton)
{
  unsigned long timeoutInterval = 10000; // Time given to user to press a button (ms)

  // Reset timer
  prevTime = millis();

recheck:
  if (millis() - prevTime > timeoutInterval) // Start timer for input timeout
  {

#ifdef DEV_PROMPTS
    // Show user took too long to press a button
    Serial.println("Input timeout.");
#else

    if (timeout()) // J-P has tried messing with the robot
    {
      // Show J-P we know it's him
      PrintLCD(0, "ALEX ET JP TIME");
      myDFPlayer.playFolder(8, millis() % 2 + 1);

      delay(8000);
      return 2; // J-P messing with us, no reward
    }
    else
    {
      return 1;
    }

#endif
  }
  else // Input timeout not reached
  {
    for (int i = 1; i < 10; i++) // Go through every button
    {
      if (digitalRead(ButtonToPin(i))) // Detected an input
      {
        delay(10);

        // Play corresponding sound
        myDFPlayer.playFolder(NOTE, i);

        // (Shorter) time given to user to release the button (ms)
        timeoutInterval = 5000;

        // Reset timer
        prevTime = millis();

        while (digitalRead(ButtonToPin(i))) // Wait for user to release button
        {
          if (millis() - prevTime > timeoutInterval) // Start timer for release timeout
          {

#ifdef DEV_PROMPTS
            // Show user has pressed the button too long (dev)
            Serial.println("Release timeout.");
#else
            // Reset pressed button's LED's state
            digitalWrite(ButtonToLEDPin(i), LOW);

            // J-P has tried messing with the bot
            PrintLCD(0, "ALEX ET JP TIME");
            myDFPlayer.playFolder(8, millis() % 2 + 1);

            delay(8000);

            return 2; // JP's messing with us, no reward
#endif
          }
          else // Release timeout not reached
          {
            // Acknowledge input by lighting up pressed button's LED
            digitalWrite(ButtonToLEDPin(i), HIGH);
          }
        }
        delay(10);

        // Reset pressed button's LED's state
        digitalWrite(ButtonToLEDPin(i), LOW);

        /*** Debugging section ***/
#ifdef IO_CHECKER_DEBUG
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
          PrintLCD(0, "Partie terminee");
          delay(2000); // Delay for user acknowledgement
#endif

          // End function
          return 1;
        }
        else // Input is correct
        {
#ifdef DEV_PROMPTS
          // Show user the input was correct
          Serial.print("OK\t");
#endif
          // End function
          return 0;
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

#ifdef DEV_PROMPTS
  Serial.println("Initializing LCD screen ... (May take 3~5 seconds)");
#endif

  // Specify the number of columns and rows of the LCD display
  lcd.begin(COLS, ROWS);

  // Initialize the display
  lcd.init();

  // Light up the screen's backlight
  lcd.backlight();

  // Create default eye character
  lcd.createChar(EYE, eye);

  // Create default mouth characters
  lcd.createChar(NORMAL_MOUTH_C, bouche_centre);
  lcd.createChar(NORMAL_MOUTH_1L, bouche_niveau1_gauche);
  lcd.createChar(NORMAL_MOUTH_2L, bouche_niveau2_gauche);
  lcd.createChar(NORMAL_MOUTH_3L, bouche_niveau3_gauche);
  lcd.createChar(NORMAL_MOUTH_1R, bouche_niveau1_droit);
  lcd.createChar(NORMAL_MOUTH_2R, bouche_niveau2_droit);
  lcd.createChar(NORMAL_MOUTH_3R, bouche_niveau3_droit);

  // Added delay for LCD initialization completion
  delay(3000);

  // Show normal eye and mouth
  Afficher_yeux_bouche();

  // Place cursor to default position
  lcd.home();
}

int Longueur_chaine(char texte[])
{
  int Longueur_chaine = 0;

  while (texte[Longueur_chaine] != '\0')
  {
    Longueur_chaine += 1;
  }
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
  // Clear screen
  //lcd.clear();
  delay(50);//250 bon

  // Placer le curseur
  lcd.setCursor(Curseur, ligne);

  // Afficher le message
  lcd.print(texte);
  delay(50);
}

void Afficher_yeux_bouche()
{
  // Yeux
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.write(EYE);
  lcd.setCursor(9, 0);
  lcd.write(EYE);

  // Bouche
  lcd.setCursor(4, 1);
  lcd.write(NORMAL_MOUTH_3L);
  lcd.setCursor(5, 1);
  lcd.write(NORMAL_MOUTH_2L);
  lcd.setCursor(6, 1);
  lcd.write(NORMAL_MOUTH_1L);
  lcd.setCursor(7, 1);
  lcd.write(NORMAL_MOUTH_C);
  lcd.setCursor(8, 1);
  lcd.write(NORMAL_MOUTH_C);
  lcd.setCursor(9, 1);
  lcd.write(NORMAL_MOUTH_1R);
  lcd.setCursor(10, 1);
  lcd.write(NORMAL_MOUTH_2R);
  lcd.setCursor(11, 1);
  lcd.write(NORMAL_MOUTH_3R);
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

void audioSetup()
{
  mySoftwareSerial.begin(9600);

  Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");

  // Check if SD card is missing
  if (!myDFPlayer.begin(mySoftwareSerial))
  {
    // Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1. Please recheck the connection!");
    Serial.println("2. Please insert the SD card!");
    while (true)
      ;
  }

  // For user acknoledgment of MP3 initialization completion
  Serial.println("DFPlayer Mini online.");

  // Set serial communictaion time out to 500ms
  myDFPlayer.setTimeOut(500);

  // Set volume value (0~30).
  myDFPlayer.volume(DEFAULT_VOLUME);

  // We don't need a custom EQ, so normal EQ is chosen
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);

  // Setup output device
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  // Added delay for module initialization completion
  delay(3000);
}

void playRandomSoundFolder(int NumFolder)
{
  int randomFile; // File number to play
  int randomSoundInterval = 20000;

  if (millis() - prevTime > randomSoundInterval)
  {
    // Reset timer
    prevTime = millis();

    // Coin flip
    if (rand() % 2) // Play random R2-D2 noises
    {
      myDFPlayer.playFolder(NumFolder, randomFile);
    }
  }
}

bool wannaPlay()
{
  lcd.clear();
  PrintLCD(0, "Voulez-vous");
  PrintLCD(1, "jouer avec moi?");

  blinkingLED(5);

  delay(2000);

  lcd.clear();
  PrintLCD(0, "Appuyer sur le");
  PrintLCD(1, "bouton central");

  int intervalInvitation = 10000;

  // Reset timer
  prevTime = millis();

  while (!digitalRead(ButtonToPin(5)))
  {
    if (millis() - prevTime > intervalInvitation) // Timeout reached
    {
      return false;
    }
    blinkingLED(5);
  }
  return true;
}

void blinkingLED(int buttonNumber)
{
  int blinkingInterval = 1000; // Blinking interval in ms

  if (millis() - blinkPrevTime <= blinkingInterval)
  {
    if (millis() - blinkPrevTime <= blinkingInterval/2)
    {
      digitalWrite(ButtonToLEDPin(buttonNumber), HIGH);
    }
    else
    {
      digitalWrite(ButtonToLEDPin(buttonNumber), LOW);
    }
  }
  else
  {
    // Reset timer
    blinkPrevTime = millis();
  }
}

