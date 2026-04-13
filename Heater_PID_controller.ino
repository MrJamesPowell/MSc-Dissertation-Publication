#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v2.h>


#define ONE_WIRE_BUS 15 //Data wire of temperature sensor
#define HEATER_PIN 10 //Mosfet gate
#define STATUS_LED_PIN 6 //Staus LED (external status led, not used in final circuit)

// debug term used for pritnitng things on the fly if theres a problem
bool debug = false; //For normal operation, set to flase. If true P, I and D values will be printed in the command window

// PID tuning parameters (these were chosen using Ziegler-Nichols open loop method)
double Kp = 50.89; // Proportional gain
double Ki = 3.4; //Integral gain (set to 0 for P controller)
double Kd = 114.25; //Derivative gain (set to 0 for P or PI controller)

double TargetTemp = 40.0; //target temperature 
double TempC; //measured temperature, not declared yet
double Output; //PWM output 

double myLastP ;
double myLastI ;
double myLastD ;


OneWire oneWire(ONE_WIRE_BUS); //Declare one wire temp sensor pin is 15
DallasTemperature sensors(&oneWire);



PID myPID(&TempC, &Output, &TargetTemp, Kp, Ki, Kd, DIRECT);

unsigned long lastRequest = 0;
unsigned long lastTempRequest = 0;
const unsigned long sampleIntervalMs = 1000; // sample every 1s
const unsigned long sampleTempIntervalMs = 1000; // sample every 1s


void setup() {

  Serial.begin(9600);
  sensors.begin();

  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);

  //Configure PID
  myPID.SetMode(AUTOMATIC); //Turn the PID on 
  myPID.SetOutputLimits(0,255); //maps to analogwrite
  myPID.SetSampleTime(sampleIntervalMs); // Sets the sample time to 1s

  //delay(1000);
  Serial.print("Temp,PWM");
  if (debug) {
  Serial.print(",Pterm,Iterm,Dterm");
  Serial.print('\n');
  }

}

void loop() {

  unsigned long now = millis();

  // if loop only activates if the sample interval has passed
  if (now - lastTempRequest >= sampleTempIntervalMs) { 
    lastTempRequest = now;

    // request temperature
    sensors.requestTemperatures(); 
    sensors.setWaitForConversion(false); // this sets the temperature sensor to non blocking so PWM and temp can run in parallel
    TempC = sensors.getTempCByIndex(0);
  }

  if (now - lastRequest >= sampleIntervalMs) { 
    lastRequest = now;

    if (TempC < 45) { // safety cutoff
      myPID.Compute(); // calculate a new output 
      myLastP = myPID.GetLastP();
      myLastI = myPID.GetLastI();
      myLastD = myPID.GetLastD();

    }
    else { // set power to zero if safety limit is exceeded
      Output = 0;
    }


    analogWrite(HEATER_PIN, (int)Output); //PWM signal to MOSFET
    analogWrite(STATUS_LED_PIN, (int)Output); //PWM signal to status LED

  
    Serial.print(TempC); //Serial print temperature in deg C
    Serial.print(','); //Comma used as deliminator 
   
    Serial.print((int)Output); //Serial print the PWM signal in command window
  

    if (debug) {
      Serial.print(',');
      Serial.print(myLastP);
      Serial.print(',');
      Serial.print(myLastI);
      Serial.print(',');
      Serial.print(myLastD);
    }
  
    Serial.print('\n');
  
  }

}
