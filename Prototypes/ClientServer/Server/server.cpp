#include "server.hpp"

SHIZ::Server::Server(QObject *parent) : QTcpServer(parent) {}


void SHIZ::Server::startServer(qint16 port) {
	if (this->listen(QHostAddress::Any, port)) {
		qDebug() << "Сервер запущен, порт" << port;
	} else {
		qDebug() << "Не удалось запустить сервер!";
	}
}


void SHIZ::Server::uploadFile(const QByteArray &data) {
	QString baseDir = QCoreApplication::applicationDirPath();
	QString relativePath = "received_file.txt";
	QString fullPath = QDir(baseDir).filePath(relativePath);

	QFile file(fullPath);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Не удалось открыть файл для записи:" << fullPath;
		return;
	}

	file.write(data);
	file.close();
	qDebug() << "Файл успешно загружен в:" << fullPath;
}

void SHIZ::Server::sendFile(QTcpSocket *clientSocket, const QString &fileName) {
	QString baseDir = QCoreApplication::applicationDirPath();
	QString fullPath = QDir(baseDir).filePath(fileName);

	QFile file(fullPath);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Ошибка: не удалось открыть файл для чтения:" << fullPath;
		clientSocket->write("ERROR: Файл не найден");
		return;
	}

	QByteArray fileData = file.readAll();
	clientSocket->write(fileData);
	file.close();
	qDebug() << "Файл отправлен клиенту:" << fullPath;
}


void SHIZ::Server::incomingConnection(qintptr socketDescriptor) {
	QTcpSocket *clientSocket = new QTcpSocket(this);
	clientSocket->setSocketDescriptor(socketDescriptor);

	connect(clientSocket, &QTcpSocket::readyRead, this, &SHIZ::Server::onDataReceived);
	connect(clientSocket, &QTcpSocket::disconnected, this, &SHIZ::Server::onClientDisconnected);

	qDebug() << "Клиент подключен";
}


void SHIZ::Server::onClientDisconnected() {
	QTcpSocket *clientSocket = static_cast<QTcpSocket*>(sender());
	qDebug() << "Клиент отключен";
	clientSocket->deleteLater();
}

void SHIZ::Server::onDataReceived() {
	QTcpSocket *clientSocket = static_cast<QTcpSocket*>(sender());
	QByteArray receivedData = clientSocket->readAll();

	QString request(receivedData);

	if (request.startsWith("DOWNLOAD:")) {
		QString fileName = request.mid(9);
		sendFile(clientSocket, fileName);
	} else {
		uploadFile(receivedData);
	}
}
