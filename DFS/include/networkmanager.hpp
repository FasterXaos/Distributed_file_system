#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "logger.hpp"

namespace SHIZ {
	class NetworkManager : public QObject {
		Q_OBJECT

	private:
		QTcpSocket* tcpSocket;
		QTimer* reconnectTimer;

		QString host;
		quint16 port;

		Logger* logger;

	public:
		NetworkManager(Logger* logger, QObject* parent = nullptr);
		~NetworkManager();

		bool downloadFile(const QString& filePath);
		QStringList requestFileList();
		bool sendLoginRequest(const QString& login, const QString& password);
		bool sendRegistrationRequest(const QString& login, const QString& password, const QString& confirmPassword);
		void setHostAndPort(const QString& host, quint16 port);
		bool uploadFile(const QString& filePath, const QString& owner);

	signals:
		void connectionResult(bool success, const QString& message);
		void fileDeletionResult(bool success);
		void fileDownloadResult(bool success);
		void fileListReady(const QStringList& files);
		void fileUploadResult(bool success);
		void loginResult(bool success, const QString& login);
		void operationCancelled();
		void registrationResult(bool success);
		void statusMessage(const QString& message);

	public slots:
		void onCancelOperationRequested();
		void onConnectToHost(const QString& host, quint16 port);
		void onDeleteFileRequested(const QString& fileName);
		void onDisconnectRequested();
		void onDownloadFileRequested(const QString& filePath);
		void onRegistrationRequest(const QString& login, const QString& password, const QString& confirmPassword);
		void onRequestFileList();
		void onRequestLogin(const QString& login, const QString& password);
		void onUploadFileRequested(const QString& filePath, const QString& owner);

	private slots:
		void onConnected();
		void onDisconnected();
		void onReconnectToServer();
	};
}
