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
    TEST_CLASS(Communications_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_FSI
        TEST_METHOD(TestMethod_FSI)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_FSI_FULL_DATA_FRAME = "$CTFSI,020230,026140,m,5*11\r\n";
            for (size_t z = 0; z < strlen(TEST_FSI_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_FSI_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('C', 'T'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_CT");
            Assert::AreEqual(NMEA0183_FSI, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_FSI");
            Assert::AreEqual(20230000u, FrameData.FSI.TxFrequency, L"Test (Full Data), TxFrequency should be 20230000");
            Assert::AreEqual(26140000u, FrameData.FSI.RxFrequency, L"Test (Full Data), RxFrequency should be 26140000");
            Assert::AreEqual('m', FrameData.FSI.Mode, L"Test (Full Data), Mode should be 'm'");
            Assert::AreEqual('5', FrameData.FSI.PowerLevel, L"Test (Full Data), PowerLevel should be '5'");
        }
#endif
    };
}