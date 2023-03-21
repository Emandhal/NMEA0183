/*!*****************************************************************************
 * @file    NMEA0183.hpp
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.0.0
 * @date    13/02/2022
 * @brief   NMEA decoder library (C++ class wrapper header for C NMEA0183 library)
 * @details Supports common GPS frames
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
#ifndef NMEA0183_LIB_HPP_
#define NMEA0183_LIB_HPP_
//=============================================================================

//-----------------------------------------------------------------------------
#if !defined(NMEA0183_USE_INPUT_BUFFER)
#  define NMEA0183_USE_INPUT_BUFFER
#endif
//-----------------------------------------------------------------------------
#include "NMEA0183.h"
//-----------------------------------------------------------------------------
#ifdef NMEA0183_FLOAT_BASED_TOOLS
#  include <math.h>
#endif
//-----------------------------------------------------------------------------





//********************************************************************************************************************
// NMEA0183 decoder Class
//********************************************************************************************************************
class NMEA0183decoder
{
  protected:
    struct NMEA0183_InputBuffer InputData; // NMEA0183 decoder structure

  public:
    /*! @brief Constructor
     * Initialize NMEA0183 decoder input data
     */
    NMEA0183decoder() { (void)Init_NMEA0183(&InputData); };

    /*! @brief Destructor
     * Do nothing in this case
     */
    ~NMEA0183decoder() { };

  public:
    /*! @brief Add NMEA0183 received frame character data
     *
     * @param[in] data Is the char to add to input data buffer
     * @return Returns an #eERRORRESULT value enum
     */
    eERRORRESULT AddReceivedCharacter(char data) { return NMEA0183_AddReceivedCharacter(&InputData, data); };

    /*! @brief Get decoder state
     * @return Returns an #eNMEA0183_State value enum
     */
    eNMEA0183_State GetDecoderState(void) { return NMEA0183_GetDecoderState(&InputData); };

    /*! @brief Is a frame ready to process?
     * @return Returns 'true' if a frame is ready to process else 'false'
     */
    bool IsFrameReadyToProcess(void) { NMEA0183_IsFrameReadyToProcess(&InputData); };

    /*! @brief Process the NMEA0183 frame (used with the decode structure)
     *
     * @param[out] *pData Is the decoded data
     * @return Returns an #eERRORRESULT value enum
     */
    eERRORRESULT ProcessFrame(NMEA0183_DecodedData* pData) { return NMEA0183_ProcessFrame(&InputData, pData); };

    //-----------------------------------------------------------------------------

    /*! @brief Process the NMEA0183 frame string line
     *
     * This function can be used alone without using the NMEA0183_DecodeInput structure
     * This fucntion process a whole NMEA0183 line at once (from '$' to the char before the \r or \n terminal)
     * @param[in] *pDecoder Is the frame string (with '\0' terminal) to process
     * @param[out] *pData Is the decoded data
     * @return Returns an #eERRORRESULT value enum
     */
    eERRORRESULT ProcessLine(const char* string, NMEA0183_DecodedData* pData);

#ifdef NMEA0183_FLOAT_BASED_TOOLS
  public:
    /*! @brief Convert coordinate to degree (D.d)
     *
     * @param[in] &pCoordinate Is the coordinate to convert
     * @param[out] *pDegree Is where the coordinate converted will be stored
     * @return Returns an #eERRORRESULT value enum
     */
    eERRORRESULT CoordinateToDegree(NMEA0183_Coordinate& pCoordinate, double* pDegree) { return NMEA0183_CoordinateToDegree(&pCoordinate, pDegree); };

    /*! @brief Convert coordinate to degree minute (D M.m)
     *
     * @param[in] &pCoordinate Is the coordinate to convert
     * @param[out] *pDegMinute Is where the coordinate converted will be stored
     * @return Returns an #eERRORRESULT value enum
     */
    eERRORRESULT CoordinateToDegreeMinute(NMEA0183_Coordinate& pCoordinate, NMEA0183_DegreeMinute* pDegMinute) { return NMEA0183_CoordinateToDegreeMinute(&pCoordinate, pDegMinute); };

    /*! @brief Convert coordinate to degree minute seconds (D M S)
     *
     * @param[in] &pCoordinate Is the coordinate to convert
     * @param[out] *pDegMinSec Is where the coordinate converted will be stored
     * @return Returns an #eERRORRESULT value enum
     */
    eERRORRESULT CoordinateToDegreeMinuteSeconds(NMEA0183_Coordinate& pCoordinate, NMEA0183_DegreeMinuteSeconds* pDegMinSec) { return NMEA0183_CoordinateToDegreeMinuteSeconds(&pCoordinate, pDegMinSec); };
#endif
};





#if defined(NMEA0183_GPS_DECODER_CLASS) && defined(NMEA0183_FLOAT_BASED_TOOLS)
//********************************************************************************************************************
// GPS decoder Class
//********************************************************************************************************************
class GPSdecoder
{
protected:
    struct NMEA0183_InputBuffer InputData; // NMEA0183 decoder structure

private:
    NMEA0183_Time _Time; bool _NewTimeAvailable;      // Time
    NMEA0183_Date _Date; bool _NewDateAvailable;      // Date
    double _Latitude;    bool _NewLatitudeAvailable;  // Latitude in degrees
    double _Longitude;   bool _NewLongitudeAvailable; // Longitude in degrees
    int32_t _Altitude;   bool _NewAltitudeAvailable;  // Altitude mean-sea-level (geoid) in meters
    uint32_t _Speed;     bool _NewSpeedAvailable;     // Speed over the ground in knots
    uint32_t _Track;     bool _NewTrackAvailable;     // Track angle in degrees (True)
    uint16_t _HDOP;      bool _NewHDOPAvailable;      // Horizontal Dilution of Precision

public:
    /*! @brief Constructor
     * Initialize GPS decoder input data
     */
    GPSdecoder();

    /*! @brief Destructor
     * Do nothing in this case
     */
    ~GPSdecoder() { };

public:
    /*! @brief Process the received GPS frame character data
     *
     * @param[in] data Is the char to add to input data buffer
     * @return Returns an #eERRORRESULT value enum
     */
    eERRORRESULT ProcessCharacter(const char data);


public:
    /*! @brief Is a new time available?
     * @return Returns 'true' if a new time has been decoded else 'false'
     */
    inline bool IsNewTimeAvailable(void) const { return _NewTimeAvailable; };

    /*! @brief Get time
     * @return Returns the last decoded time
     */
    inline NMEA0183_Time GetTime(void) const { return _Time; };

    //-----------------------------------------------------------------------------

    /*! @brief Is a new date available?
     * @return Returns 'true' if a new date has been decoded else 'false'
     */
    inline bool IsNewDateAvailable(void) const { return _NewDateAvailable; };

    /*! @brief Get date
     * @return Returns the last decoded date
     */
    inline NMEA0183_Date GetDate(void) const { return _Date; };

    //-----------------------------------------------------------------------------

    /*! @brief Is a new latitude available?
     * @return Returns 'true' if a new latitude has been decoded else 'false'
     */
    inline bool IsNewLatitudeAvailable(void) const { return _NewLatitudeAvailable; };

    /*! @brief Get latitude in degree (D.d)
     * @return Returns the last decoded latitude in decimal degrees
     */
    inline double GetLatitudeInDegree(void) const { return _Latitude; };

    /*! @brief Get latitude in degree minute (D M.m)
     * @return Returns the last decoded latitude in decimal degrees
     */
    inline NMEA0183_DegreeMinute GetLatitudeInDegreeMinute(void) const
    {
        NMEA0183_DegreeMinute Result;
        (void)NMEA0183_DegreeToDegreeMinute(_Latitude, &Result);
        return Result;
    };

    /*! @brief Get latitude in degree minute seconds (D M S.s)
     * @return Returns the last decoded latitude in decimal degrees
     */
    inline NMEA0183_DegreeMinuteSeconds GetLatitudeInDegreeMinuteSeconds(void) const
    {
        NMEA0183_DegreeMinuteSeconds Result;
        (void)NMEA0183_DegreeToDegreeMinuteSeconds(_Latitude, &Result);
        return Result;
    };

    //-----------------------------------------------------------------------------

    /*! @brief Is a new longitude available?
     * @return Returns 'true' if a new longitude has been decoded else 'false'
     */
    inline bool IsNewLongitudeAvailable(void) const { return _NewLongitudeAvailable; };

    /*! @brief Get longitude in degree (D.d)
     * @return Returns the last decoded longitude in decimal degrees
     */
    inline double GetLongitudeInDegree(void) const { return _Longitude; };

    /*! @brief Get longitude in degree minute (D M.m)
     * @return Returns the last decoded longitude in decimal degrees
     */
    inline NMEA0183_DegreeMinute GetLongitudeInDegreeMinute(void) const
    {
        NMEA0183_DegreeMinute Result;
        (void)NMEA0183_DegreeToDegreeMinute(_Latitude, &Result);
        return Result;
    };

    /*! @brief Get longitude in degree minute seconds (D M S.s)
     * @return Returns the last decoded longitude in decimal degrees
     */
    inline NMEA0183_DegreeMinuteSeconds GetLongitudeInDegreeMinuteSeconds(void) const
    {
        NMEA0183_DegreeMinuteSeconds Result;
        (void)NMEA0183_DegreeToDegreeMinuteSeconds(_Latitude, &Result);
        return Result;
    };

    //-----------------------------------------------------------------------------

    /*! @brief Is a new altitude available?
     * @return Returns 'true' if a new altitude has been decoded else 'false'
     */
    inline bool IsNewAltitudeAvailable(void) const { return _NewAltitudeAvailable; };

    /*! @brief Get altitude mean-sea-level (geoid) in meters
     * @return Returns the last decoded altitude mean-sea-level (geoid) in meters
     */
    inline double GetAltitudeInMeter(void) const { return (double)_Altitude / 100.0; };

    //-----------------------------------------------------------------------------

    /*! @brief Is a new speed available?
     * @return Returns 'true' if a new speed has been decoded else 'false'
     */
    inline bool IsNewSpeedAvailable(void) const { return _NewSpeedAvailable; };

    /*! @brief Get speed over the ground in knots
     * @return Returns the last decoded speed over the ground in knots
     */
    inline double GetSpeedInKnots(void) const { return (_Speed != NMEA0183_NO_VALUE ? (double)_Speed / 10000.0 : NAN); };

    //-----------------------------------------------------------------------------

    /*! @brief Is a new track available?
     * @return Returns 'true' if a new track has been decoded else 'false'
     */
    inline bool IsNewTrackAvailable(void) const { return _NewTrackAvailable; };

    /*! @brief Get track angle in degrees (True)
     * @return Returns the last decoded track angle in degrees (True)
     */
    inline double GetTrackInDegree(void) const { return (_Track != NMEA0183_NO_VALUE ? (double)_Track / 10000.0 : NAN); };

    /*! @brief Convert track/course to cardinal direction
     * @return Returns the cardinal direction string
     */
    const char* const TrackToCardinal(double track) const;

    //-----------------------------------------------------------------------------

    /*! @brief Is a new HDOP available?
     * @return Returns 'true' if a new HDOP has been decoded else 'false'
     */
    inline bool IsNewHDOPAvailable(void) const { return _NewHDOPAvailable; };

    /*! @brief Get Horizontal Dilution of Precision
     * @return Returns the last decoded Horizontal Dilution of Precision
     */
    inline double GetHDOP(void) const { return (_HDOP != NMEA0183_NO_VALUE ? (double)_HDOP / 100.0 : NAN); };
};
#endif



//-----------------------------------------------------------------------------
#endif /* NMEA0183_LIB_HPP_ */