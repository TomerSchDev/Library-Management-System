#include "mainwindow.h"
#include "../ui/ui_mainwindow.h"
#include "../include/library.h"
#include "../include/addbookdialog.h"
#include "../include/addclientdialog.h"
#include "../include/familyviewdialog.h"
#include "../include/clientviewdialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), _library(Library::getInstance())
{
    ui->setupUi(this);

    updateBookList();
    updateClientList();
    updateFamilyList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateBookList()
{
    ui->bookListWidget->clear();
    for (const auto& book : _library->allBooks()) {
        ui->bookListWidget->addItem(book.toString());
    }
}

void MainWindow::updateClientList()
{
    ui->clientListWidget->clear();
    for (const auto& client : _library->allClients()) {
        ui->clientListWidget->addItem(client.toString());
    }
}

void MainWindow::updateFamilyList()
{
    ui->familyListWidget->clear();
    for (const auto& family : _library->allFamilies()) {
        ui->familyListWidget->addItem(family);
    }
}

void MainWindow::on_addBookButton_clicked()
{
    AddBookDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        _library->addBook(dialog.getTitle(), dialog.getAuthor(), dialog.getYear(), dialog.getCopies());
        updateBookList();
    }
}

void MainWindow::on_removeBookButton_clicked()
{
    QListWidgetItem* selectedItem = ui->bookListWidget->currentItem();
    if (selectedItem) {
        int index = ui->bookListWidget->row(selectedItem);
        _library->removeBook(index);
        updateBookList();
    }
}

void MainWindow::on_addCopiesButton_clicked()
{
    QListWidgetItem* selectedItem = ui->bookListWidget->currentItem();
    if (selectedItem) {
        bool ok;
        int numCopies = QInputDialog::getInt(this, "Add Copies", "Number of copies to add:", 1, 1, 1000, 1, &ok);
        if (ok) {
            int index = ui->bookListWidget->row(selectedItem);
            _library->addCopies(index, numCopies);
            updateBookList();
        }
    }
}

void MainWindow::on_addClientButton_clicked()
{
    AddClientDialog dialog(_library->allFamilies(),this);
    if (dialog.exec() == QDialog::Accepted) {
        _library->addClient(dialog.getName(), dialog.getSurname(), dialog.getFamily());
        updateClientList();
        updateFamilyList();
    }
}

void MainWindow::on_familyListWidget_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString familyName = index.data(Qt::DisplayRole).toString();
        FamilyViewDialog dialog(this);
        dialog.setFamilyInfo(familyName, _library->getClientsByFamily(familyName));
        dialog.exec();
    }
}

void MainWindow::on_clientListWidget_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString clientName = index.data(Qt::DisplayRole).toString();
        ClientViewDialog dialog(this);
        // Assuming clientName is the full string representation of the client
        // You'll need to find the actual Client object from the library
        QList<Client> allClients = _library->allClients();
        for (const auto& client : allClients) {
            if (client.toString() == clientName) {
                dialog.setClient(client);
                dialog.exec();
                return;
            }
        }
    }
}
