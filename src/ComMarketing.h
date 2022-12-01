/*
Projet S1 2022 - Équipe P-24: Memorius
File for basic communication protocol between the Arduinos
@author Thomas-Xavier Guimont
@version 1.1 11/27/2022
*/

unsigned long ComTime = 0;
unsigned long PrevComTime = 0;

int comMarketing(int Com)
{

    char readSerial[30];

    Serial1.begin(9600);

    if (Com == '0')
    {
            PrevComTime = millis();
            int IntervalMEMOMEMO = 5000;

            


        while (Com == '0')
        {
            if (millis() - PrevComTime > IntervalMEMOMEMO and score >= FIRST_REWARD_SCORE and score < SECOND_REWARD_SCORE)

            {
            myDFPlayer.playFolder(7, 3);
            PrevComTime=millis();
            }
            if(score==0){
            playRandomSoundFolder(R2_D2);
            }
            if (Serial1.available() > 0)
            {
                delay(10);
                for (int i = 0; Serial1.available() > 0; i++)
                {
                    readSerial[i] = Serial1.read();
                }
                Com = readSerial[0];
            }
        }

        Serial1.write('0');

#ifdef TEST_COM
        Serial.print("read");
        Serial.println((char)Com);
#endif

        return Com;
    }

    if (Com != '0')
    {

        PrevComTime = millis();

        while (Com != '0')
        {

            ComTime = millis();

            if (ComTime - PrevComTime > 200 and Com != '0')
            {

                Serial1.write((char)Com);

#ifdef TEST_COM
                Serial.print("write");
                Serial.println((char)Com);
#endif

                PrevComTime = ComTime;
            }
            else if (Serial1.available() > 0)
            {
                delay(10);
                for (int i = 0; Serial1.available() > 0; i++)
                {
                    readSerial[i] = Serial1.read();
                }
                Com = readSerial[0];
            }
        }
    }

#ifdef TEST_COM
    Serial.println((char)Com);
#endif

    return Com;
}
