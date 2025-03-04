#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DatabaseManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadBooks();   // Load books into the table
    void on_borrowButton_clicked();
    void on_returnButton_clicked();

private:
    Ui::MainWindow *ui;
    DatabaseManager dbManager;
};

#endif // MAINWINDOW_H
