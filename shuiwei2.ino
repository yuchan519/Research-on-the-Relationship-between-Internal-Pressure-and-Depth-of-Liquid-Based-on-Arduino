//Port Settings
#define HX710_SCK 5//hx710 clock port
#define HX710_DT 6//hx710 data port
#define LATCHPIN 4//3 digital tube ports
#define CLOCKPIN 7
#define DATAPIN 8
//The key port
#define WATERlEVEL_0 A1
#define WATERlEVEL_10 A3
#define WATERLEVEL_MEAS A2 

long HX710_Buffer = 0;
long Pressure_start = 0,Pressure_temp = 0;
int Pressure = 0;
int key0 =1;
int key10 =1;
int keyMeas =1 ;
int b = 0;
double k;
int level = 0;
//Data display value
unsigned char Dis_table[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};  //LED status display variable
unsigned char Dis_buf[]   = {0xF1,0xF2,0xF4,0xF8};
unsigned char disbuff[]  =  {0, 0, 0, 0};
//read hx710
unsigned long HX710_Read(void)  //Gain 128
{
  unsigned long count; 
  unsigned char i;
  bool Flag = 0;
  digitalWrite(HX710_DT, HIGH);
  delayMicroseconds(1);
  digitalWrite(HX710_SCK, LOW);
  delayMicroseconds(1);
    count=0; 
    while(digitalRead(HX710_DT)); 
    for(i=0;i<24;i++)
  { 
      digitalWrite(HX710_SCK, HIGH); 
    delayMicroseconds(1);
      count=count<<1; 
    digitalWrite(HX710_SCK, LOW); 
    delayMicroseconds(1);
      if(digitalRead(HX710_DT))
      count++; 
  } 
  digitalWrite(HX710_SCK, HIGH); 
  count ^= 0x800000;
  delayMicroseconds(1);
  digitalWrite(HX710_SCK, LOW); 
  delayMicroseconds(1);
  
  return(count);
}
//read the pressure
unsigned int Get_Pressure()
{
  HX710_Buffer = HX710_Read();
  HX710_Buffer = HX710_Buffer/100;

  Pressure_temp = HX710_Buffer;
  Pressure_temp = Pressure_temp - Pressure_start;       //Obtain the AD sampling value of the real object
  
  Pressure_temp = (unsigned int)((float)Pressure_temp/7.13);   
    //Because the characteristic curves of different sensors are different, each sensor needs to correct the divisor of 4.30 here
    //When it is found that the test value is too large, increase the value
    //If the test value is small, reduce the change value
    //This value is usually around 7.16. Depending on the sensor.
   
  return Pressure_temp;
}
//Digital tube display
void display()
{
  for(char i=0; i<=3; i++)//Scan four digital tubes
  {
    digitalWrite(LATCHPIN,LOW); //Add a low level to the ST_CP port to make the chip ready to receive data
    shiftOut(DATAPIN,CLOCKPIN,MSBFIRST,Dis_table[disbuff[i]]); 
    shiftOut(DATAPIN,CLOCKPIN,MSBFIRST,Dis_buf[i] );
    digitalWrite(LATCHPIN,HIGH); //Restore the pin ST_CP to high level
    delayMicroseconds(1);       //Delay 2 ms 
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(HX710_SCK, OUTPUT); 
  pinMode(HX710_DT, INPUT);
  pinMode(LATCHPIN,OUTPUT);
  pinMode(CLOCKPIN,OUTPUT);
  pinMode(DATAPIN,OUTPUT);
  pinMode(WATERlEVEL_0,INPUT);
  pinMode(WATERlEVEL_10,INPUT);
  pinMode(WATERLEVEL_MEAS,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  key0 = analogRead(WATERlEVEL_0);
  key10 = analogRead(WATERlEVEL_10);
  keyMeas = analogRead(WATERLEVEL_MEAS);
  //Button 1  Read 0mm water level pressure value
  if(key0 == 0){
    delayMicroseconds(10);
    key0 = analogRead(WATERlEVEL_0);
    if(key0 == 0){
      b = Get_Pressure();
    } 
  }
  //Button 3  Read 100mm water level pressure value
  if(key10 == 0){
     delayMicroseconds(10);
     key10 = analogRead(WATERlEVEL_10);
     if(key10 == 0){
      k = ((float)Get_Pressure() - (float)b) / 100.0;
    } 
  }
  //Button4 Read the current water level value
  if(keyMeas == 0){
    delayMicroseconds(10);
    keyMeas = analogRead(WATERLEVEL_MEAS);
    if(keyMeas == 0 ){
      level = ((float)Get_Pressure() - (float)b)/ k;
    }
  }
  Serial.println(level);        
  delay(2); 
    disbuff[0]=(int)level/1000;    
    disbuff[1]=(int)level%1000/100;  
    disbuff[2]=(int)level%100/10;    
    disbuff[3]=(int)level%10;        
    for(char time=0;time<20;time++)
    {
      display();
    }
}
