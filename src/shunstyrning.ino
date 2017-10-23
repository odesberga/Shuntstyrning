#include <Arduino.h>
#include <EEPROM.h>
#include "Wire.h"
#include "LCD.h"
#include "LiquidCrystal_I2C.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/pgmspace.h>
#define PROGMEM
const int menuitemCount=5;
const char m1r1[]  = "Kontrollera cirkpump";
const char m1r2[]  = "@";
const char m1r3[]  = "\0";
const char m1r4[]  = "\0";
const char m2r1[]  = "Stoppa pump vid:";
const char m2r2[]  = "@ ;C";
const char m2r3[]  = "\0";
const char m2r4[]  = "";
const char m3r1[]  = "Shunt intervall:";
const char m3r2[]  = "@ Sek";
const char m3r3[]  = "";
const char m3r4[]  = "";
const char m4r1[]  = "Shuntmotor sekunder";
const char m4r2[]  = "Min -> Max";
const char m4r3[]  = "@ Sek";
const char m4r4[]  = "";
const char m5r1[]  = "Shuntmotor antal";
const char m5r2[]  = "sekunder/justering";
const char m5r3[]  = "@ Sek";
const char m5r4[]  = "";

//submenu
const char sm1r1[]  = "Ange Ja/Nej";
const char sm1r2[]  = "@";
const char sm1r3[]  = "";
const char sm1r4[]  = "";
const char sm2r1[]  = "Ange ;C";
const char sm2r2[]  = "@ ;C";
const char sm2r3[]  = "";
const char sm2r4[]  = "";
const char sm3r1[]  = "Ange Sekunder";
const char sm3r2[]  = "@ Sek";
const char sm3r3[]  = "";
const char sm3r4[]  = "";
const char sm4r1[]  = "Ange Sekunder";
const char sm4r2[]  = "Min -> Max";
const char sm4r3[]  = "@ Sek";
const char sm4r4[]  = "";
const char sm5r1[]  = "Ange Sekunder";
const char sm5r2[]  = "sekunder/justering";
const char sm5r3[]  = "@ Sek";
const char sm5r4[]  = "";





const char* const menu_strings[][4]  =	   // change "string_table" name to suit
{
    {m1r1,m1r2,m1r3,m1r4},
    {m2r1,m2r2,m2r3,m2r4},
    {m3r1,m3r2,m3r3,m3r4},
    {m4r1,m4r2,m4r3,m4r4},
    {m5r1,m5r2,m5r3,m5r4},
};

const char* const sub_menu_strings[][4]  =
{
{sm1r1,sm1r2,sm1r3,sm1r4},
{sm2r1,sm2r2,sm2r3,sm2r4},
{sm3r1,sm3r2,sm3r3,sm3r4},
{sm4r1,sm4r2,sm4r3,sm4r4},
{sm5r1,sm5r2,sm5r3,sm5r4},
};



struct ssubmenuitem {
    int minvalue=0;
    int maxvalue=0;
    String submenuStringvalues[2];
};

struct smenuitem {
  int value=0;
  bool useStringvalue=false;
  bool callFunc=false;
  bool hasSubMenu=true;
  ssubmenuitem smenitem;
};
#define I2C_ADDR 0x3f
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
#define ONE_WIRE_BUS1 A2
#define CirkPumpBus 6
#define ShuntDirBus 5
#define ShuntBus 4

#define ScreentimeOutSec 10
#define ButtontimeOutSec 5
#define ROFF HIGH
#define RON LOW

OneWire oneWire1(ONE_WIRE_BUS1);
DallasTemperature TempOutSide(&oneWire1);
#define ONE_WIRE_BUS2 A3
OneWire oneWire2(ONE_WIRE_BUS2);
DallasTemperature TempPipe(&oneWire2);

const int del=100;
char buffer[21];
const int pc=3; //portcount
unsigned long ScreentimeOut=0;
unsigned long ShunttimeOut=0;
unsigned long buttoninactive=0;
unsigned long shuntdelay=0;
unsigned long runshuntdelay=0;
unsigned long btnDelay;
unsigned long gendel;
 unsigned long ShuntrunSec =3;
float val=0;
float val2=0;
int currview=0;
int currOutSideTemp=0;
int currPipeTemp=0;
int secLeft=0;
int secRight=0;
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
float factor1=1.0;
float factor2=1.0;
float lastfactor=1;
bool shuntlock=false;
int ports[pc][4] = {
  {7, 0, 0, 0} ,
  {8, 0, 0, 0} ,
  {9, 0, 0, 0} };
const int Chnums=21;



 smenuitem menuItems[menuitemCount];

  bool inMainView=false;
  bool insubmenu=false;
  int currentmenuitem=0;
  int currentsubmenuitem=0;
  int submenuItemscount=0;

bool ERROR=false;
bool firstrun=true;
int pdelay=200;
void setup()
{
defMenu();
pinMode(ShuntBus, OUTPUT);
pinMode(ShuntDirBus, OUTPUT);
pinMode(CirkPumpBus, OUTPUT);
digitalWrite(ShuntBus, ROFF);
digitalWrite(ShuntDirBus, ROFF);
digitalWrite(CirkPumpBus, ROFF);

Serial.begin(9600);
lcd.begin (20,4);
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);
/*
lcd.clear() ; // go home on LCD
char st[Chnums]="####################";
lcd.setCursor(0, 0);
lcd.print(st);
lcd.setCursor(0, 1);
lcd.print("# Utvecklad 2017   #");
lcd.setCursor(0, 2);
lcd.print("# Johan Hahne      #");
lcd.setCursor(0, 3);
lcd.print(st);
*/

//lcd.print("");

for(int i = 0;i<pc;i++){
pinMode(ports[i][0], OUTPUT);
digitalWrite(ports[i][0],ROFF);}
buttoninactive=millis();
gendel=millis();
SetCurrTemp();
  //discoverOneWireDevices();
  MainView();
}


void loop()
{

if (millis()-gendel >= pdelay) {

  val=analogRead(A0);
  val2=analogRead(A1);

setFactors();
readButtons();
if ( millis() - buttoninactive >= ButtontimeOutSec*1000 ){
    if (!insubmenu){
        if (firstrun){
        lcd.clear();
        firstrun=false;
          }
          if(!ERROR){
          TempView();
            } else {
            errorView();
            }
    }
    //buttoninactive=millis();
}
  if ( millis() - ScreentimeOut >= ScreentimeOutSec * 1000 ){
      if (!insubmenu){
            if(!ERROR){
                  lcd.setBacklight(LOW);
            } else {
                  lcd.setBacklight(HIGH);
             }

      }
  }

  }
  if ( millis() - ShuntrunSec >= menuItems[4].value *1000){
    runshunt(false,true);
  }
  if ( millis() - ShunttimeOut >= menuItems[2].value *1000){
    calcTempAndRunShunt();
    if (!insubmenu){
        if(!ERROR){
            TempView();
        } else {
            errorView();
        }
    }
  }

}


void defMenu(){
    //meny 1
    int m =0;
    menuItems[m].useStringvalue=true;
    menuItems[m].smenitem.maxvalue=1;
    menuItems[m].smenitem.minvalue=0;
    menuItems[m].smenitem.submenuStringvalues[0]="Ja";
    menuItems[m].smenitem.submenuStringvalues[1]="Nej";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }
    //meny2
    m =1;
    menuItems[m].useStringvalue=false;
    menuItems[m].smenitem.maxvalue=30;
    menuItems[m].smenitem.minvalue=0;

    menuItems[m].smenitem.submenuStringvalues[0]="";
    menuItems[m].smenitem.submenuStringvalues[1]="";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }
    //meny3
    m =2;

    menuItems[m].useStringvalue=false;
    menuItems[m].smenitem.maxvalue=30;
    menuItems[m].smenitem.minvalue=0;

    menuItems[m].smenitem.submenuStringvalues[0]="";
    menuItems[m].smenitem.submenuStringvalues[1]="";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }
    m =3;
    menuItems[m].useStringvalue=false;
    menuItems[m].smenitem.maxvalue=150;
    menuItems[m].smenitem.minvalue=80;
    menuItems[m].smenitem.submenuStringvalues[0]="";
    menuItems[m].smenitem.submenuStringvalues[1]="";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }
    m=4;

    menuItems[m].useStringvalue=false;
    menuItems[m].smenitem.maxvalue=10;
    menuItems[m].smenitem.minvalue=1;
    menuItems[m].callFunc=false;
    menuItems[m].hasSubMenu=true;

    menuItems[m].smenitem.submenuStringvalues[0]="";
    menuItems[m].smenitem.submenuStringvalues[1]="";
    if( EEPROM.read(m) > menuItems[m].smenitem.maxvalue){
    menuItems[m].value= menuItems[m].smenitem.minvalue;
    } else{
    menuItems[m].value= EEPROM.read(m);
    }


}
/*
void char *arr,String str){

for (int i =0;i<Chnums;i++){
    if(i > str.length()){
        arr[i]='\0';
        break;
    } else {
        arr[i]=str[i];
    }

}
return str;
}
*/

void runshunt(bool right,bool off){
  if (off){
    digitalWrite(ShuntDirBus, ROFF);
    digitalWrite(ShuntBus, ROFF);

  } else {
    if (right){
      if (secRight < menuItems[3].value+menuItems[4].value){

    digitalWrite(ShuntDirBus, RON);
    digitalWrite(ShuntBus, RON);
    secRight=secRight+menuItems[4].value;
    ShuntrunSec=millis();
    }
    secLeft=0;
    } else {
      if (secLeft < menuItems[3].value+menuItems[4].value){

      digitalWrite(ShuntDirBus, ROFF);
      digitalWrite(ShuntBus, RON);
      secLeft=secLeft+menuItems[4].value;
      ShuntrunSec=millis();
    }
      secRight=0;

    }
  }


}


void errorView()
{

//    lcd.clear();
    char framledning[] ="Framledning:";
    char utegivare[] ="Utegivare: ";
    char outstr[] ="*FEL*TEMPGIVARE*FEL*";
    char outtemp[4];
    char intemp[4];
    String s;
    s=(String)currOutSideTemp;
    s.toCharArray(outtemp,4);
    s=(String)currPipeTemp;
    s.toCharArray(intemp,4);

    Printstring(0,0,outstr);
    if (currOutSideTemp < -126){
    memset(outstr, 0, sizeof(outstr));
    strcpy(outstr,utegivare);
    strcat(outstr, " FEL");

    } else {
        memset(outstr, 0, sizeof(outstr));
        strcpy(outstr,utegivare);
        strcat(outstr,outtemp);
        strcat(outstr,";C");


    }
    Printstring(1,0,outstr );
     if (currPipeTemp < -126){
         memset(outstr, 0, sizeof(outstr));
         strcpy(outstr,framledning);
         strcat(outstr, " FEL");

     } else {
         memset(outstr, 0, sizeof(outstr));
         strcpy(outstr,framledning);
         strcat(outstr,intemp);
         strcat(outstr,";C");
     }
    Printstring(2,0,outstr);
    memset(outstr, 0, sizeof(outstr));
    strcpy(outstr,"Ger ut MAX temp! ");
    Printstring(3,0,outstr);

}



void MainView()
{
    lcd.clear();
    String outstr ="   Temperaturer:";
    Printstring(0,0,outstr);
    outstr ="Ute: "+(String)currOutSideTemp+";C";
    Printstring(1,0,outstr);
    outstr ="Framledning: "+(String)currPipeTemp+";C";
    Printstring(2,0,outstr);

}
void TempView()
{
    //lcd.clear();
    inMainView=true;

    SetCurrTemp();
    String outstr ="Ute:"+(String)currOutSideTemp+";C :Fr"+(String)currPipeTemp+";C";
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
    int c = Chnums-1;
    for (int i = 0; i<c;i++)
    {
        if (outstring[i] != 0)
        {
        if (outstring[i] == ';')
        {
            lcd.print((char)223);
            //Serial.print((char)223);
        } else {
            if (outstring[i] == '@'){
                if (menuItems[currentmenuitem].useStringvalue){
                    lcd.print(menuItems[currentmenuitem].smenitem.submenuStringvalues[menuItems[currentmenuitem].value]);
            //        Serial.print(menuItems[currentmenuitem].smenitem.submenuStringvalues[menuItems[currentmenuitem].value]);
            //        Serial.print((String)menuItems[currentmenuitem].value);
                } else {
                    lcd.print(menuItems[currentmenuitem].value);
            //        Serial.print((String)menuItems[currentmenuitem].value);
                }
            } else {
                lcd.print(outstring[i]);
            //    Serial.print(outstring[i]);
            }
        }
        }
    }
    //Serial.println();
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
          buttoninactive=millis();
          //Serial.print(buttoninactive);
    }


    }
}
btnDelay=millis();
}
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
Serial.print(item);
PrintMenuOnScreen(false);

}

void showsubmenuitem(int menuitem,int submenuitem){
if (menuItems[currentmenuitem].hasSubMenu){
    insubmenu=true;
    lcd.clear();
    menuItems[currentmenuitem].value=submenuitem;
    PrintMenuOnScreen(true);

}

}


void PrintMenuOnScreen(bool submenu){

for(int row=0;row<4;row++){
    memset(buffer,0,sizeof(buffer));
    if(!submenu){
    strcpy(buffer,menu_strings[currentmenuitem][row]);
    } else {
    strcpy(buffer,sub_menu_strings[currentmenuitem][row]);
    }
    Serial.print(buffer);
    lcd.setCursor(0, row);
    int c = sizeof(buffer) ;
    for (int i = 0; i<c;i++)
    {
        if (buffer[i] != 0)
        {
        if (buffer[i] == ';')
        {
            lcd.print((char)223);
            Serial.print((char)223);
        } else {
            if (buffer[i] == '@'){
                if (menuItems[currentmenuitem].useStringvalue){
                    lcd.print(menuItems[currentmenuitem].smenitem.submenuStringvalues[menuItems[currentmenuitem].value]);
                    Serial.print(menuItems[currentmenuitem].smenitem.submenuStringvalues[menuItems[currentmenuitem].value]);
                    Serial.print((String)menuItems[currentmenuitem].value);
                } else {
                    lcd.print(menuItems[currentmenuitem].value);
                    Serial.print((String)menuItems[currentmenuitem].value);
                }
            } else {
                lcd.write(buffer[i]);
                Serial.print(buffer[i]);
            }
        }
        }
    }
    Serial.println();
    }
}



/*
void discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  Serial.print("Looking for 1-Wire devices...\n\r");
  while(ds.search(addr)) {
    Serial.print("\n\rFound \'1-Wire\' device with address:\n\r");
    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!\n");
        return;
    }
  }
  Serial.print("\n\r\n\rThat's it.\r\n");
  ds.reset_search();
  return;
}
*/
void SetCurrTemp(){
TempOutSide.requestTemperatures();
currOutSideTemp=(int)TempOutSide.getTempCByIndex(0);
TempPipe.requestTemperatures();
currPipeTemp=(int)TempPipe.getTempCByIndex(0);

if ((currOutSideTemp > -126 )&&(currPipeTemp > -126)){
    ERROR=false;
} else {
    ERROR=true;

}

}

void calcTempAndRunShunt(){
SetCurrTemp();
ShunttimeOut=millis();
if(!ERROR){
        int aTemp = (int)calcTemp(currOutSideTemp);
            if (currPipeTemp < aTemp){
              runshunt(false,false);
            }
        if (currPipeTemp > aTemp){
              runshunt(true,false);
            }
        } else {
        runshunt(false,false);
       }

}
