#include "newborrowdialog.h"
#include "../ui/ui_newborrowdialog.h"
#include <QMessageBox>



NewBorrowDialog::NewBorrowDialog(const QList<Book>& availableBooks, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewBorrowDialog)
    , m_availableBooks(availableBooks)
{
    ui->setupUi(this);
    
    // Populate the book list
    for (const Book& book : availableBooks) {
        QString bookText = QString("%1 by %2 (%3) - %4 copies available")
                          .arg(book.title(), book.author(), QString::number(book.year()), QString::number(book.copies()));
        ui->bookListWidget->addItem(bookText);
    }
    
    // Set default return date to 2 weeks from today
    ui->returnDateEdit->setDate(QDate::currentDate().addDays(14));
    ui->returnDateEdit->setMinimumDate(QDate::currentDate().addDays(1));
    ui->returnDateEdit->setMaximumDate(QDate::currentDate().addDays(365));
    
    // Connect signals
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &NewBorrowDialog::on_buttonBox_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &NewBorrowDialog::on_buttonBox_rejected);
}

NewBorrowDialog::~NewBorrowDialog()
{
    delete ui;
}

Book NewBorrowDialog::getSelectedBook() const
{
    int selectedRow = ui->bookListWidget->currentRow();
    if (selectedRow >= 0 && selectedRow < m_availableBooks.size()) {
        return m_availableBooks[selectedRow];
    }
    return Book("", "", "",0, 0); // Invalid book
}

QDate NewBorrowDialog::getReturnDate() const
{
    return ui->returnDateEdit->date();
}

void NewBorrowDialog::on_buttonBox_accepted()
{
    if (ui->bookListWidget->currentRow() == -1) {
        QMessageBox::warning(this, "No Selection", "Please select a book to borrow.");
        return;
    }
    
    if (ui->returnDateEdit->date() <= QDate::currentDate()) {
        QMessageBox::warning(this, "Invalid Date", "Return date must be in the future.");
        return;
    }
    
    accept();
}

void NewBorrowDialog::on_buttonBox_rejected()
{
    reject();
}