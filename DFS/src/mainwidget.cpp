#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHeaderView>

#include "mainwidget.hpp"

namespace SHIZ{
	MainWidget::MainWidget(Logger* logger, NetworkManager* manager, QWidget* parent)
		: logger(logger), networkManager(manager), QWidget(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);

		statusLabel = new QLabel(this);
		layout->addWidget(statusLabel);
		connect(networkManager, &NetworkManager::statusMessage, this, &MainWidget::onStatusMessageReceived);

		filterLineEdit = new QLineEdit(this);
		filterLineEdit->setPlaceholderText("Filter by file name");
		layout->addWidget(filterLineEdit);
		connect(filterLineEdit, &QLineEdit::textChanged, this, &MainWidget::onFilterTextChanged);

		fileTableWidget = new QTableWidget(this);
		fileTableWidget->setColumnCount(4);
		fileTableWidget->setHorizontalHeaderLabels({"File Name", "Owner", "Size (bytes)", "Upload Date"});
		fileTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		fileTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
		fileTableWidget->setSortingEnabled(true);
		layout->addWidget(fileTableWidget);

		refreshButton = new QPushButton("Refresh File List", this);
		layout->addWidget(refreshButton);

		uploadButton = new QPushButton("Upload File", this);
		layout->addWidget(uploadButton);

		downloadButton = new QPushButton("Download File", this);
		layout->addWidget(downloadButton);

		deleteButton = new QPushButton("Delete File", this);
		layout->addWidget(deleteButton);

		cancelButton = new QPushButton("Cancel", this);
		cancelButton->setEnabled(false);
		layout->addWidget(cancelButton);

		logoutButton = new QPushButton("Logout", this);
		layout->addWidget(logoutButton);

		connect(refreshButton, &QPushButton::clicked, this, &MainWidget::onRefreshButtonClicked);
		connect(uploadButton, &QPushButton::clicked, this, &MainWidget::onUploadButtonClicked);
		connect(downloadButton, &QPushButton::clicked, this, &MainWidget::onDownloadButtonClicked);
		connect(deleteButton, &QPushButton::clicked, this, &MainWidget::onDeleteButtonClicked);
		connect(cancelButton, &QPushButton::clicked, this, &MainWidget::onCancelButtonClicked);
		connect(logoutButton, &QPushButton::clicked, this, &MainWidget::onLogoutButtonClicked);
	}


	void MainWidget::setOperationState(bool isOperationRunning) {
		cancelButton->setEnabled(isOperationRunning);
		refreshButton->setEnabled(!isOperationRunning);
		uploadButton->setEnabled(!isOperationRunning);
		downloadButton->setEnabled(!isOperationRunning);
		deleteButton->setEnabled(!isOperationRunning);
		logoutButton->setEnabled(!isOperationRunning);
	}


	void MainWidget::setCurrentLogin(const QString& login){
		currentLogin = login;
	}


	void MainWidget::onDownloadFileResult(bool success) {
		setOperationState(false);
		if (success) {
			QMessageBox::information(this, "Uploading", "The file has been uploaded successfully.");
		} else {
			QMessageBox::warning(this, "Download", "The file could not be downloaded.");
		}
	}

	void MainWidget::onFileDeletionResult(bool success) {
		setOperationState(false);
		if (success) {
			QMessageBox::information(this, "Delete", "File deleted successfully.");
			onRefreshButtonClicked();
		} else {
			QMessageBox::warning(this, "Delete", "File deletion failed.");
		}
	}

	void MainWidget::onFileListReceived(const QStringList& files) {
		fileTableWidget->setRowCount(files.size());
		for (int i = 0; i < files.size(); ++i) {
			QStringList fileInfo = files[i].split("|");
			if (fileInfo.size() == 4) {
				QTableWidgetItem* fileNameItem = new QTableWidgetItem(fileInfo[0]);
				QTableWidgetItem* ownerItem = new QTableWidgetItem(fileInfo[1]);
				QTableWidgetItem* sizeItem = new QTableWidgetItem(fileInfo[2]);
				QTableWidgetItem* dateItem = new QTableWidgetItem(fileInfo[3]);

				fileTableWidget->setItem(i, 0, fileNameItem);
				fileTableWidget->setItem(i, 1, ownerItem);
				fileTableWidget->setItem(i, 2, sizeItem);
				fileTableWidget->setItem(i, 3, dateItem);
			}
		}
	}

	void MainWidget::onFileUploadResult(bool success) {
		setOperationState(false);
		if (success) {
			QMessageBox::information(this, "Upload", "File uploaded successfully.");
			onRefreshButtonClicked();
		} else {
			QMessageBox::warning(this, "Upload", "File upload failed.");
		}
	}

	void MainWidget::onOperationCancelled() {
		setOperationState(false);
	}


	void MainWidget::onCancelButtonClicked() {
		emit cancelOperationRequested();
		setOperationState(true);
	}

	void MainWidget::onDeleteButtonClicked() {
		int selectedRow = fileTableWidget->currentRow();
		if (selectedRow >= 0) {
			QString fileName = fileTableWidget->item(selectedRow, 0)->text();
			emit requestFileDeletion(fileName);
			setOperationState(true);
		} else {
			QMessageBox::warning(this, "Delete", "No file selected.");
		}
	}

	void MainWidget::onDownloadButtonClicked() {
		int selectedRow = fileTableWidget->currentRow();
		if (selectedRow >= 0) {
			QString fileName = fileTableWidget->item(selectedRow, 0)->text();
			QString directory = QFileDialog::getExistingDirectory(this, "Select the download folder");

			if (!directory.isEmpty()) {
				QString filePath = directory + "/" + fileName;
				emit downloadFileRequested(filePath);
				setOperationState(true);
			}
		} else {
			QMessageBox::warning(this, "Download", "No file selected.");
		}
	}

	void MainWidget::onFilterTextChanged(const QString& text) {
		for (int i = 0; i < fileTableWidget->rowCount(); ++i) {
			bool match = fileTableWidget->item(i, 0)->text().contains(text, Qt::CaseInsensitive);
			fileTableWidget->setRowHidden(i, !match);
		}
	}

	void MainWidget::onLogoutButtonClicked() {
		fileTableWidget->setRowCount(0);
		emit showLoginWindow();
	}

	void MainWidget::onRefreshButtonClicked() {
		emit refreshFileListRequested();
	}

	void MainWidget::onStatusMessageReceived(const QString& message) {
		statusLabel->setText(message);
	}

	void MainWidget::onUploadButtonClicked() {
		setOperationState(true);
		QString filePath = QFileDialog::getOpenFileName(this, "Select a file to upload");
		if (filePath.isEmpty()) {
			setOperationState(false);
			return;
		}
		QString fileName = QFileInfo(filePath).fileName();

		onRefreshButtonClicked();
		bool fileExists = false;
		for (int i = 0; i < fileTableWidget->rowCount(); ++i) {
			if (fileTableWidget->item(i, 0)->text() == fileName) {
				fileExists = true;
				setOperationState(false);
				break;
			}
		}

		if (fileExists) {
			int response = QMessageBox::question(this, "File Exists",
												 "A file with this name already exists. Do you want to replace it?",
												 QMessageBox::Yes | QMessageBox::No);
			if (response == QMessageBox::No) {
				setOperationState(false);
				return;
			}
		}

		emit uploadFileRequested(filePath, currentLogin);
	}
}
