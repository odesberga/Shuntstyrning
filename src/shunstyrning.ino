#include <Arduino.h>
#include <EEPROM.h>
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
const int del=100;
const int menuitemCount=4;
const int pc=3; //portcount
unsigned long ScreentimeOut=0;
unsigned long ShunttimeOut=0;
unsigned long buttoninactive=0;
unsigned long shuntdelay=0;
unsigned long btnDelay;
unsigned long gendel;
float val=0;
float val2=0;
int currview=0;
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
float factor1=1.0;
float factor2=1.0;
float lastfactor=1;

int ports[pc][4] = {
  {7, 0, 0, 0} ,
  {8, 0, 0, 0} ,
  {9, 0, 0, 0} };

  struct ssubmenuitem {
      String row1="";
      String row2="";
      String row3="";
      String row4="";
      int minvalue=0;
      int maxvalue=0;
      String submenuStringvalues[2];
  };

struct smenuitem {
    String row1=""; //callfunc name
    String row2="";
    String row3="";
    String row4="";
    int value=0;
    bool useStringvalue=false;
    bool callFunc=false;
    ssubmenuitem smenitem;
};


 smenuitem menuItems[menuitemCount];

  bool inMainView=false;
  bool insubmenu=false;
  int currentmenuitem=0;
  int currentsubmenuitem=0;
  int submenuItemscount=0;


bool firstrun=true;
int pdelay=200;
void setup()
{
defMenu();
Serial.begin(9600);
lcd.begin (20,4);
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);

lcd.clear() ; // go home on LCD
lcd.setCursor(0, 0);
lcd.print("####################");
lcd.setCursor(0, 1);
lcd.print("# Utvecklad 2017   #");
lcd.setCursor(0, 2);
lcd.print("# Johan Hahne      #");
lcd.setCursor(0, 3);
lcd.print("####################");


lcd.print("");
for(int i = 0;i<pc;i++){
pinMode(ports[i][0], OUTPUT);
digitalWrite(ports[i][0],HIGH);}
buttoninactive=millis();
gendel=millis();
}


void loop()
{

if (millis()-gendel >= pdelay) {

  val=analogRead(A0);
  val2=analogRead(A1);
setFactors();
readButtons();


if ( millis() - buttoninactive >= 5000 ){
    if (!insubmenu){
        if (firstrun){
        lcd.clear();
        firstrun=false;
        }


    TempView();
    }
}
if ( millis() - ScreentimeOut >= 10000 ){
    if (!insubmenu){
    lcd.setBacklight(LOW);
    }
}

}

}


void defMenu(){
    //meny 1
    int m =0;
    menuItems[m].row1="Kontrollera cirkpump";
    menuItems[m].row2="@";
    menuItems[m].useStringvalue=true;
    menuItems[m].smenitem.maxvalue=1;
    menuItems[m].smenitem.minvalue=0;
    menuItems[m].smenitem.row1="Ange Ja/Nej";
    menuItems[m].smenitem.row2="@";
    menuItems[m].smenitem.submenuStringvalues[0]="Ja";
    menuItems[m].smenitem.submenuStringvalues[1]="Nej";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }
    //meny2
    m =1;
    menuItems[m].row1="Stoppa pump vid:";
    menuItems[m].row2="@ ;C";
    menuItems[m].useStringvalue=false;
    menuItems[m].smenitem.maxvalue=30;
    menuItems[m].smenitem.minvalue=0;
    menuItems[m].smenitem.row1="Ange ;C";
    menuItems[m].smenitem.row2="@";
    menuItems[m].smenitem.submenuStringvalues[0]="";
    menuItems[m].smenitem.submenuStringvalues[1]="";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }
    //meny3
    m =2;
    menuItems[m].row1="Shunt intervall:";
    menuItems[m].row2="@ Sek";
    menuItems[m].useStringvalue=false;
    menuItems[m].smenitem.maxvalue=30;
    menuItems[m].smenitem.minvalue=0;
    menuItems[m].smenitem.row1="Ange Sek";
    menuItems[m].smenitem.row2="@";
    menuItems[m].smenitem.submenuStringvalues[0]="";
    menuItems[m].smenitem.submenuStringvalues[1]="";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }
    m =3;
    menuItems[m].row1="Shuntmotor sekunder";
    menuItems[m].row2="Min -> Max";
    menuItems[m].row3="@ Sek";
    menuItems[m].useStringvalue=false;
    menuItems[m].smenitem.maxvalue=150;
    menuItems[m].smenitem.minvalue=80;
    menuItems[m].smenitem.row1="Ange Sek";
    menuItems[m].smenitem.row2="@";
    menuItems[m].smenitem.submenuStringvalues[0]="";
    menuItems[m].smenitem.submenuStringvalues[1]="";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }

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
    //lcd.clear();
    inMainView=true;
    String outstr ="Temp Ute:Framledning";
    Printstring(0,0,outstr);

     outstr ="20;:"+(String)(int)calcTemp(20)+"; I "+"-10;:"+(String)(int)calcTemp(-10)+";";
    Printstring(1,0,outstr);


     outstr ="10;:"+(String)(int)calcTemp(10)+"; I "+"-20;:"+(String)(int)calcTemp(-20)+";";

Printstring(2,0,outstr);
     outstr =" 0;:"+(String)(int)calcTemp(0)+"; I "+"-30;:"+(String)(int)calcTemp(-30)+";";
Printstring(3,0,outstr);


/*
    String outstr ="U/F 20/"+(String)(int)calcTemp(20)+" 10/"+(String)(int)calcTemp(10)+" 0/"+(String)(int)calcTemp(0);


     +" -20/"+(String)(int)calcTemp(-20)+" -30/"+(String)(int)calcTemp(-30);
    Printstring(1,0,outstr);
     outstr ="F1: "+(String)factor1 + "   F2: "+(String)factor2;
    Printstring(3,0,outstr);
    //outstr ="";
    //Printstring(3,0,outstr);
*/
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
            if (outstring[i] == '@'){
                if (menuItems[currentmenuitem].useStringvalue){
                    lcd.print(menuItems[currentmenuitem].smenitem.submenuStringvalues[menuItems[currentmenuitem].value]);
                    Serial.print(menuItems[currentmenuitem].smenitem.submenuStringvalues[menuItems[currentmenuitem].value]);
                } else {
                    lcd.print(menuItems[currentmenuitem].value);
                }
            } else {
                lcd.write(outstring[i]);
            }
        }
    }
  }

void setFactors()
{
    factor1=((val/1022)+1);
    factor2=(val2/1022)+0.5+(val/1022);
    if(!(((int)(val+val2) > lastfactor-3) && ((int)(val+val2) < lastfactor+3)) ){
        ScreentimeOut=millis();
        lcd.setBacklight(HIGH);
    }

    lastfactor = (int)(val+val2);

}

float calcTemp(int outTemp)
{
float c = 10;
float v =0;

c=c*factor1;
for(int i=20;i > -31; i-- ){
    if(i==outTemp){
        v=(c*factor2);//*factor2;

            //v=(c*factor1)*factor2;
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
         // buttoninactive=millis();
          //Serial.print(buttoninactive);
    }


    }
}
btnDelay=millis();
}
}

void runSHunt (){
    if(millis() - shuntdelay >= menuItems[2].value){

    }
shuntdelay = millis();

}

void buttonpush(int button){
    lcd.setBacklight(HIGH);
    ScreentimeOut=millis();
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
    buttoninactive=millis();
  if (!insubmenu){
  if (currentmenuitem==0){
    currentmenuitem=menuitemCount-1;
  } else {
    currentmenuitem--;
  }
  showmenuitem(currentmenuitem);

  } else {
  if (currentsubmenuitem==menuItems[currentmenuitem].smenitem.minvalue){
    currentsubmenuitem=menuItems[currentmenuitem].smenitem.maxvalue;
  } else {
    currentsubmenuitem--;
  }
showsubmenuitem(currentmenuitem,currentsubmenuitem);
  }
}



void Nexbutton(){
    buttoninactive=millis();
  if (!insubmenu){
  if (currentmenuitem==menuitemCount-1){
    currentmenuitem=0;
  } else {
    currentmenuitem++;
  }
  showmenuitem(currentmenuitem);

} else {
  if (currentsubmenuitem==menuItems[currentmenuitem].smenitem.maxvalue){
    currentsubmenuitem=menuItems[currentmenuitem].smenitem.minvalue;
  } else {
    currentsubmenuitem++;

  }
showsubmenuitem(currentmenuitem,currentsubmenuitem);
}

}


void OkButton()
{

if (!inMainView){
    if (!insubmenu){
        currentsubmenuitem=menuItems[currentmenuitem].value;
      showsubmenuitem(currentmenuitem,currentsubmenuitem);
    } else {
        EEPROM.write(currentmenuitem, menuItems[currentmenuitem].value);
      insubmenu=false;
      showmenuitem(currentmenuitem);
    }
    buttoninactive=millis();
    }
}

void showmenuitem(int item){
lcd.clear();
inMainView=false;
Printstring(0, 0, menuItems[currentmenuitem].row1);
Printstring(1, 0, menuItems[currentmenuitem].row2);
Printstring(2, 0, menuItems[currentmenuitem].row3);
Printstring(3, 0, menuItems[currentmenuitem].row4);

}

void showsubmenuitem(int menuitem,int submenuitem){
    insubmenu=true;
    lcd.clear();
    menuItems[currentmenuitem].value=submenuitem;

    Printstring(0, 0, menuItems[currentmenuitem].smenitem.row1);
    Printstring(1, 0, menuItems[currentmenuitem].smenitem.row2);
    Printstring(2, 0, menuItems[currentmenuitem].smenitem.row3);
    Printstring(3, 0, menuItems[currentmenuitem].smenitem.row4);


}
