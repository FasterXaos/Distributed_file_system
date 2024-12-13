#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "registrationwidget.hpp"

namespace SHIZ{
	RegistrationWidget::RegistrationWidget(Logger* logger, NetworkManager* manager, QWidget* parent)
		: logger(logger), networkManager(manager), QWidget(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);


		QLabel* loginLabel = new QLabel("Login:", this);
		layout->addWidget(loginLabel);

		loginInput = new QLineEdit(this);
		layout->addWidget(loginInput);


		QLabel* passwordLabel = new QLabel("Password:", this);
		layout->addWidget(passwordLabel);

		passwordInput = new QLineEdit(this);
		passwordInput->setEchoMode(QLineEdit::Password);
		layout->addWidget(passwordInput);


		QLabel* confirmPasswordLabel = new QLabel("Confirm:", this);
		layout->addWidget(confirmPasswordLabel);

		confirmPasswordInput = new QLineEdit(this);
		confirmPasswordInput->setEchoMode(QLineEdit::Password);
		layout->addWidget(confirmPasswordInput);


		enterButton = new QPushButton("Enter", this);
		layout->addWidget(enterButton);

		loginButton = new QPushButton("Login", this);
		layout->addWidget(loginButton);

		disconnectButton = new QPushButton("Disconnect", this);
		layout->addWidget(disconnectButton);


		connect(enterButton, &QPushButton::clicked, this, &RegistrationWidget::onEnterButtonClicked);
		connect(loginButton, &QPushButton::clicked, this, &RegistrationWidget::onLoginButtonClicked);
		connect(disconnectButton, &QPushButton::clicked, this, &RegistrationWidget::onDisconnectButtonClicked);
	}


	void RegistrationWidget::onRegistrationResult(bool success) {
		if (success) {
			loginInput->clear();
			passwordInput->clear();
			confirmPasswordInput->clear();
			emit registrationSuccessful(loginInput->text());
			logger->log("Registration Successful: " + loginInput->text());
		} else {
			QMessageBox::warning(this, "Registration error", "Registration failed.");
		}
	}


	void RegistrationWidget::onDisconnectButtonClicked() {
		emit disconnectRequested();
		loginInput->clear();
		passwordInput->clear();
		confirmPasswordInput->clear();

		emit showConnectionWindow();
	}

	void RegistrationWidget::onEnterButtonClicked() {
		QString login = loginInput->text();
		QString password = passwordInput->text();
		QString confirmPassword = confirmPasswordInput->text();

		if (login.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
			QMessageBox::warning(this, "Registration error", "All fields must be filled in.");
			return;
		}

		if (password != confirmPassword) {
			QMessageBox::warning(this, "Registration error", "Passwords don't match.");
			return;
		}

		emit registrationRequest(login, password, confirmPassword);
	}

	void RegistrationWidget::onLoginButtonClicked() {
		loginInput->clear();
		passwordInput->clear();
		confirmPasswordInput->clear();

		emit showLoginWindow();
	}
}
