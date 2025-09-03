#include "../ui/ui_clientviewdialog.h"
#include "../include/clientviewdialog.h"

#include <qcoreevent.h>

#include "mainWindow.h"
#include "../include/library.h"

ClientViewDialog::ClientViewDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ClientViewDialog)
{
    ui->setupUi(this);

    // Install an event filter on the family name label
    ui->familyValueLabel->installEventFilter(this);
}

ClientViewDialog::~ClientViewDialog()
{
    delete ui;
}

void ClientViewDialog::setClient(const Client& client)
{
    ui->nameValueLabel->setText(client.name());
    ui->surnameValueLabel->setText(client.surname());
    ui->familyValueLabel->setText(client.family());

    // Clear and populate the borrowed books list
    ui->borrowedBooksListWidget->clear();
    for (const auto& book : client.borrowedBooks()) {
        ui->borrowedBooksListWidget->addItem(book.title());
    }
}


bool ClientViewDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui->familyValueLabel && event->type() == QEvent::MouseButtonDblClick) {
        Library* lib = Library::getInstance();
        if (lib) {
            lib->on_familyListWidget_doubleClicked_rep(ui->familyValueLabel->text(),this);
        }
        return true; // Event handled
    }
    return QDialog::eventFilter(obj, event);
}
