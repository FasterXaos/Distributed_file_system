#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include "client.hpp"

SHIZ::FileClient::FileClient(QWidget *parent)
	: QWidget(parent), bytesSent(0), socket(new QTcpSocket(this)) {

	QVBoxLayout *layout = new QVBoxLayout(this);

	hostInput = new QLineEdit(this);
	hostInput->setPlaceholderText("127.0.0.1 / 192.168.0.107");
	layout->addWidget(hostInput);

	portInput = new QLineEdit(this);
	portInput->setPlaceholderText("12345");
	layout->addWidget(portInput);

	uploadFilePathInput = new QLineEdit(this);
	uploadFilePathInput->setPlaceholderText("Введите путь к файлу для загрузки на сервер");
	layout->addWidget(uploadFilePathInput);

	downloadDirectoryInput = new QLineEdit(this);
	downloadDirectoryInput->setPlaceholderText("Введите директорию для скачивания");
	layout->addWidget(downloadDirectoryInput);

	QPushButton *connectButton = new QPushButton("Подключиться", this);
	layout->addWidget(connectButton);
	connect(connectButton, &QPushButton::clicked, this, &FileClient::onConnectButtonClicked);

	QPushButton *disconnectButton = new QPushButton("Отключиться", this);
	layout->addWidget(disconnectButton);
	connect(disconnectButton, &QPushButton::clicked, this, &FileClient::onDisconnectButtonClicked);

	QPushButton *uploadButton = new QPushButton("Загрузить файл", this);
	layout->addWidget(uploadButton);
	connect(uploadButton, &QPushButton::clicked, this, &FileClient::onUploadButtonClicked);

	QPushButton *downloadButton = new QPushButton("Скачать файл", this);
	layout->addWidget(downloadButton);
	connect(downloadButton, &QPushButton::clicked, this, &FileClient::onDownloadButtonClicked);

	setLayout(layout);

	connect(socket, &QTcpSocket::connected, this, &FileClient::onConnected);
	connect(socket, &QTcpSocket::disconnected, this, &FileClient::onDisconnected);
}


void SHIZ::FileClient::connectToServer(const QString &host, qint16 port) {
	if (!socket->isOpen()) {
		socket->connectToHost(host, port);
		if (!socket->waitForConnected(3000)) {
			qDebug() << "Не удалось подключиться к серверу:";
		}
	} else {
		qDebug() << "Уже подключено!";
	}
}

void SHIZ::FileClient::disconnectFromServer() {
	if (socket->isOpen()) {
		socket->disconnectFromHost();
	} else {
		qDebug() << "Соединение уже разорвано!";
	}
}

void SHIZ::FileClient::downloadFile(const QString &directoryPath) {
	if (!socket->isOpen()) {
		qDebug() << "Ошибка: нет соединения с сервером!";
		return;
	}

	QString fileName = "received_file.txt";
	QString request = "DOWNLOAD:" + fileName;
	socket->write(request.toUtf8());
	qDebug() << "Запрос на скачивание файла отправлен: " << request;

	connect(socket, &QTcpSocket::readyRead, this, [this, directoryPath, fileName]() {
		QByteArray receivedData = socket->readAll();
		QString fullPath = QDir(directoryPath).filePath(fileName);

		QFile file(fullPath);
		if (!file.open(QIODevice::WriteOnly)) {
			qDebug() << "Ошибка: не удалось открыть файл для записи:" << fullPath;
			return;
		}

		file.write(receivedData);
		file.close();
		qDebug() << "Файл успешно скачан в:" << fullPath;

		disconnect(socket, &QTcpSocket::readyRead, nullptr, nullptr);
	});
}

void SHIZ::FileClient::uploadFile(const QString &filePath) {
	if (!socket->isOpen()) {
		qDebug() << "Ошибка: нет соединения с сервером!";
		return;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Не удалось открыть файл!" << filePath;
		return;
	}

	QByteArray fileData = file.readAll();
	file.close();

	qint64 bytesWritten = socket->write(fileData);
	if (bytesWritten == -1) {
		qDebug() << "Ошибка отправки данных:" << socket->errorString();
		return;
	}
	qDebug() << "Отправка файла начата. Отправлено байт:" << bytesWritten;

	if (!socket->waitForBytesWritten(3000)) {
		qDebug() << "Ошибка: не все данные были отправлены!" << socket->errorString();
	}
}


void SHIZ::FileClient::onConnected() {
	qDebug() << "Соединение с сервером установлено!";
}

void SHIZ::FileClient::onConnectButtonClicked() {
	QString host = hostInput->text();
	qint16 port = portInput->text().toInt();
	connectToServer(host, port);
}

void SHIZ::FileClient::onDisconnected() {
	qDebug() << "Соединение разорвано!";
}

void SHIZ::FileClient::onDisconnectButtonClicked() {
	disconnectFromServer();
}

void SHIZ::FileClient::onDownloadButtonClicked() {
	QString directoryPath = downloadDirectoryInput->text();
	if (directoryPath.isEmpty()) {
		qDebug() << "Ошибка: директория для скачивания не указана!";
		return;
	}
	downloadFile(directoryPath);
}

void SHIZ::FileClient::onUploadButtonClicked() {
	QString filePath = uploadFilePathInput->text();
	if (filePath.isEmpty()) {
		qDebug() << "Ошибка: путь к файлу для загрузки не указан!";
		return;
	}
	uploadFile(filePath);
}
