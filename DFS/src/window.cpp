#include <QVBoxLayout>

#include "window.hpp"

namespace SHIZ{
	Window::Window(Logger* logger, NetworkManager* networkManager, QWidget* parent)
		: logger(logger), networkManager(networkManager), QWidget(parent)
	{
		stackedWidget = new QStackedWidget(this);

		connectionWidget = new ConnectionWidget(logger, networkManager, this);
		loginWidget = new LoginWidget(logger, networkManager, this);
		mainWidget = new MainWidget(logger, networkManager, this);
		registrationWidget = new RegistrationWidget(logger, networkManager, this);

		stackedWidget->addWidget(connectionWidget);
		stackedWidget->addWidget(loginWidget);
		stackedWidget->addWidget(mainWidget);
		stackedWidget->addWidget(registrationWidget);

		stackedWidget->setCurrentWidget(connectionWidget);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(stackedWidget);
		setLayout(mainLayout);

		connect(connectionWidget, &ConnectionWidget::requestConnection, networkManager, &NetworkManager::onConnectToHost);
		connect(connectionWidget, &ConnectionWidget::ConnectionSuccessful, this, &Window::onConnectionSuccessful);

		connect(loginWidget, &LoginWidget::disconnectRequested, networkManager, &NetworkManager::onDisconnectRequested);
		connect(loginWidget, &LoginWidget::requestLogin, networkManager, &NetworkManager::onRequestLogin);
		connect(loginWidget, &LoginWidget::loginSuccessful, this, &Window::onLoginSuccessful);
		connect(loginWidget, &LoginWidget::showConnectionWindow, this, &Window::onSwitchToConnectionWindow);
		connect(loginWidget, &LoginWidget::showRegistrationWindow, this, &Window::onSwitchToRegistrationWindow);

		connect(mainWidget, &MainWidget::cancelOperationRequested, networkManager, &NetworkManager::onCancelOperationRequested);
		connect(mainWidget, &MainWidget::requestFileDeletion, networkManager, &NetworkManager::onDeleteFileRequested);
		connect(mainWidget, &MainWidget::downloadFileRequested, networkManager, &NetworkManager::onDownloadFileRequested);
		connect(mainWidget, &MainWidget::uploadFileRequested, networkManager, &NetworkManager::onUploadFileRequested);
		connect(mainWidget, &MainWidget::refreshFileListRequested, networkManager, &NetworkManager::onRequestFileList);
		connect(mainWidget, &MainWidget::showLoginWindow, this, &Window::onSwitchToLoginWindow);

		connect(networkManager, &NetworkManager::connectionResult, connectionWidget, &ConnectionWidget::onConnectionResult);
		connect(networkManager, &NetworkManager::loginResult, loginWidget, &LoginWidget::onLoginResult);
		connect(networkManager, &NetworkManager::operationCancelled, mainWidget, &MainWidget::onOperationCancelled);
		connect(networkManager, &NetworkManager::fileDownloadResult, mainWidget, &MainWidget::onDownloadFileResult);
		connect(networkManager, &NetworkManager::fileDeletionResult, mainWidget, &MainWidget::onFileDeletionResult);
		connect(networkManager, &NetworkManager::fileListReady, mainWidget, &MainWidget::onFileListReceived);
		connect(networkManager, &NetworkManager::fileUploadResult, mainWidget, &MainWidget::onFileUploadResult);
		connect(networkManager, &NetworkManager::registrationResult, registrationWidget, &RegistrationWidget::onRegistrationResult);

		connect(registrationWidget, &RegistrationWidget::disconnectRequested, networkManager, &NetworkManager::onDisconnectRequested);
		connect(registrationWidget, &RegistrationWidget::registrationRequest, networkManager, &NetworkManager::onRegistrationRequest);
		connect(registrationWidget, &RegistrationWidget::registrationSuccessful, this, &Window::onRegistrationSuccessful);
		connect(registrationWidget, &RegistrationWidget::showLoginWindow, this, &Window::onSwitchToLoginWindow);
		connect(registrationWidget, &RegistrationWidget::showConnectionWindow, this, &Window::onSwitchToConnectionWindow);
	}


	void Window::onConnectionSuccessful(const QString& host, quint16 port){
		logger->log(QString("Current connection IP: %1 and port: %2").arg(host).arg(port));
		onSwitchToLoginWindow();
	}

	void Window::onLoginSuccessful(const QString& login) {
		mainWidget->setCurrentLogin(login);
		onSwitchToMainWindow();
	}

	void Window::onRegistrationSuccessful(const QString& login) {
		mainWidget->setCurrentLogin(login);
		onSwitchToMainWindow();
	}

	void Window::onSwitchToConnectionWindow() {
		stackedWidget->setCurrentWidget(connectionWidget);
	}

	void Window::onSwitchToLoginWindow() {
		stackedWidget->setCurrentWidget(loginWidget);
	}

	void Window::onSwitchToMainWindow() {
		stackedWidget->setCurrentWidget(mainWidget);
	}

	void Window::onSwitchToRegistrationWindow() {
		stackedWidget->setCurrentWidget(registrationWidget);
	}
}
