#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QWidget>

namespace SHIZ {
	class Client : public QWidget {
		Q_OBJECT

		private:
			qint64 bytesSent;
			QLineEdit *downloadDirectoryInput;
			QFile file;
			QLineEdit *hostInput;
			QLineEdit *portInput;
			QTcpSocket *socket;
			QLineEdit *uploadFilePathInput;

		public:
			explicit Client(QWidget *parent = nullptr);

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

#endif // CLIENT_HPP
