void motorReset()
{
  digitalWrite(EN_PIN, HIGH);
  int interval = 200;
  int microstep = 256;
  driver.microsteps(microstep);
  saveSettings();
}

void readSettings()
{
  int address = 0;
  interval = (EEPROM.read(address) << 8) + EEPROM.read(address + 1);

  if (interval == 0)
  {
    interval = 200;
  }
}

void saveSettings()
{
  int address = 0;

  EEPROM.write(address, interval >> 8);
  EEPROM.write(address + 1, interval & 0xFF);
}

void beep()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1); 
  digitalWrite(LED_BUILTIN, LOW);
}

void longBeep()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20); 
  digitalWrite(LED_BUILTIN, LOW);
}
