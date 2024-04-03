#include "pain.h"
#include <glad/gl.h>

void pain::Pain::initiate()
{
  pain::LogWrapper::Init();
  PLOG_T("Initialized Log!");
}

void pain::Pain::runApplication(Application *app)
{
  Application application = std::move(*app);
  application.run();
}
