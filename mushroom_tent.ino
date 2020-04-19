/*
 * This code controls the lighting, humidification and air quality 
 * of the mushroom tent.
 * 
 * A four-channel relay module is used.(LOW activates the relay)
 * Relay1 and Relay2 are used in parallel for extra current capacity
 * DHT11 is used for humidity and temperature reading.
 * MQ-135 is used to determine air quality.(has 20s of preheat duration)
 * 
 */

#include <dht11.h>

#define DHT11_PIN 2  // Data pin of DHT11
#define RELAY1 7 // Defining pins for the four-channel relay module
#define RELAY2 6
#define RELAY3 5
#define RELAY4 4
#define MQPIN A0  // MQ-135 pin

int lights = 0;  // State of the lights; 1=On, 0=Off

int humid;  // Variable to store humidity (May not be necessary)
int temp;   // Variable to store temperature (May not be necessary)
int valve_state = 0;
int fan_state = 0;
int ppm;  // Variable to store MQ-135 sensor reading

int light_on_time = 12;   // Led lighting on time duration in hours
int light_off_time = 12;  // Led lighting off time duration in hours
int desiredHumidity = 75;      // Desired value of the humidity
int desiredHumidityError = 5;  // Plus/Minus range in huumidity
int desiredPPM = 20;      // Desired air quality (lower is better)
int desiredPPMerror = 8;  // Desired air quality error
unsigned long current_time = 0;   // This variable will store the current time
unsigned long previous_time = 0;  // This variable will store the previous time

dht11 DHT11;

void setup()
{
  Serial.begin(9600);
  pinMode(RELAY1, OUTPUT); // Define relay pins as output
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);
  delay(3000); // 20s delay for MQ-135 to preheat
}

void loop()
{
  delay(300);  // Slow down the program
  int chk = DHT11.read(DHT11_PIN);
  ppm = analogRead(MQPIN);   // Store air quality value
  humid = DHT11.humidity;    // Store humidity
  temp = DHT11.temperature;  // Store temperature
  Serial.print("Air Quality(PPM); ");
  Serial.print(ppm);
  display_HumidTemp();  // Function that (serial)prints humidity and temperature


  /* Lighting Control */
  current_time = millis();
  if (current_time > previous_time + light_off_time*3600000  && lights == 0)   // If led lighting off time duration
                                                                               // has been completed, turn the lights on
  {
    digitalWrite(RELAY1, HIGH);  // Turn on the lights (deactivate the relay)
    digitalWrite(RELAY2, HIGH);
    Serial.println("--- Lights are turned ON ---");
    lights = 1;
    previous_time = current_time;  // Store the time when the lights are turned on
    delay(2000);
  }
  if (current_time > previous_time + light_on_time*3600000  && lights == 1)   // If led lighting on time duration
                                                                              // has been completed, turn the lights off
  {
    digitalWrite(RELAY1, LOW);  // Turn off the lights
    digitalWrite(RELAY2, LOW);
    Serial.println("--- Lights are turned OFF ---");
    lights = 0;
    previous_time = current_time;  // Store the time when the lights are turned off
    delay(2000);
  }


  /* Humidity Control */
  if (humid < desiredHumidity - desiredHumidityError && valve_state == 0)  // If the humidity is below a certain value and 
                                                                           // the valve is not already open, open the valve
  {
    digitalWrite(RELAY3,LOW);  // This will activate the Relay3 (open the valve)
    Serial.println("--- Valve is opened ---");
    valve_state = 1;
    delay(2000);
  }
  if (humid > desiredHumidity + desiredHumidityError && valve_state == 1)  // If the humidity is above a certain value and 
                                                                           // the valve is open, close the valve
  {
    digitalWrite(RELAY3,HIGH); // This will de-activate the Relay3 (close the valve)
    Serial.println("--- Valve is closed ---");
    valve_state = 0;
    delay(2000);
  }


  /* Air Quality Control */
  if (ppm > desiredPPM + desiredPPMerror && fan_state == 0)
  {
    digitalWrite(RELAY4,LOW); // This will activate the Relay4 (turn on the fan)
    Serial.println("--- The fan is working ---");
    fan_state = 1;
    delay(2000);
  }
  if (ppm < desiredPPM - desiredPPMerror && fan_state == 1)
  {
    digitalWrite(RELAY4,HIGH); // This will de-activate the Relay4 (turn off the fan)
    Serial.println("--- The fan has stopped ---");
    fan_state = 0;
    delay(2000);
  }
}


void display_HumidTemp()  // Function that (serial)prints humidity and temperature
{
  Serial.print(" - Humidity: ");
  Serial.print(humid);
  Serial.print(" %  -  ");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
}
