
#include "AbstractSimulation.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "webworker/WebWorkersSimulation.hpp"

std::unique_ptr<AbstractSimulation> AbstractSimulation::create()
{
  return std::make_unique<WebWorkersSimulation>();
}

#else

#include "pthread/PthreadSimulation.hpp"

std::unique_ptr<AbstractSimulation> AbstractSimulation::create()
{
  return std::make_unique<PthreadSimulation>();
}

#endif


