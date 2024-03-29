#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define         MQ_PIN                       (0)     
#define         RL_VALUE                     (5)     
#define         RO_CLEAN_AIR_FACTOR          (9.83)  
 
#define         CALIBARAION_SAMPLE_TIMES     (50)    
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   
                                                     
#define         READ_SAMPLE_INTERVAL         (50)    
#define         READ_SAMPLE_TIMES            (5)     
                                                     
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)

LiquidCrystal_I2C lcd(0x27,20,4);

int redLED= 4;
int yellowLED = 2;
float nrCoValue = 0;
float nrCo2Value = 0;
float LPGCurve[3]  =  {2.3,0.21,-0.47};  
float COCurve[3]  =  {2.3,0.72,-0.34};    
float SmokeCurve[3] ={2.3,0.53,-0.44};                                                  
float Ro           =  10;                 
 
void setup()
{
  Serial.begin(9600);                               
  Serial.print("Calibrating...\n"); 

  pinMode (redLED,OUTPUT);
  pinMode (yellowLED,OUTPUT);
   
   lcd.init();                      
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Calibrating...\n");
  lcd.print("");
  lcd.print(""); 
               
  Ro = MQCalibration(MQ_PIN);                       
  
  Serial.print("Calibration is done...\n"); 
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\n");

  lcd.init();                      
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Calibration done...");
  lcd.print("");
  lcd.print("");
  lcd.setCursor(0,1);
  lcd.print("Ro=");
  lcd.print(Ro);
  lcd.print("kohm");
  lcd.print("");
   lcd.setCursor(0,2);
  lcd.print("");
   lcd.setCursor(2,3);
  lcd.print("");
  delay(500);
}
 
void loop()
{
   Serial.print("LPG:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_LPG) );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("CO:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO) );
   Serial.print( "ppm" );
   Serial.print("    ");   
   Serial.print("SMOKE:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE) );
   Serial.print( "ppm" );
   Serial.print("\n");
   
   lcd.init();                      
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("CO: ");
  lcd.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO) );
  lcd.print(" ppm" );
  lcd.setCursor(0,1);
  lcd.print("CO2: ");
  lcd.print(MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE) );
  lcd.print(" ppm");
  lcd.print("");
   lcd.setCursor(0,2);
  lcd.print("");
   lcd.setCursor(2,3);
  lcd.print("");
   delay(200);

if (MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO)==nrCoValue){
  digitalWrite(redLED,HIGH);
  }

  else {
  digitalWrite(redLED,LOW);
  }

  if (MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE)==nrCo2Value){
  digitalWrite(yellowLED,HIGH);
  }

  else {
  digitalWrite(yellowLED,LOW);
  }


  
}

float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
 

float MQCalibration(int mq_pin)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   
 
  val = val/RO_CLEAN_AIR_FACTOR;                        
 
  return val; 
}

float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}

int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_LPG ) {
     return MQGetPercentage(rs_ro_ratio,LPGCurve);
  } else if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  }    
 
  return 0;
}
 
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
