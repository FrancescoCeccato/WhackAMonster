#include <LiquidCrystal.h>
#include <EEPROM.h>

#define BTN1 A0
#define BTN2 A1
#define BTN3 A2
#define BUZZ 2
#define DELAYSHORTEST 70
#define DELAYSHORT 100
#define DELAYBTN 300
#define DELAYPHASE 1000

const int rs=9,en=8,d4=7,d5=6,d6=5,d7=4;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

bool firstRun = true;
String myName;

byte heart[] = {B00000,B01010,B11111,B11111,B01110,B00100,B00000,B00000};
byte noHeart[] = {B00000,B01010,B10101,B10001,B01010,B00100,B00000,B00000};
byte halfHeart[] = {B00000,B01010,B11101,B11001,B01010,B00100,B00000,B00000};
byte item1[] = {B00000,B01010,B10001,B01010,B00000,B10001,B01110,B00000};
byte item2[] = {B10001,B01010,B00100,B01010,B00000,B01110,B10001,B00000};
byte crown[] = {B00000,B00000,B10101,B11111,B11111,B11111,B00000,B00000};


int squares[] = {5,9,13};
int buttons[] = {BTN1,BTN2,BTN3};
int lives;

float init_time;
double fraction;
double attempts;
double start_time;
double this_time;

void setup() {
  Serial.begin(9600);
  if(firstRun){ 
    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    pinMode(BTN3, INPUT_PULLUP);
    pinMode(BUZZ, OUTPUT);
    create_chars();
    
    lcd.begin(16,2);
    intro();
    randomSeed(analogRead(0));
    firstRun = false; 
  }
  start_time = 650;
  attempts = 0;
  fraction = 1;
  lives = 3;
  myName = "";

}

void loop() {
   setup();
   set_name();
   set_games();
   
   while(lives>0){
   play_game();
   attempts++;
  }
  highlights();
}

void set_games(){
  lives = 3;
  lcd.clear();
  create_hearts();
  lcd.setCursor(0,1); lcd.print(myName);
  game_countdown();
  init_time = millis();
}

void play_game(){
  int pos_1 = random(3), pos_2;
  pos_2 = random(2);
  switch(pos_1)
  {
    case 0: pos_2 +=1; break;
    case 1: pos_2 *= 2; break;
  }
  int special = random(5);
  int isGood = random(2);
  
  fraction = 1;
  set_thistime();
  
  create_graphics(pos_1,pos_2,special,isGood);
  start_action(pos_1,pos_2,special,isGood);
}

void create_graphics(int p1, int p2, int spec, int good)
{
  create_hearts();
  clear_slots();
  lcd.setCursor(squares[p1],1);
  lcd.write(byte(3));
  if(spec<1){
    lcd.setCursor(squares[p2],1);
    lcd.write(byte(good? 0 : 4));
  }
  lcd.setCursor(4,0);
}

void start_action(int p1, int p2, int spec, int good)
{
  bool yes = false, over = false;
  int count = millis(), passed;
  while(!over && (passed = millis()-count) < this_time)
  {
    print_timebar(passed);
    if(digitalRead(buttons[p1]) == LOW){
      yes = over = true;
      sound_yes();
    }
    if(digitalRead(buttons[p2]) == LOW){
      if(spec<1){
        if(good){
          if(lives<6){lives++;}
          yes=true; sound_life();
        }
        else {
          sound_lose();
        }
     over = true;
    }
   }
  }
  if(!yes){lives--;}
}

void create_hearts()
{
    lcd.setCursor(0,0);
    for(int i = 0; i<3; i++)
    {
        lcd.write(byte(2));
    }
    int even = lives/2*2, copy = lives;
    
    lcd.setCursor(0,0);
    for(int i=0; i<even; i+=2)
    {
        lcd.write(byte(0));
        copy -= 2;
    }
    if(copy == 1) {lcd.write(byte(1));}
}

void clear_slots()
{
  lcd.setCursor(3,0); 
  for(int i=0;i<13;i++) {lcd.write(' ');}
  lcd.setCursor(3,1); 
  for(int i=0;i<13;i++) {lcd.write(' ');}
}

void highlights()
{
  attempts = 0;
  double this_time = float(millis()-init_time)/1000;
  lcd.setCursor(6,0); lcd.print(String(this_time)+"s");
  lcd.setCursor(0,1); lcd.print("      [OK]      ");
  create_hearts();
  while(digitalRead(BTN2)==HIGH);
    
  int i = eeprom_update(myName, this_time);
  Serial.println(String(i));
  print_scores(9-i);
}

void set_thistime(){
  int den = start_time-20*attempts;
  den = den > 0 ?  den : 0;
  this_time = den + 350;
}

void print_timebar(int passed) {
  int requested = this_time * fraction/12;
  int diff = abs(passed - requested);
  if(diff<30)
  {
    lcd.write('-');
    fraction++;   
  }
}

void print_scores(int j){
  Serial.println("param:"+String(j));
  lcd.clear();
  String results[10] = {};
  int start[10] = {};
  String up;
  for(int i = 10; i>0; i--)
  {
      up = "";
      up += String(11-i)+"."; 
      up += eeprom_readname(i-1) + " ";
      up += String(eeprom_readvalue(i-1));
      results[10-i] = up;
      start[10-i] = (16-up.length())/2;
  }
  Serial.println("start");
  for(int i= 0; i<10; i++)
  {
    Serial.println(results[i]);
  }
  Serial.println("end");
  for(int i=0; i<10;i++)
  {
        lcd.clear();
        lcd.setCursor(start[i],0);
        lcd.print(results[i]); 
        
        if(i == j){
          lcd.setCursor(0,0); 
          if(i == 0){
            lcd.write(byte(5));
            lcd.setCursor(15,0);
            lcd.write(byte(5));
            sound_life();
          }
          else{
            lcd.setCursor(0,0);
            lcd.print(">");
            lcd.setCursor(15,0);
            lcd.write("<");
          }
        }
    
        if(i+1 == j){
          lcd.setCursor(0,1);
          lcd.print(">              <");
        }
        lcd.setCursor(start[i+1],1);
        lcd.print(results[i+1]);
        delay(DELAYPHASE);
  }
  lcd.setCursor(6,1); lcd.print("[OK]");
  
  while(digitalRead(BTN2)==HIGH);
  while(digitalRead(BTN2)==LOW); 
}

void game_countdown()
{
  for(int i = 3; i>0; i--)
  {
    lcd.setCursor(9,0); 
    lcd.print(String(i) + "   ");
    lcd.setCursor(10,0);
    for(int j = 0; j < 3; j++)
    {
        lcd.write('.');
        delay(300);
    }
  }
  lcd.setCursor(9,0); 
  lcd.print("    ");
}

void set_name(){
  print_bar();
  lcd.setCursor(3,0); lcd.print("Name:  ___");
  String output = "";
  String chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  int cur = 10, letters = 0, chr = 0;
  while(letters<3){
    lcd.setCursor(cur,0); lcd.print(chars[chr]);
    if(digitalRead(BTN1)==LOW && chr>0){
      hold_lock(BTN1);
      chr--;
    }
    if(digitalRead(BTN3)==LOW && chr<35){
      hold_delay();
      chr++;
    }
    if(digitalRead(BTN2)==LOW){
      hold_lock(BTN2);
      myName+=chars[chr];
      chr=0; cur++;
      letters++;
    }
  }
}

void create_chars(){
  lcd.createChar(0,heart);
  lcd.createChar(1,halfHeart);
  lcd.createChar(2,noHeart);  
  lcd.createChar(3,item1);
  lcd.createChar(4,item2);
  lcd.createChar(5,crown);
}

void print_bar(){
  lcd.clear();
  lcd.setCursor(6,1); lcd.print("[OK]");
  lcd.setCursor(0,1); lcd.print("[-]");
  lcd.setCursor(13,1); lcd.print("[+]");
}


void intro(){
  print_title();
  int i1=3, i2=12;
  while(i1>=0){
     char c = i1 == 0 ? '|' : '.';
     lcd.setCursor(i1,1); lcd.print(c);
     lcd.setCursor(i2,1); lcd.print(c);

      lcd.setCursor(i1,0); lcd.print(c);
      lcd.setCursor(i2,0); lcd.print(c);
      
      delay(DELAYSHORT);
      i1--; i2++;
  }
 delay(DELAYPHASE);
}

void print_title()
{
  for(int i = 0; i<6; i++)
  { 
    lcd.clear();
    delay(DELAYSHORT);
    
    String up = "Whack-a-", low = "Smile!";
    lcd.setCursor(4,0); lcd.print(up);
    lcd.setCursor(5,1); lcd.print(low);
    
    delay(DELAYSHORT);
  }
}

void hold_lock(int i) {
  while(digitalRead(i) == LOW);
}

void hold_delay(){
  delay(DELAYBTN);
}

//SOUNDS

void sound_yes(){
  tone(BUZZ,1000,DELAYSHORT);
  delay(DELAYSHORT);
  tone(BUZZ,1500,DELAYSHORT);
  delay(DELAYSHORT);
}

void sound_life(){
  tone(BUZZ,1700,2*DELAYSHORT);
  delay(3*DELAYSHORT);
  tone(BUZZ,1700,DELAYSHORTEST);
  delay(DELAYSHORT);
  tone(BUZZ,1700,4*DELAYSHORT);
}

void sound_lose(){
  tone(BUZZ,200,DELAYSHORT);
  delay(DELAYSHORT);
  tone(BUZZ,50,DELAYSHORT);
}

//EEPROM

void eeprom_write(String myName, double score, int pos){
    int address = 3*pos;
    for(int i = 0; i<3; i++)
    {
        EEPROM.write(address,myName[i]);
        address++;
    }
    address = 30+sizeof(double)*pos;
    EEPROM.put(address,score);    
}

String eeprom_readname (int pos){
    int address = 3*pos;
    String output = "";
    for(int i = 0; i<3;i++)
    {
        output += char(EEPROM.read(address));
        address++;
    }
    return output;
}

double eeprom_readvalue(int pos){
    int address = 30+sizeof(double)*pos;
    double output = 0;
    EEPROM.get(address,output);
    return output;
}

int eeprom_update(String myname, double score){
    int pos = -1; bool done = false;
    while (!done && pos<10)
    {        if(score<eeprom_readvalue(pos+1))
        {  done = true; }
        else pos++;
    } 
    for(int i = 1; i<=pos; i++)
    {
      String _name = eeprom_readname(i);
      double _score = eeprom_readvalue(i);   
      eeprom_write(_name,_score,i-1);
    }
   eeprom_write(myname,score,pos);
   return pos;  
}
