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

#if defined(NMEA0183_DECODE_DBK)/* || defined(NMEA0183_DECODE_DBS)*/ || defined(NMEA0183_DECODE_DBT)
        TEST_METHOD(TestMethod_DBx)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

#  ifdef NMEA0183_DECODE_DBK
            //=== Test (Low Data) ============================================
            const char* const TEST_DBx_LOW_DATA_FRAME = "$SDDBK,,f,22.5,M,,F*2C\r\n";
            for (size_t z = 0; z < strlen(TEST_DBx_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_DBx_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('S', 'D'), FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_SD");
            Assert::AreEqual(NMEA0183_DBK, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_DBK");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.DBK.DepthFeet, L"Test (Low Data), DepthFeet should be 0xFFFFFFFF");
            Assert::AreEqual(22500u, FrameData.DBK.DepthMeter, L"Test (Low Data), DepthMeter should be 22500");
            Assert::AreEqual(0xFFFFFFFFu, FrameData.DBK.DepthFathom, L"Test (Low Data), DepthFathom should be 0xFFFFFFFF");
#  endif
#  ifdef NMEA0183_DECODE_DBT
            //=== Test (Full Data) ============================================
            const char* const TEST_DBx_FULL_DATA_FRAME = "$SDDBT,7.8,f,2.4,M,1.3,F*0D\r\n";
            for (size_t z = 0; z < strlen(TEST_DBx_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_DBx_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('S', 'D'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_SD");
            Assert::AreEqual(NMEA0183_DBT, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_DBT");
            Assert::AreEqual(7800u, FrameData.DBT.DepthFeet, L"Test (Full Data), DepthFeet should be 7800");
            Assert::AreEqual(2400u, FrameData.DBT.DepthMeter, L"Test (Full Data), DepthMeter should be 2400");
            Assert::AreEqual(1300u, FrameData.DBT.DepthFathom, L"Test (Full Data), DepthFathom should be 1300");
#  endif
        }

#ifdef NMEA0183_DECODE_DPT
        TEST_METHOD(TestMethod_DPT)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //=== Test (Lowest Data) ============================================
            const char* const TEST_DPT_LOWEST_DATA_FRAME = "$SDDPT,4.1,*7C\r\n";
            for (size_t z = 0; z < strlen(TEST_DPT_LOWEST_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_DPT_LOWEST_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Lowest Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Lowest Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Lowest Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Lowest Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('S', 'D'), FrameData.TalkerID, L"Test (Lowest Data), TalkerID should be NMEA0183_SD");
            Assert::AreEqual(NMEA0183_DPT, FrameData.SentenceID, L"Test (Lowest Data), SentenceID should be NMEA0183_DPT");
            Assert::AreEqual((uint32_t)4100u, FrameData.DPT.DepthMeter, L"Test (Lowest Data), DepthMeter should be 4100");
            Assert::AreEqual((int16_t)0, FrameData.DPT.OffsetTrans, L"Test (Lowest Data), OffsetTrans should be 0");
            Assert::AreEqual((uint32_t)0xFFFFFFFFu, FrameData.DPT.RangeScale, L"Test (Lowest Data), RangeScale should be 0xFFFFFFFF");

            //=== Test (Low Data) ============================================
            const char* const TEST_DPT_LOW_DATA_FRAME = "$SDDPT,2.4,,*53\r\n";
            for (size_t z = 0; z < strlen(TEST_DPT_LOW_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_DPT_LOW_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Low Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Low Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Low Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Low Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('S', 'D'), FrameData.TalkerID, L"Test (Low Data), TalkerID should be NMEA0183_SD");
            Assert::AreEqual(NMEA0183_DPT, FrameData.SentenceID, L"Test (Low Data), SentenceID should be NMEA0183_DPT");
            Assert::AreEqual((uint32_t)2400u, FrameData.DPT.DepthMeter, L"Test (Low Data), DepthMeter should be 2400");
            Assert::AreEqual((int16_t)0, FrameData.DPT.OffsetTrans, L"Test (Low Data), OffsetTrans should be 0");
            Assert::AreEqual((uint32_t)0xFFFFFFFFu, FrameData.DPT.RangeScale, L"Test (Low Data), RangeScale should be 0xFFFFFFFF");

            //=== Test (Full Data) ============================================
            const char* const TEST_DPT_FULL_DATA_FRAME = "$SDDPT,76.1,,100*54\r\n";
            for (size_t z = 0; z < strlen(TEST_DPT_FULL_DATA_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_DPT_FULL_DATA_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Full Data), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR_OK, LastError, L"Test (Full Data), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Full Data), state should be NMEA0183_WAIT_START");
            //--- Test data ---
            Assert::AreEqual(true, FrameData.ParseIsValid, L"Test (Full Data), ParseIsValid should be true");
            Assert::AreEqual((eNMEA0183_TalkerID)NMEA0183_TALKER_ID('S', 'D'), FrameData.TalkerID, L"Test (Full Data), TalkerID should be NMEA0183_SD");
            Assert::AreEqual(NMEA0183_DPT, FrameData.SentenceID, L"Test (Full Data), SentenceID should be NMEA0183_DPT");
            Assert::AreEqual(76100u, FrameData.DPT.DepthMeter, L"Test (Full Data), DepthMeter should be 76100");
            Assert::AreEqual((int16_t)0, FrameData.DPT.OffsetTrans, L"Test (Full Data), OffsetTrans should be 0");
            Assert::AreEqual(100u, FrameData.DPT.RangeScale, L"Test (Full Data), RangeScale should be 100");
        }
#endif
#endif
    };
}