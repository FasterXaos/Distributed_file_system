#include <QTcpServer>
#include <QTcpSocket>

class FileServer : public QTcpServer {
	Q_OBJECT

public:
	explicit FileServer(QObject *parent = nullptr);
	void startServer(qint16 port);

private slots:
	void incomingConnection(qintptr socketDescriptor);
	void handleClientConnection();
	void onClientDisconnected();

private:
	void handleFileUpload(const QByteArray &data);
	void sendFileToClient(QTcpSocket *clientSocket, const QString &fileName);
};
