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
    TEST_CLASS(Heading_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_HDG
        TEST_METHOD(TestMethod_HDG)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_HDG_FULL_DATA_FRAME = "$HCHDG,98.3,0.0,E,12.6,W*57\r\n";
            for (size_t z = 0; z < strlen(TEST_HDG_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_HDG_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('H', 'C'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_HC");
            Assert::AreEqual(NMEA0183_HDG, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_HDG");
            Assert::AreEqual((uint16_t)9830u, FrameData.HDG.Heading, L"Test (Full Data), Speed should be 9830");
            Assert::AreEqual((uint16_t)0, FrameData.HDG.Deviation.Value, L"Test (Full Data), Deviation.Value should be 0");
            Assert::AreEqual('E', FrameData.HDG.Deviation.Direction, L"Test (Full Data), Deviation.Direction should be 'E'");
            Assert::AreEqual((uint16_t)1260, FrameData.HDG.Variation.Value, L"Test (Full Data), Variation.Value should be 1260");
            Assert::AreEqual('W', FrameData.HDG.Variation.Direction, L"Test (Full Data), Variation.Direction should be 'W'");
        }
#endif
    };
}