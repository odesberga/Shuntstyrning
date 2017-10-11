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

float val=0;
float val2=0;
int currview=0;
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
float factor1=1.0;
float factor2=1.0;

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
}


void loop()
{

  val=analogRead(A3);
  val2=analogRead(A4);
setFactors();
    showview(1);
delay(1000);

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
//    lcd.clear();
    String outstr ="Ute temp:"+(String)outtemp+";C";
    Printstring(0,0,outstr);
     outstr ="Shunt temp:"+(String)pipetemp+";C";
    Printstring(1,0,outstr);

}
void TempView()
{
//    lcd.clear();
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
    factor1=((val/1022)+0.8)*1.3;
    factor2=(val2/1022)+1;
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
