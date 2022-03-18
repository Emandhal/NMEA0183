# NMEA0183
NMEA0183 is a library  that decodes NMEA0183 frames

# Presentation
This library can be used in C or C++ applications

## Feature

This driver has been designed to:
* Be fully configurable (each known sentences, by the library, can be specified to be decoded)
* A separated GPS class can be used to decode easily GPS frames received (only C++)
* By default, does not use float/double
* Can use multiples decoders to decode multiples sources

# Usage

## Installation

### Get the sources
Get and add the 4 following files to your project
```
NMEA0183.cpp
NMEA0183.h
NMEA0183.hpp (in case of C++ use)
Conf_NMEA0183_Template.h
ErrorsDef.h
```
Copy or rename the file `Conf_NMEA0183_Template.h` to `Conf_NMEA0183.h`... Et voila!

## Others directories

### Tests\UnitTest\ directory
_The **Tests\UnitTest** folder contains unit test for Visual Studio 2017 and are not required._

# Configuration

## C version
To set up one or more decoders in the project, you must:
* Configure the library (`Conf_NMEA0183.h`) which will be the same for all decoders
* Declare a NMEA0183_DecodeInput structure for each decoder you will use
* Initialize, with Init_NMEA0183(), the decoder with the NMEA0183_DecodeInput structure previously defined

## C++ version
To set up one or more decoders in the project, you must:
* Configure the library (`Conf_NMEA0183.h`) which will be the same for all decoders
* Declare a NMEA0183decoder class for each decoder you will use

## C++ GPS automated version
To set up one or more decoders in the project, you must:
* Configure the library (`Conf_NMEA0183.h`) which will be the same for all decoders
* Declare a GPSdecoder class for each decoder you will use

# Library usage

## C version
To set up one or more decoders in the project, you must:
* Add each character received by using NMEA0183_AddReceivedCharacter()
* Check if a frame is ready to decode with NMEA0183_IsFrameReadyToProcess()
* If a frame is ready to process, use the NMEA0183_ProcessFrame() function with a NMEA0183_DecodedData structure to receive the decoded data
* Check the NMEA0183_DecodedData.SentenceID to know which decoded data have been processed (if 'SentenceID' = NMEA0183_GGA then the data are in NMEA0183_DecodedData.GGA)

C example:
```c
NMEA0183_DecodeInput NMEA;
NMEA0183_DecodedData FrameData;

NMEA0183_Init_NMEA0183(&NMEA);
while (Serial.CharAvailable()) // Use your own Serial function
{
  NMEA0183_AddReceivedCharacter(&NMEA, Serial.Read()); // Use your own Serial function
  if (NMEA0183_IsFrameReadyToProcess(&NMEA))
  {
    NMEA0183_ProcessFrame(&NMEA, &FrameData);
    // Do what you want with the decoded data in FrameData
  }
}
```

Whole frame string C example:
```cpp
NMEA0183_DecodedData FrameData;

NMEA0183_ProcessLine(NMEA_STRING_TO_PROCESS, &FrameData); // Use you own NMEA0183 string
// Do what you want with the decoded data in FrameData
```

## C++ version
To set up one or more decoders in the project, you must:
* Add each character received by using NMEA0183decoder.AddReceivedCharacter()
* Check if a frame is ready to decode with NMEA0183decoder.IsFrameReadyToProcess()
* If a frame is ready to process, use the NMEA0183decoder.NMEA0183_ProcessFrame() function with a NMEA0183_DecodedData structure to receive the decoded data
* Check the NMEA0183_DecodedData.SentenceID to know which decoded data have been processed (if 'SentenceID' = NMEA0183_GGA then the data are in NMEA0183_DecodedData.GGA)

C++ example:
```cpp
NMEA0183decoder NMEA;
NMEA0183_DecodedData FrameData;

while (Serial.CharAvailable()) // Use your own Serial function
{
  NMEA.AddReceivedCharacter(Serial.Read()); // Use your own Serial function
  if (NMEA.IsFrameReadyToProcess())
  {
    ProcessFrame(&FrameData);
    // Do what you want with the decoded data in FrameData
  }
}
```

Whole frame string C++ example:
```cpp
NMEA0183decoder NMEA;
NMEA0183_DecodedData FrameData;

NMEA.ProcessLine(NMEA_STRING_TO_PROCESS, &FrameData); // Use you own NMEA0183 string
// Do what you want with the decoded data in FrameData
```

## C++ GPS automated version
To set up one or more decoders in the project, you must:
* Add each character received by using GPSdecoder.ProcessCharacter()
* Check the if a new data have been decoded and get the new data (example if GPSdecoder.IsNewTimeAvailable() then get the data with GPSdecoder.GetTime())

C++ example:
```cpp
GPSdecoder GPS;

while (Serial.CharAvailable()) // Use your own Serial function
{
  GPS.ProcessCharacter(Serial.Read()); // Use your own Serial function

  if (GPS.IsNewTimeAvailable())
  {
    // Do what you want with GPS.GetTime()
  }
  if (GPS.IsNewDateAvailable())
  {
    // Do what you want with GPS.GetDate()
  }
  if (GPS.IsNewLatitudeAvailable())
  {
    // Do what you want with GPS.GetLatitudeInDegree()
    // or GPS.GetLatitudeInDegreeMinute()
    // or GPS.GetLatitudeInDegreeMinuteSeconds()
  }
  if (GPS.IsNewLongitudeAvailable())
  {
    // Do what you want with GPS.GetLongitudeInDegree()
    // or GPS.GetLongitudeInDegreeMinute()
    // or GPS.GetLongitudeInDegreeMinuteSeconds()
  }
  if (GPS.IsNewAltitudeAvailable())
  {
    // Do what you want with GPS.GetAltitudeInMeter()
  }
  if (GPS.IsNewSpeedAvailable())
  {
    // Do what you want with GPS.GetSpeedInKnots()
  }
  if (GPS.IsNewTrackAvailable())
  {
    // Do what you want with GPS.GetTrackInDegree()
    // Can use GPS.TrackToCardinal() to convert to cardianl
  }
  if (GPS.IsNewHDOPAvailable())
  {
    // Do what you want with GPS.GetHDOP()
  }
}
```