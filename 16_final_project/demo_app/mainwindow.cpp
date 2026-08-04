#include "mainwindow.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "../license_core/license_core.h"

MainWindow::MainWindow(QWidget* parent) : 
    QWidget(parent) 
{
    keyEdit = new QLineEdit(this);
    keyEdit->setPlaceholderText("XXXX-XXXX-XXXX-XXXX");

    auto* checkButton = new QPushButton("Check", this);
    resultLabel = new QLabel(this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(keyEdit);
    layout->addWidget(checkButton);
    layout->addWidget(resultLabel);
    
    connect(checkButton, &QPushButton::clicked, this, &MainWindow::OnCheckClicked);
}

void MainWindow::OnCheckClicked()
{
    QByteArray keyBytes = keyEdit->text().toUtf8();
    LicenseInfo info = ParseLicenseKey(keyBytes.constData(), keyBytes.size());

    if (info.valid)
    {
        resultLabel->setText(QString("Valid. ProductId=%1 Expiration=%2")
                                  .arg(info.productId)
                                  .arg(info.expirationDate));
    }
    else
    {
        resultLabel->setText("Invalid key");
    }
}
