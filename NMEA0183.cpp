/*******************************************************************************
 * @file    NMEA0183.c
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.0.0
 * @date    13/02/2022
 * @brief   NMEA decoder library
 *
 * Supports common GPS frames
 ******************************************************************************/

//-----------------------------------------------------------------------------
#if !defined(__cplusplus)
#  include "NMEA0183.h"
#else
#  if !defined(ARDUINO)
#    include <cstdint>
#  endif
#  include "NMEA0183.hpp"
extern "C" {
#endif
//-----------------------------------------------------------------------------





//=============================================================================
// Prototypes for private functions
//=============================================================================
/*! @brief Convert a string to int float
 * This function will stop parsing at first char not in '0'..'9', '.'
 * @param[in/out] **pStr Is the string to parse (the original pointer will be advanced) and returns the new position in the string, the field separator ',' or the end of the string
 * @param[in] max Is the max count of digit to extract. Set to 0 if no limit wanted. If >20 then it is the stop character
 * @param[in] digits Is the digit count to extract from the string after the decimal separator '.'
 * @return Returns the value extracted from string
 */
static int32_t __NMEA0183_StringToInt(char** pStr, size_t max, size_t digits);
//-----------------------------------------------------------------------------
/*! @brief Hex string to value
 * This function will stop parsing at first char not in '0'..'9', 'a'..'f', 'A'..'F'
 * @param[in] *pStr Is the string to parse
 * @param[in] charCount Is the Hex char count to extract
 * @return Returns the value extracted
 */
static uint32_t __NMEA0183_HexStringToUint(const char* pStr, size_t charCount);
//-----------------------------------------------------------------------------
/*! @brief Extract coordinate from string
 * This function will stop parsing at first parsing error
 * @param[in] **pStr Is the string to parse (the original pointer will be advanced)
 * @param[out] *pData Is the coordinate extracted
 * @return Returns 'true' if the parse is successful else 'false'
 */
static bool __NMEA0183_ExtractCoordinate(char** pStr, NMEA0183_Coordinate* pData);
/*! @brief Extract time from string
 * This function will stop parsing at first parsing error
 * @param[in] **pStr Is the string to parse (the original pointer will be advanced)
 * @param[out] *pData Is the time extracted
 * @return Returns 'true' if the parse is successful else 'false'
 */
static bool __NMEA0183_ExtractTime(char** pStr, NMEA0183_Time* pData);
//-----------------------------------------------------------------------------
#define NMEA0183_CHECK_FIELD_DELIMITER  do{ if (*pStr != NMEA0183_FIELD_DELIMITER) return ERR__PARSE_ERROR; ++pStr; } while(0) // Should be a ',' and go to the next character of the string
//-----------------------------------------------------------------------------





//********************************************************************************************************************
// NMEA0183 decoder API
//********************************************************************************************************************
//=============================================================================
// Initialize NMEA0183
//=============================================================================
eERRORRESULT Init_NMEA0183(NMEA0183_DecodeInput* pDecoder)
{
#ifdef CHECK_NULL_PARAM
  if (pDecoder == NULL) return ERR__PARAMETER_ERROR;
#endif
  //--- Initialize vars ---
  memset(pDecoder, 0, sizeof(NMEA0183_DecodeInput)); // Init GPS input structure
  pDecoder->PosCRC = sizeof(pDecoder->CRC);          // Set that this is not the CRC for now
  return ERR_OK;
}

//-----------------------------------------------------------------------------





//**********************************************************************************************************************************************************
//=============================================================================
// Add NMEA0183 received frame character data
//=============================================================================
eERRORRESULT NMEA0183_AddReceivedCharacter(NMEA0183_DecodeInput* pDecoder, char data)
{
#ifdef CHECK_NULL_PARAM
  if (pDecoder == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error = ERR_OK;

  //--- Checks ---
  if (pDecoder->State == NMEA0183_IN_PROCESS) return ERR__BUSY; // The previous frame is being parced

  //--- Process character ---
  switch (data) // Do something with the current character received
  {
    case NMEA0183_START_DELIMITER: // Start a new frame
      pDecoder->BufferPos = 0;                            // Initialize buffer position
      pDecoder->PosCRC    = sizeof(pDecoder->CRC);        // Initialize to current char is for frame input
      pDecoder->CurrCalcCRC = 0;                          // Initialize the current CRC calculus
      if (pDecoder->State == NMEA0183_TO_PROCESS) Error = ERR__BUFFER_OVERRIDE; // The previous buffer does not have been processed but new data available
      pDecoder->State = NMEA0183_ACCUMULATE;
      break;

    case NMEA0183_CHECKSUM_DELIMITER: // Next characters will be for CRC
      pDecoder->PosCRC = 0;                               // Next will be the CRC value
      break;

    case NMEA0183_END_CR_DELIMITER:
    case NMEA0183_END_LF_DELIMITER:
      if (pDecoder->State != NMEA0183_WAIT_START)
        pDecoder->State = NMEA0183_TO_PROCESS;            // Frame have to be processed as soon as possible (in the main loop). Not now, in case we are in an interrupt...
      data = '\0';                                        // The data received is now a end of string character
      break;

    default:
      if (pDecoder->PosCRC < sizeof(pDecoder->CRC))       // In CRC?
      {
        pDecoder->CRC[pDecoder->PosCRC] = (char)data;     // Set CRC char
        ++pDecoder->PosCRC;                               // Select next char
      }
      else pDecoder->CurrCalcCRC ^= data;                 // Else compute CRC
      break;
  }

  //--- Add char to raw frame buffer ---
  if (pDecoder->State != NMEA0183_TO_PROCESS)
  {
    if (pDecoder->BufferPos < NMEA0183_FRAME_BUFFER_SIZE)
    {
      pDecoder->RawFrame[pDecoder->BufferPos] = (char)data; // Set Frame char
      pDecoder->BufferPos++;                                // Select next char
    }
    else if (pDecoder->State != NMEA0183_WAIT_START) Error = ERR__BUFFER_FULL;
  }
  return Error;
}
//-----------------------------------------------------------------------------





//**********************************************************************************************************************************************************
//=============================================================================
// [STATIC] Convert a string to int
//=============================================================================
int32_t __NMEA0183_StringToInt(char** pStr, size_t max, size_t digits)
{
  if (**pStr == '\0') return NMEA0183_NO_VALUE;    // Empty string? return error value
  bool Sign = (**pStr == '-');                     // Minus character? Save it
  if (Sign || (**pStr == '+')) ++(*pStr);          // Minus character or Plus character? Go to next one
  if (**pStr == '\0') return NMEA0183_NO_VALUE;    // Empty string? return error value
  if ((**pStr == NMEA0183_FIELD_DELIMITER) || (**pStr == NMEA0183_CHECKSUM_DELIMITER)) return NMEA0183_NO_VALUE; // If the field is empty, set no data
  size_t CharCount = (max == 0 ? NMEA0183_FRAME_BUFFER_SIZE : max); // If max = 0, then extract until ',', '*', or '\0'
  int32_t Result = 0;

  //--- Extract value ---
  bool PointFound = false;
  while (CharCount > 0)
  {
    if (**pStr == max) break;                      // Stop char found
    if (**pStr != '.')
    {
      if ((uint_fast8_t)(**pStr - '0') > 9) break; // If pStr[0] = '\0' or other char, the result should be > 9 then break the while...
      if (PointFound)                              // Decrement digit only if the char '.' have been found
      {
        if (digits == 0) break;
        --digits;
      }
      Result *= 10;                                // Multiply the int part by 10
      Result += (int32_t)(**pStr - '0');           // Add the unit value
    }
    else PointFound = true;
    --CharCount;
    ++(*pStr);                                     // Next char
  }
  while (digits > 0) { Result *= 10; --digits; }   // Force multiplier to digits
  if (max == 0)
    while ((**pStr != NMEA0183_FIELD_DELIMITER) && (**pStr != NMEA0183_CHECKSUM_DELIMITER) && (**pStr != 0)) ++(*pStr); // Go to the end of the field or the end of the string
  if (Sign) Result = -Result;                      // If negative value, then set negative IntPart as result
  return Result;                                   // Return the new position
}

//-----------------------------------------------------------------------------



//=============================================================================
// [STATIC] Hex string to value
//=============================================================================
uint32_t __NMEA0183_HexStringToUint(const char* pStr, size_t charCount)
{
  char* pChar = (char*)pStr;
  uint32_t Result = 0;

  //--- Extract uint32 from hex string ---
  while ((charCount > 0) && (*pChar != 0))
  {
    uint32_t CurChar = (uint32_t)(*pChar);
    if ((CurChar - 0x30) <= 9u)           // Char in '0'..'9' ?
    {
      Result <<= 4;
      Result += (CurChar - 0x30);         // 0x30 for '0'
    }
    else
    {
      CurChar &= 0xDF;                    // Transform 'a'..'f' into 'A'..'F'
      if ((CurChar - 0x41) <= 5u)         // Char in 'A'..'F' ?
      {
        Result <<= 4;
        Result += (CurChar - 0x41) + 10u; // 0x41 for 'A' and add 10 to the value
      }
      else break;
    }
    ++pChar;
    --charCount;
  }
  return Result;
}

//-----------------------------------------------------------------------------





//**********************************************************************************************************************************************************
//=============================================================================
// [STATIC] Extract coordinate
//=============================================================================
bool __NMEA0183_ExtractCoordinate(char** pStr, NMEA0183_Coordinate* pData)
{
  if (**pStr != NMEA0183_FIELD_DELIMITER)                         // Value found?
  {
    int32_t Value = __NMEA0183_StringToInt(pStr, '.', 0);         //*** Get degree and minute <(d)ddmm>
    //--- Get Degree ---
    pData->Degree = (uint8_t)(Value / 100);                       //    And save degree
    //--- Get Minute ---
    pData->Minute = (uint32_t)__NMEA0183_StringToInt(pStr, 0, 7); //*** Get and save decimal minute <.mmmm[m][m][m]> (divide by 10^7 to get the real minute)*/
    pData->Minute += ((Value % 100) * 10000000);                  //    And save minute with decimal minute
    if (**pStr != NMEA0183_FIELD_DELIMITER) return false;         // Parsing: Should be a ','
    ++(*pStr);                                                    // Parsing: Skip ','
  }
  else                                                            // else no value found
  {
    pData->Degree = (uint8_t)NMEA0183_NO_VALUE;
    pData->Minute = (uint32_t)NMEA0183_NO_VALUE;
    ++(*pStr);                                                    // Parsing: Skip ','
  }
  //--- Get Direction ---
  if (**pStr != NMEA0183_FIELD_DELIMITER)                         // Value found?
  {
    pData->Direction = **pStr;                                    //*** Get coordinate direction <N/S or E/W>
    ++(*pStr);                                                    // Parsing: Skip <N/S or E/W>
  }
  else pData->Direction = ' ';                                    // Else no direction
  if (**pStr != NMEA0183_FIELD_DELIMITER) return false;           // Parsing: Should be a ','
  ++(*pStr);                                                      // Parsing: Skip ','
  return true;
}


//=============================================================================
// [STATIC] Extract time
//=============================================================================
bool __NMEA0183_ExtractTime(char** pStr, NMEA0183_Time* pData)
{
  //--- Get Time ---
  pData->Hour   = (uint8_t)__NMEA0183_StringToInt(pStr, 2, 0);    //*** Get and save hour <hh>
  pData->Minute = (uint8_t)__NMEA0183_StringToInt(pStr, 2, 0);    //*** Get and save minute <mm>
  pData->Second = (uint8_t)__NMEA0183_StringToInt(pStr, 2, 0);    //*** Get and save second <ss>
  //--- Get milliseconds ---
  if (**pStr == '.')                                              // If the next char is '.' then there is a millisecond value to get
  {
    pData->MilliS = (uint16_t)__NMEA0183_StringToInt(pStr, 0, 3); //*** Get and save milliseconds <zzz>
  } else pData->MilliS = (uint16_t)NMEA0183_NO_VALUE;             // else set no value
  if (**pStr != NMEA0183_FIELD_DELIMITER) return false;           // Parsing: Should be a ','
  ++(*pStr);                                                      // Parsing: Skip ','
  return true;
}

//-----------------------------------------------------------------------------





//**********************************************************************************************************************************************************
#ifdef NMEA0183_DECODE_AAM
//=============================================================================
// Process the AAM (Waypoint Arrival Alarm) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessAAM(const char* pSentence, NMEA0183_AAMdata* pData)
{ // Format: $--AAM,<Entered:A/V>,<Waypoint:A/V>,<Circle:r.rr[r][r]>,N,<WaypointID>*<CheckSum>
  char* pStr = (char*)pSentence;
  eERRORRESULT Error = ERR_OK;

  //--- Get Status ---
  pData->ArrivalStatus = *pStr;                                        //*** Get status of arrival: 'A' = arrival circle entered ; 'V' = arrival circle not entered
  ++pStr;                                                              // Parsing: Skip <A/V>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->PassedWaypoint = *pStr;                                       //*** Get status of the passed waypoint: 'A' = arrival circle entered ; 'V' = arrival circle not entered
  ++pStr;                                                              // Parsing: Skip <A/V>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Arrival circle radius ---
  pData->CircleRadius = (uint32_t)__NMEA0183_StringToInt(&pStr, 0, 4); //*** Get and save Arrival circle radius <Circle:r.rr[r][r]> (divide by 10^4 to get the circle radius in nautical miles)
  NMEA0183_CHECK_FIELD_DELIMITER;
  if (*pStr != 'N') return ERR__PARSE_ERROR;                           // Parsing: Should be 'N'
  ++pStr;                                                              // Parsing: Skip 'N'
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get text of the message ---
  size_t TxtPos = 0;
  while (TxtPos < (NMEA0183_AAM_WAYPOINT_ID_MAX_SIZE-1))
  {
    if ((*pStr == '\0') || (*pStr == NMEA0183_CHECKSUM_DELIMITER)) break;
    pData->WaypointID[TxtPos] = *pStr;                                 //*** Get char
    ++TxtPos;
    ++pStr;
  }
  pData->WaypointID[TxtPos] = '\0';
  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) Error = ERR__PARSE_ERROR;  // Should be a '*'
  return Error;
}
#endif



#ifdef NMEA0183_DECODE_GGA
//=============================================================================
// Process the GGA (Global positioning system fixed data) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessGGA(const char* pSentence, NMEA0183_GGAdata* pData)
{ // Format: $--GGA,<hhmmss.zzz>,<Latitude:ddmm.mmmm[m][m][m]>,<N/S>,<Longitude:dddmm.mmmm[m][m][m]>,<E/W>,<GPSquality:0/1/2/3/4/5/6/7/8>,<SatUsed:ss>,<HDOP:h.h(h)>,<Altitude:(-)aaa.a[a]>,M,<GeoidSep:(-)gg.g[g]>,M,<AgeDiff:cc.c[c]>,<DiffRef:rrrr>*<CheckSum>
  char* pStr = (char*)pSentence;

  //--- Get Time ---
  if (__NMEA0183_ExtractTime(&pStr, &pData->Time) == false) return ERR__PARSE_ERROR; //*** Get time

  //--- Get Latitude and Longitude ---
  if (__NMEA0183_ExtractCoordinate(&pStr, &pData->Latitude ) == false) return ERR__PARSE_ERROR; //*** Get latitude
  if (__NMEA0183_ExtractCoordinate(&pStr, &pData->Longitude) == false) return ERR__PARSE_ERROR; //*** Get longitude

  //--- Get GPS Quality Indicator ---
  pData->GPSquality = *pStr;                                            //*** Get GPS quality indicator <0/1/2/3/4/5/6/7/8>
  ++pStr;                                                               // Parsing: Skip <0/1/2/3/4/5/6/7/8>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Satellite Used ---
  pData->SatellitesUsed = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save satellite used <ss>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get HDOP ---
  pData->HDOP = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 2);          //*** Get and save HDOP <h.h(h)> (divide by 100 to get the real HDOP)
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Altitude ---
  pData->Altitude = __NMEA0183_StringToInt(&pStr, 0, 2);                //*** Get and save altitude <(-)aaa.a[a]> (divide by 10^2 to get the real altitude)
  NMEA0183_CHECK_FIELD_DELIMITER;
  if (*pStr != 'M') return ERR__PARSE_ERROR;                            // Parsing: Should be 'M'
  ++pStr;                                                               // Parsing: Skip 'M'
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Geoid Separation ---
  pData->GeoidSeparation = __NMEA0183_StringToInt(&pStr, 0, 2);         //*** Get and save geoid separation <(-)gg.g[g]> (divide by 10^2 to get the real geoid separation)
  NMEA0183_CHECK_FIELD_DELIMITER;
  if (*pStr != 'M') return ERR__PARSE_ERROR;                            // Parsing: Should be 'M'
  ++pStr;                                                               // Parsing: Skip 'M'

  if (*pStr == NMEA0183_FIELD_DELIMITER)
  {
      //--- Get Age of differential GPS data (if available) ---
      ++pStr;                                                           // Parsing: Skip ','
      pData->AgeOfDiffCorr = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 2); //*** Get age of differential GPS data <cc.c[c]> (divide by 10^2 to get the real age of differential GPS data)

      if (*pStr == NMEA0183_FIELD_DELIMITER)
      {
          //--- Get Differential reference station ID (if available) ---
          ++pStr;                                                           // Parsing: Skip ','
          pData->DiffRefStationID = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save differential reference station ID <rrrr>
      }
      else pData->DiffRefStationID = (uint16_t)NMEA0183_NO_VALUE;           //*** Set age of differential GPS data not specified
  }
  else pData->AgeOfDiffCorr = (uint16_t)NMEA0183_NO_VALUE;                  //*** Set age of differential GPS data not specified

  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) return ERR__PARSE_ERROR;        // Should be a '*'
  return ERR_OK;
}
#endif



#ifdef NMEA0183_DECODE_GLL
//=============================================================================
// Process the GLL (Geographic Position - Latitude/Longitude) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessGLL(const char* pSentence, NMEA0183_GLLdata* pData)
{ // Format: $--GLL,<Latitude:ddmm.mmmm[m][m][m]>,<N/S>,<Longitude:dddmm.mmmm[m][m][m]>,<E/W>,<hhmmss.zzz>,<Status:A/V>,<FAA:A/D/E/M/S/N>*<CheckSum>
  char* pStr = (char*)pSentence;

  //--- Get Latitude and Longitude ---
  if (__NMEA0183_ExtractCoordinate(&pStr, &pData->Latitude ) == false) return ERR__PARSE_ERROR; //*** Get latitude
  if (__NMEA0183_ExtractCoordinate(&pStr, &pData->Longitude) == false) return ERR__PARSE_ERROR; //*** Get longitude

  //--- Get Time ---
  if (__NMEA0183_ExtractTime(&pStr, &pData->Time) == false) return ERR__PARSE_ERROR; //*** Get time

  //--- Get Status ---
  pData->Status = *pStr;                                             //*** Get status: A=Active=Good ; V=Void=NotGood
  ++pStr;                                                            // Parsing: Skip <A/V>

  if (*pStr == NMEA0183_FIELD_DELIMITER)
  {
    //--- Get FAA mode (if available) ---
    ++pStr;                                                          // Parsing: Skip ','
    pData->FAAmode = *pStr;                                          //*** Get FAA mode <A/D/E/M/S/N>
    ++pStr;                                                          // Parsing: Skip <A/D/E/M/S/N>
  }
  else pData->FAAmode = ' ';                                         //*** Set FAA mode not specified
  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) return ERR__PARSE_ERROR; // Should be a '*'
  return ERR_OK;
}
#endif



#ifdef NMEA0183_DECODE_GSA
//=============================================================================
// Process the GSA (GNSS DOP and Active Satellites) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessGSA(const char* pSentence, NMEA0183_GSAdata* pData)
{ // Format: $--GSA,<Mode1:A/M>,<Mode2:1/2/3>,[<Sat1:xx>],[<Sat2:xx>],[<Sat3:xx>],[<Sat4:xx>],[<Sat5:xx>],[<Sat6:xx>],[<Sat7:xx>],[<Sat8:xx>],[<Sat9:xx>],[<Sat10:xx>],[<Sat11:xx>],[<Sat12:xx>],<PDOP:p.p>,<HDOP:h.h>,<VDOP:v.v>*<CheckSum>
  char* pStr = (char*)pSentence;

  //--- Get Mode 1 ---
  pData->Mode1 = *pStr;                                              //*** Get mode <A/M>
  ++pStr;                                                            // Parsing: Skip <A/M>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Mode 2 ---
  pData->Mode2 = *pStr;                                              //*** Get mode <1/2/3>
  ++pStr;                                                            // Parsing: Skip <1/2/3>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Satellite IDs ---
  for (size_t zSat = 0; zSat < NMEA0183_SATELLITE_ID_COUNT; ++zSat)
  {
    //--- Get Satellite ID ---
    pData->SatelliteIDs[zSat] = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save satellite ID <xx>
    NMEA0183_CHECK_FIELD_DELIMITER;
  }

  //--- Get DOPs ---
  pData->PDOP = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 2);       //*** Get PDOP <p.p> (divide by 100 to get the real PDOP)
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->HDOP = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 2);       //*** Get HDOP <h.h> (divide by 100 to get the real HDOP)
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->VDOP = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 2);       //*** Get VDOP <v.v> (divide by 100 to get the real VDOP)

  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) return ERR__PARSE_ERROR; // Should be a '*'
  return ERR_OK;
}
#endif



#ifdef NMEA0183_DECODE_GSV
//=============================================================================
// Process the GSV (GNSS Satellites in View) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessGSV(const char* pSentence, NMEA0183_GSVdata* pData)
{ // Format: $--GSV,<Total:t>,<Curr:c>,<SatCount:ss>,<SV1:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>[,<SV2:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>][,<SV3:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>][,<SV4:<SatNum:nn>,<Elev:ee>,<Azim:aaa>,<SNR:rr>>],<Text>*<CheckSum>
  char* pStr = (char*)pSentence;

  //--- Get message informations ---
  pData->TotalSentence  = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save total sentence <t>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->SentenceNumber = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save sequence number <c>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->TotalSatellite = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save total satellite <ss>
  NMEA0183_CHECK_FIELD_DELIMITER;

  size_t zSat = 0;
  while (zSat < NMEA0183_SAT_VIEW_COUNT_PER_MESSAGES)
  {
    //--- Get Satellite informations ---
    pData->SatView[zSat].SatelliteID =  (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save satellite ID <nn>
    NMEA0183_CHECK_FIELD_DELIMITER;
    pData->SatView[zSat].Elevation   =  (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save elevation <ee>
    NMEA0183_CHECK_FIELD_DELIMITER;
    pData->SatView[zSat].Azimuth     = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save azimuth <aaa>
    NMEA0183_CHECK_FIELD_DELIMITER;
    pData->SatView[zSat].SNR         =  (uint8_t)__NMEA0183_StringToInt(&pStr, 2, 0); //*** Get and save SNR <rr>
    ++zSat;
    if (*pStr == NMEA0183_CHECKSUM_DELIMITER) break;
    NMEA0183_CHECK_FIELD_DELIMITER;
  }
  for (size_t z = zSat; z < NMEA0183_SAT_VIEW_COUNT_PER_MESSAGES; ++z)
    memset(&pData->SatView[z], 0xFF, sizeof(NMEA0183_SatelliteView)); // Clear data of satellite in view not setted

  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) return ERR__PARSE_ERROR;  // Should be a '*'
  return ERR_OK;
}
#endif



#ifdef NMEA0183_DECODE_RMC
//=============================================================================
// Process the RMC (Recommended Minimum sentence C) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessRMC(const char* pSentence, NMEA0183_RMCdata* pData)
{ // Format: $--RMC,<hhmmss.zzz>,<Status:A/V>,<Latitude:ddmm.mmmm[m][m][m]>,<N/S>,<Longitude:dddmm.mmmm[m][m][m]>,<E/W>,<Speed:sss.ss[s][s]>,<Track:ttt.tt[t][t]>,<ddmmyy>,<MagVar:vv.v[v]>,<E/W>[,<FAA:A/D/E/M/S/N>][,<NavStatus:S/C/U/V>]*<CheckSum>
  char* pStr = (char*)pSentence;

  //--- Get Time ---
  if (__NMEA0183_ExtractTime(&pStr, &pData->Time) == false) return ERR__PARSE_ERROR; //*** Get time

  //--- Get Status ---
  pData->Status = *pStr;                                             //*** Get status: A=Active=Good ; V=Void=NotGood
  ++pStr;                                                            // Parsing: Skip <A/V>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Latitude and Longitude ---
  if (__NMEA0183_ExtractCoordinate(&pStr, &pData->Latitude ) == false) return ERR__PARSE_ERROR; //*** Get latitude
  if (__NMEA0183_ExtractCoordinate(&pStr, &pData->Longitude) == false) return ERR__PARSE_ERROR; //*** Get longitude

  //--- Get Speed ---
  pData->Speed = (uint32_t)__NMEA0183_StringToInt(&pStr, 0, 4);      //*** Get speed <sss.ss[s][s]> (divide by 10^4 to get the real minute)
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Track ---
  pData->Track = (uint32_t)__NMEA0183_StringToInt(&pStr, 0, 4);      //*** Get track <ttt.tt[t][t]> (divide by 10^4 to get the real track)
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Date ---
  pData->Date.Day   =  (uint8_t)__NMEA0183_StringToInt(&pStr, 2, 0); //*** Get and save day <dd>
  pData->Date.Month =  (uint8_t)__NMEA0183_StringToInt(&pStr, 2, 0); //*** Get and save month <mm>
  pData->Date.Year  = (uint16_t)__NMEA0183_StringToInt(&pStr, 2, 0); //*** Get and save year <yy>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Magnetic Variation ---
  pData->Magnetic.Variation = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 2); //*** Get track <vv.v[v]> (divide by 10^2 to get the real magnetic variation)
  NMEA0183_CHECK_FIELD_DELIMITER;
  if (*pStr != NMEA0183_FIELD_DELIMITER)
  {
    pData->Magnetic.Direction = *pStr;                               //*** Get magnetic variation direction <E/W>
    ++pStr;                                                          // Parsing: Skip <E/W>
  } else pData->Magnetic.Direction = ' ';                            //*** Set magnetic variation direction not specified

  if (*pStr == NMEA0183_FIELD_DELIMITER)
  {
    //--- Get FAA mode (if available) ---
    ++pStr;                                                          // Parsing: Skip ','
    pData->FAAmode = *pStr;                                          //*** Get FAA mode <A/D/E/M/S/N>
    ++pStr;                                                          // Parsing: Skip <A/D/E/M/S/N>
    if (*pStr == NMEA0183_FIELD_DELIMITER)
    {
      //--- Get Navigation Status (if available) ---
      ++pStr;                                                        // Parsing: Skip ','
      pData->NavigationStatus = *pStr;                               //*** Get Navigation Status <S/C/U/V>
      ++pStr;                                                        // Parsing: Skip <S/C/U/V>
    } else pData->NavigationStatus = ' ';                            //*** Set Navigation Status not specified
  } else pData->FAAmode = ' ';                                       //*** Set FAA mode not specified
  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) return ERR__PARSE_ERROR; // Should be a '*'
  return ERR_OK;
}
#endif



#ifdef NMEA0183_DECODE_TXT
//=============================================================================
// Process the TXT (Text Transmission) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessTXT(const char* pSentence, NMEA0183_TXTdata* pData)
{ // Format: $--TXT,<Total:tt>,<Curr:cc>,<TextID:ii>,<Text>,*<CheckSum>
  char* pStr = (char*)pSentence;
  eERRORRESULT Error = ERR_OK;
  pData->TextMessage[0] = '\0';

  //--- Get message informations ---
  pData->TotalSentence  = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save total sentence <tt>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->SentenceNumber = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save sequence number <cc>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->TextIdentifier = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save text identifier <ii>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get text of the message ---
  size_t TxtPos = 0;
  while (TxtPos < NMEA0183_TXT_MESSAGE_MAX_SIZE)
  {
    if ((*pStr == '\0') || (*pStr == NMEA0183_CHECKSUM_DELIMITER)) break;
    if (*pStr != NMEA0183_CHAR_HEX_DELIMITER) pData->TextMessage[TxtPos] = *pStr; //*** Get char ('^' detected)
    else
    {
      ++pStr;
      if ((*pStr == '\0') || (*pStr == NMEA0183_CHECKSUM_DELIMITER))
      { Error = ERR__PARSE_ERROR; break; }
      pData->TextMessage[TxtPos] = (char)__NMEA0183_HexStringToUint(pStr, 2);     //*** Get hex encoded char (2 hex chars to decode)
      ++pStr;                                                                     // Parsing: Skip 2 hex char
      if ((*pStr == '\0') || (*pStr == NMEA0183_CHECKSUM_DELIMITER))
      { Error = ERR__PARSE_ERROR; break; }
    }
    ++TxtPos;
    ++pStr;
  }
  pData->TextMessage[TxtPos] = '\0';
  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) Error = ERR__PARSE_ERROR; // Should be a '*'
  return Error;
}
#endif



#ifdef NMEA0183_DECODE_VTG
//=============================================================================
// Process the VTG (Course Over Ground and Ground Speed) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessVTG(const char* pSentence, NMEA0183_VTGdata* pData)
{ // Format: $--VTG,<Course�True:t.t[t][t][t]>[,T],<Course�Mag:m.m[m][m][m]>[,M],<SpeedKnots:k.k[k][k][k]>[,N],<SpeedKmHr:h.h[h][h][h]>[,K][,<FAA:A/D/E/M/S/N>]*<CheckSum>
  char* pStr = (char*)pSentence;

  //--- Get Course ---
  pData->CourseTrue     = (uint32_t)__NMEA0183_StringToInt(&pStr, 0, 4); //*** Get course over ground in degrees True <ttt.t[t][t][t]> (divide by 10^4 to get the real track)
  NMEA0183_CHECK_FIELD_DELIMITER;
  if (*pStr == 'T') { ++pStr; NMEA0183_CHECK_FIELD_DELIMITER; }          // Parsing: Can be 'T' in new frame so, skip 'T'
  pData->CourseMagnetic = (uint32_t)__NMEA0183_StringToInt(&pStr, 0, 4); //*** Get course over ground in degrees Magnetic <mmm.m[m][m][m]> (divide by 10^4 to get the real track)
  NMEA0183_CHECK_FIELD_DELIMITER;
  if (*pStr == 'M') { ++pStr; NMEA0183_CHECK_FIELD_DELIMITER; }          // Parsing: Can be 'M' in new frame so, skip 'M'

  //--- Get Speed ---
  pData->SpeedKnots = (uint32_t)__NMEA0183_StringToInt(&pStr, 0, 4);     //*** Get speed over the ground in knots <kkk.k[k][k][k]> (divide by 10^4 to get the real track)
  NMEA0183_CHECK_FIELD_DELIMITER;
  if (*pStr == 'N') { ++pStr; NMEA0183_CHECK_FIELD_DELIMITER; }          // Parsing: Can be 'N' in new frame so, skip 'N'
  pData->SpeedKmHr = (uint32_t)__NMEA0183_StringToInt(&pStr, 0, 4);      //*** Get speed over the ground in km/hr <hhh.h[h][h][h]> (divide by 10^4 to get the real track)
  if (*pStr != NMEA0183_CHECKSUM_DELIMITER)
  {
    NMEA0183_CHECK_FIELD_DELIMITER;
    if (*pStr == 'K') { ++pStr; }                                        // Parsing: Can be 'K' in new frame so, skip 'K'

    if (*pStr != NMEA0183_CHECKSUM_DELIMITER)
    {
      NMEA0183_CHECK_FIELD_DELIMITER;
      //--- Get FAA mode ---
      pData->FAAmode = *pStr;                                            //*** Get FAA mode <A/D/E/M/S/N>
      ++pStr;                                                            // Parsing: Skip <A/D/E/M/S/N>
    }
  }

  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) return ERR__PARSE_ERROR;     // Should be a '*'
  return ERR_OK;
}
#endif



#ifdef NMEA0183_DECODE_ZDA
//=============================================================================
// Process the ZDA (Time & Date) sentence
//=============================================================================
static eERRORRESULT NMEA0183_ProcessZDA(const char* pSentence, NMEA0183_ZDAdata* pData)
{ // Format: $--ZDA,<hhmmss.zzz>,<Day:dd>,<Month:mm>,<Year:yyyy>,<LocalHour:(-)hh>,<LocalMinute:mm>*<CheckSum>
  char* pStr = (char*)pSentence;

  //--- Get Time ---
  if (__NMEA0183_ExtractTime(&pStr, &pData->Time) == false) return ERR__PARSE_ERROR; //*** Get time

  //--- Get Date ---
  pData->Date.Day   =  (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0);     //*** Get and save day <dd>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->Date.Month =  (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0);     //*** Get and save month <mm>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->Date.Year  = (uint16_t)__NMEA0183_StringToInt(&pStr, 0, 0);     //*** Get and save year <yyyy>
  NMEA0183_CHECK_FIELD_DELIMITER;

  //--- Get Local Zone Time ---
  pData->LocalZoneHour   =  (int8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save local zone hour <(-)hh>
  NMEA0183_CHECK_FIELD_DELIMITER;
  pData->LocalZoneMinute = (uint8_t)__NMEA0183_StringToInt(&pStr, 0, 0); //*** Get and save local zone minute <mm>

  if (*pStr != NMEA0183_CHECKSUM_DELIMITER) return ERR__PARSE_ERROR;     // Should be a '*'
  return ERR_OK;
}
#endif

//-----------------------------------------------------------------------------





//**********************************************************************************************************************************************************
//=============================================================================
// Get decoder state
//=============================================================================
eNMEA0183_State NMEA0183_GetDecoderState(NMEA0183_DecodeInput* pDecoder)
{
#ifdef CHECK_NULL_PARAM
    if (pDecoder == NULL) return false;
#endif
    return pDecoder->State;
}


//=============================================================================
// Is a frame ready to process?
//=============================================================================
bool NMEA0183_IsFrameReadyToProcess(NMEA0183_DecodeInput* pDecoder)
{
#ifdef CHECK_NULL_PARAM
  if (pDecoder == NULL) return false;
#endif
  return pDecoder->State == NMEA0183_TO_PROCESS;
}


//=============================================================================
// Process the NMEA0183 frame
//=============================================================================
eERRORRESULT NMEA0183_ProcessFrame(NMEA0183_DecodeInput* pDecoder, NMEA0183_DecodedData* pData)
{
#ifdef CHECK_NULL_PARAM
  if ((pDecoder == NULL) || (pData == NULL)) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error = ERR_OK;
  pData->ParseIsValid = false;
  pDecoder->State = NMEA0183_WAIT_START; // Frame is Processed, wait for a new frame

  //--- Frame control ---
  if (pDecoder->RawFrame[0] != NMEA0183_START_DELIMITER) return ERR__BAD_FRAME_TYPE;  // The frame shall start with '$'
  const uint8_t FrameCRC = (uint8_t)__NMEA0183_HexStringToUint(&pDecoder->CRC[0], 2); // Get frame CRC
  if (FrameCRC != pDecoder->CurrCalcCRC) return ERR__CRC_ERROR;                       // The frame CRC shall correspond to the one calculated

  //--- Select sentence ---
  char* pRaw = &pDecoder->RawFrame[1];
  pData->TalkerID   = (eNMEA0183_TalkerID)NMEA0183_TALKER_ID(pRaw[0], pRaw[1]);               // Extract talker ID
  pData->SentenceID = (eNMEA0183_SentencesID)NMEA0183_SENTENCE_ID(pRaw[2], pRaw[3], pRaw[4]); // Extract sentence ID
  pRaw += 5;                                                                                  // Skip Talker and Sentence IDs
  if (*pRaw != NMEA0183_FIELD_DELIMITER) pData->SentenceID = NMEA0183_UNKNOWN;                // Sentence ID >3 chars so unknown by this library. User needs to parce externaly
  while ((*pRaw != NMEA0183_FIELD_DELIMITER) && (*pRaw != 0)) ++pRaw;                         // Go to the end of the field or the end of the string
  if (*pRaw != NMEA0183_FIELD_DELIMITER) return ERR__PARSE_ERROR; ++pRaw;                     // Check field delimiter

  //--- Parse sentence ---
  switch (pData->SentenceID)
  {
#ifdef NMEA0183_DECODE_AAM
    case NMEA0183_AAM: Error = NMEA0183_ProcessAAM(pRaw, &pData->AAM); break;
#endif
#ifdef NMEA0183_DECODE_GGA
    case NMEA0183_GGA: Error = NMEA0183_ProcessGGA(pRaw, &pData->GGA); break;
#endif
#ifdef NMEA0183_DECODE_GLL
    case NMEA0183_GLL: Error = NMEA0183_ProcessGLL(pRaw, &pData->GLL); break;
#endif
#ifdef NMEA0183_DECODE_GSA
    case NMEA0183_GSA: Error = NMEA0183_ProcessGSA(pRaw, &pData->GSA); break;
#endif
#ifdef NMEA0183_DECODE_GSV
    case NMEA0183_GSV: Error = NMEA0183_ProcessGSV(pRaw, &pData->GSV); break;
#endif
#ifdef NMEA0183_DECODE_RMC
    case NMEA0183_RMC: Error = NMEA0183_ProcessRMC(pRaw, &pData->RMC); break;
#endif
#ifdef NMEA0183_DECODE_VTG
    case NMEA0183_VTG: Error = NMEA0183_ProcessVTG(pRaw, &pData->VTG); break;
#endif
#ifdef NMEA0183_DECODE_TXT
    case NMEA0183_TXT: Error = NMEA0183_ProcessTXT(pRaw, &pData->TXT); break;
#endif
#ifdef NMEA0183_DECODE_ZDA
    case NMEA0183_ZDA: Error = NMEA0183_ProcessZDA(pRaw, &pData->ZDA); break;
#endif

    default:
      memcpy(&pData->Frame[0], &pDecoder->RawFrame[0], NMEA0183_FRAME_BUFFER_SIZE); // Copy the whole unknown frame for the user
      pData->SentenceID = NMEA0183_UNKNOWN;                                         // Sentence ID unknown by this library. User needs to parce externaly
      Error = ERR__UNKNOWN_ELEMENT;
  }
  pData->ParseIsValid = (Error == ERR_OK);
  return Error;
}

//-----------------------------------------------------------------------------





//********************************************************************************************************************
// NMEA0183 tools API
//********************************************************************************************************************
#ifdef NMEA0183_FLOAT_BASED_TOOLS
//=============================================================================
// Convert coordinate to degree (D.d)
//=============================================================================
eERRORRESULT NMEA0183_CoordinateToDegree(NMEA0183_Coordinate* pCoordinate, double* pDegree)
{
#ifdef CHECK_NULL_PARAM
  if ((pCoordinate == NULL) || (pDegree == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (pCoordinate->Minute == NMEA0183_NO_VALUE) return ERR__NO_DATA_AVAILABLE;
  *pDegree = ((double)pCoordinate->Minute / (10000000.0 * 60.0)); // Get .d (M.m / 60)
  *pDegree += (double)pCoordinate->Degree;                        // Add D
  if ((pCoordinate->Direction != 'N') || (pCoordinate->Direction != 'E')) *pDegree = -*pDegree;
  return ERR_OK;
}


//=============================================================================
// Convert coordinate to degree minute (D M.m)
//=============================================================================
eERRORRESULT NMEA0183_CoordinateToDegreeMinute(NMEA0183_Coordinate* pCoordinate, NMEA0183_DegreeMinute* pDegMinute)
{
#ifdef CHECK_NULL_PARAM
  if ((pCoordinate == NULL) || (pDegMinute == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (pCoordinate->Minute == NMEA0183_NO_VALUE) return ERR__NO_DATA_AVAILABLE;
  pDegMinute->Degree = (int16_t)pCoordinate->Degree;             // Get D
  if ((pCoordinate->Direction != 'N') || (pCoordinate->Direction != 'E')) pDegMinute->Degree = -pDegMinute->Degree;
  pDegMinute->Minute = (double)pCoordinate->Minute / 10000000.0; // Get M.m
  return ERR_OK;
}


//=============================================================================
// Convert coordinate to degree minute seconds (D M S.s)
//=============================================================================
eERRORRESULT NMEA0183_CoordinateToDegreeMinuteSeconds(NMEA0183_Coordinate* pCoordinate, NMEA0183_DegreeMinuteSeconds* pDegMinSec)
{
#ifdef CHECK_NULL_PARAM
  if ((pCoordinate == NULL) || (pDegMinSec == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (pCoordinate->Minute == NMEA0183_NO_VALUE) return ERR__NO_DATA_AVAILABLE;
  pDegMinSec->Degree = (int16_t)pCoordinate->Degree;                                 // Get D
  if ((pCoordinate->Direction != 'N') || (pCoordinate->Direction != 'E')) pDegMinSec->Degree = -pDegMinSec->Degree;
  pDegMinSec->Minute = (uint8_t)(pCoordinate->Minute / 10000000u);                   // Get M
  uint32_t Value = pCoordinate->Minute - ((uint32_t)pDegMinSec->Minute * 10000000u); // Get .m
  pDegMinSec->Seconds = (double)Value * (60.0 / 10000000.0);                         // Get S.s (.m * 60 = S.s)
  return ERR_OK;
}

//-----------------------------------------------------------------------------



//=============================================================================
// Convert degree (D.d) to degree minute (D M.m)
//=============================================================================
eERRORRESULT NMEA0183_DegreeToDegreeMinute(double degree, NMEA0183_DegreeMinute* pDegMinute)
{
#ifdef CHECK_NULL_PARAM
  if (pDegMinute == NULL) return ERR__PARAMETER_ERROR;
#endif
  if (isnan(degree)) return ERR__NO_DATA_AVAILABLE;
  pDegMinute->Degree = (int16_t)degree;                              // Get D
  pDegMinute->Minute = (degree - (double)pDegMinute->Degree) * 60.0; // Get M.m (.d * 60 = M.m)
  return ERR_OK;
}


//=============================================================================
// Convert degree (D.d) to degree minute seconds (D M S.s)
//=============================================================================
eERRORRESULT NMEA0183_DegreeToDegreeMinuteSeconds(double degree, NMEA0183_DegreeMinuteSeconds* pDegMinSec)
{
#ifdef CHECK_NULL_PARAM
  if (pDegMinSec == NULL) return ERR__PARAMETER_ERROR;
#endif
  if (isnan(degree)) return ERR__NO_DATA_AVAILABLE;
  pDegMinSec->Degree  = (int16_t)degree;          // Get D
  degree = (degree - (double)pDegMinSec->Degree);
  pDegMinSec->Minute  = (uint8_t)(degree * 60.0); // Get M (.d * 60 = M.m)
  degree = (degree - ((double)pDegMinSec->Minute / 60.0));
  pDegMinSec->Seconds = (degree * 3600.0);        // Get S.s (.m * 60 = S.s)
  return ERR_OK;
}


//=============================================================================
// Convert degree minute (D M.m) to degree (D.d)
//=============================================================================
eERRORRESULT NMEA0183_DegreeMinuteToDegree(NMEA0183_DegreeMinute* pDegMinute, double* pDegree)
{
#ifdef CHECK_NULL_PARAM
  if ((pDegMinute == NULL) || (pDegree == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (isnan(pDegMinute->Minute)) return ERR__NO_DATA_AVAILABLE;
  *pDegree = (double)pDegMinute->Degree + ((double)pDegMinute->Minute / 60.0); // Get D.d (D + M.m / 60)
  return ERR_OK;
}


//=============================================================================
// Convert degree minute (D M.m) to degree minute seconds (D M S.s)
//=============================================================================
eERRORRESULT NMEA0183_DegreeMinuteToDegreeMinuteSeconds(NMEA0183_DegreeMinute* pDegMinute, NMEA0183_DegreeMinuteSeconds* pDegMinSec)
{
#ifdef CHECK_NULL_PARAM
  if ((pDegMinute == NULL) || (pDegMinSec == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (isnan(pDegMinute->Minute)) return ERR__NO_DATA_AVAILABLE;
  pDegMinSec->Degree  = pDegMinute->Degree;                       // Get D
  pDegMinSec->Minute  = (uint8_t)pDegMinute->Minute;              // Get M from M.m
  double Value = pDegMinute->Minute - (double)pDegMinSec->Minute; // Get .m
  pDegMinSec->Seconds = Value * 60.0;                             // Get S.s (.m * 60 = S.s)
  return ERR_OK;
}


//=============================================================================
// Convert degree minute seconds (D M S.s) to degree (D.d)
//=============================================================================
eERRORRESULT NMEA0183_DegreeMinuteSecondsToDegree(NMEA0183_DegreeMinuteSeconds* pDegMinSec, double* pDegree)
{
#ifdef CHECK_NULL_PARAM
  if ((pDegMinSec == NULL) || (pDegree == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (isnan(pDegMinSec->Seconds)) return ERR__NO_DATA_AVAILABLE;
  *pDegree = (double)pDegMinSec->Degree + ((double)pDegMinSec->Minute / 60.0) + (pDegMinSec->Seconds / 3600.0); // Get D.d (D + M/60 + S.s/3600)
  return ERR_OK;
}


//=============================================================================
// Convert degree minute seconds (D M S.s) to degree minute (D M.m)
//=============================================================================
eERRORRESULT NMEA0183_DegreeMinuteSecondsToDegreeMinute(NMEA0183_DegreeMinuteSeconds* pDegMinSec, NMEA0183_DegreeMinute* pDegMinute)
{
#ifdef CHECK_NULL_PARAM
  if ((pDegMinSec == NULL) || (pDegMinute == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (isnan(pDegMinSec->Seconds)) return ERR__NO_DATA_AVAILABLE;
  pDegMinute->Degree = pDegMinSec->Degree;                                        // Get D
  pDegMinute->Minute = (double)pDegMinSec->Minute + (pDegMinSec->Seconds / 60.0); // Get M.m (M + S.s / 60)
  return ERR_OK;
}

//-----------------------------------------------------------------------------
#endif // NMEA0183_FLOAT_BASED_TOOLS
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif





//-----------------------------------------------------------------------------
#if defined(NMEA0183_GPS_DECODER_CLASS) && defined(NMEA0183_FLOAT_BASED_TOOLS) && defined(__cplusplus)
//********************************************************************************************************************
// GPS decoder Class
//********************************************************************************************************************
//=============================================================================
// Constructor of the GPS decoder class
//=============================================================================
GPSdecoder::GPSdecoder() :
    _Time{ 0xFF, 0xFF, 0xFF, 0xFFFF },     _NewTimeAvailable(false),      // Time
    _Date{ 0xFF, 0xFF, 0xFFFF },           _NewDateAvailable(false),      // Date
    _Latitude(NAN),                        _NewLatitudeAvailable(false),  // Latitude in degrees
    _Longitude(NAN),                       _NewLongitudeAvailable(false), // Longitude in degrees
    _Altitude((int32_t)NMEA0183_NO_VALUE), _NewAltitudeAvailable(false),  // Altitude mean-sea-level (geoid) in meters
    _Speed((uint32_t)NMEA0183_NO_VALUE),   _NewSpeedAvailable(false),     // Speed over the ground in knots
    _Track((uint32_t)NMEA0183_NO_VALUE),   _NewTrackAvailable(false),     // Track angle in degrees (True)
    _HDOP((uint16_t)NMEA0183_NO_VALUE),    _NewHDOPAvailable(false)       // Horizontal Dilution of Precision
{
    (void)Init_NMEA0183(&InputData);
}


//=============================================================================
// Process the received GPS frame character data
//=============================================================================
eERRORRESULT GPSdecoder::ProcessCharacter(const char data)
{
    eERRORRESULT Error = NMEA0183_AddReceivedCharacter(&InputData, data);
    if (Error != ERR_OK) return Error;
    eNMEA0183_State State = NMEA0183_GetDecoderState(&InputData);
    if (NMEA0183_IsFrameReadyToProcess(&InputData))
    {
        NMEA0183_DecodedData DecodedData;
        Error = NMEA0183_ProcessFrame(&InputData, &DecodedData);
        switch (DecodedData.SentenceID)
        {
# ifdef NMEA0183_DECODE_GGA
            case NMEA0183_GGA:
                (void)NMEA0183_CoordinateToDegree(&DecodedData.GGA.Latitude,  &_Latitude);  _NewLatitudeAvailable  = true; // Get new latitude data
                (void)NMEA0183_CoordinateToDegree(&DecodedData.GGA.Longitude, &_Longitude); _NewLongitudeAvailable = true; // Get new longitude data
                _Altitude = DecodedData.GGA.Altitude;            _NewAltitudeAvailable = (_Altitude != NMEA0183_NO_VALUE); // Get new altitude data
                _HDOP     = DecodedData.GGA.HDOP;                _NewHDOPAvailable     = (_HDOP     != NMEA0183_NO_VALUE); // Get new HDOP data
                break;
# endif
# ifdef NMEA0183_DECODE_GLL
            case NMEA0183_GLL:
                (void)NMEA0183_CoordinateToDegree(&DecodedData.GLL.Latitude,  &_Latitude);  _NewLatitudeAvailable  = true; // Get new new latitude data
                (void)NMEA0183_CoordinateToDegree(&DecodedData.GLL.Longitude, &_Longitude); _NewLongitudeAvailable = true; // Get new longitude data
                break;
# endif
# ifdef NMEA0183_DECODE_GSA
            case NMEA0183_GSA:
                _HDOP = DecodedData.GSA.HDOP;                        _NewHDOPAvailable = (_HDOP     != NMEA0183_NO_VALUE); // Get new HDOP data
                break;
# endif
# ifdef NMEA0183_DECODE_RMC
            case NMEA0183_RMC:
                (void)NMEA0183_CoordinateToDegree(&DecodedData.RMC.Latitude,  &_Latitude);  _NewLatitudeAvailable  = true; // Get new new latitude data
                (void)NMEA0183_CoordinateToDegree(&DecodedData.RMC.Longitude, &_Longitude); _NewLongitudeAvailable = true; // Get new longitude data
                _Speed = DecodedData.RMC.Speed;                        _NewSpeedAvailable = (_Speed != NMEA0183_NO_VALUE); // Get new speed data
                _Track = DecodedData.RMC.Track;                        _NewTrackAvailable = (_Track != NMEA0183_NO_VALUE); // Get new track data
                break;
# endif
            default:
                break;
        }
    }
    return Error;
};


//=============================================================================
// Convert track/course to cardinal direction
//=============================================================================
const char* const GPSdecoder::TrackToCardinal(double track) const
{
    static const char* const _DIRECTIONS[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW", '\0' };
    if (isnan(track) || isfinite(track)) return _DIRECTIONS[16]; // Return nothing
    size_t Direction = (size_t)((track + 11.25) / 22.5);
    return _DIRECTIONS[Direction & 0xF];
}
//-----------------------------------------------------------------------------
#endif // defined(NMEA0183_GPS_DECODER_CLASS) && defined(NMEA0183_FLOAT_BASED_TOOLS) && defined(__cplusplus)