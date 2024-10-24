#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include "server.hpp"

FileServer::FileServer(QObject *parent) : QTcpServer(parent) {}

void FileServer::startServer(qint16 port) {
	if (this->listen(QHostAddress::Any, port)) {
		qDebug() << "Server started on port" << port;
	} else {
		qDebug() << "Server failed to start!";
	}
}

void FileServer::incomingConnection(qintptr socketDescriptor) {
	QTcpSocket *clientSocket = new QTcpSocket(this);
	clientSocket->setSocketDescriptor(socketDescriptor);

	connect(clientSocket, &QTcpSocket::readyRead, this, &FileServer::handleClientConnection);
	connect(clientSocket, &QTcpSocket::disconnected, this, &FileServer::onClientDisconnected);

	qDebug() << "Client connected";
}

void FileServer::handleClientConnection() {
	QTcpSocket *clientSocket = static_cast<QTcpSocket*>(sender());
	QByteArray receivedData = clientSocket->readAll();

	QString request(receivedData);

	if (request.startsWith("DOWNLOAD:")) {
		QString fileName = request.mid(9);
		sendFileToClient(clientSocket, fileName);
	} else {
		handleFileUpload(receivedData);
	}
}

void FileServer::handleFileUpload(const QByteArray &data) {
	QString baseDir = QCoreApplication::applicationDirPath();

	QString relativePath = "received_file.txt";
	QString fullPath = QDir(baseDir).filePath(relativePath);

	QFile file(fullPath);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open file for writing:" << fullPath;
		return;
	}

	file.write(data);
	file.close();
	qDebug() << "File uploaded successfully to:" << fullPath;
}

void FileServer::sendFileToClient(QTcpSocket *clientSocket, const QString &fileName) {
	QString baseDir = QCoreApplication::applicationDirPath();
	QString fullPath = QDir(baseDir).filePath(fileName);

	QFile file(fullPath);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Error: could not open file for reading:" << fullPath;
		clientSocket->write("ERROR: File not found");
		return;
	}

	QByteArray fileData = file.readAll();
	clientSocket->write(fileData);
	file.close();
	qDebug() << "File sent to client:" << fullPath;
}

void FileServer::onClientDisconnected() {
	QTcpSocket *clientSocket = static_cast<QTcpSocket*>(sender());
	qDebug() << "Client disconnected";
	clientSocket->deleteLater();
}
