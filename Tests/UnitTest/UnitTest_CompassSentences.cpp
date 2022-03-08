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
    TEST_CLASS(Compass_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_VTG
        TEST_METHOD(TestMethod_VTG)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_VTG_FULL_DATA_FRAME = "$HCVTG,220.86,T,,M,2.550,N,4.724,K,A*28\r\n";
            for (size_t z = 0; z < strlen(TEST_VTG_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_VTG_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('H', 'C'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_HC");
            Assert::AreEqual(NMEA0183_VTG, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_VTG");
            Assert::AreEqual((uint32_t)2208600u, FrameData.VTG.CourseTrue, L"Test (Full Data), CourseTrue should be 2208600");
            Assert::AreEqual((uint32_t)0xFFFFFFFF, FrameData.VTG.CourseMagnetic, L"Test (Full Data), CourseMagnetic should be 0xFFFFFFFF");
            Assert::AreEqual((uint32_t)25500u, FrameData.VTG.SpeedKnots, L"Test (Full Data), SpeedKnots should be 25500");
            Assert::AreEqual((uint32_t)47240u, FrameData.VTG.SpeedKmHr, L"Test (Full Data), SpeedKmHr should be 47240");
            Assert::AreEqual('A', FrameData.VTG.FAAmode, L"Test (Full Data), FAAmode should be 'A'");
        }
#endif
    };
}