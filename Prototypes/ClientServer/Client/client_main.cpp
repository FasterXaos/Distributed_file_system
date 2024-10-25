#include "client.hpp"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	SHIZ::Client client;
	client.setWindowTitle("File Client");
	client.resize(400, 300);
	client.show();

	return app.exec();
}
