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

char example[10];

void setup() {
  int i;
  
  Serial.begin(115200, SERIAL_8N1);
  Serial.print("p");
  for (i = 0; i < 10;)
    if (Serial.available()) {
      example[i] = Serial.read();
      if (example[i] == '#')
        break;
      i++;
    }
  example[i] = '\0';
  Serial.print("l");
  while (Serial.available() == 0) ;
  Serial.read();
  Serial.print(example);
  while (Serial.available() == 0) ;
  Serial.read();
  Serial.print("x");
}

void loop() {}
