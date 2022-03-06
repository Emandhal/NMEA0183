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

## C++ version
To set up one or more decoders in the project, you must:
* Add each character received by using NMEA0183decoder.AddReceivedCharacter()
* Check if a frame is ready to decode with NMEA0183decoder.IsFrameReadyToProcess()
* If a frame is ready to process, use the NMEA0183decoder.NMEA0183_ProcessFrame() function with a NMEA0183_DecodedData structure to receive the decoded data
* Check the NMEA0183_DecodedData.SentenceID to know which decoded data have been processed (if 'SentenceID' = NMEA0183_GGA then the data are in NMEA0183_DecodedData.GGA)

## C++ GPS automated version
To set up one or more decoders in the project, you must:
* Add each character received by using GPSdecoder.ProcessCharacter()
* Check the if a new data have been decoded and get the new data (example if GPSdecoder.IsNewTimeAvailable() then get the data with GPSdecoder.GetTime())