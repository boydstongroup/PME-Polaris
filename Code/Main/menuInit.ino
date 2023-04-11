void menuInit() {
  //menu text display

  unsigned int i = 0;

  menu[i].code = 0;
  menu[i++].text = "EMERGENCY STOP";

  menu[i].code = 1;
  menu[i++].text = "MOTOR CONTROL";
  menu[i].code = 10;
  menu[i++].text = "RETURN";
  menu[i].code = 11;
  menu[i++].text = "SET INTERVAL";
  menu[i].code = 111;
  menu[i++].text = "INTERVAL:";

  menu[i].code = 12;
  menu[i++].text = "TOGGLE MOTOR";

  menu[i].code = 13;
  menu[i++].text = "REVERSE DIR";

  menu[i].code = 14;
  menu[i++].text = "SET MICROSTEPS";
  menu[i].code = 141;
  menu[i++].text = "MICROSTEPS:";

  menu[i].code = 2;
  menu[i++].text = "RESET";

  menu[i].code = 3;
  menu[i++].text = "SAVE SETTINGS";

  maxMenuItems = i + 1;

  readSettings();
}
