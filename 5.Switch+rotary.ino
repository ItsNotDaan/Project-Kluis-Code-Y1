/*
  Name:
  Date:
  Author: Daan Heetkamp

  Description:

  Revision:

*/

// ----- include libraries -----

// ----- Declare Constants -----

// ----- Declare Objects -----

// ----- Declare subroutines and/or functions -----

// ----- Declare Global Variables -----


/**************Switch Register*******************/
int klokPin = 2;  // Latch pin of 74HC595 is connected to Digital pin 5 SRCLK
int latchPin = 3;  // Clock pin of 74HC595 is connected to Digital pin 6 RCLK
int dataPin = 4;  // Data pin of 74HC595 is connected to Digital pin 4 SER

byte data;    //Slaat de data van de bcd op.

/**************BCD*******************/
int TransBCD1 = 5;
int TransBCD2 = 6;
int TransBCD3 = 7;

/**************Rotaryencoder*******************/
unsigned long huidigeTijd;

int rotaryKnop = 8;
int rotaryLinks = 9;
int rotaryRechts = 10;

byte bcdWaarde[3] = {0, 0, 0}; //Maak een lege array met nummers.
byte rotaryWaarde = 0; //Dit slaat de huidige waarde van de rotary encoder op.
byte displayWaarde = 0;
byte laatsteDraaiwaarde = 0; //Dit is om te ontdekken of er gedraaid wordt.
byte bcdLine = 0; //Dit is om te zorgen dat de waardes niet tussen 0 en 9 komen.

bool laatsteGedrukt = false;

// Setup
void setup()
{
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(klokPin, OUTPUT);

  pinMode(TransBCD1, OUTPUT);
  pinMode(TransBCD2, OUTPUT);
  pinMode(TransBCD3, OUTPUT);

  pinMode(rotaryKnop, INPUT_PULLUP);
  pinMode(rotaryLinks, INPUT);
  pinMode(rotaryRechts, INPUT);

  laatsteDraaiwaarde = digitalRead(rotaryLinks); //Sla waarde op van de laatste positie.

  data = B00000000; //0
  updateShiftRegister(); //Register is leeg
}

// Main loop
void loop()
{
  /**************Rotaryencoder knop*******************/
  if(laatsteGedrukt == HIGH && digitalRead(rotaryKnop) == LOW) //Is er gedrukt?
  {
    bcdWaarde[bcdLine] = rotaryWaarde; //De waarde van de knop de waarde van bcdLine.
    bcdLine++;
    if(bcdLine == 3)// Als die 4 is moet die naar nummer 1 gaan.
    {
      bcdLine = 0;
    }
    rotaryWaarde = bcdWaarde[bcdLine];
    Serial.println("Knop is gedrukt");
    //delay(100);
  }
  laatsteGedrukt = digitalRead(rotaryKnop);

  /**************Rotaryencoder*******************/

  if(digitalRead(rotaryLinks) != laatsteDraaiwaarde)
  {
    if(digitalRead(rotaryRechts) == laatsteDraaiwaarde)
    {
      if(rotaryWaarde == 18){}
      else
      {
        rotaryWaarde++;
        Serial.print(rotaryWaarde);
      }
    }
    else
    {
      if(rotaryWaarde == 0){}
      else
      {
        rotaryWaarde--;
        Serial.print(rotaryWaarde);
      }
    }
  }

  laatsteDraaiwaarde = digitalRead(rotaryLinks);
  /**************Rotary data to Register*******************/

  for(int i = 0; i < 3; i++) //Doe 3 keer
  {

    if(bcdLine == i) //Is deze geselecteerd?
    {
      data = rotaryWaarde; //Stop de waarde in data
    }
    else
    {
      data = bcdWaarde[i]; //Stop de opgeslagen waarde in data.
    }

    data =  data / 2;

    digitalWrite(TransBCD1, LOW); //Zet alles uit
    digitalWrite(TransBCD2, LOW);
    digitalWrite(TransBCD3, LOW);

    switch(i) //Zet seven segment aan welke data nodig heeft..
    {
      case 0:
        digitalWrite(TransBCD1, HIGH);
        delay(1); //Dit is om het andere scherm te laten doven.
      break;
      case 1:
        digitalWrite(TransBCD2, HIGH);
        delay(1); //Dit is om het andere scherm te laten doven.
      break;
      case 2:
        digitalWrite(TransBCD3, HIGH);
        delay(1); //Dit is om het andere scherm te laten doven.
      break;
    }
    updateShiftRegister(); //PLaatst de data op het actieve 7-segmenten display.


  }
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, klokPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}
