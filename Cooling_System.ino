// Anjalee Gitthens
// December 13 2022
// Final Project

#include <Wire.h>
#include <RTClib.h>
#include <dht.h>
#include <Stepper.h>
#include <LiquidCrystal.h>

#define RDA 0x80
#define TBE 0x20
#define dht_apin A0
#define SENSOR_MIN 0
#define SENSOR_MAX 172

RTC_DS1307 rtc;
dht DHT;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

volatile unsigned char* port_b = (unsigned char*) 0x25;
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; 
volatile unsigned char* pin_b  = (unsigned char*) 0x23;

volatile unsigned char* port_e = (unsigned char*) 0x2E;
volatile unsigned char* ddr_e  = (unsigned char*) 0x2D; 
volatile unsigned char* pin_e  = (unsigned char*) 0x2C;

volatile unsigned char* port_f = (unsigned char*) 0x31;
volatile unsigned char* ddr_f  = (unsigned char*) 0x30; 
volatile unsigned char* pin_f  = (unsigned char*) 0x2F;

volatile unsigned char* port_g = (unsigned char*) 0x34;
volatile unsigned char* ddr_g  = (unsigned char*) 0x33; 
volatile unsigned char* pin_g  = (unsigned char*) 0x32;

volatile unsigned char* port_h = (unsigned char*) 0x102;
volatile unsigned char* ddr_h  = (unsigned char*) 0x101; 
volatile unsigned char* pin_h  = (unsigned char*) 0x100;

volatile unsigned char *myUCSR0A = (unsigned char *)0xC0;
volatile unsigned char *myUCSR0B = (unsigned char *)0xC1;
volatile unsigned char *myUCSR0C = (unsigned char *)0xC2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0xC4;
volatile unsigned char *myUDR0   = (unsigned char *)0xC6;
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
unsigned int water_reading = A5;//A5 reading for water
unsigned int temp_reading = A0; //A0 reading humidity/temp
int value = 0;
int level = 0;
const int stepsPerRevolution = 90;
Stepper myStepper(stepsPerRevolution, 22, 24, 26, 28);

void setup(){
  Serial.begin(9600);
  
  myStepper.setSpeed(5);
  lcd.begin(16,2);
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();
  *ddr_b |= 0b10000000; //pin 13 red light
  *ddr_b |= 0b01000000; //pin 12 blue light
  *ddr_b |= 0b00100000; //pin 11 green light
  *ddr_b |= 0b00010000; //pin 10 yellow light
  
  *ddr_e |= 0b00000100; //pin 2 
  *ddr_e |= 0b00001000; //pin 3
  *ddr_e |= 0b00100000; //pin 5
  
  *ddr_h |= 0b00001000; //pin 6
  *ddr_h |= 0b00010000; //pin 7

  *ddr_g |= 0b00100000; //pin 4

  delay(1000);
}

void loop(){
  print_val();
  if (*pin_g &= 0b11011111){
    *port_g &= 0b11011111;
    disabled();
  }

  else{
    print_temp(temp_reading);
      if(water_reading > 200){
        running_state();
      }
      else if(water_reading >= 20 && water_reading <= 150 ){
        // drive PB7 LOW
        *port_g &= 0xb11111110;
        idel_state();
      }
  
      else{
        // drive PB7 HIGH
        *port_g |= 0xb00000001;
        error_state();
      }
  }
  
  // print it to the serial port
  print_int(water_reading);
 *port_g |= 0xb00000001;
 delay(200);
}

void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit 7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}
void print_int(unsigned int out_num)
{
  // clear a flag (for printing 0's in the middle of numbers)
  unsigned char print_flag = 0;
  // if its greater than 1000
  if(out_num >= 1000)
  {
    // get the 1000's digit, add to '0' 
    U0putchar(out_num / 1000 + '0');
    // set the print flag
    print_flag = 1;
    // mod the out num by 1000
    out_num = out_num % 1000;
  }
  // if its greater than 100 or we've already printed the 1000's
  if(out_num >= 100 || print_flag)
  {
    // get the 100's digit, add to '0'
    U0putchar(out_num / 100 + '0');
    // set the print flag
    print_flag = 1;
    // mod the output num by 100
    out_num = out_num % 100;
  } 
  // if its greater than 10, or we've already printed the 10's
  if(out_num >= 10 || print_flag)
  {
    U0putchar(out_num / 10 + '0');
    print_flag = 1;
    out_num = out_num % 10;
  } 
  // always print the last digit (in case it's 0)
  U0putchar(out_num + '0');
  // print a newline
  U0putchar('\n');
}
void U0init(int U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
unsigned char U0kbhit()
{
  return *myUCSR0A & RDA;
}
unsigned char U0getchar()
{
  return *myUDR0;
}
void U0putchar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}

void disabled(){
  *port_e |= 0b00010000; // yellow light
}

void idel_state(){
   *port_e |= 0b00100000; // green light
   stepper(); 
}

void error_state(){
   *port_e |= 0b10000000; // red light
   stepper();
}

void running_state(){
   *ddr_e |= 0b010000000; // blue light
   stepper();
}

void print_val(){
  DateTime now = rtc.now();
  U0putchar("Date & Time: ");
  U0putchar(now.year(), DEC);
  U0putchar('/');
  U0putchar(now.month(), DEC);
  U0putchar('/');
  U0putchar(now.day(), DEC);
  U0putchar(" (");
  U0putchar(daysOfTheWeek[now.dayOfTheWeek()]);
  U0putchar(") ");
  U0putchar(now.hour(), DEC);
  U0putchar(':');
  U0putchar(now.minute(), DEC);
  U0putchar(':');
  U0putchar(now.second(), DEC);

  delay(1000); // delay 1 seconds
}

void print_temp(unsigned int num){
 int val= DHT.read11(7);
 int cel= DHT.temperature;
 int humi=DHT.humidity;

  lcd.print("Temperature: ");
  lcd.print(cel);// display the temperature
  lcd.print((char)223);
  lcd.setCursor(0, 1);
  lcd.print("humidity:  ");
  lcd.print(humi); // display the humidity
  delay(1000);
  lcd.clear();
}

void stepper(){
   //Serial.println("clockwise");
   myStepper.step(stepsPerRevolution);
   delay(500);
   // step one revolution in the other direction:
   //Serial.println("counterclockwise");
   myStepper.step(-stepsPerRevolution);
   delay(500);
}
