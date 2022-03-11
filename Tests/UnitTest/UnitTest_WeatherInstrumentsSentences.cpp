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
    TEST_CLASS(WeatherInstruments_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_MWV
        TEST_METHOD(TestMethod_MWV)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_MWV_FULL_DATA_FRAME = "$WIMWV,214.8,R,0.1,K,A*28\r\n";
            for (size_t z = 0; z < strlen(TEST_MWV_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_MWV_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('W', 'I'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_WI");
            Assert::AreEqual(NMEA0183_MWV, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_MWV");
            Assert::AreEqual((uint16_t)21480u, FrameData.MWV.WindAngle, L"Test (Full Data), WindAngle should be 21480");
            Assert::AreEqual('R', FrameData.MWV.Reference, L"Test (Full Data), Reference should be 'R'");
            Assert::AreEqual((uint16_t)10u, FrameData.MWV.WindSpeed, L"Test (Full Data), TotalSentence should be 10");
            Assert::AreEqual('K', FrameData.MWV.WindSpeedUnit, L"Test (Full Data), WindSpeedUnit should be 'K'");
            Assert::AreEqual('A', FrameData.MWV.Status, L"Test (Full Data), Status should be 'A'");
        }
#endif
    };
}