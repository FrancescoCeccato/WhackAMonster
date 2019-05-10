#include <LiquidCrystal.h>
#define BTN1 A0
#define BTN2 A1
#define BTN3 A2
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

int squares[] = {5,9,13};
int buttons[] = {A0,A1,A2};
int lives;
int myTime;
int bestTime;
bool record;

void setup() {
  Serial.begin(9600);
  if(firstRun){
    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    pinMode(BTN3, INPUT_PULLUP);
    createChars();
    
    lcd.begin(16,2);
    intro();
    setName();

    randomSeed(analogRead(0));
    firstRun = false; 
  }
  
  lives = 3;
  myTime = 0;
  bestTime = 0;
  record = false;
}

void loop() {
  setGame();
  while(lives>0)
  {playGame();}
  record = myTime>bestTime;
  highlights();
}

void setGame(){
  lcd.clear();
  for(int i = 0; i<lives; i++)
  {lcd.setCursor(i,0); lcd.write(byte(2));}
  lcd.setCursor(0,1); lcd.print(myName);
  game_countdown();
}

void playGame(){
  int pos_1 = random(3), pos_2;
  int ran = random(2);
  switch(pos_1)
  {
    case 0: pos_2 = ran+1; break;
    case 1: pos_2 = ran*2; break;
    case 2: pos_2 = ran; break;
  }
  int special = random(5);
  int isGood = random(2);
  create_graphics(pos_1,pos_2,special,isGood);
  start_Counter(pos_1,pos_2,special,isGood);
}

void create_graphics(int p1, int p2, int spec, int good)
{
  lcd.setCursor(squares[p1],0);
  lcd.write(byte(3));
  if(spec<1){
    lcd.setCursor(squares[p2],0);
    lcd.write(byte(good? 0:4));
  }
}

void start_Counter(int p1, int p2, int spec, int good)
{
  bool yes = false, over = false;
  int count = millis();
  while(!over||millis()-count < 5000)
  {
    Serial.println(String(millis()-count));
    if(digitalRead(buttons[p1]) == LOW){
      Serial.println("OOOOOOOOOOOOOOOOOOOF");
      yes = over = true;
    }
    if(digitalRead(buttons[p2]) == LOW){
      if(spec<1){
        if(good){
          lives++;
          yes=over=true;}
        else{
          over = true;}
      }
    }
  }
    
}

void highlights(){}

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

void setName(){
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

void createChars(){
  lcd.createChar(0,heart);
  lcd.createChar(1,noHeart);
  lcd.createChar(2,halfHeart);  
  lcd.createChar(3,item1);
  lcd.createChar(4,item2);
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
    
    String up = "Whack-a-", down = " Mole!";
    lcd.setCursor(4,0); lcd.print(up);
    lcd.setCursor(5,1); lcd.print(down);
    
    delay(DELAYSHORT);
  }
}

void hold_lock(int i) {
  while(digitalRead(i) == LOW);
}

void hold_delay(){
  delay(DELAYBTN);
}
  
