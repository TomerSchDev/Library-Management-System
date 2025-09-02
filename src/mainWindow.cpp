#include "mainwindow.h"
#include "../ui/ui_mainwindow.h"
#include "addbookdialog.h"
#include "addclientdialog.h"
#include "familyviewdialog.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&_library, &Library::booksUpdated, this, &MainWindow::updateBookList);
    connect(&_library, &Library::clientsUpdated, this, &MainWindow::updateClientList);
    connect(&_library, &Library::familiesUpdated, this, &MainWindow::updateFamilyList);


    updateBookList();
    updateClientList();
    updateFamilyList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateBookList() {
    ui->bookListWidget->clear();
    const QList<Book>& books = _library.allBooks();
    for (const Book& book : books) {
        ui->bookListWidget->addItem(book.toString());
    }
}

void MainWindow::updateClientList() {
    ui->clientListWidget->clear();
    const QList<Client>& clients = _library.allClients();
    for (const Client& client : clients) {
        ui->clientListWidget->addItem(client.toString());
    }
}

void MainWindow::updateFamilyList() {
    ui->familyListWidget->clear();
    const QList<QString>& families = _library.allFamilies();
    for (const QString& family : families) {
        ui->familyListWidget->addItem(family);
    }
}

void MainWindow::on_addBookButton_clicked()
{
    AddBookDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        _library.addBook(dialog.getTitle(), dialog.getAuthor(), dialog.getYear(), dialog.getCopies());
    }
}

void MainWindow::on_removeBookButton_clicked()
{
    QListWidgetItem* item = ui->bookListWidget->currentItem();
    if (item) {
        int index = ui->bookListWidget->row(item);
        _library.removeBook(index);
    } else {
        QMessageBox::warning(this, "No Selection", "Please select a book to remove.");
    }
}

void MainWindow::on_addCopiesButton_clicked()
{
    QListWidgetItem* item = ui->bookListWidget->currentItem();
    if (item) {
        int index = ui->bookListWidget->row(item);
        _library.addCopies(index, 1);
    } else {
        QMessageBox::warning(this, "No Selection", "Please select a book to add copies to.");
    }
}

void MainWindow::on_addClientButton_clicked()
{
    AddClientDialog dialog(_library.allFamilies(), this);
    if (dialog.exec() == QDialog::Accepted) {
        _library.addClient(dialog.getName(), dialog.getSurname(), dialog.getFamily());
    }
}

void MainWindow::on_familyListWidget_doubleClicked(const QModelIndex &index)
{
    QString familyName = index.data().toString();
    QList<Client> clients = _library.getClientsByFamilyName(familyName);

    FamilyViewDialog dialog(this);
    dialog.setFamilyInfo(familyName, clients);
    dialog.exec();
}
