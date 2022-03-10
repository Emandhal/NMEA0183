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
    TEST_CLASS(Autopilot_Sentences)
    {
    public:

#ifdef NMEA0183_DECODE_APB
        TEST_METHOD(TestMethod_APB)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Full Data) ============================================
            const char* const TEST_APB_FULL_DATA_FRAME = "$AGAPB,A,A,0.10,R,N,V,V,011,M,DEST,011,M,011,M*2D\r\n";
            for (size_t z = 0; z < strlen(TEST_APB_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_APB_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('A', 'G'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_AG");
            Assert::AreEqual(NMEA0183_APB, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_APB");
            Assert::AreEqual('A', FrameData.APB.Status1, L"Test (Full Data), Status1 should be 'A'");
            Assert::AreEqual('A', FrameData.APB.Status2, L"Test (Full Data), Status2 should be 'A'");
            Assert::AreEqual((int32_t)1000, FrameData.APB.MagnitudeXTE, L"Test (Full Data), MagnitudeXTE should be 1000");
            Assert::AreEqual('R', FrameData.APB.DirectionSteer, L"Test (Full Data), DirectionSteer should be 'R'");
            Assert::AreEqual('N', FrameData.APB.XTEunit, L"Test (Full Data), XTEunit should be 'N'");
            Assert::AreEqual('V', FrameData.APB.ArrivalStatus, L"Test (Full Data), ArrivalStatus should be 'V'");
            Assert::AreEqual('V', FrameData.APB.PassedWaypoint, L"Test (Full Data), PassedWaypoint should be 'V'");
            Assert::AreEqual((uint16_t)1100u, FrameData.APB.BearingOriginToDest, L"Test (Full Data), BearingOriginToDest should be 1100");
            Assert::AreEqual('M', FrameData.APB.BearingOtoDunit, L"Test (Full Data), BearingOtoDunit should be 'M'");
            Assert::AreEqual(0, strncmp("DEST", &FrameData.APB.WaypointID[0], strlen(FrameData.TXT.TextMessage)), L"Test (Full Data), WaypointID should be 'DEST'");
            Assert::AreEqual((uint16_t)1100u, FrameData.APB.BearingCurPosToDest, L"Test (Full Data), BearingCurPosToDest should be 1100");
            Assert::AreEqual('M', FrameData.APB.BearingCPtoDunit, L"Test (Full Data), BearingCPtoDunit should be 'M'");
            Assert::AreEqual((uint16_t)1100u, FrameData.APB.HeadingToSteerToDest, L"Test (Full Data), HeadingToSteerToDest should be 1100");
            Assert::AreEqual('M', FrameData.APB.H2StoDunit, L"Test (Full Data), H2StoDunit should be 'M'");
            Assert::AreEqual(' ', FrameData.APB.FAAmode, L"Test (Full Data), FAAmode should be ' '");
        }
    };
#endif
}