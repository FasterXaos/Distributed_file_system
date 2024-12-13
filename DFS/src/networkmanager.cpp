#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "clientmacros.hpp"
#include "networkmanager.hpp"

namespace SHIZ {
	NetworkManager::NetworkManager(Logger* logger, QObject* parent)
		: logger(logger), QObject(parent), host(""), port(DEFAULT_PORT)
	{
		tcpSocket = new QTcpSocket(this);
		reconnectTimer = new QTimer(this);

		connect(tcpSocket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
		connect(tcpSocket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
		connect(reconnectTimer, &QTimer::timeout, this, &NetworkManager::onReconnectToServer);
	}

	NetworkManager::~NetworkManager() {
		onDisconnectRequested();
	}


	bool NetworkManager::downloadFile(const QString& filePath) {
		QString fileName = QFileInfo(filePath).fileName();
		emit statusMessage("Requesting file download: " + fileName);
		logger->log("Requesting file download: " + fileName);

		QDataStream out(tcpSocket);
		out << QString(COMMAND_DOWNLOAD) << fileName;
		tcpSocket->flush();

		if (!tcpSocket->waitForReadyRead(30000)) {
			emit statusMessage("Server did not respond in time for download.");
			logger->log("Server did not respond in time for download.");
			return false;
		}

		QDataStream in(tcpSocket);
		QString response;
		qint64 fileSize = 0;

		in >> response;
		if (response != RESPONSE_DOWNLOAD_READY) {
			emit statusMessage("Server not ready for download.");
			logger->log("Server not ready for download.");
			return false;
		}

		in >> fileSize;
		emit statusMessage("Downloading " + fileName +
						   " (" + QString::number(fileSize) + " bytes)...");
		logger->log("Downloading " + fileName +
					" (" + QString::number(fileSize) + " bytes)...");

		out << QString(RESPONSE_READY_FOR_DATA);
		tcpSocket->flush();

		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly)) {
			emit statusMessage("Failed to open file for writing.");
			logger->log("Failed to open file for writing.");
			return false;
		}

		qint64 totalReceived = 0;
		const qint64 chunkSize = 1024;
		QByteArray chunk;

		while (totalReceived < fileSize) {
			if (!tcpSocket->waitForReadyRead(30000)) {
				emit statusMessage("No response from server during download.");
				logger->log("No response from server during download.");
				file.close();
				return false;
			}

			in >> chunk;
			file.write(chunk);
			totalReceived += chunk.size();

			emit statusMessage("Downloading " + fileName + ": " +
							   QString::number(totalReceived) + " of " +
							   QString::number(fileSize) + " bytes received");
			logger->log("Downloading " + fileName + ": " +
						QString::number(totalReceived) + " of " +
						QString::number(fileSize) + " bytes received");

			out << QString(RESPONSE_CHUNK_RECEIVED);
			tcpSocket->flush();
		}

		file.close();
		emit statusMessage("Download complete: " + fileName);
		logger->log("Download complete: " + fileName);
		return true;
	}

	QStringList NetworkManager::requestFileList() {
		QDataStream out(tcpSocket);
		out << QString(COMMAND_GET_FILES);
		tcpSocket->flush();

		if (tcpSocket->waitForReadyRead(30000)) {
			QDataStream in(tcpSocket);
			QString response;
			QStringList fileList;

			in >> response;
			if (response == RESPONSE_FILES_LIST) {
				in >> fileList;
				return fileList;
			}
		}
		return QStringList();
	}

	bool NetworkManager::sendLoginRequest(const QString& login, const QString& password) {
		QDataStream out(tcpSocket);
		out << QString(COMMAND_LOGIN) << login << password;
		tcpSocket->flush();

		if (tcpSocket->waitForReadyRead(3000)) {
			QDataStream in(tcpSocket);
			QString response;
			in >> response;

			if (response == RESPONSE_LOGIN_SUCCESS) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
		return false;
	}

	bool NetworkManager::sendRegistrationRequest(const QString& login, const QString& password, const QString& confirmPassword) {
		QDataStream out(tcpSocket);
		out << QString(COMMAND_REGISTER) << login << password;
		tcpSocket->flush();

		if (tcpSocket->waitForReadyRead(3000)) {
			QDataStream in(tcpSocket);
			QString response;
			in >> response;

			if (response == RESPONSE_REGISTER_SUCCESS) {
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

	void NetworkManager::setHostAndPort(const QString& host, quint16 port) {
		this->host = host;
		this->port = port;
	}

	bool NetworkManager::uploadFile(const QString& filePath, const QString& owner) {
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly)) {
			emit statusMessage("Cannot open file for reading.");
			logger->log("Cannot open file for reading.");
			return false;
		}

		QString fileName = QFileInfo(filePath).fileName();
		qint64 fileSize = file.size();
		emit statusMessage("Uploading file: " + fileName);
		logger->log("Uploading file:" + fileName + "Owner:" + owner + "Size:" + QString::number(fileSize));

		QDataStream out(tcpSocket);
		out << QString(COMMAND_UPLOAD) << fileName << owner << fileSize;
		tcpSocket->flush();

		if (!tcpSocket->waitForReadyRead(30000)) {
			emit statusMessage("Server did not respond in time for upload.");
			logger->log("Server did not respond in time for upload.");
			return false;
		}

		QDataStream in(tcpSocket);
		QString response;
		in >> response;

		if (response != RESPONSE_READY_FOR_DATA) {
			emit statusMessage("Server is not ready for data.");
			logger->log("Server is not ready for data.");
			return false;
		}

		const qint64 chunkSize = 1024;
		qint64 totalSent = 0;

		while (!file.atEnd()) {
			QByteArray buffer = file.read(chunkSize);
			out << buffer;
			tcpSocket->flush();
			totalSent += buffer.size();

			emit statusMessage("Uploading " + fileName + ": " +
							   QString::number(totalSent) + " of " +
							   QString::number(fileSize) + " bytes sent");
			logger->log("Uploading " + fileName + ": " +
						QString::number(totalSent) + " of " +
						QString::number(fileSize) + " bytes sent");

			if (!tcpSocket->waitForReadyRead(30000)) {
				emit statusMessage("No response from server during upload.");
				logger->log("No response from server after sending chunk.");
				return false;
			}

			in >> response;
			if (response != RESPONSE_CHUNK_RECEIVED) {
				emit statusMessage("Server did not acknowledge chunk.");
				logger->log("Server did not acknowledge chunk.");
				return false;
			}
		}

		out << QByteArray();
		tcpSocket->flush();

		if (tcpSocket->waitForReadyRead(30000)) {
			in >> response;
			bool success = response == RESPONSE_UPLOAD_SUCCESS;
			emit statusMessage(success ? "File uploaded successfully." : "File upload failed.");
			logger->log(success ? "File uploaded successfully." : "File upload failed.");
			return true;
		}

		return false;
	}


	void NetworkManager::onCancelOperationRequested() {
		emit operationCancelled();
		emit statusMessage("Operation cancelled.");
	}

	void NetworkManager::onConnectToHost(const QString& host, quint16 port) {
		this->host = host;
		this->port = port;

		logger->log(QString("Attempting to connect to host: %1 on port: %2").arg(host).arg(port));

		tcpSocket->connectToHost(host, port);

		if (tcpSocket->waitForConnected(3000)) {
			logger->log("Connected to server.");
			QDataStream out(tcpSocket);
			out << QString(CLIENT);
			tcpSocket->flush();
			emit connectionResult(true, "Connected to main server successfully");
		} else {
			logger->log("Failed to connect to server.");
			emit connectionResult(false, "Failed to main server connect");
		}
	}

	void NetworkManager::onDeleteFileRequested(const QString& fileName) {
		emit statusMessage("Requesting file deletion...");
		logger->log("Requesting file deletion...");

		QDataStream out(tcpSocket);
		out << QString(COMMAND_DELETE) << fileName;
		tcpSocket->flush();

		if (tcpSocket->waitForReadyRead(3000)) {
			QDataStream in(tcpSocket);
			QString response;
			in >> response;

			bool success = response == RESPONSE_DELETE_SUCCESS;
			emit fileDeletionResult(success);
			emit statusMessage(success ? "File deleted successfully." : "File deletion failed.");
			logger->log(success ? "File deleted successfully." : "File deletion failed.");
		} else {
			emit fileDeletionResult(false);
			emit statusMessage("Server response timed out.");
			logger->log("Server response timed out.");
		}
	}

	void NetworkManager::onDisconnectRequested() {
		if (tcpSocket->isOpen()) {
			tcpSocket->disconnectFromHost();
		}
		reconnectTimer->stop();
		logger->log("Disconnected from server and stopped reconnect attempts.");
	}

	void NetworkManager::onDownloadFileRequested(const QString& filePath) {
		bool success = downloadFile(filePath);
		emit fileDownloadResult(success);
	}

	void NetworkManager::onRegistrationRequest(const QString& login, const QString& password, const QString& confirmPassword) {
		bool success = sendRegistrationRequest(login, password, confirmPassword);
		emit registrationResult(success);
	}

	void NetworkManager::onRequestFileList() {
		QStringList files = requestFileList();
		emit fileListReady(files);
	}

	void NetworkManager::onRequestLogin(const QString& login, const QString& password) {
		bool success = sendLoginRequest(login, password);
		emit loginResult(success, login);
	}

	void NetworkManager::onUploadFileRequested(const QString& filePath, const QString& owner) {
		bool success = uploadFile(filePath, owner);
		emit fileUploadResult(success);
	}


	void NetworkManager::onConnected() {
		reconnectTimer->stop();
		emit statusMessage("Connected to server.");
		logger->log("Connected to server.");
	}

	void NetworkManager::onDisconnected() {
		reconnectTimer->start(RECONNECT_INTERVAL);
		emit statusMessage("Disconnected from server.");
		logger->log("Disconnected from server. Reconnecting...");
	}

	void NetworkManager::onReconnectToServer() {
		if (host.isEmpty() || tcpSocket->state() != QTcpSocket::UnconnectedState) {
			return;
		}
		onConnectToHost(host, port);
	}
}
