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
    showview(0);
delay(1000);
    showview(1);
    delay(1000);

}

void showview(int viewnr)
{
    switch (viewnr) {
        case 0: MainView(10,30);
            break;
        case 1: MainView(0,40);
            break;
    }
}




void MainView(int outtemp,int pipetemp)
{
    lcd.clear();
    String outstr ="Ute temp:"+(String)outtemp+";C";
    Printstring(0,0,outstr);
     outstr ="Shunt temp:"+(String)pipetemp+";C";
    Printstring(1,0,outstr);

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

void setFactors()
{
    factor1=0.9;
    factor2=1.0;
}

integer calcTemp(int outTemp)
{

}


}
