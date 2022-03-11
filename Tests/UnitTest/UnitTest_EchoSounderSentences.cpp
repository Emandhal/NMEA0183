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
    TEST_CLASS(EchoSounder_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_MTW
        TEST_METHOD(TestMethod_MTW)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_MTW_FULL_DATA_FRAME = "$SDMTW,26.8,C*08\r\n";
            for (size_t z = 0; z < strlen(TEST_MTW_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_MTW_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('S', 'D'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_SD");
            Assert::AreEqual(NMEA0183_MTW, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_MTW");
            Assert::AreEqual((int16_t)2680, FrameData.MTW.WaterTemp, L"Test (Full Data), Speed should be 2680");
        }
#endif
    };
}