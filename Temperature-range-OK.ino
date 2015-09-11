#include <OneWire.h> 
#include <SoftwareSerial.h>
#include <Nextion.h>

int Temp_Pin = 7; //DS18S20 Signal pin on digital 7

//Temperature chip i/o
OneWire ds(Temp_Pin); // on digital pin 7
SoftwareSerial nextion(2, 3);   // Nextion's pins TX 2, RX 3
Nextion myNextion(nextion, 9600);


void setup(void) {
 Serial.begin(9600);
 myNextion.init(); 
}

void loop(void) {
  //functions
  float temperature = getTemp();
  int flag = statetemp(temperature);
  boolean o;
  int i;
  //String boton = "a";
  String message = myNextion.listen();
  
  //Turn on notifications
  if (message == "65 0 1 0 ffff ffff ffff"){
         
         //turn on notifications                        
           
              if (flag == 1){
                //temp normal
                myNextion.listen();
                myNextion.sendCommand("p0.pic=6");
                myNextion.sendCommand("p1.pic=5"); 
  
              }
              else if (flag == 2){
                //temp hot
                myNextion.listen();
                myNextion.sendCommand("p0.pic=0");
                myNextion.sendCommand("p1.pic=3"); 
                }
              else if (flag == 3){
                //temp cold
                myNextion.listen();
                myNextion.sendCommand("p0.pic=2");
                myNextion.sendCommand("p1.pic=4");
              }
              myNextion.sendCommand("p2.pic=12");
  }
  if (message == "65 0 5 0 ffff ffff ffff"){
     //turned off
     o=false;
     myNextion.listen();
     myNextion.sendCommand("p0.pic=6");
     myNextion.sendCommand("p1.pic=5");   
     myNextion.sendCommand("p2.pic=11");                                             
  }  
     
  
 //print on Nextion
 myNextion.listen();
 myNextion.setComponentText("t0",String(temperature));
  
 Serial.println(temperature);
 delay(1000); //just here to slow down the output so it is easier to read

 }
 
int statetemp(float temp){
 //llamado a valor de temp para obtener mascarado
 float temperature = temp;
 // Validaciones
 int flag; 
 int a,b,c,d;
// a=15;
// b=28;
// c=30;
// d=39; 
 a=26;
 b=30;
 c=35;
 

 if (int(temperature) >= a && int(temperature) <=b) {
    flag = 1;
 }
else if (int(temperature) >= c ){
    //temperature is higher than 31 degrees, the Nextion shows alert
    flag = 2;
 }  
 else if (int(temperature) < a){
    flag = 3;
 }
 
  return flag;  
  
}


//---------------------------------------------------------------------------------------------------------------------------//

float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
  
}
