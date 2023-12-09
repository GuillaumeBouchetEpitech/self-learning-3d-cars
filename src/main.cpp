
#include "application/Application.hpp"
#include "application/defines.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/math/validation-helpers.hpp"
#include "geronimo/system/parser-utils/ValuesParsers.hpp"

#include <array>
#include <sstream>

#include <cstdint> // <= EXIT_SUCCESS
#include <cstdlib> // <= EXIT_SUCCESS

#if defined D_WEB_BUILD
#include <emscripten/emscripten.h> // <= EMSCRIPTEN_KEEPALIVE
#endif

namespace {

void
_validateInputs(uint32_t inWidth, uint32_t inHeight, uint32_t inTotalGenomes, uint32_t inTotalCores) {
  if (inWidth < 800) {
    D_THROW(std::runtime_error, "argument 0 (width) cannot be < 800");
  }
  if (inHeight < 600) {
    D_THROW(std::runtime_error, "argument 1 (height) cannot be < 600");
  }
  if (inTotalGenomes < 100) {
    D_THROW(std::runtime_error, "argument 2 (total genomes) cannot be < 100");
  }
  if (inTotalCores < 1) {
    D_THROW(std::runtime_error, "argument 3 (total cores) cannot be < 1");
  }
}

} // namespace

#if defined D_NATIVE_PTHREAD_BUILD

namespace {

void
_processCommandLineArgs(Application::Definition& def, int argc, char** argv) {
  gero::valuesParsers::IntValueParser intValueParser;

  if (argc >= 2) {
    auto value = intValueParser.validate(argv[1]);
    if (!value)
      D_THROW(std::runtime_error, "argument 0 (width) is not a valid number");
    def.width = *value;
  }

  if (argc >= 3) {
    auto value = intValueParser.validate(argv[2]);
    if (!value)
      D_THROW(std::runtime_error, "argument 1 (height) is not a valid number");
    def.height = *value;
  }

  if (argc >= 4) {
    auto value = intValueParser.validate(argv[3]);
    if (!value)
      D_THROW(std::runtime_error, "argument 2 (total genomes) is not a valid number");
    def.totalGenomes = *value;
  }

  if (argc >= 5) {
    auto value = intValueParser.validate(argv[4]);
    if (!value)
      D_THROW(std::runtime_error, "argument 3 (total cores) is not a valid number");
    def.totalCores = *value;
  }
}

} // namespace

int
main(int argc, char** argv) {
  Application::Definition def;
  def.width = 800;
  def.height = 600;
  def.totalGenomes = 1000;
  def.totalCores = 3;

  _processCommandLineArgs(def, argc, argv);

  _validateInputs(def.width, def.height, def.totalGenomes, def.totalCores);

  Application myApplication(def);
  myApplication.run();

  return EXIT_SUCCESS;
}

#else

namespace /* anonymous */
{
Application* myApplication = nullptr;
};

extern "C" {

EMSCRIPTEN_KEEPALIVE
void
startApplication(uint32_t inWidth, uint32_t inHeight, uint32_t inTotalGenomes, uint32_t inTotalCores) {
  if (myApplication)
    return;

  Application::Definition def;
  def.width = inWidth;
  def.height = inHeight;
  def.totalGenomes = inTotalGenomes;
  def.totalCores = inTotalCores;

  _validateInputs(def.width, def.height, def.totalGenomes, def.totalCores);

  myApplication = new Application(def);
}

EMSCRIPTEN_KEEPALIVE
void
updateApplication(uint32_t inDelta) {
  if (!myApplication)
    return;

  myApplication->update(inDelta);
}

EMSCRIPTEN_KEEPALIVE
void
renderApplication() {
  if (!myApplication)
    return;

  myApplication->render();
}
}

#endif
