/*	test_OOShaderProgram.mm
	Unit tests for OOShaderProgram (src/Core/Materials/OOShaderProgram.h).

	A linked GLSL program, made from sources or from shader files and shared through a cache keyed
	by what made it. This pins, through the C++ API its callers use (OOShaderMaterial,
	OOShaderUniform, DustEntity, Universe), what it did before the conversion, in a hidden GL
	context: the programs made (the prefix prepended, an empty prefix counting as none, the
	attribute bindings), the failures (no source, a source that does not compile, a file that is not
	there), the cache (one program per key while it lives, none without a key, a new one once it has
	gone), the files asked for, and apply() / applyNone() (the program in use, kept while it is).
	There is no game view here, so no standard matrix uniforms are bound. The cases ask the C++
	class (null for nil, a
	held Ref for retain/release). Run: bash tools/check-core-tests.sh
*/

#include <gtest/gtest.h>

#include "ShaderProgram.h"
#include "RegExpMatcher.h"
#include "MyOpenGLView.h"

#include "oo_gl_test_context.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>


// --- Stubs --------------------------------------------------------------------------------------

// OOLogging.mm's (it would bring the resource manager into the link).
namespace oo {

void LogIndent(void)  {}
void LogOutdent(void)  {}
const char *const kLogFileNotFound = "files.notFound";

}  // namespace oo



// The resource manager: the extension manager's two class methods, and shader files from a table.
static std::map<std::string, std::string> gShaderFiles;
static std::vector<std::string> gShaderFileRequests;

#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
std::vector<std::string> ResourceManager::paths()  { return {}; }
oo::PList ResourceManager::dictionaryFromFilesNamed(const std::string &/*fileName*/, const std::optional<std::string> &/*folderName*/, bool /*mergeFiles*/)  { return oo::PList(oo::PList::Dict{}); }
std::optional<std::string> ResourceManager::stringFromFilesNamed(const std::string &fileName, const std::optional<std::string> &folderName)
{
	gShaderFileRequests.push_back(folderName.value_or("") + "/" + fileName);
	auto found = gShaderFiles.find(fileName);
	if (found == gShaderFiles.end())  return std::nullopt;
	return found->second;
}



// OORegExpMatcher stub: answers no match.
std::shared_ptr<oo::RegExpMatcher> oo::RegExpMatcher::regExpMatcher()  { return oo::makeShared<oo::RegExpMatcher>(); }
bool oo::RegExpMatcher::string(const std::string &, const std::string &)  { return false; }
namespace oo {

RegExpMatcher::~RegExpMatcher()  {}


ShaderSetting ShaderSettingFromString(const std::string &string)
{
	return string == "SHADERS_FULL" ? ShaderSetting::kFull : ShaderSetting::kNotSupported;
}

}  // namespace oo



// UNIVERSE is gSharedUniverse: nil, so there is no game view and no matrix
// manager. The view is C++ (MyOpenGLView), and its manager accessor is never
// reached.
class Universe;
Universe *gSharedUniverse = nullptr;

oo::OpenGLMatrixManager *MyOpenGLView::getOpenGLMatrixManager()  { return matrixManager.get(); }


namespace {

const std::string kVertex =
	"attribute vec3 tangent;\n"
	"void main() { gl_Position = gl_Vertex + vec4(tangent, 0.0); }\n";
const std::string kFragment =
	"void main() { gl_FragColor = vec4(1.0); }\n";
const std::string kFragmentNeedingPrefix =
	"void main() { gl_FragColor = vec4(PREFIXED); }\n";

const oo::PList kBindings = oo::PList(oo::PList::Dict{ { "tangent", oo::PList::signedInteger(15) } });


typedef GLint (APIENTRY *GetAttribLocationProc)(GLhandleARB, const GLcharARB *);
typedef GLboolean (APIENTRY *IsProgramProc)(GLuint);
GetAttribLocationProc gGetAttribLocation = nullptr;
IsProgramProc gIsProgram = nullptr;


bool SetUp()
{
	if (!OOTestGLContext())  return false;
	oo::OpenGLExtensionManager::sharedManager();	// loads the ARB shader entry points
	gGetAttribLocation = (GetAttribLocationProc)SDL_GL_GetProcAddress("glGetAttribLocationARB");
	gIsProgram = (IsProgramProc)SDL_GL_GetProcAddress("glIsProgram");
	return gGetAttribLocation != nullptr && gIsProgram != nullptr;
}


std::shared_ptr<oo::ShaderProgram> Make(const std::optional<std::string> &vertex, const std::optional<std::string> &fragment, const std::optional<std::string> &prefix = std::nullopt, const std::optional<std::string> &key = std::nullopt)
{
	return oo::ShaderProgram::shaderProgramWithVertexShader(vertex, fragment, std::string("v"), std::string("f"), prefix, kBindings, key);
}


GLint CurrentProgram()
{
	GLint current = -1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &current);
	return current;
}


/*	A failed initialiser's teardown deletes program 0 (glDeleteObjectARB(0), as upstream's -dealloc
	does). Drivers differ there: NVIDIA reports GL_INVALID_VALUE, Mesa ignores it (as every driver
	ignores glDeleteProgram(0)). That one error or none, and nothing else, is left behind.
*/
bool OnlyTheTeardownsError()
{
	const GLenum first = glGetError();
	return (first == GL_INVALID_VALUE || first == GL_NO_ERROR) && glGetError() == GL_NO_ERROR;
}

}	// namespace


TEST(OOShaderProgram, programsFromSources)
{
	if (!::SetUp())  { EXPECT_TRUE(false); return; }
	{
		std::shared_ptr<oo::ShaderProgram> p = Make(kVertex, kFragment);
		EXPECT_TRUE(p != nullptr && p->program() != 0);
		EXPECT_TRUE(gGetAttribLocation(p->program(), "tangent") == 15);	// the attribute bindings

		EXPECT_TRUE(Make(std::nullopt, kFragment) != nullptr);	// one shader is enough
		EXPECT_TRUE(Make(kVertex, std::nullopt) != nullptr);
		EXPECT_TRUE(glGetError() == GL_NO_ERROR);

		/*	A failed initialiser leaves at most its teardown's error behind (OnlyTheTeardownsError),
			and a successful one none.
		*/
		EXPECT_TRUE(Make(std::nullopt, std::nullopt) == nullptr);	// none is not
		EXPECT_TRUE(OnlyTheTeardownsError());

		// The prefix is prepended to both sources; an empty one is none.
		EXPECT_TRUE(Make(kVertex, kFragmentNeedingPrefix) == nullptr);
		EXPECT_TRUE(OnlyTheTeardownsError());
		EXPECT_TRUE(Make(kVertex, kFragmentNeedingPrefix, std::string("")) == nullptr);
		EXPECT_TRUE(OnlyTheTeardownsError());
		EXPECT_TRUE(Make(kVertex, kFragmentNeedingPrefix, std::string("#define PREFIXED 0.5\n")) != nullptr);
		EXPECT_TRUE(glGetError() == GL_NO_ERROR);

		EXPECT_TRUE(Make(std::string("this is not GLSL"), kFragment) == nullptr);	// does not compile
		EXPECT_TRUE(OnlyTheTeardownsError());
	}
}


TEST(OOShaderProgram, cache)
{
	if (!::SetUp())  { EXPECT_TRUE(false); return; }
	std::shared_ptr<oo::ShaderProgram> kept;
	GLhandleARB handle = 0;
	{
		std::shared_ptr<oo::ShaderProgram> a = Make(kVertex, kFragment, std::nullopt, std::string("key-a"));
		EXPECT_TRUE(a != nullptr && Make(kVertex, kFragment, std::nullopt, std::string("key-a")) == a);	// one per key
		EXPECT_TRUE(Make(std::string("anything: it is not compiled"), std::nullopt, std::nullopt, std::string("key-a")) == a);
		EXPECT_TRUE(Make(kVertex, kFragment, std::nullopt, std::string("key-b")) != a);
		EXPECT_TRUE(Make(kVertex, kFragment).get() != Make(kVertex, kFragment).get());	// no key: never shared
		kept = a;
		handle = a->program();
	}
	{
		// Still alive (kept): still the cached one.
		EXPECT_TRUE(Make(kVertex, kFragment, std::nullopt, std::string("key-a")) == kept);
	}
	kept = nullptr;
	{
		// Gone: the cache forgot it, and its GL program was deleted.
		EXPECT_TRUE(!gIsProgram(handle));
		std::shared_ptr<oo::ShaderProgram> again = Make(kVertex, kFragment, std::nullopt, std::string("key-a"));
		EXPECT_TRUE(again != nullptr && again->program() != 0);
	}
}


TEST(OOShaderProgram, programsFromFiles)
{
	if (!::SetUp())  { EXPECT_TRUE(false); return; }
	{
		gShaderFiles = { { "test.vertex", kVertex }, { "test.frag", kFragment }, { "other.vert", kVertex } };
		gShaderFileRequests.clear();

		// Each name bare, then with its type's two extensions until one is found.
		std::shared_ptr<oo::ShaderProgram> p = oo::ShaderProgram::shaderProgramWithVertexShaderName("test", "test", std::nullopt, kBindings);
		EXPECT_TRUE(p != nullptr && gGetAttribLocation(p->program(), "tangent") == 15);
		EXPECT_TRUE(gShaderFileRequests == (std::vector<std::string>{ "Shaders/test", "Shaders/test.vertex", "Shaders/test", "Shaders/test.fragment", "Shaders/test.frag" }));

		// Cached by the names and the prefix: the files are not read again.
		gShaderFileRequests.clear();
		EXPECT_TRUE(oo::ShaderProgram::shaderProgramWithVertexShaderName("test", "test", std::string(""), kBindings) == p);
		EXPECT_TRUE(gShaderFileRequests.empty());
		std::shared_ptr<oo::ShaderProgram> prefixed = oo::ShaderProgram::shaderProgramWithVertexShaderName("test", "test", std::string("#define UNUSED 1\n"), kBindings);
		EXPECT_TRUE(prefixed != nullptr && prefixed != p && gShaderFileRequests.size() == 5);

		// A name with one of the extensions is not extended; a file that is not there fails.
		gShaderFileRequests.clear();
		EXPECT_TRUE(oo::ShaderProgram::shaderProgramWithVertexShaderName("other.vert", "missing.frag", std::nullopt, kBindings) == nullptr);
		EXPECT_TRUE(gShaderFileRequests == (std::vector<std::string>{ "Shaders/other.vert", "Shaders/missing.frag" }));
		gShaderFiles.clear();
	}
}


TEST(OOShaderProgram, applyAndApplyNone)
{
	if (!::SetUp())  { EXPECT_TRUE(false); return; }
	std::shared_ptr<oo::ShaderProgram> p;
	{
		p = Make(kVertex, kFragment);
		std::shared_ptr<oo::ShaderProgram> q = Make(kVertex, kFragment);
		const long retained = p.use_count();
		p->apply();
		EXPECT_TRUE(CurrentProgram() == (GLint)p->program());
		EXPECT_TRUE(p.use_count() == retained + 1);	// kept while in use
		p->apply();	// again: no change
		EXPECT_TRUE(p.use_count() == retained + 1);
		q->apply();
		EXPECT_TRUE(CurrentProgram() == (GLint)q->program() && p.use_count() == retained);
		oo::ShaderProgram::applyNone();
		EXPECT_TRUE(CurrentProgram() == 0);
		oo::ShaderProgram::applyNone();	// nothing in use: nothing
		EXPECT_TRUE(CurrentProgram() == 0);
		EXPECT_TRUE(glGetError() == GL_NO_ERROR);
	}
	p = nullptr;
}


// --- The C++ API ----------------------------------------------------------------------------------

TEST(OOShaderProgram, cxxAPI)
{
	if (!::SetUp())  { EXPECT_TRUE(false); return; }
	{
		EXPECT_TRUE(oo::ShaderProgram::shaderProgramWithVertexShader(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, kBindings, std::nullopt) == nullptr);
		const std::shared_ptr<oo::ShaderProgram> p = oo::ShaderProgram::shaderProgramWithVertexShader(kVertex, kFragment, std::string("v"), std::string("f"), std::nullopt, kBindings, std::string("cxx-key"));
		EXPECT_TRUE(p != nullptr && p->program() != 0 && gGetAttribLocation(p->program(), "tangent") == 15);

		// The cache shares it.
		EXPECT_TRUE(oo::ShaderProgram::shaderProgramWithVertexShader(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, kBindings, std::string("cxx-key")).get() == p.get());
		EXPECT_TRUE(Make(kVertex, kFragment, std::nullopt, std::string("cxx-key")).get() == p.get());

		gShaderFiles = { { "c.vert", kVertex }, { "c.frag", kFragment } };
		const std::shared_ptr<oo::ShaderProgram> fromFiles = oo::ShaderProgram::shaderProgramWithVertexShaderName("c.vert", "c.frag", std::nullopt, kBindings);
		EXPECT_TRUE(fromFiles != nullptr && fromFiles.get() == oo::ShaderProgram::shaderProgramWithVertexShaderName("c.vert", "c.frag", std::nullopt, kBindings).get());
		EXPECT_TRUE(oo::ShaderProgram::shaderProgramWithVertexShaderName("none.vert", "c.frag", std::nullopt, kBindings) == nullptr);
		gShaderFiles.clear();

		p->apply();
		EXPECT_TRUE(CurrentProgram() == (GLint)p->program());
		oo::ShaderProgram::applyNone();
		EXPECT_TRUE(CurrentProgram() == 0);
	}
}


// --- OOTranslateGLSL330To120 (upstream #766, used on macOS) ----------------------------------------------------

namespace {

// A shader from Resources/Shaders, found from this file's directory or the working directory upwards;
// empty when it is not there.
std::string ShaderFile(const std::string &name)
{
	namespace fs = std::filesystem;
	const fs::path rel = fs::path("Resources") / "Shaders" / name;
	for (const fs::path &start : { fs::path(__FILE__).parent_path(), fs::current_path() })
	{
		std::error_code ec;
		fs::path dir = fs::absolute(start, ec);
		for (int i = 0; i < 8 && !dir.empty(); i++)
		{
			if (fs::exists(dir / rel, ec))
			{
				std::ifstream in(dir / rel, std::ios::binary);
				std::stringstream text;
				text << in.rdbuf();
				return text.str();
			}
			if (dir == dir.parent_path())  break;
			dir = dir.parent_path();
		}
	}
	return std::string();
}


bool Contains(const std::string &text, const std::string &part)
{
	return text.find(part) != std::string::npos;
}


// What upstream's regular-expression rewrite gives, run through the C++ one.
std::string Translated(const std::string &source, bool isVertexShader, oo::PList::Dict &bindings)
{
	return oo::TranslateGLSL330To120(source, isVertexShader, bindings).value_or("(null)");
}


const std::string kGLSL120Prefix = "#version 120\n#extension GL_EXT_gpu_shader4 : enable\n";

}	// namespace


TEST(OOShaderProgram, translateGLSL330To120)
{
	oo::PList::Dict bindings;
	EXPECT_TRUE(oo::TranslateGLSL330To120(std::nullopt, true, bindings) == std::nullopt);
	EXPECT_TRUE(bindings.empty());

	// Vertex: located inputs become attributes and bindings, outputs varyings.
	EXPECT_EQ(Translated("layout (location = 0) in vec3 aPos;\n"
	                       "layout(location=3) in vec2 aTexCoords;\n"
	                       "out vec2 TexCoords;\n"
	                       "void main() { TexCoords = aTexCoords; }\n", true, bindings),
	            std::string("attribute vec3 aPos;\n"
	                        "attribute vec2 aTexCoords;\n"
	                        "varying vec2 TexCoords;\n"
	                        "void main() { TexCoords = aTexCoords; }\n"));
	EXPECT_TRUE(bindings.size() == 2);
	EXPECT_TRUE(bindings["aPos"] == oo::PList::signedInteger(0));
	EXPECT_TRUE(bindings["aTexCoords"] == oo::PList::signedInteger(3));

	// Fragment: vec4 outputs become gl_FragData defines (location 0 when none is given), inputs
	// varyings; texture() and textureSize() take their 1.20 names, and a qualifier inside a function
	// is left alone.
	bindings.clear();
	EXPECT_EQ(Translated("layout (location = 1) out vec4 bright;\n"
	                       "out vec4 FragColor;\n"
	                       "in vec2 TexCoords;\n"
	                       "uniform sampler2D image;\n"
	                       "void f(in float x, out float y) { y = x; }\n"
	                       "void main() {\n"
	                       "\tvec2 s = 1.0 / textureSize(image, 0);\n"
	                       "\tFragColor = texture (image, TexCoords);\n"
	                       "\tbright = texture2D(image, s);\n"
	                       "}\n", false, bindings),
	            std::string("#define bright gl_FragData[1]\n"
	                        "#define FragColor gl_FragData[0]\n"
	                        "varying vec2 TexCoords;\n"
	                        "uniform sampler2D image;\n"
	                        "void f(in float x, out float y) { y = x; }\n"
	                        "void main() {\n"
	                        "\tvec2 s = 1.0 / textureSize2D(image, 0);\n"
	                        "\tFragColor = texture2D(image, TexCoords);\n"
	                        "\tbright = texture2D(image, s);\n"
	                        "}\n"));
	EXPECT_TRUE(bindings.empty());

	// isnan() is not in GLSL 1.20: a shim is prepended and called instead.
	EXPECT_EQ(Translated("void main() { bool b = isnan(1.0) || isnan (2.0); }\n", false, bindings),
	            std::string("bool oo_isnan(float x) { return !(x < 0.0 || x > 0.0 || x == 0.0); }\n"
	                        "void main() { bool b = oo_isnan(1.0) || oo_isnan(2.0); }\n"));
}


TEST(OOShaderProgram, translateGameShadersGLSL330To120)
{
	struct Program { const char *vertex; const char *fragment; };
	const Program programs[] = {
		{ "oolite-texture.vertex", "oolite-texture.fragment" },
		{ "oolite-blur.vertex", "oolite-blur.fragment" },
		{ "oolite-final.vertex", "oolite-final.fragment" },
	};
	const std::regex texture330(R"(\btexture\s*\()");
	const std::regex inOut330(R"(^\s*(in|out)\s)", std::regex::ECMAScript | std::regex::multiline);
	const bool canCompile = ::SetUp() && oo::OpenGLExtensionManager::sharedManager()->haveExtension("GL_EXT_gpu_shader4");
	if (!canCompile)  std::printf("note: no GL_EXT_gpu_shader4, so the translated game shaders are not compiled\n");

	for (const Program &program : programs)
	{
		const std::string vertex330 = ShaderFile(program.vertex);
		const std::string fragment330 = ShaderFile(program.fragment);
		EXPECT_TRUE(!vertex330.empty() && !fragment330.empty());
		oo::PList::Dict bindings;
		const std::string vertex = Translated(vertex330, true, bindings);
		const std::string fragment = Translated(fragment330, false, bindings);

		// Every game vertex shader takes its two inputs at locations 0 and 1.
		EXPECT_TRUE(bindings.size() == 2);
		for (const auto &[name, location] : bindings)
			EXPECT_TRUE(location == oo::PList::signedInteger(0) || location == oo::PList::signedInteger(1));
		for (const std::string *translated : { &vertex, &fragment })
		{
			EXPECT_TRUE(!Contains(*translated, "layout"));
			EXPECT_TRUE(!std::regex_search(*translated, texture330));
			EXPECT_TRUE(!std::regex_search(*translated, inOut330));
		}
		EXPECT_TRUE(Contains(vertex, "attribute "));
		EXPECT_TRUE(Contains(vertex, "varying "));
		EXPECT_TRUE(Contains(fragment, "gl_FragData[0]"));
		EXPECT_TRUE(Contains(fragment, "varying "));
		EXPECT_TRUE(Contains(fragment, "isnan(") == Contains(fragment, "bool oo_isnan(float x)"));

		if (canCompile)
		{
			// GLSL 1.20 with GL_EXT_gpu_shader4, as the macOS legacy context runs it.
			{
				std::shared_ptr<oo::ShaderProgram> compiled = Make(vertex, fragment, kGLSL120Prefix);
				if (compiled == nullptr)  std::printf("FAIL: %s + %s do not compile as GLSL 1.20\n", program.vertex, program.fragment);
				EXPECT_TRUE(compiled != nullptr);
			}
		}
	}
	// The texture program's two outputs go to the two colour attachments.
	oo::PList::Dict bindings;
	const std::string texture = Translated(ShaderFile("oolite-texture.fragment"), false, bindings);
	EXPECT_TRUE(Contains(texture, "#define out_color gl_FragData[0]"));
	EXPECT_TRUE(Contains(texture, "#define bright_color gl_FragData[1]"));
}
