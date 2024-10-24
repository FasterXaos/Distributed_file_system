#ifndef CLIENT_INTERFACE_HPP
#define CLIENT_INTERFACE_HPP

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>

class ClientInterface : public QWidget {
	Q_OBJECT

public:
	ClientInterface(QWidget *parent = nullptr);

	void setStatus(const QString &status);

signals:
	void sendFileRequested(const QString &filePath);

private:
	QLineEdit *filePathLineEdit;
	QPushButton *sendFileButton;
	QLabel *statusLabel;
};

#endif // CLIENT_INTERFACE_HPP
