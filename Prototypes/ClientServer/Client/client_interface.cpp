#include "client_interface.hpp"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>

ClientInterface::ClientInterface(QWidget *parent) : QWidget(parent) {
	filePathLineEdit = new QLineEdit(this);
	sendFileButton = new QPushButton("Send File", this);
	statusLabel = new QLabel(this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(filePathLineEdit);
	layout->addWidget(sendFileButton);
	layout->addWidget(statusLabel);

	connect(sendFileButton, &QPushButton::clicked, [this]() {
		QString filePath = QFileDialog::getOpenFileName(this, "Select File to Send");
		if (!filePath.isEmpty()) {
			filePathLineEdit->setText(filePath);
			emit sendFileRequested(filePath);
		}
	});
}

void ClientInterface::setStatus(const QString &status) {
	statusLabel->setText(status);
}
