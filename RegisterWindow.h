#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QDialog>

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

private slots:
    void onRegisterButtonClicked();
    void onCalcleButtonClicked();

private:
    Ui::RegisterWindow *ui;
};

#endif // REGISTERWINDOW_H
