// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int backlight = 10;
int red = 11;
int green = 12;
int blue = 13;
int fan = 3;

//Key message
//char msgs[5][15] = {"Right Key OK ",
//"Up Key OK ",
//"Down Key OK ",
//"Left Key OK ",
//"Select Key OK" };

bool needsRefresh = true;
bool screenDisplay = true;

int row = 0;
int col = 0;

char menu[2][15] = {"Led", "Fan"};
char power[2][15] = {"Off", "On"};
int ledPower = 1;
int fanPower = 1;
int fanSpeed = 255;
char colors[7][15] = {"White", "Blue", "Green", "Red", "Yellow", "Cyan", "Magenta"};
int color = 1;

int adc_key_val[5] = {30, 150, 360, 535, 760 };
int NUM_KEYS = 5;
int adc_key_in;
int key = -1;
int oldkey = -1;

void setup() {
  lcd.begin(16, 2);

  pinMode(backlight, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(fan, OUTPUT);

  analogWrite(fan,fanSpeed);
  analogWrite(red,0);
  digitalWrite(green,0);
  digitalWrite(blue,1);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  ///lcd.setCursor(0, 1);
  printScreen();

  adc_key_in = analogRead(0); // read the value from the sensor

  key = get_key(adc_key_in); // convert into key press

  if (key != oldkey) { // if keypress is detected
    delay(50); // wait for debounce time
    needsRefresh = true;
    adc_key_in = analogRead(0); // read the value from the sensor
    key = get_key(adc_key_in); // convert into key press
    if (key != oldkey) {
      oldkey = key;
      if (key >= 0) {
        switch (key) {
          case 0://right
            if (col < 2) {
              col++;
              return;
            }
            if (col == 2 && row == 0) color = 1;
            if (col == 2 && row == 1) cycleFanSpeed();
            break;
          case 1://up
            if (row > 0 && col == 0)row--;
            if (row == 0 && col == 1)flipInt(&ledPower);
            if (row == 1 && col == 1)flipInt(&fanPower);
            if (row == 0 && col == 2)ledColorChange(1);
            if (row == 1 && col == 2 && fanSpeed < 255)fanSpeed += 5;
            break;
          case 2://down
            if (row < 1 && col == 0)row++;
            if (row == 0 && col == 1)flipInt(&ledPower);
            if (row == 1 && col == 1)flipInt(&fanPower);
            if (row == 0 && col == 2)ledColorChange(-1);
            if (row == 1 && col == 2 && fanSpeed > 0)fanSpeed -= 5;
            break;
          case 3://left
            if (col > 0)col--;
            break;
          case 4://select
            lcd.display();
            flipBool(&screenDisplay);
            break;
          default:
            break;
        }
      }
    }
  }
}

void printScreen() {
  digitalWrite(backlight, screenDisplay);
  if (!needsRefresh)return;
  needsRefresh = false;
  if (screenDisplay == false) {
    lcd.noDisplay();
    return;
  }
  lcd.clear();
  if (col == 0) {
    printMenu();
  }
  else {
    if (row == 0) {
      printLeds();
    }
    if (row == 1) {
      printFan();
    }
  }
  displaySelected();
}

void printMenu() {
  lcd.setCursor(1, 0);
  lcd.print(menu[0]);
  lcd.print(" ");
  lcd.print(power[ledPower]);
  lcd.print(" ");
  lcd.print(colors[color]);

  lcd.setCursor(1, 1);
  lcd.print(menu[1]);
  lcd.print(" ");
  lcd.print(power[fanPower]);
  lcd.print(" ");
  lcd.print(fanSpeed);
}

void printLeds() {
  lcd.setCursor(5, 0);
  lcd.print(menu[0]);

  lcd.setCursor(4, 1);
  lcd.print(power[ledPower]);
  lcd.print("  ");
  lcd.print(colors[color]);
}

void printFan() {
  lcd.setCursor(6, 0);
  lcd.print(menu[1]);

  lcd.setCursor(4, 1);
  lcd.print(power[fanPower]);
  lcd.print("   ");
  lcd.print(fanSpeed);
  if(fanPower)analogWrite(fan, fanSpeed);
  else digitalWrite(fan,0);
}

void displaySelected() {
  int width = 0;
  if (row == 0) {
    if (ledPower == 0) width = 7;
    else width = 6;
    switch (col) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("[");
        lcd.setCursor(4, 0);
        lcd.print("]");
        break;
      case 1:
        lcd.setCursor(3, 1);
        lcd.print("[");
        lcd.setCursor(width, 1);
        lcd.print("]");
        break;
      case 2:
        width += 1;
        lcd.setCursor(width, 1);
        lcd.print("[");
        switch (color) {
          case 0: //white
            width += 6;
            digitalWrite(red,1);
            digitalWrite(green,1);
            digitalWrite(blue,1);
            break;
          case 1: //blue
            width += 5;
            analogWrite(red,0);
            digitalWrite(green,0);
            digitalWrite(blue,1);
            break;
          case 2: //green
            width += 6;
            analogWrite(red,0);
            digitalWrite(green,1);
            digitalWrite(blue,0);
            break;
          case 3: //red
            width += 4;
            digitalWrite(red,1);
            digitalWrite(green,0);
            digitalWrite(blue,0);
            break;
          case 4: //yellow
            width += 7;
            digitalWrite(red,1);
            digitalWrite(green,1);
            digitalWrite(blue,0);
            break;
          case 5: //cyan
            width += 5;
            analogWrite(red,0);
            digitalWrite(green,1);
            digitalWrite(blue,1);
            break;
          case 6: //magenta
            width += 8;
            digitalWrite(red,1);
            digitalWrite(green,0);
            digitalWrite(blue,1);
            break;
        }
        if(!ledPower){
          digitalWrite(red,0);
          digitalWrite(green,0);
          digitalWrite(blue,0);
        }
        lcd.setCursor(width, 1);
        lcd.print("]");
        break;
    }
  }
  if (row == 1) {
    if (fanPower == 0) width = 7;
    else width = 6;
    switch (col) {
      case 0:
        lcd.setCursor(0, 1);
        lcd.print("[");
        lcd.setCursor(4, 1);
        lcd.print("]");
        break;
      case 1:
        lcd.setCursor(3, 1);
        lcd.print("[");
        lcd.setCursor(width, 1);
        lcd.print("]");
        break;
      case 2:
        width += 2;
        lcd.setCursor(width, 1);
        lcd.print("[");
        lcd.setCursor(width + 2, 1);
        if (fanSpeed >= 10) lcd.setCursor(width + 3, 1);
        if (fanSpeed >= 100) lcd.setCursor(width + 4, 1);
        lcd.print("]");
        break;
    }
  }
}

void flipBool(bool *ptr) {
  if (*ptr == true) *ptr = false;
  else *ptr = true;
}

void flipInt(int *ptr) {
  if (*ptr == 1) *ptr = 0;
  else *ptr = 1;
}

void ledColorChange(int i) {
  if (color > 0 && color < 6) {
    color += i;
    return;
  }
  if ((i == -1 && color == 6) || (i == 1 && color == 0)) {
    color += i;
    return;
  }
  if (i == -1) color = 6;
  else color = 0;
}

void cycleFanSpeed() {
  switch (fanSpeed) {
    case 0:
      fanSpeed = 125;
      break;
    case 125:
      fanSpeed = 255;
      break;
    case 255:
      fanSpeed = 0;
      break;
    default:
      fanSpeed = 125;
      break;
  }
}

// Convert ADC value to key number
int get_key(unsigned int input)
{ int k;
  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k]) {
      return k;
    }
  }
  if (k >= NUM_KEYS)
    k = -1; // No valid key pressed
  return k;
}

