#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t arrow[8] = {0x00, 0x04, 0x06, 0x1F, 0x1F, 0x06, 0x04, 0x00};

/////Rotary Encoder Pin Assignment/////
#define rotaryPin_A 5
#define rotaryPin_B 6
#define rotaryButtonPin 7

#define EN_PIN    9  // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN  11  // Step on rising edge
#define RX_PIN    10  // SoftwareSerial pins
#define TX_PIN    8  //
#define DIR_PIN    12  //
#define Buzzer_PIN    13  //

#include <TMC2208Stepper.h>
TMC2208Stepper driver = TMC2208Stepper(RX_PIN, TX_PIN);

int interval = 200;
int currentLimit = 1000;
int microstep = 256;
unsigned long NextTime = 0;

unsigned int maxMenuItems;     //number of menu items
unsigned char encoder_A = 0;
unsigned char encoder_B = 0;
unsigned char encoder_A_prev = 0;
unsigned char encoder_C = 1;  //encoder button
unsigned char encoder_C_prev = 1;
unsigned long currentTime;
unsigned long loopTime;
/////End of Rotary Encoder Declarations/////

//////Menu declarations///////
unsigned int menuPos = 0; //current menu position
unsigned int lastMenuPos = 0; //previous menu position
unsigned int parentMenuPos = 0; //parent menu position
bool humanInputActive = false;  //flag to indicate if input session is active
unsigned subMenuActive = 0;   //flag to indicate a sub selection menu session is active: 0 - main menu; 1 - number selection 1- 255; 2 - binary selection yes/no; 3 - time setting;
unsigned int subMenuPos = 0;  //sub menu rotary position
unsigned int subMenuClick = 0; //sub menu click counter
unsigned long CurrentTimeS;
unsigned long CurrentTimeE;

typedef struct menu_type
{

  menu_type()
    : code(0), text("")
  {
    //do nothing
  }

  unsigned int code; //code that indicate address (position) in the menu tree
  String text; //text to be displayed for the menu selection

}  menu_type;

menu_type menu[30] = {}; //initilizing menu array, use a number >= than the number of menu options


/////End of Menu declarations////

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  menuInit();
  Serial.begin(9600);
  Wire.begin();
  rotaryEncoderInit();
  

  lcd.init();
  lcd.backlight();
  lcd.createChar(1, arrow);

  lcd.setCursor(4, 0);
  lcd.print("COSE PME");

  lcd.setCursor(5, 1);
  lcd.print("V 1.00");

  pinMode(rotaryPin_A, INPUT_PULLUP);
  pinMode(rotaryPin_B, INPUT_PULLUP);
  pinMode(rotaryButtonPin, INPUT_PULLUP);

  pinMode(Buzzer_PIN, OUTPUT);





  driver.beginSerial(115200);
  // Push at the start of setting up the driver resets the register to default
  driver.push();
  // Prepare pins
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  driver.pdn_disable(true);     // Use PDN/UART pin for communication
  driver.I_scale_analog(false); // Use internal voltage reference
  driver.rms_current(currentLimit);      // Set driver current = 500mA, 0.5 multiplier for hold current and RSENSE = 0.11.
  driver.toff(2);               // Enable driver in software
  driver.mstep_reg_select(true);
  driver.microsteps(microstep);
  driver.dedge(true);

  digitalWrite(EN_PIN, HIGH);    // Enable driver in hardware

  motorReset();
}

void loop() {
  //  CurrentTimeS = micros();
  rotaryEncoderUpdate();

  if (micros() < NextTime)
    NextTime = micros();

  if (micros() - NextTime > interval) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    NextTime = micros();         // reset for next pulse
  }

}

void rotaryEncoderInit() {
  //Rotary encoder initialization
  currentTime = millis();
  loopTime = currentTime;
}

void rotaryEncoderUpdate() {

  //rotary encoder update, to be called from main loop()

  currentTime = millis();
  if (currentTime >= (loopTime + 1) ) {
    encoder_A = digitalRead(rotaryPin_A);
    encoder_B = digitalRead(rotaryPin_B);
    encoder_C = digitalRead(rotaryButtonPin);

    //handling knob rotation
    if ( (encoder_A == 0) && (encoder_A_prev == 1) ) {  //encoder changed position
      beep();

      if (encoder_B == 1) { //B is high, so encoder moved clockwise
        //Serial.println("encoder rotated CW");

        switch (subMenuActive) { //send encoder action to appropriate menu handler
          case 0: //main menu
            menuUpdate(2);
            break;

          case 1: //subMenu1
            subMenu1Update(2);
            break;

          case 2: //subMenu1
            subMenu2Update(2);
            break;

          default:
            menuUpdate(2);
            break;
        }


      } else {  //else, encoder moved counter-clockwise
        //Serial.println("encoder rotated CCW");

        switch (subMenuActive) {  //call the appropriate menuupdater depending on which sub menu is active
          case 0:
            menuUpdate(3);
            break;

          case 1:
            subMenu1Update(3);
            break;

          case 2: //subMenu1
            subMenu2Update(3);
            break;

          default:
            menuUpdate(3);
            break;
        }
      }

    }

    //handling push button
    if ( (encoder_C == 0) && (encoder_C_prev == 1) ) { //button pushed
      //Serial.println("encoder button closed.");
      longBeep();
      switch (subMenuActive) {  //call the appropriate menuupdater depending on which sub menu is active
        case 0:
          menuUpdate(1);
          break;

        case 1:
          subMenu1Update(1);
          break;

        case 2: //subMenu1
          subMenu2Update(1);
          break;

        default:
          menuUpdate(1);
          break;
      }

    } else if ( (encoder_C == 1) && (encoder_C_prev == 0) )  {  //button
      //Serial.println("encoder button opened.");

    }

    encoder_A_prev = encoder_A;
    encoder_C_prev = encoder_C;
    loopTime = currentTime;

  }

}



unsigned int menuVerifyPos(unsigned int menuCode) {
  //accepts a code that represents position in the menu
  //checks against the menu, verify it exist, and returns it
  //if the menu code given does not exist,
  //returns the closest code smaller than the one given

  bool confirmCode = false; //flag to keep track of whether code has been confirmed in menu tree

  for (unsigned int k = 0; k <= (maxMenuItems - 1); k++) {

    if (menuCode == menu[k].code) {  //found exact code, returns it
      menuPos = menu[k].code;
      confirmCode = true;
      lastMenuPos = menuCode;
      return menuPos;
    }

  }

  if (confirmCode == false) {
    menuPos = lastMenuPos;
    return menuPos;   //cannot find menu option, go back to previous menu option
  }

}

void updateMenuDisplay(unsigned int menuCode) {
  //prints menu selection to the LCD display
  //in order to have a scrolling menu effect, this code looks at item before and after current menu item and display them in a row


  String curMenu;
  String nextMenu;

  curMenu = findMenuTextFromCode(menuCode);
  nextMenu = findMenuTextFromCode((menuCode + 1));

  //starts printing to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printByte(1);
  //char index, row index, on LCD
  lcd.print(curMenu);

  lcd.setCursor(1, 1);
  lcd.print(nextMenu); //print the next menu text in the 3rd row
}

String findMenuTextFromCode(unsigned int menuCode) {
  //accepts a code representing the code in menu, and returns the corresponding text

  for (unsigned int j = 0; j <= (maxMenuItems - 1); j++ ) {
    if (menuCode == menu[j].code) {
      return menu[j].text;
      break;
    }
  }
}
