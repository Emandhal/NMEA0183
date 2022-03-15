/*******************************************************************************
 * @file    NMEA0183.h
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.0.0
 * @date    13/02/2022
 * @brief   NMEA decoder library
 *
 * Supports common GPS frames
 ******************************************************************************/
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

/* Revision history:s
 * 1.0.0    Release version
 *****************************************************************************/
#ifndef NMEA0183_LIB_H_
#define NMEA0183_LIB_H_
//=============================================================================

//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
//-----------------------------------------------------------------------------
#include "Conf_NMEA0183.h"
#include "ErrorsDef.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
#  define __NMEA0183_PACKED__
#  ifdef ARDUINO
#    define NMEA0183_PACKITEM           __attribute__((packed))
#    define NMEA0183_UNPACKITEM
#  else
#    define NMEA0183_PACKITEM           __pragma(pack(push, 1))
#    define NMEA0183_UNPACKITEM         __pragma(pack(pop))
#  endif
#  define NMEA0183_PACKENUM(name,type)  typedef enum name : type
#  define NMEA0183_UNPACKENUM(name)     name
extern "C" {
#else
#  define __NMEA0183_PACKED__           __attribute__((packed))
#  define NMEA0183_PACKITEM
#  define NMEA0183_UNPACKITEM
#  define NMEA0183_PACKENUM(name,type)  typedef enum __NMEA0183_PACKED__
#  define NMEA0183_UNPACKENUM(name)     name
#endif
//-----------------------------------------------------------------------------

//--- Reserved characters ---
#define NMEA0183_END_CR_DELIMITER               '\r' //! Carriage return - End of sentence delimiter
#define NMEA0183_END_LF_DELIMITER               '\n' //! Line feed - End of sentence delimiter
#define NMEA0183_START_DELIMITER                '$'  //! Start of Parametric sentence delimiter
#define NMEA0183_CHECKSUM_DELIMITER             '*'  //! Checksum field delimiter
#define NMEA0183_FIELD_DELIMITER                ','  //! Field delimiter
#define NMEA0183_START_ENCAPSULATION_DELIMITER  '!'  //! Start of Encapsulation sentence delimiter
#define NMEA0183_RESERVED_BACKSLASH_DELIMITER   '\\' //! Reserved for future use
#define NMEA0183_CHAR_HEX_DELIMITER             '^'  //! Code delimiter for HEX representation of ISO 8859-1 characters
#define NMEA0183_RESERVED_TILDE_DELIMITER       '~'  //! Reserved for future use
#define NMEA0183_RESERVED_DEL_DELIMITER         '\x7F' //! Reserved for future use

//-----------------------------------------------------------------------------

#define NMEA0183_FRAME_BUFFER_SIZE  ( 82+1 ) //! NMEA0183 frame buffer size (According to NMEA 3.01)
#define NMEA0183_NO_VALUE     ( 0xFFFFFFFF ) //! No value or value in error

//-----------------------------------------------------------------------------

//! Sign extending from a variable 'bitwidth' at 'pos' without branching (will be simplified at compile time with 'pos' and 'bitwidth' fixed)
#define NMEA0183_DATA_EXTRACT_TO_SIGNED(out_type,data,pos,bitwidth)  (out_type)( ((((uint32_t)(data) >> (pos)) & ((1 << (bitwidth)) - 1)) ^ (1 << ((bitwidth) - 1))) - (1 << ((bitwidth) - 1)) )

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// NMEA0183 frames data API
//********************************************************************************************************************

//! Talker ID builder of the frame
#define NMEA0183_TALKER_ID(char1, char2)  ( (uint16_t)(char1) | ((uint16_t)(char2) << 8u) )

//! Talker ID enumerator
NMEA0183_PACKENUM(eNMEA0183_TalkerID, uint16_t)
{
  NMEA0183_GA = NMEA0183_TALKER_ID('G', 'A'), //!< Galileo Positioning System
  NMEA0183_GB = NMEA0183_TALKER_ID('G', 'B'), //!< BeiDou (China)
  NMEA0183_GI = NMEA0183_TALKER_ID('G', 'I'), //!< NavIC, IRNSS (India)
  NMEA0183_GL = NMEA0183_TALKER_ID('G', 'L'), //!< GLONASS, according to IEIC 61162-1
  NMEA0183_GN = NMEA0183_TALKER_ID('G', 'N'), //!< Combination of multiple satellite systems (NMEA 1083)
  NMEA0183_GP = NMEA0183_TALKER_ID('G', 'P'), //!< Global Positioning System receiver
  NMEA0183_GQ = NMEA0183_TALKER_ID('G', 'Q'), //!< QZSS regional GPS augmentation system (Japan)
} NMEA0183_UNPACKENUM(eNMEA0183_TalkerID);

//-----------------------------------------------------------------------------

//! Sentence ID builder of the frame
#define NMEA0183_SENTENCE_ID(char1, char2, char3)  ( (uint32_t)(char1) | ((uint32_t)(char2) << 8u) | ((uint32_t)(char3) << 16u) )

//! Talker ID enumerator
NMEA0183_PACKENUM(eNMEA0183_SentencesID, uint32_t)
{
#ifdef NMEA0183_DECODE_AAM
  NMEA0183_AAM = NMEA0183_SENTENCE_ID('A', 'A', 'M'), //!< Waypoint Arrival Alarm
#endif
#ifdef NMEA0183_DECODE_ALM
  NMEA0183_ALM = NMEA0183_SENTENCE_ID('A', 'L', 'M'), //!< GPS Almanac Data
#endif
#ifdef NMEA0183_DECODE_APB
  NMEA0183_APB = NMEA0183_SENTENCE_ID('A', 'P', 'B'), //!< Heading/Track Controller (Autopilot) Sentence "B"
#endif
#ifdef NMEA0183_DECODE_BEC
  NMEA0183_BEC = NMEA0183_SENTENCE_ID('B', 'E', 'C'), //!< Bearing and distance to waypoint - dead reckoning
#endif
#ifdef NMEA0183_DECODE_BOD
  NMEA0183_BOD = NMEA0183_SENTENCE_ID('B', 'O', 'D'), //!< Bearing - Origin to Destination
#endif
#ifdef NMEA0183_DECODE_BWW
  NMEA0183_BWW = NMEA0183_SENTENCE_ID('B', 'W', 'W'), //!< Bearing - Waypoint to Waypoint
#endif
#ifdef NMEA0183_DECODE_GGA
  NMEA0183_GGA = NMEA0183_SENTENCE_ID('G', 'G', 'A'), //!< Global positioning system fixed data
#endif
#ifdef NMEA0183_DECODE_GLL
  NMEA0183_GLL = NMEA0183_SENTENCE_ID('G', 'L', 'L'), //!< Geographic position - latitude / longitude
#endif
#ifdef NMEA0183_DECODE_GSA
  NMEA0183_GSA = NMEA0183_SENTENCE_ID('G', 'S', 'A'), //!< GNSS DOP and active satellites
#endif
#ifdef NMEA0183_DECODE_GSV
  NMEA0183_GSV = NMEA0183_SENTENCE_ID('G', 'S', 'V'), //!< GNSS satellites in view
#endif
#ifdef NMEA0183_DECODE_HDG
  NMEA0183_HDG = NMEA0183_SENTENCE_ID('H', 'D', 'G'), //!< Heading, Deviation & Variation
#endif
#ifdef NMEA0183_DECODE_HDM
  NMEA0183_HDM = NMEA0183_SENTENCE_ID('H', 'D', 'M'), //!< Heading - Magnetic
#endif
#ifdef NMEA0183_DECODE_HDT
  NMEA0183_HDT = NMEA0183_SENTENCE_ID('H', 'D', 'T'), //!< Heading - True
#endif
#ifdef NMEA0183_DECODE_MTW
  NMEA0183_MTW = NMEA0183_SENTENCE_ID('M', 'T', 'W'), //!< Water Temperature
#endif
#ifdef NMEA0183_DECODE_MWV
  NMEA0183_MWV = NMEA0183_SENTENCE_ID('M', 'W', 'V'), //!< Wind Speed and Angle
#endif
#ifdef NMEA0183_DECODE_RMC
  NMEA0183_RMC = NMEA0183_SENTENCE_ID('R', 'M', 'C'), //!< Recommended minimum specific GNSS data
#endif
#ifdef NMEA0183_DECODE_TXT
  NMEA0183_TXT = NMEA0183_SENTENCE_ID('T', 'X', 'T'), //!< Texte message
#endif
#ifdef NMEA0183_DECODE_VHW
  NMEA0183_VHW = NMEA0183_SENTENCE_ID('V', 'H', 'W'), //!< Water Speed and Heading
#endif
#ifdef NMEA0183_DECODE_VTG
  NMEA0183_VTG = NMEA0183_SENTENCE_ID('V', 'T', 'G'), //!< Course Over Ground and Ground Speed
#endif
#ifdef NMEA0183_DECODE_ZDA
  NMEA0183_ZDA = NMEA0183_SENTENCE_ID('Z', 'D', 'A'), //!< Time & Date
#endif
  NMEA0183_UNKNOWN = 0xFFFFFFFF,                      //!< Unknown delimiter (>3 characters)
} NMEA0183_UNPACKENUM(eNMEA0183_SentencesID);

//*****************************************************************************

//! Time structure
NMEA0183_PACKITEM
typedef struct __NMEA0183_PACKED__ NMEA0183_Time
{
  uint8_t Hour;    //!< Hour extracted (0 to 23)
  uint8_t Minute;  //!< Minute extracted (0 to 59)
  uint8_t Second;  //!< Second extracted (0 to 59, 60 in case of leap second)
  uint16_t MilliS; //!< Millisecond extracted (0 to 999)
} NMEA0183_Time;
NMEA0183_UNPACKITEM;

//-----------------------------------------------------------------------------

//! Date structure
NMEA0183_PACKITEM
typedef struct __NMEA0183_PACKED__ NMEA0183_Date
{
  uint8_t Day;   //!< Day extracted (1 to 31 max)
  uint8_t Month; //!< Month extracted (1 to 12)
  uint16_t Year; //!< Second extracted (00 to 99 or 0000 to 9999)
} NMEA0183_Date;
NMEA0183_UNPACKITEM;

//-----------------------------------------------------------------------------

//! Latitude/Longitude coordinate structure
NMEA0183_PACKITEM
typedef struct __NMEA0183_PACKED__ NMEA0183_Coordinate
{
  char Direction;  //!< In case of latitude: 'N' or 'S'. In case of longitude: 'E' or 'W'
  uint8_t Degree;  //!< Degree extracted (Latitude: 0 to 90, Longitude: 0 to 180)
  uint32_t Minute; //!< Minute extracted (divide by 10^7 to get the real minute)
} NMEA0183_Coordinate;
NMEA0183_UNPACKITEM;

//-----------------------------------------------------------------------------

//! Magnetic sensor structure
NMEA0183_PACKITEM
typedef struct __NMEA0183_PACKED__ NMEA0183_Magnetic
{
    uint16_t Value; //!< Value of the sensor extracted (divide by 10^2 to get the real value)
    char Direction; //!< Direction: 'E' = East ; 'W' = West
} NMEA0183_Magnetic;
NMEA0183_UNPACKITEM;

//-----------------------------------------------------------------------------

//! Satellite View structure
typedef struct NMEA0183_SatelliteView
{
  uint8_t SatelliteID; //!< Satellite ID number (0xFF if no satellite)
  uint8_t Elevation;   //!< Elevation, degrees, 90� maximum
  uint16_t Azimuth;    //!< Azimuth, degrees True (000 to 359)
  uint8_t SNR;         //!< SNR (C/No) 00-99 dB-Hz, 0xFF when not tracking
} NMEA0183_SatelliteView;

//*****************************************************************************

#define NMEA0183_AAM_WAYPOINT_ID_MAX_SIZE  ( 25 ) //! ASCII characters + 0 terminal

/*! @brief AAM (Waypoint Arrival Alarm) sentence fields extraction structure
 * Format: $--AAM,<Entered:A/V>,<Waypoint:A/V>,<Circle:r.rr[r][r]>,N,<WaypointID>*<CheckSum>
 * Status of arrival (entering the arrival circle, or pass the perpendicular of the course line) at waypoint
 */
typedef struct NMEA0183_AAMdata
{
  char ArrivalStatus;    //!< Status of arrival: 'A' = arrival circle entered ; 'V' = arrival circle not entered
  char PassedWaypoint;   //!< Status of the passed waypoint: 'A' = arrival circle entered ; 'V' = arrival circle not entered
  uint32_t CircleRadius; //!< Arrival circle radius in nautical miles (divide by 10^4 to get the circle radius in nautical miles)
  char WaypointID[NMEA0183_AAM_WAYPOINT_ID_MAX_SIZE]; //!< Waypoint ID
} NMEA0183_AAMdata;

//-----------------------------------------------------------------------------

/*! @brief ALM (GPS Almanac Data) sentence fields extraction structure
 * Format: $--ALM,<Total:t>,<Curr:c>,<SatPRN:ss>,<WeekNum:[w][w][w]w>,<SV:vv>,<e:eeee>,<toa:yy>,<Sigma_i:iiii>,<OMEGADOT:dddd>,<rootA:rrrrrr>,<OMEGA:oooooo>,<OMEGA0:aaaaaa>,<Mo:mmmmmm>,<af0:aaa>,<af1:bbb>*<CheckSum>
 * Contains GPS week number, satellite health and the complete almanac data for one satellite.
 * Multiple sentences may be transmitted, one for each satellite in the GPS constellation, up to a maximum of 32 sentences.
 */
typedef struct NMEA0183_ALMdata
{
  uint8_t TotalSentence;      //!< Total number of sentences extracted
  uint8_t SentenceNumber;     //!< Sentence number extracted
  uint8_t SatellitePRNnumber; //!< Satellite PRN number (01 to 32) extracted
  uint16_t GPSweekNumber;     //!< GPS week number (0 to 9999) extracted
  uint8_t SV_NAVhealth;       //!< NAV+SV health, bits 17-24 of each almanac page extracted
  uint16_t e;                 //!< e, eccentricity (Scale factor LSB: 2^-21) extracted
  uint8_t toa;                //!< toa, almanac reference time in seconds (Scale factor LSB: 2^12) extracted
  int16_t Sigma_i;            //!< Sigma i (Relative to i0 = 0.3 semi-circles), inclination angle in semi-circles (Scale factor LSB: 2^-19) extracted
  int16_t OMEGADOT;           //!< OMEGADOT, rate of right ascension in semi-circles/sec (Scale factor LSB: 2^-38) extracted
  uint32_t Root_A;            //!< A^(1/2), root of semi-major axis in meters^(1/2) (Scale factor LSB: 2^-11) extracted
  int32_t OMEGA;              //!< OMEGA, longitude of ascension node in semi-circles (Scale factor LSB: 2^-23) extracted
  int32_t OMEGA_0;            //!< OMEGA 0, argument of perigee in semi-circles (Scale factor LSB: 2^-23) extracted
  int32_t Mo;                 //!< Mo, mean anomaly in semi-circles (Scale factor LSB: 2^-23) extracted
  int16_t af0;                //!< af0, clock parameter in seconds (Scale factor LSB: 2^-20) extracted
  int16_t af1;                //!< af1, clock parameter in sec/sec (Scale factor LSB: 2^-38) extracted
} NMEA0183_ALMdata;

//! Codes for health of SV signal components enum
typedef enum
{
  NMEA0183_ALL_SIGNALS_OK        = 0b00000, //!< All signals OK
  NMEA0183_ALL_SIGNALS_WEAK      = 0b00001, //!< All Signals weak (3 to 6 dB below specified power level due to reduced power output, excess phase noise, SV attitude, etc.)
  NMEA0183_ALL_SIGNALS_DEAD      = 0b00010, //!< All signals dead
  NMEA0183_ALL_SIGNALS_NO_MOD    = 0b00011, //!< All signals have no data modulation
  NMEA0183_L1_P_SIGNALS_WEAK     = 0b00100, //!< L1 P signals weak
  NMEA0183_L1_P_SIGNALS_DEAD     = 0b00101, //!< L1 P signals dead
  NMEA0183_L1_P_SIGNALS_NO_MOD   = 0b00110, //!< L1 P signals have no data modulation
  NMEA0183_L2_P_SIGNALS_WEAK     = 0b00111, //!< L2 P signals weak
  NMEA0183_L2_P_SIGNALS_DEAD     = 0b01000, //!< L2 P signals dead
  NMEA0183_L2_P_SIGNALS_NO_MOD   = 0b01001, //!< L2 P signals have no data modulation
  NMEA0183_L1_C_SIGNALS_WEAK     = 0b01010, //!< L1 C signals weak
  NMEA0183_L1_C_SIGNALS_DEAD     = 0b01011, //!< L1 C signals dead
  NMEA0183_L1_C_SIGNALS_NO_MOD   = 0b01100, //!< L1 C signals have no data modulation
  NMEA0183_L2_C_SIGNALS_WEAK     = 0b01101, //!< L2 C signals weak
  NMEA0183_L2_C_SIGNALS_DEAD     = 0b01110, //!< L2 C signals dead
  NMEA0183_L2_C_SIGNALS_NO_MOD   = 0b01111, //!< L2 C signals have no data modulation
  NMEA0183_L1L2_P_SIGNALS_WEAK   = 0b10000, //!< L1 & L2 P signals weak
  NMEA0183_L1L2_P_SIGNALS_DEAD   = 0b10001, //!< L1 & L2 P signals dead
  NMEA0183_L1L2_P_SIGNALS_NO_MOD = 0b10010, //!< L1 & L2 P signals have no data modulation
  NMEA0183_L1L2_C_SIGNALS_WEAK   = 0b10011, //!< L1 & L2 C signals weak
  NMEA0183_L1L2_C_SIGNALS_DEAD   = 0b10100, //!< L1 & L2 C signals dead
  NMEA0183_L1L2_C_SIGNALS_NO_MOD = 0b10101, //!< L1 & L2 C signals have no data modulation
  NMEA0183_L1_SIGNALS_WEAK       = 0b10110, //!< L1 signals weak (3 to 6 dB below specified power level due to reduced power output, excess phase noise, SV attitude, etc.)
  NMEA0183_L1_SIGNALS_DEAD       = 0b10111, //!< L1 signals dead
  NMEA0183_L1_SIGNALS_NO_MOD     = 0b11000, //!< L1 signals have no data modulation
  NMEA0183_L2_SIGNALS_WEAK       = 0b11001, //!< L2 signals weak (3 to 6 dB below specified power level due to reduced power output, excess phase noise, SV attitude, etc.)
  NMEA0183_L2_SIGNALS_DEAD       = 0b11010, //!< L2 signals dead
  NMEA0183_L2_SIGNALS_NO_MOD     = 0b11011, //!< L2 signals have no data modulation
  NMEA0183_SV_OUT                = 0b11100, //!< SV is temporarily out (Do not use this SV during current pass)
  NMEA0183_SV_SOON_OUT           = 0b11101, //!< SV will be temporarily out (use with caution)
  NMEA0183_SPARE                 = 0b11110, //!< Spare
  NMEA0183_MULTIPLE_ANOMALIES    = 0b11111, //!< More than one combination would be required to describe anomalies (except NMEA0183_SV_OUT and NMEA0183_SV_SOON_OUT)
} eNMEA0183_SVhealth;

#define NMEA0183_ALM_SV_HEALTH_Pos         0
#define NMEA0183_ALM_SV_HEALTH_Mask        (0x1Fu << NMEA0183_ALM_SV_HEALTH_Pos)
#define NMEA0183_ALM_SV_HEALTH_GET(value)  (eNMEA0183_SVhealth)(((uint8_t)(value) & NMEA0183_ALM_SV_HEALTH_Mask) >> NMEA0183_ALM_SV_HEALTH_Pos) //!< Get SV health

//! Codes for NAV data health indications enum
typedef enum
{
  NMEA0183_ALL_DATA_OK            = 0b000, //!< All data OK
  NMEA0183_PARITY_FAILURE         = 0b100, //!< Parity failure - some or all parity bad
  NMEA0183_TLM_HOW_FORMAT_PROBLEM = 0b010, //!< TLM/HOW format problem - any departure from standard format (e.g., preamble misplaced and/or incorrect, etc.), except for incorrect Z-count, as reported in HOW
  NMEA0183_ZCOUNT_IN_HOW_BAD      = 0b110, //!< Z-count in HOW bad - any problem with Z-count value not reflecting actual code phase
  NMEA0183_SUBFRAMES_1_2_3        = 0b001, //!< Subframes 1, 2, 3 - one or more elements in words three through ten of one or more subframes are bad
  NMEA0183_SUBFRAMES_4_5          = 0b101, //!< Subframes 4, 5 - one or more elements in words three through ten of one or more subframes are bad
  NMEA0183_ALL_UPLOADED_DATA_BAD  = 0b011, //!< All uploaded data bad - one or more elements in words three through ten of any one (or more) subframes are bad
  NMEA0183_ALL_DATA_BAD           = 0b111, //!< All data bad - TLM word and/or HOW and one or more elements in any one (or more) subframes are bad
} eNMEA0183_NAVhealth;

#define NMEA0183_ALM_NAV_HEALTH_Pos         5
#define NMEA0183_ALM_NAV_HEALTH_Mask        (0x7u << NMEA0183_ALM_NAV_HEALTH_Pos)
#define NMEA0183_ALM_NAV_HEALTH_GET(value)  (eNMEA0183_NAVhealth)(((uint8_t)(value) & NMEA0183_ALM_NAV_HEALTH_Mask) >> NMEA0183_ALM_NAV_HEALTH_Pos) //!< Get NAV health

//-----------------------------------------------------------------------------

#define NMEA0183_APB_WAYPOINT_ID_MAX_SIZE  ( 10 ) //! ASCII characters + 0 terminal

/*! @brief APB (Heading/Track Controller (Autopilot) Sentence "B") sentence fields extraction structure
 * Format: $--APB,<Status:A/V>,<Status:A/V>,<Magnitude:m.m[m][m][m]>,<L/R>,<N/K>,<A/V>,<A/V>,<BOtoD:b[.b][b]>,<M/T>,<WaypointID>,<BCPtoD:c[.c][c]>,<M/T>,<H2StoD:h[.h][h]>,<M/T>,<FAA:A/D/E/M/S/N>*<CheckSum>
 * Commonly used by autopilots, this sentence contains navigation receiver warning flag status, cross-trackerror, waypoint arrival status, initial bearing from origin waypoint to the destination,
 * continuous bearing from present position to destination and recommended heading-to-steer to destination waypoint for the active navigation leg of the journey.
 */
typedef struct NMEA0183_APBdata
{
  char Status1;                  //!< Status of the frame: 'A' = Data valid ; 'V' = Loran-C Blink or SNR warning ; 'V' = General warning flag for other navigation systems when a reliable fix is not available
  char Status2;                  //!< Status of the frame: 'A' = Data valid or not used ; 'V' = Loran-C Cycle Lock warning flag
  int32_t MagnitudeXTE;          //!< Magnitude of XTE (cross-track-error), see XTEunit for the unit (divide by 10^4 to get the real error magnitude)
  char DirectionSteer;           //!< Direction to steer: 'L' = Left ; 'R' = Right
  char XTEunit;                  //!< XTE units: 'N' = nautical miles ; 'K' = kilometers
  char ArrivalStatus;            //!< Status of arrival: 'A' = arrival circle entered ; 'V' = arrival circle not entered
  char PassedWaypoint;           //!< Status of the passed waypoint: 'A' = arrival circle entered ; 'V' = arrival circle not entered
  uint16_t BearingOriginToDest;  //!< Bearing origin to destination, see BearingOtoDunit for the unit (divide by 10^2 to get the real bearing origin to destination)
  char BearingOtoDunit;          //!< Bearing origin to destination unit: 'M' = magnetic ; 'T' = true
  char WaypointID[NMEA0183_APB_WAYPOINT_ID_MAX_SIZE]; //!< Destination waypoint ID
  uint16_t BearingCurPosToDest;  //!< Bearing present position to destination, see BearingCPtoDunit for the unit (divide by 10^2 to get the real Bearing, present position to destination)
  char BearingCPtoDunit;         //!< Bearing present position to destination unit: 'M' = magnetic ; 'T' = true
  uint16_t HeadingToSteerToDest; //!< Heading-to-steer to destination waypoint, see H2StoDunit for the unit (divide by 10^2 to get the real heading-to-steer to destination waypoint)
  char H2StoDunit;               //!< Heading-to-steer to destination waypoint unit: 'M' = magnetic ; 'T' = true
  char FAAmode;                  //!< FAA mode indicator (NMEA 3.0 and later): ' ' = Not specified in the frame ; 'A' = Autonomous mode ; 'D' = Differential Mode ; 'E' = Estimated (dead-reckoning) mode ; 'M' = Manual Input Mode ; 'S' = Simulated Mode ; 'N' = Data Not Valid
} NMEA0183_APBdata;

//-----------------------------------------------------------------------------

#define NMEA0183_BEC_WAYPOINT_ID_MAX_SIZE  ( 10 ) //! ASCII characters + 0 terminal

/*! @brief BEC (Bearing and distance to waypoint - dead reckoning) sentence fields extraction structure
 * Format: $--BEC,<hhmmss.zzz>,<Latitude:ddmm.mmmm[m][m][m]>,<N/S>,<Longitude:dddmm.mmmm[m][m][m]>,<E/W>,<BearingTrue:t[.t][t]>,T,<BearingMag:m[.m][m]>,M,<Distance:sss.ss[s][s]>,N,<WaypointID>*<CheckSum>
 * Time (UTC) and distance & bearing to, and location of, a specified waypoint from the dead-reckoned present position
 */
typedef struct NMEA0183_BECdata
{
  NMEA0183_Time Time;               //!< Time extracted
  NMEA0183_Coordinate WaypointLat;  //!< Latitude extracted
  NMEA0183_Coordinate WaypointLong; //!< Longitude extracted
  uint16_t BearingTrue;             //!< Bearing, degrees True (divide by 10^2 to get the real bearing True)
  uint16_t BearingMagnetic;         //!< Bearing, degrees Magnetic (divide by 10^2 to get the real bearing Magnetic)
  uint32_t Distance;                //!< Distance, nautical miles (divide by 10^4 to get the real distance)
  char WaypointID[NMEA0183_BEC_WAYPOINT_ID_MAX_SIZE]; //!< Destination waypoint ID
} NMEA0183_BECdata;

//-----------------------------------------------------------------------------

#define NMEA0183_BOD_WAYPOINT_ID_MAX_SIZE  ( 10 ) //! ASCII characters + 0 terminal

/*! @brief BOD (Bearing - Origin to Destination) sentence fields extraction structure
 * Format: $--BOD,<BearingTrue:t[.t][t]>,T,<BearingMag:m[.m][m]>,M,<DestWaypointID>,<OriginWaypointID>*<CheckSum>
 * Bearing angle of the line, calculated at the origin waypoint, extending to the destination waypoint from theorigin waypoint for the active navigation leg of the journey
 */
typedef struct NMEA0183_BODdata
{
  uint16_t BearingTrue;     //!< Bearing, degrees True (divide by 10^2 to get the real bearing True)
  uint16_t BearingMagnetic; //!< Bearing, degrees Magnetic (divide by 10^2 to get the real bearing Magnetic)
  char DestWaypointID[NMEA0183_BOD_WAYPOINT_ID_MAX_SIZE];   //!< Destination waypoint ID
  char OriginWaypointID[NMEA0183_BOD_WAYPOINT_ID_MAX_SIZE]; //!< Origin waypoint ID
} NMEA0183_BODdata;

//-----------------------------------------------------------------------------

#define NMEA0183_BWW_WAYPOINT_ID_MAX_SIZE  ( 10 ) //! ASCII characters + 0 terminal

/*! @brief BWW (Bearing - Waypoint to Waypoint) sentence fields extraction structure
 * Format: $--BWW,<BearingTrue:t[.t][t]>,T,<BearingMag:m[.m][m]>,M,<DestWaypointID>,<OriginWaypointID>*<CheckSum>
 * Bearing angle of the line, between the "TO" and the "FROM" waypoints, calculated at the "FROM" waypoint for any two arbitrary waypoints
 */
typedef struct NMEA0183_BWWdata
{
  uint16_t BearingTrue;     //!< Bearing, degrees True (divide by 10^2 to get the real bearing True)
  uint16_t BearingMagnetic; //!< Bearing, degrees Magnetic (divide by 10^2 to get the real bearing Magnetic)
  char FromWaypointID[NMEA0183_BWW_WAYPOINT_ID_MAX_SIZE]; //!< FROM waypoint ID
  char ToWaypointID[NMEA0183_BWW_WAYPOINT_ID_MAX_SIZE];   //!< TO waypoint ID
} NMEA0183_BWWdata;

//-----------------------------------------------------------------------------

/*! @brief GGA (Global positioning system fixed data) sentence fields extraction structure
 * Format: $--GGA,<hhmmss.zzz>,<Latitude:ddmm.mmmm[m][m][m]>,<N/S>,<Longitude:dddmm.mmmm[m][m][m]>,<E/W>,<GPSquality:0/1/2/3/4/5/6/7/8>,<SatUsed:ss>,<HDOP:h.h(h)>,<Altitude:(-)aaa.a[a]>,M,<GeoidSep:(-)gg.g[g]>,M,<AgeDiff:cc.c[c]>,<DiffRef:rrrr>*<CheckSum>
 * Time, position and fix related data for a GPS receiver
 */
typedef struct NMEA0183_GGAdata
{
  NMEA0183_Time Time;            //!< Time extracted
  NMEA0183_Coordinate Latitude;  //!< Latitude extracted
  NMEA0183_Coordinate Longitude; //!< Longitude extracted
  char GPSquality;               //!< GPS Quality Indicator: '0' = Fix not available or invalid ; '1' = GPS SPS Mode, fix valid ; '2' = Differential GPS, SPS Mode, fix valid ; '3' = GPS PPS Mode, fix valid ; '4' = Real Time Kinematic. System used in RTK mode with fixed integers ; '5' = Float RTK. Satellite system used in RTK mode, floating integers ; '6' = Estimated (dead reckoning) Mode ; '7' = Manual Input Mode ; '8' = Simulator Mode
  uint8_t SatellitesUsed;        //!< Number of satellites in use (00 to 12), may be different from the number in view
  uint16_t HDOP;                 //!< Horizontal Dilution of Precision (divide by 100 to get the real HDOP)
  int32_t Altitude;              //!< Altitude re: mean-sea-level (geoid) in meters (divide by 10^2 to get the real altitude)
  int32_t GeoidSeparation;       //!< Geoidal separation in meters, '-' mean-sea-level surface below WGS-84 ellipsoid surface (divide by 10^2 to get the real geoid separation)
  uint16_t AgeOfDiffCorr;        //!< Age of Differential GPS data. Time in seconds since last SC104 Type 1 or 9 update, null field when DGPS is not used (divide by 10^2 to get the real age of differential correction)
  uint16_t DiffRefStationID;     //!< Differential reference station ID (0000 to 1023)
} NMEA0183_GGAdata;

//-----------------------------------------------------------------------------

/*! @brief GLL (Geographic Position - Latitude/Longitude) sentence fields extraction structure
 * Format: $--GLL,<Latitude:ddmm.mmmm[m][m][m]>,<N/S>,<Longitude:dddmm.mmmm[m][m][m]>,<E/W>,<hhmmss.zzz>,<Status:A/V>,<FAA:A/D/E/M/S/N>*<CheckSum>
 * Latitude and Longitude of vessel position, time of position fix and status
 */
typedef struct NMEA0183_GLLdata
{
  NMEA0183_Coordinate Latitude;  //!< Latitude extracted
  NMEA0183_Coordinate Longitude; //!< Longitude extracted
  NMEA0183_Time Time;            //!< Time extracted
  char Status;                   //!< Status of the frame: 'A' = valid ; 'V' = void = warning
  char FAAmode;                  //!< FAA mode indicator (NMEA 3.0 and later): ' ' = Not specified in the frame ; 'A' = Autonomous mode ; 'D' = Differential Mode ; 'E' = Estimated (dead-reckoning) mode ; 'M' = Manual Input Mode ; 'S' = Simulated Mode ; 'N' = Data Not Valid
} NMEA0183_GLLdata;

//-----------------------------------------------------------------------------

#define NMEA0183_SATELLITE_ID_COUNT  ( 12 )

/*! @brief GSA (GNSS DOP and Active Satellites) sentence fields extraction structure
 * Format: $--GSA,<Mode1:A/M>,<Mode2:1/2/3>,[<Sat1:xx>],[<Sat2:xx>],[<Sat3:xx>],[<Sat4:xx>],[<Sat5:xx>],[<Sat6:xx>],[<Sat7:xx>],[<Sat8:xx>],[<Sat9:xx>],[<Sat10:xx>],[<Sat11:xx>],[<Sat12:xx>],<PDOP:p.p>,<HDOP:h.h>,<VDOP:v.v>*<CheckSum>
 * GNSS receiver operating mode, satellites used in the navigation solution reported by the GGA or GNS sentence, and DOP values
 */
typedef struct NMEA0183_GSAdata
{
  char Mode1;   //!< Mode: 'A' = Automatic, allowed to automatically switch 2D/3D ; 'M' = Manual, forced to operate in 2D or 3D mode
  char Mode2;   //!< Mode: '1' = Fix not available ; '2' = 2D (<4 SVs used) ; '3' = 3D (>3 SVs used)
  uint8_t SatelliteIDs[NMEA0183_SATELLITE_ID_COUNT]; //!< ID numbers of satellites used in solution
  uint16_t PDOP; //!< Position Dilution of Precision (divide by 100 to get the real PDOP)
  uint16_t HDOP; //!< Horizontal Dilution of Precision (divide by 100 to get the real HDOP)
  uint16_t VDOP; //!< Vertical Dilution of Precision (divide by 100 to get the real VDOP)
} NMEA0183_GSAdata;

//-----------------------------------------------------------------------------

#define NMEA0183_SAT_VIEW_COUNT_PER_MESSAGES  ( 4 )

/*! @brief GSV (GNSS Satellites in View) sentence fields extraction structure
 * Format: $--GSV,<Total:t>,<Curr:c>,<SatCount:ss>,<SV1:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>[,<SV2:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>][,<SV3:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>][,<SV4:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>],<Text>*<CheckSum>
 * Number of satellites (SV) in view, satellite ID numbers, elevation, azimuth, and SNR value.
 * Four satellites maximum per transmission. Total number of sentences being transmitted and the number of the sentence being transmitted are indicated in the first two fields
 */
typedef struct NMEA0183_GSVdata
{
  uint8_t TotalSentence;  //!< Total number of sentences (1 to 9) extracted
  uint8_t SentenceNumber; //!< Sentence number (1 to 9) extracted
  uint8_t TotalSatellite; //!< Total number of satellites in view extracted
  NMEA0183_SatelliteView SatView[NMEA0183_SAT_VIEW_COUNT_PER_MESSAGES]; //!< Satellite view description
} NMEA0183_GSVdata;

//-----------------------------------------------------------------------------

/*! @brief HDG (Heading - Deviation and Variation) sentence fields extraction structure
 * Format: $--HDG,<Heading:hh.h[h]>,<MagDev:dd.d[d]>,<E/W>,<MagVar:vv.v[v]>,<E/W>*<CheckSum>
 * Heading (magnetic sensor reading), which if corrected for deviation, will produce Magnetic heading, which if offset by variation will provide True heading
 */
typedef struct NMEA0183_HDGdata
{
  uint16_t Heading;            //!< Heading, in degree (divide by 10^2 to get the real heading)
  NMEA0183_Magnetic Deviation; //!< Magnetic deviation in degrees extracted
  NMEA0183_Magnetic Variation; //!< Magnetic variation in degrees extracted
} NMEA0183_HDGdata;

//-----------------------------------------------------------------------------

/*! @brief HDM (Heading - Magnetic) sentence fields extraction structure
 * Format: $--HDM,<Heading:hh.h[h]>,M*<CheckSum>
 * Actual vessel heading in degrees Magnetic produced by any device or system producing magnetic heading.
 */
typedef struct NMEA0183_HDMdata
{
  uint16_t Heading; //!< Heading, in degree Magnetic (divide by 10^2 to get the real heading)
} NMEA0183_HDMdata;

//-----------------------------------------------------------------------------

/*! @brief HDT (Heading - True) sentence fields extraction structure
 * Format: $--HDT,<Heading:hh.h[h]>,T*<CheckSum>
 * Actual vessel heading in degrees True produced by any device or system producing true heading.
 */
typedef struct NMEA0183_HDTdata
{
  uint16_t Heading; //!< Heading, in degree True (divide by 10^2 to get the real heading)
} NMEA0183_HDTdata;

//-----------------------------------------------------------------------------

/*! @brief MTW (Water Temperature) sentence fields extraction structure
 * Format: $--MTW,<WaterTemp:t.t[t]>,C*<CheckSum>
 */
typedef struct NMEA0183_MTWdata
{
  int16_t WaterTemp; //!< Water temperature, in degree Celcius (divide by 10^2 to get the real temperature)
} NMEA0183_MTWdata;

//-----------------------------------------------------------------------------

/*! @brief MWV (Wind Speed and Angle) sentence fields extraction structure
 * Format: $--MWV,<WindAngle:www[.w][w]>,<T/R>,<WindSpeed:ss[.s][s]>,<K/M/N/S>,<A/V>*<CheckSum>
 * When the reference field is set to R (Relative), data is provided giving the wind angle in relation to the vessel's bow/centerline and the wind speed, both relative to the (moving) vessel. Also called apparent wind, this is the wind speed as felt when standing on the (moving) ship.
 * When the reference field is set to T (Theoretical, calculated actual wind), data is provided giving the wind angle in relation to the vessel's bow/centerline and the wind speed as if the vessel was stationary. On a moving ship these data can be calculated by combining the measured relative wind with the vessel's own speed.
 */
typedef struct NMEA0183_MWVdata
{
  uint16_t WindAngle; //!< Wind Angle, degrees (000.00 to 359.00) extracted (divide by 10^2 to get the real angle)
  char Reference;     //!< Reference: 'R' = Relative ; 'T' = Theoretical
  uint16_t WindSpeed; //!< WindSpeed extracted, see WindSpeedUnit for the unit (divide by 10^2 to get the real speed)
  char WindSpeedUnit; //!< Wind speed units: 'K' = Kilometres per hour ; 'M' = Meter per second ; 'N' = Knots ; 'S' = ?
  char Status;        //!< Status of the frame: 'A' = valid ; 'V' = void = warning
} NMEA0183_MWVdata;

//-----------------------------------------------------------------------------

/*! @brief RMC (Recommended Minimum sentence C) sentence fields extraction structure
 * Format: $--RMC,<hhmmss.zzz>,<Status:A/V>,<Latitude:ddmm.mmmm[m][m][m]>,<N/S>,<Longitude:dddmm.mmmm[m][m][m]>,<E/W>,<Speed:sss.ss[s][s]>,<Track:ttt.tt[t][t]>,<ddmmyy>,<MagVar:vv.v[v]>,<E/W>[,<FAA:A/D/E/M/S/N>][,<NavStatus:S/C/U/V>]*<CheckSum>
 * Time, date, position, course and speed data provided by a GNSS navigation receiver. This sentence is transmitted at intervals not exceeding 2-seconds and is always accompanied by RMB when a destination waypoint is active.
 * RMC and RMB are the recommended minimum data to be provided by a GNSS receiver. All data fields must be provided, null fields used only when data is temporarily unavailable
 */
typedef struct NMEA0183_RMCdata
{
  NMEA0183_Time Time;            //!< Time extracted
  char Status;                   //!< Status of the frame: 'A' = valid ; 'V' = void = warning
  NMEA0183_Coordinate Latitude;  //!< Latitude extracted
  NMEA0183_Coordinate Longitude; //!< Longitude extracted
  uint32_t Speed;                //!< Speed over the ground in knots extracted (divide by 10^4 to get the real speed)
  uint32_t Track;                //!< Track angle in degrees (True) extracted (divide by 10^4 to get the real track)
  NMEA0183_Date Date;            //!< Date extracted
  NMEA0183_Magnetic Variation;   //!< Magnetic variation in degrees extracted
  char FAAmode;                  //!< FAA mode indicator (NMEA 2.3 and later): ' ' = Not specified in the frame ; 'A' = Autonomous mode ; 'D' = Differential Mode ; 'E' = Estimated (dead-reckoning) mode ; 'M' = Manual Input Mode ; 'S' = Simulated Mode ; 'N' = Data Not Valid
  char NavigationStatus;         //!< Navigational Status (NMEA 4.1 and later): ' ' = Not specified in the frame ; 'S' = Safe ; 'C' = Caution ; 'U' = Unsafe ; 'V' = Void
} NMEA0183_RMCdata;

//-----------------------------------------------------------------------------

#define NMEA0183_TXT_MESSAGE_MAX_SIZE  ( 61 ) //! ASCII characters, and code delimiters if needed, up to the maximum permitted sentence length (i.e., up to 61 characters including any code delimiters)

/*! @brief TXT (Text Transmission) sentence fields extraction structure
 * Format: $--TXT,<Total:tt>,<Curr:cc>,<TextID:ii>,<Text>,*<CheckSum>
 * Text messages may consist of the transmission of multiple sentences all containing identical field formats when sending a complete message
 */
typedef struct NMEA0183_TXTdata
{
  uint8_t TotalSentence;  //!< Total number of sentences (01 to 99) extracted
  uint8_t SentenceNumber; //!< Sentence number (01 to 99) extracted
  uint8_t TextIdentifier; //!< Text identifier (01 to 99) extracted
  char TextMessage[NMEA0183_TXT_MESSAGE_MAX_SIZE+1]; //!< Text extracted (with a 0 terminal)
} NMEA0183_TXTdata;

//-----------------------------------------------------------------------------

/*! @brief VHW (Water Speed and Heading) sentence fields extraction structure
 * Format: $--VHW,<CourseTrue:t.t[t][t][t]>,T,<CourseMag:m.m[m][m][m]>,M,<SpeedKnots:k.k[k][k][k]>,N,<SpeedKmHr:h.h[h][h][h]>,K*<CheckSum>
 * The compass heading to which the vessel points and the speed of the vessel relative to the water
 */
typedef struct NMEA0183_VHWdata
{
    uint32_t HeadingTrue;     //!< Heading, degrees True (000.0000 to 359.0000) extracted (divide by 10^4 to get the real course)
    uint32_t HeadingMagnetic; //!< Heading, degrees Magnetic (000.0000 to 359.0000) extracted (divide by 10^4 to get the real course)
    uint32_t SpeedKnots;      //!< Speed over the ground in knots (000.0000 to 999.9999) extracted (divide by 10^4 to get the real speed)
    uint32_t SpeedKmHr;       //!< Speed over the ground in km/hr (000.0000 to 999.9999) extracted (divide by 10^4 to get the real speed)
} NMEA0183_VHWdata;

//-----------------------------------------------------------------------------

/*! @brief VTG (Course Over Ground and Ground Speed) sentence fields extraction structure
 * Format: $--VTG,<CourseTrue:t.t[t][t][t]>[,T],<CourseMag:m.m[m][m][m]>[,M],<SpeedKnots:k.k[k][k][k]>[,N],<SpeedKmHr:h.h[h][h][h]>[,K][,<FAA:A/D/E/M/S/N>]*<CheckSum>
 * The actual course and speed relative to the ground
 */
typedef struct NMEA0183_VTGdata
{
  uint32_t CourseTrue;     //!< Course over ground in degrees True (000.0000 to 359.0000) extracted (divide by 10^4 to get the real course)
  uint32_t CourseMagnetic; //!< Course over ground in degrees Magnetic (000.0000 to 359.0000) extracted (divide by 10^4 to get the real course)
  uint32_t SpeedKnots;     //!< Speed over the ground in knots (000.0000 to 999.9999) extracted (divide by 10^4 to get the real speed)
  uint32_t SpeedKmHr;      //!< Speed over the ground in km/hr (000.0000 to 999.9999) extracted (divide by 10^4 to get the real speed)
  char FAAmode;            //!< FAA mode indicator (NMEA 2.3 and later): ' ' = Not specified in the frame ; 'A' = Autonomous mode ; 'D' = Differential Mode ; 'E' = Estimated (dead-reckoning) mode ; 'M' = Manual Input Mode ; 'S' = Simulated Mode ; 'N' = Data Not Valid
} NMEA0183_VTGdata;

//-----------------------------------------------------------------------------

/*! @brief ZDA (Time & Date) sentence fields extraction structure
 * Format: $--ZDA,<hhmmss.zzz>,<Day:dd>,<Month:mm>,<Year:yyyy>,<LocalHour:(-)hh>,<LocalMinute:mm>*<CheckSum>
 * UTC, day, month, year and local time zone
 */
typedef struct NMEA0183_ZDAdata
{
  NMEA0183_Time Time;      //!< Time extracted
  NMEA0183_Date Date;      //!< Date extracted
  int8_t LocalZoneHour;    //!< Local zone hours (00 to +/-13)
  uint8_t LocalZoneMinute; //!< Local Zone Minutes (00 to 59)
} NMEA0183_ZDAdata;

//-----------------------------------------------------------------------------





//*****************************************************************************

//! NMEA0183 decoded values buffer
typedef struct NMEA0183_DecodedData
{
  eNMEA0183_TalkerID TalkerID;      //!< This is the talker ID of the last decoded frame
  eNMEA0183_SentencesID SentenceID; //!< This is the sentence ID of the last decoded frame
  bool ParseIsValid;                //!< 'true' to indicate that the parsing of the frame is valid else 'false'
  union
  {
#ifdef NMEA0183_DECODE_AAM
    NMEA0183_AAMdata AAM;                   //!< AAM (Waypoint Arrival Alarm) extracted. Use if 'SentenceID' = NMEA0183_AAM
#endif
#ifdef NMEA0183_DECODE_ALM
    NMEA0183_ALMdata ALM;                   //!< ALM (GPS Almanac Data) extracted. Use if 'SentenceID' = NMEA0183_ALM
#endif
#ifdef NMEA0183_DECODE_APB
    NMEA0183_APBdata APB;                   //!< APB (Heading/Track Controller (Autopilot) Sentence "B") extracted. Use if 'SentenceID' = NMEA0183_APB
#endif
#ifdef NMEA0183_DECODE_BEC
    NMEA0183_BECdata BEC;                   //!< BEC (Bearing and distance to waypoint - dead reckoning) extracted. Use if 'SentenceID' = NMEA0183_BEC
#endif
#ifdef NMEA0183_DECODE_BOD
    NMEA0183_BODdata BOD;                   //!< BOD (Bearing - Origin to Destination) extracted. Use if 'SentenceID' = NMEA0183_BOD
#endif
#ifdef NMEA0183_DECODE_BWW
    NMEA0183_BWWdata BWW;                   //!< BWW (Bearing - Waypoint to Waypoint) extracted. Use if 'SentenceID' = NMEA0183_BWW
#endif
#ifdef NMEA0183_DECODE_GGA
    NMEA0183_GGAdata GGA;                   //!< GGA (Global positioning system fixed data) extracted. Use if 'SentenceID' = NMEA0183_GGA
#endif
#ifdef NMEA0183_DECODE_GLL
    NMEA0183_GLLdata GLL;                   //!< GLL (Geographic Position - Latitude/Longitude) extracted. Use if 'SentenceID' = NMEA0183_GLL
#endif
#ifdef NMEA0183_DECODE_GSA
    NMEA0183_GSAdata GSA;                   //!< GSA (GNSS DOP and Active Satellites) extracted. Use if 'SentenceID' = NMEA0183_GSA
#endif
#ifdef NMEA0183_DECODE_GSV
    NMEA0183_GSVdata GSV;                   //!< GSV (GNSS Satellites in View) extracted. Use if 'SentenceID' = NMEA0183_GSV
#endif
#ifdef NMEA0183_DECODE_HDG
    NMEA0183_HDGdata HDG;                   //!< HDG (Heading - Deviation and Variation) extracted. Use if 'SentenceID' = NMEA0183_HDG
#endif
#ifdef NMEA0183_DECODE_HDM
    NMEA0183_HDMdata HDM;                   //!< HDM (Heading - Magnetic) extracted. Use if 'SentenceID' = NMEA0183_HDM
#endif
#ifdef NMEA0183_DECODE_HDT
    NMEA0183_HDTdata HDT;                   //!< HDT (Heading - True) extracted. Use if 'SentenceID' = NMEA0183_HDT
#endif
#ifdef NMEA0183_DECODE_MTW
    NMEA0183_MTWdata MTW;                   //!< MTW (Water Temperature) extracted. Use if 'SentenceID' = NMEA0183_MTW
#endif
#ifdef NMEA0183_DECODE_MWV
    NMEA0183_MWVdata MWV;                   //!< MWV (Wind Speed and Angle) extracted. Use if 'SentenceID' = NMEA0183_MWV
#endif
#ifdef NMEA0183_DECODE_RMC
    NMEA0183_RMCdata RMC;                   //!< RMC (Recommended Minimum sentence C) extracted. Use if 'SentenceID' = NMEA0183_RMC
#endif
#ifdef NMEA0183_DECODE_TXT
    NMEA0183_TXTdata TXT;                   //!< TXT (Text Transmission) extracted. Use if 'SentenceID' = NMEA0183_TXT
#endif
#ifdef NMEA0183_DECODE_VHW
    NMEA0183_VHWdata VHW;                   //!< VHW (Water Speed and Heading) extracted. Use if 'SentenceID' = NMEA0183_VHW
#endif
#ifdef NMEA0183_DECODE_VTG
    NMEA0183_VTGdata VTG;                   //!< VTG (Course Over Ground and Ground Speed) extracted. Use if 'SentenceID' = NMEA0183_VTG
#endif
#ifdef NMEA0183_DECODE_ZDA
    NMEA0183_ZDAdata ZDA;                   //!< ZDA (Time & Date) extracted. Use if 'SentenceID' = NMEA0183_ZDA
#endif
    char Frame[NMEA0183_FRAME_BUFFER_SIZE]; //!< Raw of the frame. This is the default result of an unknown sentence
  };
} NMEA0183_DecodedData;

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// NMEA0183 decoder API
//********************************************************************************************************************

//! Talker ID enumerator
typedef enum
{
  NMEA0183_WAIT_START, //!< Wait for a '$' character
  NMEA0183_ACCUMULATE, //!< The buffer is accumulating frame characters
  NMEA0183_TO_PROCESS, //!< The frame needs to be processed
  NMEA0183_IN_PROCESS, //!< The frame is currently processed
} eNMEA0183_State;

//-----------------------------------------------------------------------------


typedef struct NMEA0183_InputBuffer NMEA0183_DecodeInput; //! Type definition of the NMEA0183 decoder

//! NMEA0183 Input buffer
struct NMEA0183_InputBuffer
{
  //--- Frame buffer ---
  size_t BufferPos;                          //!< Position in the buffer
  char RawFrame[NMEA0183_FRAME_BUFFER_SIZE]; //!< Raw buffer with the frame to be processed
  eNMEA0183_State State;                     //!< Indicate the state of the frame

  //--- CRC ---
  uint8_t CurrCalcCRC;                       //!< CRC currently calculated
  char CRC[2];                               //!< CRC from the frame
  size_t PosCRC;                             //!< If <2 then the frame character is for CRC else for frame buffer
};

//-----------------------------------------------------------------------------



/*! @brief Initialize GPS pins and port
 *
 * @param[in] *pDecoder Is the decode input to initialize
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT Init_NMEA0183(NMEA0183_DecodeInput* pDecoder);

//-----------------------------------------------------------------------------


/*! @brief Add NMEA0183 received frame character data
 *
 * @param[in] *pDecoder Is the decode input to use
 * @param[in] data Is the char to add to input data buffer
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_AddReceivedCharacter(NMEA0183_DecodeInput* pDecoder, char data);

/*! @brief Get decoder state
 *
 * @param[in] *pDecoder Is the decode input to use
 * @return Returns an #eNMEA0183_State value enum
 */
eNMEA0183_State NMEA0183_GetDecoderState(NMEA0183_DecodeInput* pDecoder);

/*! @brief Is a frame ready to process?
 *
 * @param[in] *pDecoder Is the decode input to use
 * @return Returns 'true' if a frame is ready to process else 'false'
 */
bool NMEA0183_IsFrameReadyToProcess(NMEA0183_DecodeInput* pDecoder);

/*! @brief Process the NMEA0183 frame
 *
 * @param[in] *pDecoder Is the decode input to use
 * @param[out] *pData Is the decoded data
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_ProcessFrame(NMEA0183_DecodeInput* pDecoder, NMEA0183_DecodedData* pData);

//********************************************************************************************************************





//********************************************************************************************************************
// NMEA0183 tools API
//********************************************************************************************************************
#ifdef NMEA0183_FLOAT_BASED_TOOLS

//! Latitude/Longitude in degree + minute coordinate structure
typedef struct NMEA0183_DegreeMinute
{
    int16_t Degree; //!< Degree (Latitude: -90 (S) to 90 (N), Longitude: -180 (W) to 180 (E))
    double Minute;  //!< Minute with decimal
} NMEA0183_DegreeMinute;

typedef NMEA0183_DegreeMinute NMEA0183_DMm; //! Alias definition

//-----------------------------------------------------------------------------

//! Latitude/Longitude in degree + minute + seconds coordinate structure
typedef struct NMEA0183_DegreeMinuteSeconds
{
    int16_t Degree; //!< Degree (Latitude: -90 (S) to 90 (N), Longitude: -180 (W) to 180 (E))
    uint8_t Minute; //!< Minute
    double Seconds; //!< Seconds with decimal
} NMEA0183_DegreeMinuteSeconds;

typedef NMEA0183_DegreeMinuteSeconds NMEA0183_DMS; //! Alias definition

//********************************************************************************************************************



/*! @brief Convert coordinate to degree (D.d)
 *
 * @param[in] *pCoordinate Is the coordinate to convert
 * @param[out] *pDegree Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_CoordinateToDegree(NMEA0183_Coordinate* pCoordinate, double* pDegree);

/*! @brief Convert coordinate to degree minute (D M.m)
 *
 * @param[in] *pCoordinate Is the coordinate to convert
 * @param[out] *pDegMinute Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_CoordinateToDegreeMinute(NMEA0183_Coordinate* pCoordinate, NMEA0183_DegreeMinute* pDegMinute);

/*! @brief Convert coordinate to degree minute seconds (D M S.s)
 *
 * @param[in] *pCoordinate Is the coordinate to convert
 * @param[out] *pDegMinSec Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_CoordinateToDegreeMinuteSeconds(NMEA0183_Coordinate* pCoordinate, NMEA0183_DegreeMinuteSeconds* pDegMinSec);

//-----------------------------------------------------------------------------


/*! @brief Convert degree (D.d) to degree minute (D M.m)
 *
 * @param[in] degree Is the degree to convert
 * @param[out] *pDegMinute Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_DegreeToDegreeMinute(double degree, NMEA0183_DegreeMinute* pDegMinute);

/*! @brief Convert degree (D.d) to degree minute seconds (D M S.s)
 *
 * @param[in] degree Is the degree to convert
 * @param[out] *pDegMinSec Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_DegreeToDegreeMinuteSeconds(double degree, NMEA0183_DegreeMinuteSeconds* pDegMinSec);

/*! @brief Convert degree minute (D M.m) to degree (D.d)
 *
 * @param[in] *pDegMinute Is the degree minute to convert
 * @param[out] *pDegree Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_DegreeMinuteToDegree(NMEA0183_DegreeMinute* pDegMinute, double* pDegree);

/*! @brief Convert degree minute (D M.m) to degree minute seconds (D M S.s)
 *
 * @param[in] *pDegMinute Is the degree minute to convert
 * @param[out] *pDegMinSec Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_DegreeMinuteToDegreeMinuteSeconds(NMEA0183_DegreeMinute* pDegMinute, NMEA0183_DegreeMinuteSeconds* pDegMinSec);

/*! @brief Convert degree minute seconds (D M S.s) to degree (D.d)
 *
 * @param[in] *pDegMinSec Is the degree minute seconds to convert
 * @param[out] *pDegree Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_DegreeMinuteSecondsToDegree(NMEA0183_DegreeMinuteSeconds* pDegMinSec, double* pDegree);

/*! @brief Convert degree minute seconds (D M S.s) to degree minute (D M.m)
 *
 * @param[in] *pDegMinSec Is the degree minute seconds to convert
 * @param[out] *pDegMinute Is where the coordinate converted will be stored
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT NMEA0183_DegreeMinuteSecondsToDegreeMinute(NMEA0183_DegreeMinuteSeconds* pDegMinSec, NMEA0183_DegreeMinute* pDegMinute);
#endif
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif /* NMEA0183_LIB_H_ */