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

#include "RegExpMatcher.h"
#include "ShaderUniformMethodType.h"
#include "ValueType.h"
#include "oo_test.hpp"
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
OO_TEST(typesFromEncodings) {
  OO_CHECK(TypeOf<float>() == oo::kShaderUniformTypeFloat);
  OO_CHECK(TypeOf<double>() == oo::kShaderUniformTypeDouble);
  OO_CHECK(TypeOf<signed char>() == oo::kShaderUniformTypeChar);
  OO_CHECK(TypeOf<unsigned char>() == oo::kShaderUniformTypeUnsignedChar);
  OO_CHECK(TypeOf<signed short>() == oo::kShaderUniformTypeShort);
  OO_CHECK(TypeOf<unsigned short>() == oo::kShaderUniformTypeUnsignedShort);
  OO_CHECK(TypeOf<signed int>() == oo::kShaderUniformTypeInt);
  OO_CHECK(TypeOf<unsigned int>() == oo::kShaderUniformTypeUnsignedInt);
  OO_CHECK(TypeOf<signed long>() == oo::kShaderUniformTypeLong);
  OO_CHECK(TypeOf<unsigned long>() == oo::kShaderUniformTypeUnsignedLong);
  OO_CHECK(TypeOf<Vector>() == oo::kShaderUniformTypeVector);
  OO_CHECK(TypeOf<HPVector>() == oo::kShaderUniformTypeHPVector);
  OO_CHECK(TypeOf<Quaternion>() == oo::kShaderUniformTypeQuaternion);
  OO_CHECK(TypeOf<oo::Matrix>() == oo::kShaderUniformTypeMatrix);
  OO_CHECK(TypeOf<NSPoint>() == oo::kShaderUniformTypePoint);
  OO_CHECK(TypeOf<oo::ObjectResult>() == oo::kShaderUniformTypeObject);
}

/*	The types the table has no row for, through the encoding.
 */
OO_TEST(encodingsTheTableHasNot) {
  // bool is encoded as a signed char here.
  OO_CHECK(TypeOf<bool>() == oo::kShaderUniformTypeChar);
  // The table has no long long entries: on this platform (LLP64) a long is 32
  // bits, so a long long matches nothing.
  OO_CHECK(TypeOf<long long>() == oo::kShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<unsigned long long>() == oo::kShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<void>() == oo::kShaderUniformTypeInvalid);
  OO_CHECK(TypeOf<NSRect>() == oo::kShaderUniformTypeInvalid);
  OO_CHECK(oo::ShaderUniformTypeOf(oo::ValueType::kOther) ==
           oo::kShaderUniformTypeInvalid);
}

OO_TEST_MAIN()
