#include "addbookdialog.h"
#include "../ui/ui_addbookdialog.h"

AddBookDialog::AddBookDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddBookDialog)
{
    ui->setupUi(this);
}

AddBookDialog::~AddBookDialog()
{
    delete ui;
}

QString AddBookDialog::getTitle() const {
    return ui->titleLineEdit->text();
}

QString AddBookDialog::getAuthor() const {
    return ui->authorLineEdit->text();
}

int AddBookDialog::getYear() const {
    return ui->yearSpinBox->value();
}

int AddBookDialog::getCopies() const {
    return ui->copiesSpinBox->value();
}
