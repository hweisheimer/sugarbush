/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

#include "Particle.h"
#include "../lib/SparkFun_Photon_Weather_Shield_Library/src/SparkFun_Photon_Weather_Shield_Library.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

Weather sensor;
int pollDelaySec = 600;

// setup() runs once, when the device is first turned on
void setup() {
  //Initialize the I2C sensors and ping them
  sensor.begin();

  sensor.setModeBarometer();
  sensor.enableEventFlags();
  sensor.setOversampleRate(7);
  
  sensor.heaterOff();
	
  //Give the sensors some time to initialize
  delay(10000);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  //Measure Relative Humidity from the HTU21D or Si7021
  float htuRH = sensor.getRH();

  //Measure Temperature from the HTU21D or Si7021
  float htuTemp = sensor.getTempF();
  //Temperature is measured every time RH is requested.
  //It is faster, therefore, to read it from previous RH
  //measurement with getTemp() instead with readTemp()

  float baroPa = sensor.readPressure();
  float baroInHg = (baroPa/100) * 0.0295300;
  float baroTemp = sensor.readBaroTempF();
  
  float temp = (htuTemp + baroTemp) / 2;
  
  // build a JSON payload
  char payload[255];
  snprintf(payload, sizeof(payload),"{\"humidity\":%f, \"temp\":%f, \"barometer\":%f}", htuRH, temp, baroInHg);
  Log.info(payload);

  // Publish an event to the cloud - further handling via webhooks
  Particle.publish("sugarbush/weatherman/env-outdoor", payload, PRIVATE);
  
  // Calm down, sir.
  delay(pollDelaySec * 1000);
}
