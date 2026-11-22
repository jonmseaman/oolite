/*	test_OOShaderUniformMethodType.mm
        Unit tests for src/Core/Materials/OOShaderUniformMethodType.h.

        The shader uniform member tables give the type of a bound member's
   return type by its value type (oo::ShaderUniformTypeOf(), from the kind
   oo::kValueType<> gives the type). This pins the type each return type is
   given (the ones a uniform has, and those it has not): typesFromEncodings,
   encodingsTheTableHasNot. Run: tests/run.sh unit
*/

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "RegExpMatcher.h"
#include "ShaderUniformMethodType.h"
#include "ValueType.h"
#include "oofnd/PList.hpp"

/*	OOMatrix.mm (the test's matrices) links the extension manager, whose
   collaborators are stubbed as in test_OOOpenGLExtensionManager; none of them
   runs here.
*/
#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
std::vector<std::string> ResourceManager::paths() { return {}; }
oo::PList ResourceManager::dictionaryFromFilesNamed(
    const std::string& /*fileName*/,
    const std::optional<std::string>& /*folderName*/, bool /*mergeFiles*/) {
  return oo::PList(oo::PList::Dict{});
}

// OORegExpMatcher stub: answers no match.
std::shared_ptr<oo::RegExpMatcher> oo::RegExpMatcher::regExpMatcher() {
  return oo::makeShared<oo::RegExpMatcher>();
}
bool oo::RegExpMatcher::string(const std::string&, const std::string&) {
  return false;
}
namespace oo {

RegExpMatcher::~RegExpMatcher() {}

// OOLogging.mm's (it would bring the resource manager into the link);
// OOOpenGL.mm's OOGL error check and state dump call them.
void LogIndent(void) {}
void LogOutdent(void) {}

ShaderSetting ShaderSettingFromString(const std::string& string) {
  return string == "SHADERS_FULL" ? ShaderSetting::kFull : ShaderSetting::kNotSupported;
}

}  // namespace oo


namespace {

// The type of a return type, by its value type: the member tables' rows give
// oo::kValueType<> of their types (an object result's keyed oo::ObjectResult).
template <typename T>
oo::ShaderUniformType TypeOf() {
  return oo::ShaderUniformTypeOf(oo::kValueType<T>);
}

}  // namespace

/*	The types of the return types, one each, are the types their encodings
   are given.
*/
TEST(OOShaderUniformMethodType, typesFromEncodings) {
  EXPECT_TRUE(TypeOf<float>() == oo::kShaderUniformTypeFloat);
  EXPECT_TRUE(TypeOf<double>() == oo::kShaderUniformTypeDouble);
  EXPECT_TRUE(TypeOf<signed char>() == oo::kShaderUniformTypeChar);
  EXPECT_TRUE(TypeOf<unsigned char>() == oo::kShaderUniformTypeUnsignedChar);
  EXPECT_TRUE(TypeOf<signed short>() == oo::kShaderUniformTypeShort);
  EXPECT_TRUE(TypeOf<unsigned short>() == oo::kShaderUniformTypeUnsignedShort);
  EXPECT_TRUE(TypeOf<signed int>() == oo::kShaderUniformTypeInt);
  EXPECT_TRUE(TypeOf<unsigned int>() == oo::kShaderUniformTypeUnsignedInt);
  EXPECT_TRUE(TypeOf<signed long>() == oo::kShaderUniformTypeLong);
  EXPECT_TRUE(TypeOf<unsigned long>() == oo::kShaderUniformTypeUnsignedLong);
  EXPECT_TRUE(TypeOf<Vector>() == oo::kShaderUniformTypeVector);
  EXPECT_TRUE(TypeOf<HPVector>() == oo::kShaderUniformTypeHPVector);
  EXPECT_TRUE(TypeOf<Quaternion>() == oo::kShaderUniformTypeQuaternion);
  EXPECT_TRUE(TypeOf<oo::Matrix>() == oo::kShaderUniformTypeMatrix);
  EXPECT_TRUE(TypeOf<NSPoint>() == oo::kShaderUniformTypePoint);
  EXPECT_TRUE(TypeOf<oo::ObjectResult>() == oo::kShaderUniformTypeObject);
}

/*	The types the table has no row for, through the encoding.
 */
TEST(OOShaderUniformMethodType, encodingsTheTableHasNot) {
  // bool is encoded as a signed char here.
  EXPECT_TRUE(TypeOf<bool>() == oo::kShaderUniformTypeChar);
  // The table has no long long entries: on this platform (LLP64) a long is 32
  // bits, so a long long matches nothing.
  EXPECT_TRUE(TypeOf<long long>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<unsigned long long>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<void>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(TypeOf<NSRect>() == oo::kShaderUniformTypeInvalid);
  EXPECT_TRUE(oo::ShaderUniformTypeOf(oo::ValueType::kOther) ==
           oo::kShaderUniformTypeInvalid);
}
