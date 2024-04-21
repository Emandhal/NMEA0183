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
}   }   }





namespace NMEA0183test
{
    TEST_CLASS(Other_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_TXT
        TEST_METHOD(TestMethod_TXT)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_TXT_FULL_DATA_FRAME = "$GPTXT,01,01,25,DR MODE - ANTENNA FAULT^21*38\r\n";
            for (size_t z = 0; z < strlen(TEST_TXT_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_TXT_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_TXT, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_TXT");
            Assert::AreEqual((uint8_t)1u, FrameData.TXT.TotalSentence, L"Test (Full Data), TotalSentence should be 1");
            Assert::AreEqual((uint8_t)1u, FrameData.TXT.SentenceNumber, L"Test (Full Data), SentenceNumber should be 1");
            Assert::AreEqual((uint8_t)25u, FrameData.TXT.TextIdentifier, L"Test (Full Data), TextIdentifier should be 25");
            Assert::AreEqual(0, strncmp("DR MODE - ANTENNA FAULT!", &FrameData.TXT.TextMessage[0], strlen(FrameData.TXT.TextMessage)), L"Test (Full Data), TextMessage should be 'DR MODE - ANTENNA FAULT!'");
        }
#endif

#ifdef NMEA0183_DECODE_VHW
        TEST_METHOD(TestMethod_VHW)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_VHW_FULL_DATA_FRAME = "$IIVHW,245.1,T,243.8,M,047.01,N,087.04,K*53\r\n";
            for (size_t z = 0; z < strlen(TEST_VHW_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_VHW_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('I', 'I'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_II");
            Assert::AreEqual(NMEA0183_VHW, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_VHW");
            Assert::AreEqual((uint32_t)2451000u, FrameData.VHW.HeadingTrue, L"Test (Full Data), CourseTrue should be 2451000");
            Assert::AreEqual((uint32_t)2438000u, FrameData.VHW.HeadingMagnetic, L"Test (Full Data), CourseMagnetic should be 2438000");
            Assert::AreEqual((uint32_t)470100u, FrameData.VHW.SpeedKnots, L"Test (Full Data), SpeedKnots should be 470100");
            Assert::AreEqual((uint32_t)870400u, FrameData.VHW.SpeedKmHr, L"Test (Full Data), SpeedKmHr should be 870400");
        }
#endif

#ifdef NMEA0183_DECODE_ZDA
        TEST_METHOD(TestMethod_ZDA)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_ZDA_FULL_DATA_FRAME = "$GPZDA,160012.71,11,03,2004,3,00*52\r\n";
            for (size_t z = 0; z < strlen(TEST_ZDA_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_ZDA_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_ZDA, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_ZDA");
            Assert::AreEqual((uint8_t)16u, FrameData.ZDA.Time.Hour, L"Test (Full Data), Time.Hour should be 16");
            Assert::AreEqual((uint8_t)00u, FrameData.ZDA.Time.Minute, L"Test (Full Data), Time.Minute should be 0");
            Assert::AreEqual((uint8_t)12u, FrameData.ZDA.Time.Second, L"Test (Full Data), Time.Second should be 12");
            Assert::AreEqual((uint16_t)710u, FrameData.ZDA.Time.MilliS, L"Test (Full Data), Time.MilliS should be 710");
            Assert::AreEqual((uint8_t)11u, FrameData.ZDA.Date.Day, L"Test (Full Data), Date.Day should be 11");
            Assert::AreEqual((uint8_t)03u, FrameData.ZDA.Date.Month, L"Test (Full Data), Date.Month should be 3");
            Assert::AreEqual((uint16_t)2004u, FrameData.ZDA.Date.Year, L"Test (Full Data), Date.Year should be 2004");
            Assert::AreEqual((int8_t)3, FrameData.ZDA.LocalZoneHour, L"Test (Full Data), LocalZoneHour should be 3");
            Assert::AreEqual((int8_t)00, FrameData.ZDA.LocalZoneMinute, L"Test (Full Data), LocalZoneMinute should be 0");

            //=== Test -10:00 ============================================
            const char* const TEST_ZDA_MINUS10_DATA_FRAME = "$GPZDA,160012.71,11,03,2004,-10,00*4D\r\n";
            for (size_t z = 0; z < strlen(TEST_ZDA_MINUS10_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_ZDA_MINUS10_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test -10:00, state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test -10:00, error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test -10:00, state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test -10:00, ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test -10:00, TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_ZDA, FrameData.SentenceID, L"Test -10:00, SentenceID should be NMEA0183_ZDA");
            Assert::AreEqual((uint8_t)16u, FrameData.ZDA.Time.Hour, L"Test -10:00, Time.Hour should be 16");
            Assert::AreEqual((uint8_t)00u, FrameData.ZDA.Time.Minute, L"Test -10:00, Time.Minute should be 0");
            Assert::AreEqual((uint8_t)12u, FrameData.ZDA.Time.Second, L"Test -10:00, Time.Second should be 12");
            Assert::AreEqual((uint16_t)710u, FrameData.ZDA.Time.MilliS, L"Test -10:00, Time.MilliS should be 710");
            Assert::AreEqual((uint8_t)11u, FrameData.ZDA.Date.Day, L"Test -10:00, Date.Day should be 11");
            Assert::AreEqual((uint8_t)03u, FrameData.ZDA.Date.Month, L"Test -10:00, Date.Month should be 3");
            Assert::AreEqual((uint16_t)2004u, FrameData.ZDA.Date.Year, L"Test -10:00, Date.Year should be 2004");
            Assert::AreEqual((int8_t)-10, FrameData.ZDA.LocalZoneHour, L"Test -10:00, LocalZoneHour should be -10");
            Assert::AreEqual((int8_t)00, FrameData.ZDA.LocalZoneMinute, L"Test -10:00, LocalZoneMinute should be 0");

            //=== Test -00:30 =====================================
            const char* const TEST_ZDA_MINUS00_DATA_FRAME = "$GPZDA,160012.71,11,03,2004,-00,30*4F\r\n";
            for (size_t z = 0; z < strlen(TEST_ZDA_MINUS00_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_ZDA_MINUS00_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test -00:30, state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test -00:30, error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test -00:30, state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test -00:30, ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test -00:30, TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_ZDA, FrameData.SentenceID, L"Test -00:30, SentenceID should be NMEA0183_ZDA");
            Assert::AreEqual((uint8_t)16u, FrameData.ZDA.Time.Hour, L"Test -00:30, Time.Hour should be 16");
            Assert::AreEqual((uint8_t)00u, FrameData.ZDA.Time.Minute, L"Test -00:30, Time.Minute should be 0");
            Assert::AreEqual((uint8_t)12u, FrameData.ZDA.Time.Second, L"Test -00:30, Time.Second should be 12");
            Assert::AreEqual((uint16_t)710u, FrameData.ZDA.Time.MilliS, L"Test -00:30, Time.MilliS should be 710");
            Assert::AreEqual((uint8_t)11u, FrameData.ZDA.Date.Day, L"Test -00:30, Date.Day should be 11");
            Assert::AreEqual((uint8_t)03u, FrameData.ZDA.Date.Month, L"Test -00:30, Date.Month should be 3");
            Assert::AreEqual((uint16_t)2004u, FrameData.ZDA.Date.Year, L"Test -00:30, Date.Year should be 2004");
            Assert::AreEqual((int8_t)0, FrameData.ZDA.LocalZoneHour, L"Test -00:30, LocalZoneHour should be 0");
            Assert::AreEqual((int8_t)-30, FrameData.ZDA.LocalZoneMinute, L"Test -00:30, LocalZoneMinute should be -30");
        }
#endif
    };
}