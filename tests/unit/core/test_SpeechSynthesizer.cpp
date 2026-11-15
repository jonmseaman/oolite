/*	test_SpeechSynthesizer.cpp
        Unit tests for oo::SpeechSynthesizer (src/Core/SpeechSynthesizer.h).

        The class keeps espeak-ng behind one game class. Its voice list is a
   copy of espeak's, so the tests plant names through the test seam and pin
   the index arithmetic the options screen relies on: next and previous wrap,
   an index out of range names "-", an unknown name numbers UINT_MAX, and an
   empty list keeps the unsigned wrap-around (the previous voice of 0 is
   UINT_MAX). No espeak call is made.
        Run: tests/run.sh unit
*/

#include <climits>
#include <cstdint>
#include <string>

#include "OOFoundation.h"
#include "SpeechSynthesizer.h"
#include "oo_test.hpp"

// main.cpp defines the debug flags, and the test has its own main (it links
// the whole game, for the description lookup setVoice makes).
#ifndef NDEBUG
std::uint32_t gDebugFlags = 0;
#endif

OO_TEST(voiceListArithmetic) {
  oo::SpeechSynthesizer& speech = oo::SpeechSynthesizer::shared();
  speech.setVoiceNamesForTesting({"zero", "one", "two"});

  OO_CHECK(speech.nextVoice(0) == 1 && speech.nextVoice(2) == 0);
  OO_CHECK(speech.prevVoice(1) == 0 && speech.prevVoice(0) == 2);
  OO_CHECK(speech.voiceName(1) == "one" && speech.voiceName(3) == "-");
  OO_CHECK(speech.voiceNumber("two") == 2 &&
           speech.voiceNumber("nine") == UINT_MAX);
  // An index out of range selects nothing and is handed back.
  OO_CHECK(speech.setVoice(3, true) == 3);

  speech.setVoiceNamesForTesting({});
}

OO_TEST(emptyVoiceList) {
  oo::SpeechSynthesizer& speech = oo::SpeechSynthesizer::shared();
  speech.setVoiceNamesForTesting({});

  OO_CHECK(speech.nextVoice(0) == 0);
  OO_CHECK(speech.prevVoice(0) == UINT_MAX);
  OO_CHECK(speech.voiceName(0) == "-");
  OO_CHECK(speech.voiceNumber("zero") == UINT_MAX);
}

OO_TEST(sharedIsOneObject) {
  OO_CHECK(&oo::SpeechSynthesizer::shared() ==
           &oo::SpeechSynthesizer::shared());
}

// The pronunciation guide's third column is espeak's own replacement.
OO_TEST(pronunciationColumn) {
  const oo::SpeechSynthesizer& speech = oo::SpeechSynthesizer::shared();
  OO_CHECK(speech.pronunciationColumn(2) == 1);
  OO_CHECK(speech.pronunciationColumn(3) == (OOLITE_ESPEAK ? 2u : 1u));
}

OO_TEST_MAIN()
