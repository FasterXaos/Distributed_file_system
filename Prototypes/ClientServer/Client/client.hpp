#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>

class FileClient : public QWidget {
	Q_OBJECT

public:
	explicit FileClient(QWidget *parent = nullptr);

private slots:
	void onConnectButtonClicked();
	void onDisconnectButtonClicked();
	void onUploadButtonClicked();
	void onDownloadButtonClicked();
	void onConnected();
	void onDisconnected();

private:
	void connectToServer(const QString &host, qint16 port);
	void disconnectFromServer();
	void uploadFile(const QString &filePath);
	void downloadFile(const QString &directoryPath);

	QTcpSocket *socket;
	QFile file;
	QLineEdit *hostInput;
	QLineEdit *portInput;
	QLineEdit *uploadFilePathInput;
	QLineEdit *downloadDirectoryInput;
	qint64 bytesSent;
};
