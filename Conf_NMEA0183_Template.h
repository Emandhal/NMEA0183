/* @page License
 *
 * Copyright (c) 2020-2022 Fabien MAILLY
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
#ifndef CONF_NMEA0183_H
#define CONF_NMEA0183_H
//=============================================================================

// If in debug mode, check NULL parameters that are mandatory in each functions of the driver
#ifdef DEBUG
#  define CHECK_NULL_PARAM
#endif

//-----------------------------------------------------------------------------

//! Uncomment the following line if you want the float based tools
#define NMEA0183_FLOAT_BASED_TOOLS // This will add float use of the library to support unit conversion and data conversions

//-----------------------------------------------------------------------------

//! Uncomment the following line if you want the GPS decoder class available (needs NMEA0183_FLOAT_BASED_TOOLS define)
#define NMEA0183_GPS_DECODER_CLASS // This will add the GPS decoder class in case you only want to decode GPS frames

//-----------------------------------------------------------------------------



/******************************************************************************
 * Uncomment each sentence you want to decode.
 * Each sentence commented will not be decoded but available in NMEA0183_DecodedData.SentenceID
 * as NMEA0183_UNKNOWN and the frame will be here: NMEA0183_DecodedData.Frame
 */

//=============================================================================
// Autopilot
//=============================================================================
#define NMEA0183_DECODE_APB
#define NMEA0183_DECODE_BEC


//=============================================================================
// Communications
//=============================================================================


//=============================================================================
// Compass
//=============================================================================
#define NMEA0183_DECODE_VTG


//=============================================================================
// Echo Sounder
//=============================================================================
#define NMEA0183_DECODE_MTW


//=============================================================================
// GPS sentences
//=============================================================================
#define NMEA0183_DECODE_AAM
#define NMEA0183_DECODE_ALM
#define NMEA0183_DECODE_GGA
#define NMEA0183_DECODE_GSA
#define NMEA0183_DECODE_GSV
#define NMEA0183_DECODE_GLL
#define NMEA0183_DECODE_RMC


//=============================================================================
// Heading
//=============================================================================
#define NMEA0183_DECODE_HDG
#define NMEA0183_DECODE_HDM
#define NMEA0183_DECODE_HDT


//=============================================================================
// Others sentences
//=============================================================================
#define NMEA0183_DECODE_TXT
#define NMEA0183_DECODE_VHW
#define NMEA0183_DECODE_ZDA


//=============================================================================
// Weather Instruments
//=============================================================================
#define NMEA0183_DECODE_MWV
//-----------------------------------------------------------------------------



//=============================================================================
// Checks
//=============================================================================
#if defined(NMEA0183_GPS_DECODER_CLASS) && !defined(NMEA0183_FLOAT_BASED_TOOLS)
# error NMEA0183_GPS_DECODER_CLASS needs NMEA0183_FLOAT_BASED_TOOLS to work
#endif
//-----------------------------------------------------------------------------
#endif /* CONF_NMEA0183_H */