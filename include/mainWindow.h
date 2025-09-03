#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include "library.h"

// Forward declaration of the UI namespace
namespace Ui {
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addBookButton_clicked();
    void on_removeBookButton_clicked();
    void on_addCopiesButton_clicked();
    void on_addClientButton_clicked();
    void on_familyListWidget_doubleClicked(const QModelIndex &index);
    void on_clientListWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    Library* _library;

    void updateBookList();
    void updateClientList();
    void updateFamilyList();
};

#endif // MAINWINDOW_H
