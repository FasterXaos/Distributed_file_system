#pragma once

#include <QFile>
#include <QLineEdit>
#include <QTcpSocket>

namespace SHIZ {
	class FileClient : public QWidget {
		Q_OBJECT

		private:
			qint64 bytesSent;
			QFile file;

			QLineEdit *downloadDirectoryInput;
			QLineEdit *hostInput;
			QLineEdit *portInput;
			QLineEdit *uploadFilePathInput;
			QTcpSocket *socket;

		public:
			explicit FileClient(QWidget *parent = nullptr);

		private:
			void connectToServer(const QString &host, qint16 port);
			void disconnectFromServer();
			void downloadFile(const QString &directoryPath);
			void uploadFile(const QString &filePath);

		signals:
			void connected();
			void disconnected();

		private slots:
			void onConnected();
			void onConnectButtonClicked();
			void onDisconnected();
			void onDisconnectButtonClicked();
			void onDownloadButtonClicked();
			void onUploadButtonClicked();
	};
}
