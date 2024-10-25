#pragma once

#include <QTcpServer>
#include <QTcpSocket>

namespace SHIZ {
	class FileServer : public QTcpServer {
		Q_OBJECT

		private:
			QTcpSocket *clientSocket;

		public:
			explicit FileServer(QObject *parent = nullptr);
			void startServer(qint16 port);

		private:
			void uploadFile(const QByteArray &data);
			void sendFile(QTcpSocket *clientSocket, const QString &fileName);

		protected:
			void incomingConnection(qintptr socketDescriptor) override;

		private slots:
			void onClientDisconnected();
			void onDataReceived();
	};
}
