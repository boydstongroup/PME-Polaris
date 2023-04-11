void menuAction(unsigned int menuCode) {
  //gets called when rotary encoder is clicked,
  //check the current location inside the menu,
  //execute approriate actions
  //returns true if some action is executed
  //returns false if nothing is done
  //Serial.print("menuAction called with menuCode = ");
  //Serial.println(menuCode);

  switch (menuCode) {

    case 0:
      {
        motorReset();
      }
      break;

    case 111:  //set frame rpm
      if (subMenuActive != 1) {  //initialize the subMenu3

        lcd.clear();
        subMenuActive = 1;
        subMenuPos = 0;
        subMenuClick = 0;
        subMenu1Update(0); //calls subMenu3Update with 0 (no action)

      } else if (subMenuActive == 1) {  //already initialized

        subMenuActive = 0;  //deactivate submenu, activate main menu
        lcd.clear();
        menuPos = 11;
        updateMenuDisplay(menuPos);
        //code to set time variables
      }
      break;

    case 12:
      digitalWrite(EN_PIN, !digitalRead(EN_PIN));

      break;

    case 13:
      digitalWrite(DIR_PIN, !digitalRead(DIR_PIN));

      break;


    case 141:
      if (subMenuActive != 2) {  //initialize the subMenu3

        lcd.clear();
        subMenuActive = 2;
        subMenuPos = 0;
        subMenuClick = 0;
        subMenu2Update(0); //calls subMenu3Update with 0 (no action)

      } else if (subMenuActive == 2) {  //already initialized

        subMenuActive = 0;  //deactivate submenu, activate main menu
        lcd.clear();
        menuPos = 14;
        updateMenuDisplay(menuPos);
        //code to set time variables
      }
      break;


    case 2:  //reset
      {
        motorReset();
      }
      break;

    case 3:  //save
      {
        saveSettings();
      }
      break;
  }
}
