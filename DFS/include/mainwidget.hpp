#pragma once

#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include "networkmanager.hpp"

namespace SHIZ{
	class MainWidget : public QWidget{
		Q_OBJECT

		private:
			NetworkManager* networkManager;
			QString currentLogin;

			QLabel* statusLabel;
			QLineEdit* filterLineEdit;
			QTableWidget* fileTableWidget;
			QPushButton* refreshButton;
			QPushButton* uploadButton;
			QPushButton* downloadButton;
			QPushButton* deleteButton;
			QPushButton* cancelButton;
			QPushButton* logoutButton;

			Logger* logger;

			bool operationInProgress = false;

		public:
			MainWidget(Logger* logger, NetworkManager* manager, QWidget* parent = nullptr);

			void setCurrentLogin(const QString& login);
			void setOperationState(bool isOperationRunning);

		signals:
			void cancelOperationRequested();
			void downloadFileRequested(const QString& filePath);
			void refreshFileListRequested();
			void requestFileDeletion(const QString& fileName);
			void showLoginWindow();
			void uploadFileRequested(const QString& filePath, const QString& owner);

		public slots:
			void onDownloadFileResult(bool success);
			void onFileDeletionResult(bool success);
			void onFileListReceived(const QStringList& files);
			void onFileUploadResult(bool success);
			void onOperationCancelled();
			void onRefreshButtonClicked();

		private slots:
			void onCancelButtonClicked();
			void onDeleteButtonClicked();
			void onDownloadButtonClicked();
			void onFilterTextChanged(const QString& text);
			void onLogoutButtonClicked();

			void onStatusMessageReceived(const QString& message);
			void onUploadButtonClicked();
	};
}
