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

#include <gtest/gtest.h>

#include "Foundation.h"
#include "SpeechSynthesizer.h"

// main.cpp defines the debug flags, and the test has its own main (it links
// the whole game, for the description lookup setVoice makes).
#ifndef NDEBUG
std::uint32_t gDebugFlags = 0;
#endif

TEST(SpeechSynthesizer, voiceListArithmetic) {
  oo::SpeechSynthesizer& speech = oo::SpeechSynthesizer::shared();
  speech.setVoiceNamesForTesting({"zero", "one", "two"});

  EXPECT_TRUE(speech.nextVoice(0) == 1 && speech.nextVoice(2) == 0);
  EXPECT_TRUE(speech.prevVoice(1) == 0 && speech.prevVoice(0) == 2);
  EXPECT_TRUE(speech.voiceName(1) == "one" && speech.voiceName(3) == "-");
  EXPECT_TRUE(speech.voiceNumber("two") == 2 &&
           speech.voiceNumber("nine") == UINT_MAX);
  // An index out of range selects nothing and is handed back.
  EXPECT_TRUE(speech.setVoice(3, true) == 3);

  speech.setVoiceNamesForTesting({});
}

TEST(SpeechSynthesizer, emptyVoiceList) {
  oo::SpeechSynthesizer& speech = oo::SpeechSynthesizer::shared();
  speech.setVoiceNamesForTesting({});

  EXPECT_TRUE(speech.nextVoice(0) == 0);
  EXPECT_TRUE(speech.prevVoice(0) == UINT_MAX);
  EXPECT_TRUE(speech.voiceName(0) == "-");
  EXPECT_TRUE(speech.voiceNumber("zero") == UINT_MAX);
}

TEST(SpeechSynthesizer, sharedIsOneObject) {
  EXPECT_TRUE(&oo::SpeechSynthesizer::shared() ==
           &oo::SpeechSynthesizer::shared());
}

// The pronunciation guide's third column is espeak's own replacement.
TEST(SpeechSynthesizer, pronunciationColumn) {
  const oo::SpeechSynthesizer& speech = oo::SpeechSynthesizer::shared();
  EXPECT_TRUE(speech.pronunciationColumn(2) == 1);
  EXPECT_TRUE(speech.pronunciationColumn(3) == (OOLITE_ESPEAK ? 2u : 1u));
}
