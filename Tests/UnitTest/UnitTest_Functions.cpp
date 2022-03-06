#include "pch.h"
#include "CppUnitTest.h"
#include <cstdint>
#include <windows.h>
#include <WinUser.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework
{
    template<> inline std::wstring ToString<eERRORRESULT>(const eERRORRESULT& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_State>(const eNMEA0183_State& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_TalkerID>(const eNMEA0183_TalkerID& t) { RETURN_WIDE_STRING(t); }
    template<> inline std::wstring ToString<eNMEA0183_SentencesID>(const eNMEA0183_SentencesID& t) { RETURN_WIDE_STRING(t); }
}   }   }





namespace NMEA0183test
{
    TEST_CLASS(WraperClassTest)
    {
    public:

        TEST_METHOD(TestMethod_AddReceivedCharacter)
        {
            NMEA0183decoder NMEA;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //--- Test (Device create) ---
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Device create), state should be NMEA0183_WAIT_START");

            //--- Test (Add chars in wait start state) ---
            for (size_t z = 0; z < (NMEA0183_FRAME_BUFFER_SIZE + 1); ++z)
            {
                LastError = NMEA.AddReceivedCharacter('A');
                Assert::AreEqual(ERR_OK, LastError, L"Test (Add chars in wait start state), error should be ERR_OK");
                CurrentState = NMEA.GetDecoderState();
                Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Add chars in wait start state), state should be NMEA0183_WAIT_START");
            }
            LastError = NMEA.AddReceivedCharacter('\r');
            Assert::AreEqual(ERR_OK, LastError, L"Test (Add chars in wait start state), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Add chars in wait start state), state should be NMEA0183_WAIT_START");
            LastError = NMEA.AddReceivedCharacter('\n');
            Assert::AreEqual(ERR_OK, LastError, L"Test (Add chars in wait start state), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Add chars in wait start state), state should be NMEA0183_WAIT_START");

            //--- Test (Add start of frame char) ---
            LastError = NMEA.AddReceivedCharacter('$');
            Assert::AreEqual(ERR_OK, LastError, L"Test (Add start of frame char), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_ACCUMULATE, CurrentState, L"Test (Add start of frame char), state should be NMEA0183_ACCUMULATE");

            //--- Test (Add char data after start of frame) ---
            for (size_t z = 0; z < (NMEA0183_FRAME_BUFFER_SIZE - 1); ++z)
            {
                LastError = NMEA.AddReceivedCharacter('A');
                Assert::AreEqual(ERR_OK, LastError, L"Test (Add char data after start of frame), error should be ERR_OK");
                CurrentState = NMEA.GetDecoderState();
                Assert::AreEqual(NMEA0183_ACCUMULATE, CurrentState, L"Test (Add char data after start of frame), state should be NMEA0183_ACCUMULATE");
            }
            LastError = NMEA.AddReceivedCharacter('A');
            Assert::AreEqual(ERR__BUFFER_FULL, LastError, L"Test (Add char data after start of frame), error should be ERR__BUFFER_FULL");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_ACCUMULATE, CurrentState, L"Test (Add char data after start of frame), state should be NMEA0183_ACCUMULATE");

            //--- Test (Add full frame #1) ---
            const char* const TEST_FRAME = "$TestFrame*00";
            for (size_t z = 0; z < strlen(TEST_FRAME); ++z)
            {
                LastError = NMEA.AddReceivedCharacter(TEST_FRAME[z]);
                Assert::AreEqual(ERR_OK, LastError, L"Test (Add full frame #1), error should be ERR_OK");
                CurrentState = NMEA.GetDecoderState();
                Assert::AreEqual(NMEA0183_ACCUMULATE, CurrentState, L"Test (Add full frame #1), state should be NMEA0183_ACCUMULATE");
            }
            LastError = NMEA.AddReceivedCharacter('\r');
            Assert::AreEqual(ERR_OK, LastError, L"Test (Add full frame #1), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Add full frame #1), state should be NMEA0183_TO_PROCESS");

            //--- Test (Add start before process) ---
            LastError = NMEA.AddReceivedCharacter('$');
            Assert::AreEqual(ERR__BUFFER_OVERRIDE, LastError, L"Test (Add start before process), error should be ERR__BUFFER_OVERRIDE");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_ACCUMULATE, CurrentState, L"Test (Add start before process), state should be NMEA0183_ACCUMULATE");

            //--- Test (Add full frame #2) ---
            for (size_t z = 0; z < strlen(TEST_FRAME); ++z)
            {
                LastError = NMEA.AddReceivedCharacter(TEST_FRAME[z]);
                Assert::AreEqual(ERR_OK, LastError, L"Test (Add full frame #2), error should be ERR_OK");
                CurrentState = NMEA.GetDecoderState();
                Assert::AreEqual(NMEA0183_ACCUMULATE, CurrentState, L"Test (Add full frame #2), state should be NMEA0183_ACCUMULATE");
            }
            LastError = NMEA.AddReceivedCharacter('\n');
            Assert::AreEqual(ERR_OK, LastError, L"Test (Add full frame #2), error should be ERR_OK");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Add full frame #2), state should be NMEA0183_TO_PROCESS");
        }

        TEST_METHOD(TestMethod_ProcessFrame)
        {
            NMEA0183decoder NMEA;
            NMEA0183_DecodedData FrameData;
            eNMEA0183_State CurrentState;
            eERRORRESULT LastError = ERR_OK;

            //--- Test (CRC error) ---
            const char* const TEST_BAD_CRC_FRAME = "$TestFrame*FF\r\n";
            for (size_t z = 0; z < strlen(TEST_BAD_CRC_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_BAD_CRC_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (CRC error), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR__CRC_ERROR, LastError, L"Test (CRC error), error should be ERR__CRC_ERROR");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (CRC error), state should be NMEA0183_WAIT_START");
            Assert::AreEqual(false, FrameData.ParseIsValid, L"Test (CRC error), ParseIsValid should be false");
            
            //--- Test (Unknown frame) ---
            const char* const TEST_UNKNOWN_FRAME = "$XXZZZ,data,00*4A\r\n";
            for (size_t z = 0; z < strlen(TEST_UNKNOWN_FRAME); ++z) (void)NMEA.AddReceivedCharacter(TEST_UNKNOWN_FRAME[z]);
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_TO_PROCESS, CurrentState, L"Test (Unknown frame), state should be NMEA0183_TO_PROCESS");
            LastError = NMEA.ProcessFrame(&FrameData);
            Assert::AreEqual(ERR__UNKNOWN_ELEMENT, LastError, L"Test (Unknown frame), error should be ERR__UNKNOWN_ELEMENT");
            CurrentState = NMEA.GetDecoderState();
            Assert::AreEqual(NMEA0183_WAIT_START, CurrentState, L"Test (Unknown frame), state should be NMEA0183_WAIT_START");
            Assert::AreEqual(false, FrameData.ParseIsValid, L"Test (Unknown frame), ParseIsValid should be false");
            Assert::AreEqual((eNMEA0183_TalkerID)0x5858, FrameData.TalkerID, L"Test (Unknown frame), TalkerID should be 0x5858");
            Assert::AreEqual(NMEA0183_UNKNOWN, FrameData.SentenceID, L"Test (Unknown frame), SentenceID should be NMEA0183_UNKNOWN");
            Assert::AreEqual(0, strncmp(&TEST_UNKNOWN_FRAME[0], &FrameData.Frame[0], strlen(TEST_UNKNOWN_FRAME) - 2), L"Test (Unknown frame), FrameData.Frame should be TEST_UNKNOWN_FRAME");
        }
    };



#ifdef NMEA0183_FLOAT_BASED_TOOLS
    TEST_CLASS(ToolsClassTest)
    {

    public:
        TEST_METHOD(TestMethod_CoordinateToDegree)
        {
            double Dd;
            NMEA0183_Coordinate Coord;
            eERRORRESULT LastError;

            Coord.Direction = 'S';
            Coord.Degree    = 61;
            Coord.Minute    = 77880000;
            LastError = NMEA0183_CoordinateToDegree(&Coord, &Dd);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual(-61.1298, Dd, 0.0000001, L"Test, Dd should be -61.1298");
        }

        TEST_METHOD(TestMethod_CoordinateToDegreeMinute)
        {
            NMEA0183_Coordinate Coord;
            NMEA0183_DegreeMinute DMm;
            eERRORRESULT LastError;

            Coord.Direction = 'S';
            Coord.Degree = 61;
            Coord.Minute = 77880000;
            LastError = NMEA0183_CoordinateToDegreeMinute(&Coord, &DMm);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual((int16_t)-61, DMm.Degree, L"Test, DMm.Degree should be -61");
            Assert::AreEqual(7.788, DMm.Minute, 0.0000001, L"Test, DMm.Minute should be 7.788");
        }

        TEST_METHOD(TestMethod_CoordinateToDegreeMinuteSeconds)
        {
            NMEA0183_Coordinate Coord;
            NMEA0183_DegreeMinuteSeconds DMS;
            eERRORRESULT LastError;

            Coord.Direction = 'S';
            Coord.Degree = 61;
            Coord.Minute = 77880000;
            LastError = NMEA0183_CoordinateToDegreeMinuteSeconds(&Coord, &DMS);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual((int16_t)-61, DMS.Degree, L"Test, DMS.Degree should be -61");
            Assert::AreEqual((uint8_t)7, DMS.Minute, L"Test, DMS.Minute should be 7");
            Assert::AreEqual(47.28, DMS.Seconds, 0.0000001, L"Test, DMS.Seconds should be 47.28");
        }

        //-----------------------------------------------------------------------------

        TEST_METHOD(TestMethod_DegreeToDegreeMinute) // Dd = 61.1298 => DM.m = 61°7.788'
        {
            double Dd;
            NMEA0183_DegreeMinute DMm;
            eERRORRESULT LastError;

            Dd = 61.1298;
            LastError = NMEA0183_DegreeToDegreeMinute(Dd, &DMm);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual((int16_t)61, DMm.Degree, L"Test, DMm.Degree should be 61");
            Assert::AreEqual(7.788, DMm.Minute, 0.0000001, L"Test, DMm.Minute should be 7.788");
        }

        TEST_METHOD(TestMethod_DegreeToDegreeMinuteSeconds) // Dd = 61.1298 => DMS.s = 61°7'47.28"
        {
            NMEA0183_DegreeMinuteSeconds DMS;
            double Dd;
            eERRORRESULT LastError;

            Dd = 61.1298;
            LastError = NMEA0183_DegreeToDegreeMinuteSeconds(Dd, &DMS);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual((int16_t)61, DMS.Degree, L"Test, DMS.Degree should be 61");
            Assert::AreEqual((uint8_t)7, DMS.Minute, L"Test, DMS.Minute should be 7");
            Assert::AreEqual(47.28, DMS.Seconds, 0.0000001, L"Test, DMS.Seconds should be 47.28");
        }

        TEST_METHOD(TestMethod_DegreeMinuteToDegree) // DM.m = 61°7.788' => Dd = 61.1298
        {
            double Dd;
            NMEA0183_DegreeMinute DMm;
            eERRORRESULT LastError;

            DMm.Degree = 61;
            DMm.Minute = 7.788;
            LastError = NMEA0183_DegreeMinuteToDegree(&DMm, &Dd);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual(61.1298, Dd, 0.0000001, L"Test, Dd should be 61.1298");
        }

        TEST_METHOD(TestMethod_DegreeMinuteToDegreeMinuteSeconds) // DM.m = 61°7.788' => DMS.s = 61°7'47.28"
        {
            NMEA0183_DegreeMinuteSeconds DMS;
            NMEA0183_DegreeMinute DMm;
            eERRORRESULT LastError;

            DMm.Degree = 61;
            DMm.Minute = 7.788;
            LastError = NMEA0183_DegreeMinuteToDegreeMinuteSeconds(&DMm, &DMS);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual((int16_t)61, DMS.Degree, L"Test, DMS.Degree should be 61");
            Assert::AreEqual((uint8_t)7, DMS.Minute, L"Test, DMS.Minute should be 7");
            Assert::AreEqual(47.28, DMS.Seconds, 0.0000001, L"Test, DMS.Seconds should be 47.28");
        }

        TEST_METHOD(TestMethod_DegreeMinuteSecondsToDegree) // DMS.s = 61°7'47.28" => Dd = 61.1298
        {
            NMEA0183_DegreeMinuteSeconds DMS;
            double Dd;
            eERRORRESULT LastError;

            DMS.Degree = 61;
            DMS.Minute = 07;
            DMS.Seconds = 47.28;
            LastError = NMEA0183_DegreeMinuteSecondsToDegree(&DMS, &Dd);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual(61.1298, Dd, 0.0000001, L"Test, Dd should be 61.1298");
        }

        TEST_METHOD(TestMethod_DegreeMinuteSecondsToDegreeMinute) // DMS.s = 61°7'47.28" => DM.m = 61°7.788'
        {
            NMEA0183_DegreeMinuteSeconds DMS;
            NMEA0183_DegreeMinute DMm;
            eERRORRESULT LastError;

            DMS.Degree  = 61;
            DMS.Minute  = 07;
            DMS.Seconds = 47.28;
            LastError = NMEA0183_DegreeMinuteSecondsToDegreeMinute(&DMS, &DMm);
            Assert::AreEqual(ERR_OK, LastError, L"Test, error should be ERR_OK");
            //--- Test data ---
            Assert::AreEqual((int16_t)61, DMm.Degree, L"Test, DMm.Degree should be 61");
            Assert::AreEqual(7.788, DMm.Minute, 0.0000001, L"Test, DMm.Minute should be 7.788");
        }
    };
#endif
}