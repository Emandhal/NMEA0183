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

#ifdef NMEA0183_DECODE_VTG
        TEST_METHOD(TestMethod_VTG)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_VTG_FULL_DATA_FRAME = "$GPVTG,220.86,T,,M,2.550,N,4.724,K,A*34\r\n";
            for (size_t z = 0; z < strlen(TEST_VTG_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_VTG_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual(NMEA0183_GP, FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_GP");
            Assert::AreEqual(NMEA0183_VTG, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_VTG");
            Assert::AreEqual((uint32_t)2208600u, FrameData.VTG.CourseTrue, L"Test (Full Data), CourseTrue should be 2208600");
            Assert::AreEqual((uint32_t)0xFFFFFFFF, FrameData.VTG.CourseMagnetic, L"Test (Full Data), CourseMagnetic should be 0xFFFFFFFF");
            Assert::AreEqual((uint32_t)25500u, FrameData.VTG.SpeedKnots, L"Test (Full Data), SpeedKnots should be 25500");
            Assert::AreEqual((uint32_t)47240u, FrameData.VTG.SpeedKmHr, L"Test (Full Data), SpeedKmHr should be 47240");
            Assert::AreEqual('A', FrameData.VTG.FAAmode, L"Test (Full Data), FAAmode should be 'A'");
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
            const char* const TEST_ZDA_FULL_DATA_FRAME = "$GPZDA,160012.71,11,03,2004,-1,00*7D\r\n";
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
            Assert::AreEqual((int8_t)-1, FrameData.ZDA.LocalZoneHour, L"Test (Full Data), LocalZoneHour should be -1");
            Assert::AreEqual((uint8_t)00u, FrameData.ZDA.LocalZoneMinute, L"Test (Full Data), LocalZoneMinute should be 0");
        }
#endif
    };
}