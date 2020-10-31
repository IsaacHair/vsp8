/*general workflow:
 * arduino can send "r" to computer, telling it to reset trasmit file counter
 * arduino can send "p" to computer, requesting to transmit a page
 * arduino can send "l" to computer, requesting to printf a string to the command line
 * computer simply responds to these commands blindly
 */

struct {
  int line[128];
  int data[128];
  int len;
  long adr;
} prog;

void setup()
{
  //send serial data as strings so it has terminating '\0'
  //ensure that '\0' is only sent when done trying to write to terminal, however
  //need to have some sort of delay system
  int buff;
  Serial.begin(115200, SERIAL_8N1);
  Serial.print("r");
  delay(200);
  Serial.flush();
  Serial.print("p");
  delay(200);
  Serial.flush();
  delay(400);
  buff = grabpage();
  Serial.print("l");
  delay(200);
  Serial.flush();
  Serial.print('r');
  Serial.print(':');
  Serial.print(buff);
  Serial.print('\0');
  Serial.flush();
}

int grabpage() {
  int i, j, buff;
  char c;
  for (buff = 0, i = 0; i < 4; i++)
    if (Serial.available() > 0) {
      if ((c = Serial.read()) >= '0' && c <= '9')
        buff = (buff << 4) + c-'0';
      else
        buff = (buff << 4) + c-'a'+10;
    }
  return (buff);
}

void loop() {}
