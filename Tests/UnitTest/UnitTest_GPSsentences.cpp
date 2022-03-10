#include "pch.h"
#include "CppUnitTest.h"
#include <cstdint>
#include <windows.h>
#include <WinUser.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework
{
    template<> inline std::wstring ToString<uint16_t>(const uint16_t& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eERRORRESULT>(const eERRORRESULT& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_State>(const eNMEA0183_State& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_TalkerID>(const eNMEA0183_TalkerID& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_SentencesID>(const eNMEA0183_SentencesID& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_SVhealth>(const eNMEA0183_SVhealth& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_NAVhealth>(const eNMEA0183_NAVhealth& t) { RETURN_WIDE_STRING(t); }
}   }   }





namespace NMEA0183test
{
    TEST_CLASS(GPS_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_AAM
        TEST_METHOD(TestMethod_AAM)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Low Data) ============================================
            const char* const TEST_AAM_LOW_DATA_FRAME = "$GPAAM,V,V,0.2,N,*14\r\n";
            for (size_t z = 0; z < strlen(TEST_AAM_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_AAM_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_AAM, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_AAM");
            Assert::AreEqual('V', FrameData.AAM.ArrivalStatus, L"Test (Low Data), ArrivalStatus should be 'V'");
            Assert::AreEqual('V', FrameData.AAM.PassedWaypoint, L"Test (Low Data), PassedWaypoint should be 'V'");
            Assert::AreEqual(2000u, FrameData.AAM.CircleRadius, L"Test (Low Data), CircleRadius should be 2000");
            Assert::AreEqual(0, strncmp("", &FrameData.AAM.WaypointID[0], strlen(FrameData.TXT.TextMessage)), L"Test (Full Data), WaypointID should be '\0'");

            //=== Test (Full Data) ===========================================
            const char* const TEST_AAM_FULL_DATA_FRAME = "$GPAAM,A,A,0.10,N,WPTNME*32\r\n";
            for (size_t z = 0; z < strlen(TEST_AAM_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_AAM_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_AAM, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_AAM");
            Assert::AreEqual('A', FrameData.AAM.ArrivalStatus, L"Test (Full Data), ArrivalStatus should be 'A'");
            Assert::AreEqual('A', FrameData.AAM.PassedWaypoint, L"Test (Full Data), PassedWaypoint should be 'A'");
            Assert::AreEqual(1000u, FrameData.AAM.CircleRadius, L"Test (Full Data), CircleRadius should be 1000");
            Assert::AreEqual(0, strncmp("WPTNME", &FrameData.AAM.WaypointID[0], strlen(FrameData.TXT.TextMessage)), L"Test (Full Data), WaypointID should be 'WPTNME'");
        }
#endif

#ifdef NMEA0183_DECODE_ALM
        TEST_METHOD(TestMethod_ALM)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ===========================================
            const char* const TEST_ALM_FULL_DATA_FRAME = "$GPALM,1,1,15,1159,74,441d,4e,16be,fd5e,a10c9f,4a2da4,686e81,a8cbe1,0a4,401*24\r\n";
            for (size_t z = 0; z < strlen(TEST_ALM_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_ALM_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_ALM, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_ALM");
            Assert::AreEqual((uint8_t)1u, FrameData.ALM.TotalSentence, L"Test (Full Data), TotalSentence should be 1");
            Assert::AreEqual((uint8_t)1u, FrameData.ALM.SentenceNumber, L"Test (Full Data), SentenceNumber should be 1");
            Assert::AreEqual((uint8_t)15u, FrameData.ALM.SatellitePRNnumber, L"Test (Full Data), SatellitePRNnumber should be 15");
            Assert::AreEqual((uint16_t)1159u, FrameData.ALM.GPSweekNumber, L"Test (Full Data), GPSweekNumber should be 1159");
            eNMEA0183_SVhealth SVhealth = NMEA0183_ALM_SV_HEALTH_GET(FrameData.ALM.SV_NAVhealth);
            Assert::AreEqual(NMEA0183_L1L2_C_SIGNALS_DEAD, SVhealth, L"Test (Full Data), SVhealth should be NMEA0183_L1L2_C_SIGNALS_DEAD");
            eNMEA0183_NAVhealth NAVhealth = NMEA0183_ALM_NAV_HEALTH_GET(FrameData.ALM.SV_NAVhealth);
            Assert::AreEqual(NMEA0183_ALL_UPLOADED_DATA_BAD, NAVhealth, L"Test (Full Data), NAVhealth should be NMEA0183_ALL_UPLOADED_DATA_BAD");
            Assert::AreEqual((uint16_t)0x441D, FrameData.ALM.e, L"Test (Full Data), e should be 0x441D");
            Assert::AreEqual((uint8_t)0x4E, FrameData.ALM.toa, L"Test (Full Data), toa should be 0x4E");
            Assert::AreEqual((int16_t)0x16BE, FrameData.ALM.Sigma_i, L"Test (Full Data), Sigma_i should be 0x16BE");
            Assert::AreEqual((int16_t)0xFD5E, FrameData.ALM.OMEGADOT, L"Test (Full Data), OMEGADOT should be 0xFD5E");
            Assert::AreEqual((uint32_t)0xA10C9F, FrameData.ALM.Root_A, L"Test (Full Data), Root_A should be 0xA10C9F");
            Assert::AreEqual((int32_t)0x4A2DA4, FrameData.ALM.OMEGA, L"Test (Full Data), OMEGA should be 0x4A2DA4");
            Assert::AreEqual((int32_t)0x686E81, FrameData.ALM.OMEGA_0, L"Test (Full Data), OMEGA_0 should be 0x686E81");
            Assert::AreEqual((int32_t)0xFFA8CBE1, FrameData.ALM.Mo, L"Test (Full Data), Mo should be 0xFFA8CBE1"); // A8CBE1 is 24-bits signed so in 32-bit signed it is 0xFFA8CBE1
            Assert::AreEqual((int16_t)0x0A4, FrameData.ALM.af0, L"Test (Full Data), af0 should be 0x0A4");
            Assert::AreEqual((int16_t)0xFC01, FrameData.ALM.af1, L"Test (Full Data), af1 should be 0xC01"); // 401 is 11-bits signed so in 16-bit signed it is 0xFC01
        }
#endif

#ifdef NMEA0183_DECODE_GGA
        TEST_METHOD(TestMethod_GGA)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Low Data) ============================================
            const char* const TEST_GGA_LOW_DATA_FRAME = "$GNGGA,001043.00,4404.14036,N,12118.85961,W,1,12,0.98,1113.0,M,-21.3,M*47\r\n";
            for (size_t z = 0; z < strlen(TEST_GGA_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GGA_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GN, FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_GN");
            Assert::AreEqual(NMEA0183_GGA, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_GGA");
            Assert::AreEqual((uint8_t)00u, FrameData.GGA.Time.Hour, L"Test (Low Data), Time.Hour should be 00");
            Assert::AreEqual((uint8_t)10u, FrameData.GGA.Time.Minute, L"Test (Low Data), Time.Minute should be 10");
            Assert::AreEqual((uint8_t)43u, FrameData.GGA.Time.Second, L"Test (Low Data), Time.Second should be 43");
            Assert::AreEqual((uint16_t)000u, FrameData.GGA.Time.MilliS, L"Test (Low Data), Time.MilliS should be 000");
            Assert::AreEqual('N', FrameData.GGA.Latitude.Direction, L"Test (Low Data), Latitude.Direction should be 'N'");
            Assert::AreEqual((uint8_t)44u, FrameData.GGA.Latitude.Degree, L"Test (Low Data), Latitude.Degree should be 44");
            Assert::AreEqual(41403600u, FrameData.GGA.Latitude.Minute, L"Test (Low Data), Latitude.Minute should be 41403600");
            Assert::AreEqual('W', FrameData.GGA.Longitude.Direction, L"Test (Low Data), Longitude.Direction should be 'W'");
            Assert::AreEqual((uint8_t)121u, FrameData.GGA.Longitude.Degree, L"Test (Low Data), Longitude.Degree should be 121");
            Assert::AreEqual(188596100u, FrameData.GGA.Longitude.Minute, L"Test (Low Data), Longitude.Minute should be 188596100");
            Assert::AreEqual('1', FrameData.GGA.GPSquality, L"Test (Low Data), GPSquality should be '1'");
            Assert::AreEqual((uint8_t)12u, FrameData.GGA.SatellitesUsed, L"Test (Low Data), SatellitesUsed should be 12");
            Assert::AreEqual((uint16_t)98u, FrameData.GGA.HDOP, L"Test (Low Data), HDOP should be 98");
            Assert::AreEqual((int32_t)111300u, FrameData.GGA.Altitude, L"Test (Low Data), Altitude should be 111300");
            Assert::AreEqual((int32_t)-2130, FrameData.GGA.GeoidSeparation, L"Test (Low Data), GeoidSeparation should be -2130");

            //=== Test (Full Data) ===========================================
            const char* const TEST_GGA_FULL_DATA_FRAME = "$GPGGA,002153.000,3342.6618,N,01115.73858,W,1,10,1.2,27.0,M,-34.2,M,,0000*6E\r\n";
            for (size_t z = 0; z < strlen(TEST_GGA_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GGA_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_GGA, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_GGA");
            Assert::AreEqual((uint8_t)00u, FrameData.GGA.Time.Hour, L"Test (Full Data), Time.Hour should be 00");
            Assert::AreEqual((uint8_t)21u, FrameData.GGA.Time.Minute, L"Test (Full Data), Time.Minute should be 21");
            Assert::AreEqual((uint8_t)53u, FrameData.GGA.Time.Second, L"Test (Full Data), Time.Second should be 53");
            Assert::AreEqual((uint16_t)000u, FrameData.GGA.Time.MilliS, L"Test (Full Data), Time.MilliS should be 000");
            Assert::AreEqual('N', FrameData.GGA.Latitude.Direction, L"Test (Full Data), Latitude.Direction should be 'N'");
            Assert::AreEqual((uint8_t)33u, FrameData.GGA.Latitude.Degree, L"Test (Full Data), Latitude.Degree should be 33");
            Assert::AreEqual(426618000u, FrameData.GGA.Latitude.Minute, L"Test (Full Data), Latitude.Minute should be 426618000");
            Assert::AreEqual('W', FrameData.GGA.Longitude.Direction, L"Test (Full Data), Longitude.Direction should be 'W'");
            Assert::AreEqual((uint8_t)11u, FrameData.GGA.Longitude.Degree, L"Test (Full Data), Longitude.Degree should be 11");
            Assert::AreEqual(157385800u, FrameData.GGA.Longitude.Minute, L"Test (Full Data), Longitude.Minute should be 157385800");
            Assert::AreEqual('1', FrameData.GGA.GPSquality, L"Test (Full Data), GPSquality should be '1'");
            Assert::AreEqual((uint8_t)10u, FrameData.GGA.SatellitesUsed, L"Test (Full Data), SatellitesUsed should be 10");
            Assert::AreEqual((uint16_t)120u, FrameData.GGA.HDOP, L"Test (Full Data), HDOP should be 120");
            Assert::AreEqual((int32_t)2700u, FrameData.GGA.Altitude, L"Test (Full Data), Altitude should be 2700");
            Assert::AreEqual((int32_t)-3420, FrameData.GGA.GeoidSeparation, L"Test (Full Data), GeoidSeparation should be -3420");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.GGA.AgeOfDiffCorr, L"Test (Full Data), AgeOfDiffCorr should be 0xFFFF");
            Assert::AreEqual((uint16_t)0u, FrameData.GGA.DiffRefStationID, L"Test (Full Data), DiffRefStationID should be 0");
        }
#endif

#ifdef NMEA0183_DECODE_GLL
        TEST_METHOD(TestMethod_GLL)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Low Data) ============================================
            const char* const TEST_GLL_LOW_DATA_FRAME = "$GPGLL,,,,,064512.498,V,N*7B\r\n";
            for (size_t z = 0; z < strlen(TEST_GLL_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GLL_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_GLL, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_GLL");
            Assert::AreEqual(' ', FrameData.GLL.Latitude.Direction, L"Test (Low Data), Latitude.Direction should be ' '");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GLL.Latitude.Degree, L"Test (Low Data), Latitude.Degree should be 0xFF");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.GLL.Latitude.Minute, L"Test (Low Data), Latitude.Minute should be 0xFFFFFFFF");
            Assert::AreEqual(' ', FrameData.GLL.Longitude.Direction, L"Test (Low Data), Longitude.Direction should be ' '");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GLL.Longitude.Degree, L"Test (Low Data), Longitude.Degree should be 0xFF");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.GLL.Longitude.Minute, L"Test (Low Data), Longitude.Minute should be 0xFFFFFFFF");
            Assert::AreEqual((uint8_t)6u, FrameData.GLL.Time.Hour, L"Test (Low Data), Time.Hour should be 06");
            Assert::AreEqual((uint8_t)45u, FrameData.GLL.Time.Minute, L"Test (Low Data), Time.Minute should be 45");
            Assert::AreEqual((uint8_t)12u, FrameData.GLL.Time.Second, L"Test (Low Data), Time.Second should be 12");
            Assert::AreEqual((uint16_t)498u, FrameData.GLL.Time.MilliS, L"Test (Low Data), Time.MilliS should be 498");
            Assert::AreEqual('V', FrameData.GLL.Status, L"Test (Low Data), Status should be 'V'");
            Assert::AreEqual('N', FrameData.GLL.FAAmode, L"Test (Low Data), FAAmode should be 'N'");

            //=== Test (Full Data) ===========================================
            const char* const TEST_GLL_FULL_DATA_FRAME = "$GNGLL,4404.14012,N,12118.85993,W,001037.10,A,A*66\r\n";
            for (size_t z = 0; z < strlen(TEST_GLL_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GLL_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GN, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GN");
            Assert::AreEqual(NMEA0183_GLL, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_GLL");
            Assert::AreEqual('N', FrameData.GLL.Latitude.Direction, L"Test (Full Data), Latitude.Direction should be 'N'");
            Assert::AreEqual((uint8_t)44u, FrameData.GLL.Latitude.Degree, L"Test (Full Data), Latitude.Degree should be 44");
            Assert::AreEqual(41401200u, FrameData.GLL.Latitude.Minute, L"Test (Full Data), Latitude.Minute should be 41401200");
            Assert::AreEqual('W', FrameData.GLL.Longitude.Direction, L"Test (Full Data), Longitude.Direction should be 'W'");
            Assert::AreEqual((uint8_t)121u, FrameData.GLL.Longitude.Degree, L"Test (Full Data), Longitude.Degree should be 121");
            Assert::AreEqual(188599300u, FrameData.GLL.Longitude.Minute, L"Test (Full Data), Longitude.Minute should be 188599300");
            Assert::AreEqual((uint8_t)00u, FrameData.GLL.Time.Hour, L"Test (Full Data), Time.Hour should be 00");
            Assert::AreEqual((uint8_t)10u, FrameData.GLL.Time.Minute, L"Test (Full Data), Time.Minute should be 10");
            Assert::AreEqual((uint8_t)37u, FrameData.GLL.Time.Second, L"Test (Full Data), Time.Second should be 37");
            Assert::AreEqual((uint16_t)100u, FrameData.GLL.Time.MilliS, L"Test (Full Data), Time.MilliS should be 100");
            Assert::AreEqual('A', FrameData.GLL.Status, L"Test (Full Data), Status should be 'A'");
            Assert::AreEqual('A', FrameData.GLL.FAAmode, L"Test (Full Data), FAAmode should be 'A'");
        }
#endif

#ifdef NMEA0183_DECODE_GSA
        TEST_METHOD(TestMethod_GSA)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Low Data) ============================================
            const char* const TEST_GSA_LOW_DATA_FRAME = "$GPGSA,A,1,,,,,,,,,,,,,,,*1E\r\n";
            for (size_t z = 0; z < strlen(TEST_GSA_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GSA_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_GSA, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_GSA");
            Assert::AreEqual('A', FrameData.GSA.Mode1, L"Test (Low Data), Mode1 should be 'A'");
            Assert::AreEqual('1', FrameData.GSA.Mode2, L"Test (Low Data), Mode2 should be '1'");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[0], L"Test (Low Data), SatelliteIDs[0] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[1], L"Test (Low Data), SatelliteIDs[1] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[2], L"Test (Low Data), SatelliteIDs[2] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[3], L"Test (Low Data), SatelliteIDs[3] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[4], L"Test (Low Data), SatelliteIDs[4] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[5], L"Test (Low Data), SatelliteIDs[5] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[6], L"Test (Low Data), SatelliteIDs[6] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[7], L"Test (Low Data), SatelliteIDs[7] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[8], L"Test (Low Data), SatelliteIDs[8] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[9], L"Test (Low Data), SatelliteIDs[9] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[10], L"Test (Low Data), SatelliteIDs[10] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[11], L"Test (Low Data), SatelliteIDs[11] should be 0xFF");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.GSA.PDOP, L"Test (Low Data), PDOP should be 0xFFFF");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.GSA.HDOP, L"Test (Low Data), HDOP should be 0xFFFF");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.GSA.VDOP, L"Test (Low Data), VDOP should be 0xFFFF");

            //=== Test (Full Data) ===========================================
            const char* const TEST_GSA_FULL_DATA_FRAME = "$GNGSA,A,3,80,71,73,79,69,,,,,,,,1.83,1.09,1.47*17\r\n";
            for (size_t z = 0; z < strlen(TEST_GSA_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GSA_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GN, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GN");
            Assert::AreEqual(NMEA0183_GSA, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_GSA");
            Assert::AreEqual('A', FrameData.GSA.Mode1, L"Test (Full Data), Mode1 should be 'A'");
            Assert::AreEqual('3', FrameData.GSA.Mode2, L"Test (Full Data), Mode2 should be '3'");
            Assert::AreEqual((uint8_t)80u, FrameData.GSA.SatelliteIDs[0], L"Test (Full Data), SatelliteIDs[0] should be 80u");
            Assert::AreEqual((uint8_t)71u, FrameData.GSA.SatelliteIDs[1], L"Test (Full Data), SatelliteIDs[1] should be 71u");
            Assert::AreEqual((uint8_t)73u, FrameData.GSA.SatelliteIDs[2], L"Test (Full Data), SatelliteIDs[2] should be 73u");
            Assert::AreEqual((uint8_t)79u, FrameData.GSA.SatelliteIDs[3], L"Test (Full Data), SatelliteIDs[3] should be 79u");
            Assert::AreEqual((uint8_t)69u, FrameData.GSA.SatelliteIDs[4], L"Test (Full Data), SatelliteIDs[4] should be 69u");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[5], L"Test (Full Data), SatelliteIDs[5] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[6], L"Test (Full Data), SatelliteIDs[6] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[7], L"Test (Full Data), SatelliteIDs[7] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[8], L"Test (Full Data), SatelliteIDs[8] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[9], L"Test (Full Data), SatelliteIDs[9] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[10], L"Test (Full Data), SatelliteIDs[10] should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSA.SatelliteIDs[11], L"Test (Full Data), SatelliteIDs[11] should be 0xFF");
            Assert::AreEqual((uint16_t)183u, FrameData.GSA.PDOP, L"Test (Full Data), PDOP should be 183u");
            Assert::AreEqual((uint16_t)109u, FrameData.GSA.HDOP, L"Test (Full Data), HDOP should be 109u");
            Assert::AreEqual((uint16_t)147u, FrameData.GSA.VDOP, L"Test (Full Data), VDOP should be 147u");
        }
#endif

#ifdef NMEA0183_DECODE_GSV
        TEST_METHOD(TestMethod_GSV)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Low Data) ============================================
            const char* const TEST_GSV_LOW_DATA_FRAME = "$GPGSV,3,1,12,01,00,000,,02,00,000,,03,00,000,,04,00,000,*7C\r\n";
            for (size_t z = 0; z < strlen(TEST_GSV_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GSV_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_GSV, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_GSV");
            Assert::AreEqual((uint8_t)3u, FrameData.GSV.TotalSentence, L"Test (Low Data), TotalSentence should be 3");
            Assert::AreEqual((uint8_t)1u, FrameData.GSV.SentenceNumber, L"Test (Low Data), SentenceNumber should be 1");
            Assert::AreEqual((uint8_t)12u, FrameData.GSV.TotalSatellite, L"Test (Low Data), TotalSatellite should be 12");
            Assert::AreEqual((uint8_t)1u, FrameData.GSV.SatView[0].SatelliteID, L"Test (Low Data), SatView[0].SatelliteID should be 1");
            Assert::AreEqual((uint8_t)0u, FrameData.GSV.SatView[0].Elevation, L"Test (Low Data), SatView[0].Elevation should be 0");
            Assert::AreEqual((uint16_t)0u, FrameData.GSV.SatView[0].Azimuth, L"Test (Low Data), SatView[0].Azimuth should be 0");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSV.SatView[0].SNR, L"Test (Low Data), SatView[0].SNR should be 0xFF");
            Assert::AreEqual((uint8_t)2u, FrameData.GSV.SatView[1].SatelliteID, L"Test (Low Data), SatView[1].SatelliteID should be 2");
            Assert::AreEqual((uint8_t)0u, FrameData.GSV.SatView[1].Elevation, L"Test (Low Data), SatView[1].Elevation should be 0");
            Assert::AreEqual((uint16_t)0u, FrameData.GSV.SatView[1].Azimuth, L"Test (Low Data), SatView[1].Azimuth should be 0");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSV.SatView[1].SNR, L"Test (Low Data), SatView[1].SNR should be 0xFF");
            Assert::AreEqual((uint8_t)3u, FrameData.GSV.SatView[2].SatelliteID, L"Test (Low Data), SatView[2].SatelliteID should be 3");
            Assert::AreEqual((uint8_t)0u, FrameData.GSV.SatView[2].Elevation, L"Test (Low Data), SatView[2].Elevation should be 0");
            Assert::AreEqual((uint16_t)0u, FrameData.GSV.SatView[2].Azimuth, L"Test (Low Data), SatView[2].Azimuth should be 0");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSV.SatView[2].SNR, L"Test (Low Data), SatView[2].SNR should be 0xFF");
            Assert::AreEqual((uint8_t)4u, FrameData.GSV.SatView[3].SatelliteID, L"Test (Low Data), SatView[3].SatelliteID should be 4");
            Assert::AreEqual((uint8_t)0u, FrameData.GSV.SatView[3].Elevation, L"Test (Low Data), SatView[3].Elevation should be 0");
            Assert::AreEqual((uint16_t)0u, FrameData.GSV.SatView[3].Azimuth, L"Test (Low Data), SatView[3].Azimuth should be 0");
            Assert::AreEqual((uint8_t)0xFF, FrameData.GSV.SatView[3].SNR, L"Test (Low Data), SatView[3].SNR should be 0xFF");

            //=== Test (Full Data) ===========================================
            const char* const TEST_GSV_FULL_DATA_FRAME = "$GPGSV,3,1,10,05,53,303,30,02,30,239,24,13,21,272,23,30,78,155,19*77\r\n";
            for (size_t z = 0; z < strlen(TEST_GSV_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_GSV_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_GSV, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_GSV");
            Assert::AreEqual((uint8_t)3u, FrameData.GSV.TotalSentence, L"Test (Full Data), TotalSentence should be 3");
            Assert::AreEqual((uint8_t)1u, FrameData.GSV.SentenceNumber, L"Test (Full Data), SentenceNumber should be 1");
            Assert::AreEqual((uint8_t)10u, FrameData.GSV.TotalSatellite, L"Test (Full Data), TotalSatellite should be 10");
            Assert::AreEqual((uint8_t)5u, FrameData.GSV.SatView[0].SatelliteID, L"Test (Full Data), SatView[0].SatelliteID should be 5");
            Assert::AreEqual((uint8_t)53u, FrameData.GSV.SatView[0].Elevation, L"Test (Full Data), SatView[0].Elevation should be 53");
            Assert::AreEqual((uint16_t)303u, FrameData.GSV.SatView[0].Azimuth, L"Test (Full Data), SatView[0].Azimuth should be 303");
            Assert::AreEqual((uint8_t)30u, FrameData.GSV.SatView[0].SNR, L"Test (Full Data), SatView[0].SNR should be 30");
            Assert::AreEqual((uint8_t)2u, FrameData.GSV.SatView[1].SatelliteID, L"Test (Full Data), SatView[1].SatelliteID should be 2");
            Assert::AreEqual((uint8_t)30u, FrameData.GSV.SatView[1].Elevation, L"Test (Full Data), SatView[1].Elevation should be 30");
            Assert::AreEqual((uint16_t)239u, FrameData.GSV.SatView[1].Azimuth, L"Test (Full Data), SatView[1].Azimuth should be 239");
            Assert::AreEqual((uint8_t)24u, FrameData.GSV.SatView[1].SNR, L"Test (Full Data), SatView[1].SNR should be 24");
            Assert::AreEqual((uint8_t)13u, FrameData.GSV.SatView[2].SatelliteID, L"Test (Full Data), SatView[2].SatelliteID should be 13");
            Assert::AreEqual((uint8_t)21u, FrameData.GSV.SatView[2].Elevation, L"Test (Full Data), SatView[2].Elevation should be 21");
            Assert::AreEqual((uint16_t)272u, FrameData.GSV.SatView[2].Azimuth, L"Test (Full Data), SatView[2].Azimuth should be 272");
            Assert::AreEqual((uint8_t)23u, FrameData.GSV.SatView[2].SNR, L"Test (Full Data), SatView[2].SNR should be 23");
            Assert::AreEqual((uint8_t)30u, FrameData.GSV.SatView[3].SatelliteID, L"Test (Full Data), SatView[3].SatelliteID should be 30");
            Assert::AreEqual((uint8_t)78u, FrameData.GSV.SatView[3].Elevation, L"Test (Full Data), SatView[3].Elevation should be 78");
            Assert::AreEqual((uint16_t)155u, FrameData.GSV.SatView[3].Azimuth, L"Test (Full Data), SatView[3].Azimuth should be 155");
            Assert::AreEqual((uint8_t)19u, FrameData.GSV.SatView[3].SNR, L"Test (Full Data), SatView[3].SNR should be 19");
        }
#endif

#ifdef NMEA0183_DECODE_RMC
        TEST_METHOD(TestMethod_RMC)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Low Data) ============================================
            const char* const TEST_RMC_LOW_DATA_FRAME = "$GPRMC,,V,,,,,,,,,,N*53\r\n";
            for (size_t z = 0; z < strlen(TEST_RMC_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_RMC_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_RMC, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_RMC");
            Assert::AreEqual((uint8_t)0xFF, FrameData.RMC.Time.Hour, L"Test (Low Data), Time.Hour should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.RMC.Time.Minute, L"Test (Low Data), Time.Minute should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.RMC.Time.Second, L"Test (Low Data), Time.Second should be 0xFF");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.RMC.Time.MilliS, L"Test (Low Data), Time.MilliS should be 0xFFFF");
            Assert::AreEqual('V', FrameData.RMC.Status, L"Test (Low Data), Status should be 'V'");
            Assert::AreEqual(' ', FrameData.RMC.Latitude.Direction, L"Test (Low Data), Latitude.Direction should be ' '");
            Assert::AreEqual((uint8_t)0xFF, FrameData.RMC.Latitude.Degree, L"Test (Low Data), Latitude.Degree should be 0xFF");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.RMC.Latitude.Minute, L"Test (Low Data), Latitude.Minute should be 0xFFFFFFFF");
            Assert::AreEqual(' ', FrameData.RMC.Longitude.Direction, L"Test (Low Data), Longitude.Direction should be ' '");
            Assert::AreEqual((uint8_t)0xFF, FrameData.RMC.Longitude.Degree, L"Test (Low Data), Longitude.Degree should be 0xFF");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.RMC.Longitude.Minute, L"Test (Low Data), Latitude.Minute should be 0xFFFFFFFF");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.RMC.Speed, L"Test (Low Data), Speed should be 0xFFFFFFFF");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.RMC.Track, L"Test (Low Data), Track should be 0xFFFFFFFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.RMC.Date.Day, L"Test (Low Data), Date.Day should be 0xFF");
            Assert::AreEqual((uint8_t)0xFF, FrameData.RMC.Date.Month, L"Test (Low Data), Date.Month should be 0xFF");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.RMC.Date.Year, L"Test (Low Data), Date.Year should be 0xFFFF");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.RMC.Variation.Value, L"Test (Low Data), Variation.Value should be 0xFFFF");
            Assert::AreEqual(' ', FrameData.RMC.Variation.Direction, L"Test (Low Data), Variation.Direction should be ' '");
            Assert::AreEqual('N', FrameData.RMC.FAAmode, L"Test (Low Data), FAAmode should be 'N'");
            Assert::AreEqual(' ', FrameData.RMC.NavigationStatus, L"Test (Low Data), NavigationStatus should be ' '");

            //=== Test (Full Data) ============================================
            const char* const TEST_RMC_FULL_DATA_FRAME = "$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E,A,S*7A\r\n";
            for (size_t z = 0; z < strlen(TEST_RMC_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_RMC_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_RMC, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_RMC");
            Assert::AreEqual((uint8_t)22, FrameData.RMC.Time.Hour, L"Test (Full Data), Time.Hour should be 22");
            Assert::AreEqual((uint8_t)54, FrameData.RMC.Time.Minute, L"Test (Full Data), Time.Minute should be 54");
            Assert::AreEqual((uint8_t)46, FrameData.RMC.Time.Second, L"Test (Full Data), Time.Second should be 46");
            Assert::AreEqual((uint16_t)0xFFFF, FrameData.RMC.Time.MilliS, L"Test (Full Data), Time.MilliS should be 0xFFFF");
            Assert::AreEqual('A', FrameData.RMC.Status, L"Test (Full Data), Status should be 'A'");
            Assert::AreEqual('N', FrameData.RMC.Latitude.Direction, L"Test (Full Data), Latitude.Direction should be 'N'");
            Assert::AreEqual((uint8_t)49, FrameData.RMC.Latitude.Degree, L"Test (Full Data), Latitude.Degree should be 49");
            Assert::AreEqual(164500000u, FrameData.RMC.Latitude.Minute, L"Test (Full Data), Latitude.Minute should be 164500000");
            Assert::AreEqual('W', FrameData.RMC.Longitude.Direction, L"Test (Full Data), Longitude.Direction should be 'W'");
            Assert::AreEqual((uint8_t)123, FrameData.RMC.Longitude.Degree, L"Test (Full Data), Longitude.Degree should be 123");
            Assert::AreEqual(111200000u, FrameData.RMC.Longitude.Minute, L"Test (Full Data), Latitude.Minute should be 111200000");
            Assert::AreEqual(5000u, FrameData.RMC.Speed, L"Test (Full Data), Speed should be 5000");
            Assert::AreEqual(547000u, FrameData.RMC.Track, L"Test (Full Data), Track should be 547000");
            Assert::AreEqual((uint8_t)19, FrameData.RMC.Date.Day, L"Test (Full Data), Date.Day should be 19");
            Assert::AreEqual((uint8_t)11, FrameData.RMC.Date.Month, L"Test (Full Data), Date.Month should be 11");
            Assert::AreEqual((uint16_t)94, FrameData.RMC.Date.Year, L"Test (Full Data), Date.Year should be 94");
            Assert::AreEqual((uint16_t)2030, FrameData.RMC.Variation.Value, L"Test (Full Data), Variation.Value should be 2030");
            Assert::AreEqual('E', FrameData.RMC.Variation.Direction, L"Test (Full Data), Variation.Direction should be 'E'");
            Assert::AreEqual('A', FrameData.RMC.FAAmode, L"Test (Full Data), FAAmode should be 'A'");
            Assert::AreEqual('S', FrameData.RMC.NavigationStatus, L"Test (Full Data), NavigationStatus should be 'S'");
        }
#endif
    };
}