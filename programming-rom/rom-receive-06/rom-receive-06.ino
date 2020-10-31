/*general workflow:
 * arduino can send "r" to computer, telling it to reset trasmit file counter
 * arduino can send "p" to computer, requesting to transmit a page
 * arduino can send "l" to computer, requesting to printf a string to the command line
 * arduino can send "x" to computer, terminating the serial end of the program
 * with the exception of "p", computer will transmit "d" to indicate completion of command
 * end of "p" is indicated with transmission of either (line ff) or (page ffff)
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
  int num;
  char buff[100];
  Serial.begin(115200, SERIAL_8N1);
  grabreset();
  grabpage();
  sprintf(buff, "got %d\n", prog.adr);
  grablog("hi there");
  grabend();
}

void grabend() {
  Serial.print("x");
}

void grablog(char buff[100]) {
  Serial.print("l");
  Serial.print(buff);
  while (Serial.read() != '#') ;
}

void grabreset() {
  Serial.print("r");
  while (Serial.read() != '#') ;
}

int grabpage() {
  int i, j, buff;
  char c;
  Serial.print("p");
  for (buff = 0, i = 0; i < 4; i++)
    if (Serial.available() > 0) {
      if ((c = Serial.read()) >= '0' && c <= '9')
        buff = (buff << 4) + c-'0';
      else
        buff = (buff << 4) + c-'a'+10;
    }
  prog.adr = buff;
  while (Serial.read() != '#') ;
  return (1);
}

void loop() {}
