#include "Wire.h"
#include "LCD.h"
#include "LiquidCrystal_I2C.h"

#define I2C_ADDR 0x3f
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
unsigned long btnDelay;
const int del=100;

float val=0;
float val2=0;
int currview=0;
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
float factor1=1.0;
float factor2=1.0;
int pc=3;
int ports[3][4] = {
  {7, 0, 0, 0} ,
  {8, 0, 0, 0} ,
  {9, 0, 0, 0} };

  String submenuItem1values[2] = {
    {"Ja"} ,
    {"Nej"} };



  bool insubmenu=false;
  int currentmenuitem=0;
  int menuitems=4;
  int currentsubmenuitem=0;
  int submenuItemscount=0;
  String menuItemvalues[4] = {
    {""} ,
    {""} ,
    {""} ,
    {""} };



void setup()
{

 lcd.begin (20,4); //  our LCD is a 20x4, change for your LCD if needed

// LCD Backlight ON
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);

lcd.clear() ; // go home on LCD
lcd.print("johan");
lcd.setCursor(5, 1);
lcd.print("hahne");
for(int i = 0;i<pc;i++){
pinMode(ports[i][0], OUTPUT);
digitalWrite(ports[i][0],HIGH);}
}


void loop()
{

  val=analogRead(14);
  val2=analogRead(15);
setFactors();


readButtons();
}

void showview(int viewnr)
{
    switch (viewnr) {
        case 0: MainView(10,factor1);
            break;
        case 1: TempView();
            break;
    }
}




void MainView(int outtemp,float pipetemp)
{
    lcd.clear();
    String outstr ="Ute temp:"+(String)outtemp+";C";
    Printstring(0,0,outstr);
     outstr ="Shunt temp:"+(String)pipetemp+";C";
    Printstring(1,0,outstr);

}
void TempView()
{
    lcd.clear();
    String outstr ="U/F 20/"+(String)(int)calcTemp(20)+" 10/"+(String)(int)calcTemp(10)+" 0/"+(String)(int)calcTemp(0);

    Printstring(0,0,outstr);
     outstr ="-10/"+(String)(int)calcTemp(-10)+" -20/"+(String)(int)calcTemp(-20)+" -30/"+(String)(int)calcTemp(-30);
    Printstring(1,0,outstr);
     outstr ="Factor1: "+(String)factor1;
    Printstring(2,0,outstr);
    outstr ="Factor2: "+(String)factor2;
    Printstring(3,0,outstr);

}
void Printstring(int row,int index,String outstring)
{
    lcd.setCursor(index, row);
    int c = outstring.length();
    for (int i = 0; i<c;i++)
    {
        if (outstring[i] == ';')
        {
            lcd.print((char)223);
        } else {
        lcd.write(outstring[i]);
        }

    }
  }

void setFactors()
{
//    factor1=((val/1022)+0.8)*1.3;
//    factor2=(val2/1022)+1;

factor1=val;
factor2=val2;
}

float calcTemp(int outTemp)
{
float c = 0;
float v =0;
for(int i=20;i > -31; i-- ){
if(i==outTemp){
  v=(i+(c*factor1))*factor2;
  break;
  }
  c++;
}
if(v > 99){
return 99;
} else {
return v;
}

}

void readButtons() {
 if (millis()-btnDelay >= del) {
for(int i = 0;i<pc;i++){
    if(digitalRead(ports[i][0]) == LOW){
     ports[i][1]=1;
    }
    else
    {
      ports[i][1]=0;
    }

    if(ports[i][1]!=ports[i][2]){
      if(ports[i][1]==0){
       ports[i][2]=0;


      }
      if(ports[i][1]==1){
           ports[i][2]=1;
          buttonpush(i);
    }


    }
}
btnDelay=millis();
}


}

void buttonpush(int button){
      switch (button) {
        case 0: Prevbutton();
          break;
        case 1: OkButton();
            break;
        case 2: Nexbutton();
                break;
      }
}
void Prevbutton(){
  if (!insubmenu){
  if (currentmenuitem==0){
    currentmenuitem=menuitems;
  } else {
    currentmenuitem--;
  }
  showmenuitem(currentmenuitem);

  } else {
  if (currentsubmenuitem==0){
    currentsubmenuitem=submenuItemscount-1;
  } else {
    currentsubmenuitem--;
  }
showsubmenuitem(currentmenuitem,currentsubmenuitem);
  }
}



void Nexbutton(){
  if (!insubmenu){
  if (currentmenuitem==menuitems){
    currentmenuitem=0;
  } else {
    currentmenuitem++;
  }
  showmenuitem(currentmenuitem);

} else {
  if (currentsubmenuitem==submenuItemscount-1){
    currentsubmenuitem=0;
  } else {
    currentsubmenuitem++;

  }
showsubmenuitem(currentmenuitem,currentsubmenuitem);
}

}


void OkButton()
{
if (!insubmenu){
  insubmenu=true;
  showsubmenuitem(currentmenuitem,0);
} else {
  insubmenu=false;
  showmenuitem(currentmenuitem);
}

}

void showmenuitem(int item){
  switch(item) {
    case 0: MainView(1,1);
    break;
    case 1: menuItem1();
    break;
    case 2: menuItem2();
    break;
  }
}

void showsubmenuitem(int menuitem,int submenuitem){
    switch(menuitem) {
      case 0: MainView(1,1);
      break;
      case 1: submenuItem1(submenuitem);
      break;
      case 2: submenuItem2(submenuitem);
      break;
    }

}

void menuItem1()
{
    lcd.clear();
    currentsubmenuitem=0;
    submenuItemscount=2;
    String outstr ="Kontrollera cirkpump";
    Printstring(0,0,outstr);
     outstr =menuItemvalues[1];
    Printstring(1,0,outstr);

}
void submenuItem1(int item)
{
    lcd.clear();
    String outstr ="ja/nej";
    Printstring(0,0,outstr);
    outstr =submenuItem1values[item];
    menuItemvalues[1]=submenuItem1values[item];
    Printstring(1,0,outstr);

}


void menuItem2()
{
    lcd.clear();
    String outstr ="Stoppa pump vid temp";
    currentsubmenuitem=0;
    submenuItemscount=20;
    Printstring(0,0,outstr);
     outstr =menuItemvalues[2];
    Printstring(1,0,outstr);
    lcd.print((char)223);
    lcd.print("C");


}
void submenuItem2(int item)
{
    lcd.clear();
    String outstr ="Ange grader:";
    Printstring(0,0,outstr);
    outstr =(String)item;
    menuItemvalues[2]=(String)item;
    Printstring(1,0,outstr);

}
