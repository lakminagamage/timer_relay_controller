#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Keypad.h>

// Initialize the RTC and LCD
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 4); // LCD with I2C address 0x27 for a 16x4 display

// Define the keypad size and layout
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables for timestamp management
const int MAX_TIMESTAMPS = 10;
unsigned long timestamps[MAX_TIMESTAMPS];
int timeBlobs[MAX_TIMESTAMPS];
int numTimestamps = 0;
DateTime currentTime;
int j = 0;

void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    lcd.println("Couldn't find RTC");
    delay(2000);
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, please set the time!");
    lcd.setCursor(0, 0);
    lcd.print("RTC lost");
    lcd.setCursor(0, 1);
    lcd.print("power.");
    lcd.setCursor(0, 2);
    lcd.print("please set ");
    lcd.setCursor(0, 3);
    lcd.print("the time!");
    delay(3000);
    rtc.adjust(setTimeStampUsingKeypad());
  }
  lcd.clear();
}

void loop() {
  
  displayTime(1);
  delay(1000);
  //get the timestamp only at first loop of the program
  if(j==0){
    collectTimestamps();
  }

  lcd.clear();

  currentTime = rtc.now().unixtime();
  // for each timestamp see if it is passed
  for(int i=0; i<numTimestamps; i++){

    if (currentTime > timestamps[i]){
      displayTime(timestamps[i]);
      timestamps[i] = 0;
      lcd.setCursor(0,0);
      lcd.print("Timestamp ");
      lcd.print(i + 1);
      lcd.setCursor(0, 1);
      lcd.print("Has Passed!");
      lcd.setCursor(-3, 2);
      lcd.print("Turning on relay");
      digitalWrite(12, HIGH);
      digitalWrite(13, HIGH);
      for(int m=0; m<timeBlobs[i]; m++){
        delay(600);
        lcd.clear();
        lcd.setCursor(2, 1);
        lcd.print("Relays are On");
        delay(400);
        lcd.clear();

      }
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Relays are Off");
    }
  }
  j++;
}


String getUserInput() {
  String userInput = "";
  char key;
  
  do {
    key = keypad.getKey();
    if (key != NO_KEY && key != 'D' && key!='C') {
      userInput += key;
      lcd.print(key);
    }
    if (key == 'C'){
      userInput = "";
      lcd.clear();
    }
  } while (key != 'D');
  
  return userInput;
}

DateTime setTimeStampUsingKeypad() {
  String userInput;
  int year, month, day, hour, minute, second;
  
  lcd.clear();
  lcd.print("Set Year(YY)");
  delay(1000);
  lcd.clear();
  
  // Get year
  lcd.setCursor(0, 1);
  userInput = getUserInput();
  year = userInput.toInt();
  userInput = "";
  
  // Get month
  lcd.clear();
  lcd.print("Set Month (MM)");
  delay(1000);
  lcd.setCursor(3, 1);
  userInput = getUserInput();
  month = userInput.toInt();
  userInput = "";
  
  // Get day
  lcd.clear();
  lcd.print("Set Day (DD)");
  delay(1000);
  lcd.setCursor(6, 1);
  userInput = getUserInput();
  day = userInput.toInt();
  userInput = "";
  
  // Get hour
  lcd.clear();
  lcd.print("Set Hour (HH)");
  delay(1000);
  lcd.setCursor(0, 1);
  userInput = getUserInput();
  hour = userInput.toInt();
  userInput = "";
  
  // Get minute
  lcd.clear();
  lcd.print("Set Minute (MM)");
  delay(1000);
  lcd.setCursor(3, 1);
  userInput = getUserInput();
  minute = userInput.toInt();
  userInput = "";
  
  // Get second
  lcd.clear();
  lcd.print("Set Second (SS)");
  delay(1000);
  lcd.setCursor(6, 1);
  userInput = getUserInput();
  second = userInput.toInt();
  userInput = "";
  
  // Set RTC time
  DateTime time = DateTime(year, month, day, hour, minute, second);
  
  lcd.clear();
  lcd.print("Time set successfully!");
  delay(2000);
  return time;
}


void displayTime(unsigned long a) {
  //to display current time send (Int 1) as parameter
  DateTime now;
  if (a==1){
    now = rtc.now();
  }
  else{
    now = DateTime(a);
  }
  lcd.setCursor(0,0);
  lcd.print(now.year());
  lcd.print(':');
  lcd.print(now.month());
  lcd.print(':');
  lcd.print(now.day());
  lcd.print("    ");
  lcd.setCursor(0,1);
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  lcd.print(now.second());
  lcd.print("    ");
  
}



void collectTimestamps() {
  lcd.clear();
  lcd.print("Enter # of timestamps:");
  delay(2000);
  lcd.setCursor(0, 1);
  numTimestamps = getUserInput().toInt();
  
  for (int i = 0; i < numTimestamps && i < MAX_TIMESTAMPS; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Timestamp ");
    lcd.print(i + 1);
    delay(1000);
    timestamps[i] = setTimeStampUsingKeypad().unixtime();
    lcd.clear();
    lcd.println("Set Timeout ");
    lcd.setCursor(0, 1);
    lcd.println("in Seconds(S) :");
    delay(1000);
    timeBlobs[i] = getUserInput().toInt();

  }
}
