#include "clientdetaildialog.h"
#include "../ui/ui_clientdetaildialog.h"
#include "newborrowdialog.h"
#include "editclientdialog.h"
#include <QApplication>
#include <QInputDialog>
#include <QStyle>

ClientDetailDialog::ClientDetailDialog(const Client& client, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ClientDetailDialog)
    , m_client(client)
    , m_library(library)
{
    ui->setupUi(this);
    setupUI();
    loadBorrowRecords();
    updateBorrowTable();

    setWindowTitle(QString("Client Details - %1 %2").arg(m_client.name(), m_client.surname()));
    resize(800, 600);
}

ClientDetailDialog::~ClientDetailDialog()
{
    delete ui;
}

void ClientDetailDialog::setupUI()
{
    // Populate the info table with client details
    ui->infoTable->setRowCount(1);
    ui->infoTable->setColumnCount(3);
    ui->infoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->infoTable->verticalHeader()->setVisible(false);

    QTableWidgetItem* nameItem = new QTableWidgetItem(m_client.name());
    ui->infoTable->setItem(0, 0, nameItem);

    QTableWidgetItem* surnameItem = new QTableWidgetItem(m_client.surname());
    ui->infoTable->setItem(0, 1, surnameItem);

    QTableWidgetItem* familyItem = new QTableWidgetItem(m_client.family());
    ui->infoTable->setItem(0, 2, familyItem);

    // Make the info table not editable
    ui->infoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Connect double-click on family cell to the slot

    // Setup table headers for borrowed books
    ui->borrowTable->setColumnCount(7);
    QStringList headers;
    headers << "Book Name" << "Book Author" << "Book ID" << "Borrow Date"
            << "Return Date" << "Extend" << "Return";
    ui->borrowTable->setHorizontalHeaderLabels(headers);
    ui->borrowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->borrowTable->verticalHeader()->setVisible(false);
}

void ClientDetailDialog::on_infoTable_cellDoubleClicked(int row, int column) const
{
    if (column == 2) { // Family column
        if (m_library) {
            m_library->on_familyListWidget_doubleClicked(this->m_client); // Pass a dummy index
        }
    }
}

void ClientDetailDialog::loadBorrowRecords()
{
    m_borrowRecords = m_library->getBorrowRecordsByClientId(m_client.id());
}

void ClientDetailDialog::updateBorrowTable()
{
    ui->borrowTable->setRowCount(m_borrowRecords.size());

    for (int i = 0; i < m_borrowRecords.size(); ++i) {
        const auto& record = m_borrowRecords.at(i);
        const Book* book = m_library->getBookById(record.bookId);
        if (!book) continue; // Skip if book not found
        // Populate table items
        ui->borrowTable->setItem(i, 0, new QTableWidgetItem(book->title()));
        ui->borrowTable->setItem(i, 1, new QTableWidgetItem(book->author()));
        ui->borrowTable->setItem(i, 2, new QTableWidgetItem(record.bookId));
        ui->borrowTable->setItem(i, 3, new QTableWidgetItem(record.borrowDate.toString("dd/MM/yyyy")));
        ui->borrowTable->setItem(i, 4, new QTableWidgetItem(record.returnDate.toString("dd/MM/yyyy")));

        if (record.isReturned) {
            ui->borrowTable->setItem(i, 5, new QTableWidgetItem("Returned"));
            ui->borrowTable->setItem(i, 6, new QTableWidgetItem("Returned"));
        } else {
            // Add extend button
            QPushButton* extendButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_ArrowRight), "");
            extendButton->setToolTip("Extend borrow time");
            extendButton->setProperty("recordId", record.id);
            connect(extendButton, &QPushButton::clicked, this, &ClientDetailDialog::handleExtendBorrowClicked);
            ui->borrowTable->setCellWidget(i, 5, extendButton);

            // Add return button
            QPushButton* returnButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton), "");
            returnButton->setToolTip("Return this book");
            returnButton->setProperty("recordId", record.id);
            connect(returnButton, &QPushButton::clicked, this, &ClientDetailDialog::handleReturnBookClicked);
            ui->borrowTable->setCellWidget(i, 6, returnButton);
        }
    }
}

void ClientDetailDialog::on_newBorrowButton_clicked()
{
    NewBorrowDialog dialog(m_library->allBooks(), this);
    if (dialog.exec() == Accepted)
    {
        loadBorrowRecords();
        updateBorrowTable();
    }

}

void ClientDetailDialog::on_editInfoButton_clicked()
{
    EditClientDialog dialog(m_client.id(), m_library->allFamilies(), this);
    if (dialog.exec() == Accepted) {
        m_client = m_library->getClientById(m_client.id());
        setupUI(); // Re-populate the info table with updated data
    }
}

void ClientDetailDialog::on_saveCloseButton_clicked()
{
    accept();
}

void ClientDetailDialog::handleReturnBookClicked()
{
    const auto* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    if (const int recordId = button->property("recordId").toInt(); m_library->returnBook(recordId)) {
        loadBorrowRecords();
        updateBorrowTable();
        QMessageBox::information(this, "Success", "Book returned successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Could not return book.");
    }
}

void ClientDetailDialog::handleExtendBorrowClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int recordId = button->property("recordId").toInt();

    // Find the book record for context
    BorrowRecord* record = nullptr;
    for (BorrowRecord& r : m_borrowRecords) {
        if (r.id == recordId) {
            record = &r;
            break;
        }
    }

    if (!record) {
        QMessageBox::warning(this, "Error", "Could not find borrow record.");
        return;
    }
    Book* book = m_library->getBookById(record->bookId);
    if (!book)
    {
        QMessageBox::warning(this, "Error", "Could not find borrow book.");
        return;
    }
    bool ok;
    QString bookTitle =book->title();
    const int days = QInputDialog::getInt(this, "Extend Borrow Time",
        QString("Extend '%1' by how many days?\n\nCurrent return date: %2")
        .arg(bookTitle, record->returnDate.toString("dd/MM/yyyy")),
        7, 1, 365, 1, &ok);

    if (ok) {
        if (m_library->extendBorrowTime(recordId, days)) {
            loadBorrowRecords();
            updateBorrowTable();

            // Find updated record to show new date
            for (const BorrowRecord& r : m_borrowRecords) {
                if (r.id == recordId) {
                    QMessageBox::information(this, "Success",
                        QString("Borrow time extended by %1 days!\n\nNew return date: %2")
                        .arg(days).arg(r.returnDate.toString("dd/MM/yyyy")));
                    break;
                }
            }
        } else {
            QMessageBox::warning(this, "Error", "Could not extend borrow time.");
        }
    }
}
