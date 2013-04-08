#include "bapplication.h"

int main(int argc, char** argv)
{
  BApplication app(argc, argv);

  if (!app.init())
    return 1;

  return app.exec();
}
