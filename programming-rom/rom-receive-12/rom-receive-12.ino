/*general workflow:
 * arduino can send "r" to computer, telling it to reset trasmit file counter
 * arduino can send "p" to computer, requesting to transmit a page
 * arduino can send "e" to computer, requesting to print errors and reset the system
 * with the exception of "p", computer will transmit "#" to indicate completion of command
 * end of "p" is indicated with transmission of either (line ff) or (page ffff)
 */

struct {
  int line[128];
  int data[128];
  int len;
  long adr;
} prog;

void setup() {
  int i, werr, rerr;
  Serial.begin(115200, SERIAL_8N1);
  while (1) {
    werr = 0;
    rerr = 0;
    confirm();
    grabreset();
    while (grabpage()) {
      pagewrite();
      if (i = errors()) {
        werr += i;
        pagewrite();
      }
    }
    grabreset();
    while (grabpage()) {
      if (i = errors()) {
        rerr += i;
      }
    }
    senderr(werr, rerr);
  }
}

void confirm() {
  while (Serial.available() == 0) ;
  Serial.read();
}

void grabreset() {
  Serial.print("r");
  confirm();
}

int grabpage() {
  int holder, i, j;
  char c;
  Serial.print("p");
  while (1) {
    for (holder = 0, i = 0; i < 4; i++) {
      while (!Serial.available()) ;
      if ((c = Serial.read()) >= '0' && c <= '9')
        holder = (holder << 4) + c-'0';
      else
        holder = (holder << 4) + c-'a'+10;
    }
    if (holder == 0xffff)
      return (0);
    prog.adr = holder;
    for (j = 0; j < 128; j++) {
      for (holder = 0, i = 0; i < 2; i++) {
        while (!Serial.available()) ;
        if ((c = Serial.read()) >= '0' && c <= '9')
          holder = (holder << 4) + c-'0';
        else
          holder = (holder << 4) + c-'a'+10;
      }
      if (holder == 0xff) {
        prog.len = j;
        return (1);
      }
      prog.line[j] = holder;
      for (holder = 0, i = 0; i < 2; i++) {
        while (!Serial.available()) ;
        if ((c = Serial.read()) >= '0' && c <= '9')
          holder = (holder << 4) + c-'0';
        else
          holder = (holder << 4) + c-'a'+10;
      }
      prog.data[j] = holder;
    }
  }
}

void senderr(int werr, int rerr)
{
  Serial.print("e");
  confirm();
  Serial.print(prog.line[0]);
  confirm();
  Serial.print(prog.data[0]);
}

void pagewrite() {
  1;
}

int errors() {
  return (0);
}

void loop() {}
