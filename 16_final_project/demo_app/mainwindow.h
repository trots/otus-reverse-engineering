#pragma once

#include <QWidget>

class QLineEdit;
class QLabel;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void OnCheckClicked();

private:
    QLineEdit* keyEdit;
    QLabel* resultLabel;
};
