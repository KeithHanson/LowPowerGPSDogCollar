#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <MicroNMEA.h>

SoftwareSerial gps(3, 4);
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

#define PMTK_Q_RELEASE "$PMTK605*31"

void printUnknownSentence(MicroNMEA& nmea) { 
}

unsigned long previousMillis = 0;  // will store last time the message was printed
const long interval = 2000;        // interval at which to print the message (milliseconds)

void setup() {
  Serial.begin(115200); // The serial port for the Arduino IDE port output
  gps.begin(9600);
  nmea.setUnknownSentenceHandler(printUnknownSentence);
  MicroNMEA::sendSentence(gps, "$PORZB,RMC,1,GGA,1");
  MicroNMEA::sendSentence(gps, PMTK_SET_NMEA_UPDATE_1HZ);
  MicroNMEA::sendSentence(gps, PMTK_Q_RELEASE);
  //mySerial.println(PMTK_SET_NMEA_OUTPUT_ALLDATA);
}


void loop() {
  unsigned long currentMillis = millis();  // grab the current time

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // save the last time you printed the message

    Serial.print("Valid fix: ");
    Serial.println(nmea.isValid() ? "yes" : "no");

    Serial.print("Nav. system: ");
    if (nmea.getNavSystem())
      Serial.println(nmea.getNavSystem());
    else
      Serial.println("none");

    Serial.print("Num. satellites: ");
    Serial.println(nmea.getNumSatellites());

    Serial.print("HDOP: ");
    Serial.println(nmea.getHDOP() / 10., 1);

    Serial.print("Date/time: ");
    Serial.print(nmea.getYear());
    Serial.print('-');
    Serial.print(int(nmea.getMonth()));
    Serial.print('-');
    Serial.print(int(nmea.getDay()));
    Serial.print('T');
    Serial.print(int(nmea.getHour()));
    Serial.print(':');
    Serial.print(int(nmea.getMinute()));
    Serial.print(':');
    Serial.println(int(nmea.getSecond()));

    long latitude_mdeg = nmea.getLatitude();
    long longitude_mdeg = nmea.getLongitude();
    Serial.print("Latitude (deg): ");
    Serial.println(latitude_mdeg / 1000000., 6);

    Serial.print("Longitude (deg): ");
    Serial.println(longitude_mdeg / 1000000., 6);

    long alt;
    Serial.print("Altitude (m): ");
    if (nmea.getAltitude(alt))
      Serial.println(alt / 1000., 3);
    else
      Serial.println("not available");

    Serial.print("Speed: ");
    Serial.println(nmea.getSpeed() / 1000., 3);
    Serial.print("Course: ");
    Serial.println(nmea.getCourse() / 1000., 3);
    Serial.println("");
    Serial.println("");
  }


  while(gps.available()) {
    char c = gps.read();
    nmea.process(c);
    //Serial.print(c);
  }
}
