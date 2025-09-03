#include "editclientdialog.h"
#include "../ui/ui_editclientdialog.h"

EditClientDialog::EditClientDialog(const Client& client, const QList<QString>& existingFamilies, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditClientDialog)
{
    ui->setupUi(this);
    
    // Populate the form with current client data
    ui->nameLineEdit->setText(client.name());
    ui->surnameLineEdit->setText(client.surname());
    
    // Populate the family combo box
    ui->familyComboBox->addItems(existingFamilies);
    ui->familyComboBox->setCurrentText(client.family());
    
    setWindowTitle(QString("Edit Client - %1 %2").arg(client.name(), client.surname()));
}

EditClientDialog::~EditClientDialog()
{
    delete ui;
}

QString EditClientDialog::getName() const {
    return ui->nameLineEdit->text();
}

QString EditClientDialog::getSurname() const {
    return ui->surnameLineEdit->text();
}

QString EditClientDialog::getFamily() const {
    return ui->familyComboBox->currentText();
}