#ifndef SERVER_HPP
#define SERVER_HPP

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTcpServer>
#include <QTcpSocket>

namespace SHIZ {

	class Server : public QTcpServer {
		Q_OBJECT

		private:
			QTcpSocket *clientSocket;

		public:
			explicit Server(QObject *parent = nullptr);
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

#endif // SERVER_HPP
