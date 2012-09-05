#include <QApplication>

#include "PureMediaServer.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  PureMediaServer PureMediaServer;
  PureMediaServer.show();
  return app.exec();
}
