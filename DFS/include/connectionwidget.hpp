#pragma once

#include <QPushButton>
#include <QLineEdit>

#include "networkmanager.hpp"

namespace SHIZ{
	class ConnectionWidget : public QWidget {
		Q_OBJECT

		private:
			NetworkManager* networkManager;

			QLineEdit* hostInput;
			QLineEdit* portInput;

			QPushButton* enterButton;
			QPushButton* quitButton;

			Logger* logger;

		public:
			ConnectionWidget(Logger* logger, NetworkManager* manager, QWidget* parent = nullptr);

		signals:
			void ConnectionSuccessful(const QString &host, quint16 port);
			void requestConnection(const QString& host, quint16 port);

		public slots:
			void onConnectionResult(bool success, const QString& message);

		private slots:
			void onEnterButtonClicked();
	};

}
