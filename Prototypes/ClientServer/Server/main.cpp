#include <QCoreApplication>

#include "server.hpp"

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);

	SHIZ::FileServer server;
	server.startServer(12345);

	return a.exec();
}
