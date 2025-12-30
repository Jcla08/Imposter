#include <LiquidCrystal.h> 
#include <Encoder.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

// char* getSport() {
//   char* sports[] = {
//     "Hockey",
//     "Volleyball",
//     "Rugby",
//     "Lacrosse",
//     "Cricket",
//     "Handball",
//     "Tennis",
//     "Golf",
//     "Boxing",
//     "Wrestling",
//     "Track & Field",
//     "Swimming",
//     "Gymnastics",
//     "Martial Arts",
//     "Bowling",
//     "Fencing",
//     "Surfing",
//     "Water Polo",
//     "Rowing",
//     "Sailing",
//     "Ice Skating",
//     "Skiing",
//     "Snowboarding",
//     "Curling",
//     "Dodgeball",
//     "Pickleball",
//     "Ultimate Frisbee",
//     "Disc Golf",
//     "Badminton",
//     "Table Tennis",
//     "Kickball"
//   };

//   int SPORTS_COUNT = sizeof(sports) / sizeof(sports[0]);

//   return sports[random(SPORTS_COUNT)];
// }

// Initialize the display with the interface pins:
// (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // Example pins

// Initialize encoder
Encoder myEnc(9, 8);
long oldPosition = -999;

// Initialize button
const int buttonPin = A5; // Use analog pin A0
int buttonState = 0;

//Initialize SD
bool sd = true;

void setup() {
  Serial.begin(9600);
  
  // initialize screen
  lcd.begin(16, 2); // Set up the LCD's number of columns and rows (16x2)

  // initialize encoder
  oldPosition = myEnc.read();
  pinMode(buttonPin, INPUT_PULLUP); // Treat analog pin as digital input
  // pinMode(buttonPin, INPUT);
  
  // initialize SD
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    // Don't do anything more:
    sd = false;
  } else {
    Serial.println("SD OK");
  }
  
  // initialize random
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
}

void loop() {
  // enter # of players
  lcd.print("Player # ");
  int playernum = 2;
  
  while (true) {
    buttonState = digitalRead(buttonPin); 
    if (buttonState == LOW) {
      break;
    }
    lcd.setCursor(0, 1); // Set cursor to column 0, line 1 (second line)
  
    long newPosition = myEnc.read();
    if (newPosition > oldPosition && playernum > 2) {
      playernum -= 1;
      delay(200);
      oldPosition = myEnc.read();
    } else if (newPosition < oldPosition) {
      playernum += 1;
      delay(200);
      oldPosition = myEnc.read();
    }
    lcd.print(playernum); // Print time in seconds
    lcd.print("               ");
  }
  while (digitalRead(buttonPin) == LOW) {
    
  }
  
  Serial.println("Check 1: Good");
  // choose a catagory
  File inFile = SD.open("_Index.txt");

  if (inFile) {
    Serial.println("Check 2: Good");
  } else if (SD.exists("_Index.txt")) {
    Serial.println("File Found\nError");
  }
  

  String cats[getFileLen(inFile) + 1];
  cats[0] = "Random";
  int CATSLEN = sizeof(cats)/sizeof(cats[0]);

  Serial.println("Check 3: Good");
  Serial.println(CATSLEN);

  for (int i = 1; i < CATSLEN; i++) {
    cats[i] = getLine(inFile);
    Serial.print(i);
    Serial.print(": ");
    Serial.println(cats[i]);
  }

  inFile.close();

  Serial.println("Check 4: Good");

  lcd.clear();
  lcd.print("Pick A Catagory:");
  int cur = 0;

  while (true) {
    buttonState = digitalRead(buttonPin); 
    if (buttonState == LOW) {
      break;
    }
    lcd.setCursor(0, 1); // Set cursor to column 0, line 1 (second line)
  
    long newPosition = myEnc.read();
    if (newPosition > oldPosition) {
      if (cur > 0) {
        cur -= 1;
      } else {
        cur = CATSLEN - 1;
      }
      delay(200);
      oldPosition = myEnc.read();
    } else if (newPosition < oldPosition) {
      if (cur < CATSLEN - 1) {
        cur += 1;
      } else {
        cur = 0;
      }
      delay(200);
      oldPosition = myEnc.read();
    }
    lcd.print(cats[cur]); // Print 
    lcd.print("               ");
  }
  while (digitalRead(buttonPin) == LOW) {
    
  }
  
  Serial.println("Check 5: Good");

  if (cur == 0) {
    cur = random(CATSLEN - 1) + 1;
    Serial.println(cur);
  }

  String wrd = getWord(cats[cur]);
  Serial.print("Word: ");
  Serial.println(wrd);
  // choose imposter
  
  int imp = random(playernum);

  // word reveal
  for (int i = 0; i < playernum; i++) {
    lcd.clear(); 
    lcd.print("Player ");
    lcd.print(i+1);

    waitForEnter();
    lcd.clear();
    lcd.print(cats[cur]);
    lcd.setCursor(0, 1);
    if (i == imp) {
      lcd.print("Imposter");
    } else {
      lcd.print(wrd);
    }

    waitForEnter();
  }

  // reveal imposter
  lcd.clear();
  lcd.print("Find The");
  lcd.setCursor(0, 1);
  lcd.print("Imposter!");
  delay(3000);

  lcd.clear();
  lcd.print("Click To Reveal");
  lcd.setCursor(0, 1);
  lcd.print("Imposter");
  waitForEnter();

  lcd.clear();
  lcd.print("Imposter Is");
  lcd.setCursor(0, 1);
  lcd.print("Player ");
  lcd.print(imp+1);
  delay(3000);

  lcd.clear();
  lcd.print("Click To Play");
  lcd.setCursor(0, 1);
  lcd.print("Again");
  waitForEnter();
  lcd.clear();
}

void waitForEnter() {
  while (true) {
    if (digitalRead(buttonPin) == LOW) {
      break;
    }
  }
  while (digitalRead(buttonPin) == LOW) {

  }
}

int getFileLen(File input) {
  int pos = input.position();
  input.seek(0);

  int cont = 0;
  while (true) {
    char r = input.read();
    if (r == '\n' ) {
      cont++;
    } else if (r == -1) {
      cont++;
      break;
    }
  }

  input.seek(pos);
  return cont;
}

String getLine(File input) {

  String output;
  while (input.available()) {
    char r = input.read();
    // Serial.println(r);
    if (r == '\n') break;
    else if (r != '\r') output += r;
  }

  return output;

  // 3
}

String getWord(String cat) {
  Serial.println(cat+".txt");
  File wordsFile = SD.open(cat+".txt");
  if (!wordsFile) {
    Serial.println("File Not Found");
  }

  int LEN = getFileLen(wordsFile);

  int rand = random(LEN);

  String word;

  for (int i = 0; i <= rand; i++) {
    word = getLine(wordsFile);
    Serial.print(i);
    Serial.print(": ");
    Serial.println(word);
  }
  wordsFile.close();
  return word;
}
