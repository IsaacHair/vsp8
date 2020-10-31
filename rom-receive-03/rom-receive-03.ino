void setup()
{
  int incomingByte, flag, i;
  Serial.begin(115200, SERIAL_8N1);
  flag = 0;
  for (i = 0; i < 100; i++, delay(1))
    if (Serial.available() > 0 && flag == 0) {
      incomingByte = Serial.read();
      flag = 1;
    }
  Serial.print("Z");
}

void loop() {}
