#include "windows/clientDetailDialog.h"
#include "../ui/ui_clientdetaildialog.h"
#include "windows/newborrowdialog.h"
#include "windows/editclientdialog.h"
#include <QApplication>
#include <QInputDialog>
#include <QStyle>
#include <QDebug>

#include "windowManager.h"
#include "windows/familyviewdialog.h"

ClientDetailDialog::ClientDetailDialog(const Client& client, Library* library, QWidget *parent)
    : AbstractWindow(parent)
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

void ClientDetailDialog::handleEvent(EventType event)
{
    loadBorrowRecords();
    updateBorrowTable();
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

    // Setup table headers for borrowTable
    ui->borrowTable->setColumnCount(7);
    QStringList headers;
    headers << "Book Name" << "Book Author" << "Book ID" << "Borrow Date"
            << "Return Date" << "Status" << "Actions";
    ui->borrowTable->setHorizontalHeaderLabels(headers);
    ui->borrowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->borrowTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->borrowTable->verticalHeader()->setVisible(false);
    ui->borrowTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->borrowTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->borrowTable->setSelectionMode(QAbstractItemView::NoSelection);
}

void ClientDetailDialog::loadBorrowRecords()
{
    // PROBLEM 1 FIX: Clear the list before loading new records to prevent duplicates.
    m_borrowRecords.clear();
    m_borrowRecords = m_library->getBorrowRecordsByClientId(m_client.id());
}

void ClientDetailDialog::updateBorrowTable()
{
    // PROBLEM 2 FIX: Clear the table before populating it to ensure
    // the UI is a true reflection of the current data.
    ui->borrowTable->setRowCount(0);
    ui->borrowTable->setRowCount(m_borrowRecords.size());
    
    for (int row = 0; row < m_borrowRecords.size(); ++row) {
        const BorrowRecord& record = m_borrowRecords.at(row);
        Book* book = m_library->getBookById(record.bookId);
        if (!book)
        {
            qDebug() << "Could not find book with ID:" << record.bookId;
            continue; // Skip this record if the book is not found
        }
        QString bookTitle = book->title();
        QString bookAuthor = book->author();
        // Populate the book information
        ui->borrowTable->setItem(row, 0, new QTableWidgetItem(bookTitle));
        ui->borrowTable->setItem(row, 1, new QTableWidgetItem(bookAuthor));
        ui->borrowTable->setItem(row, 2, new QTableWidgetItem(record.bookId));
        ui->borrowTable->setItem(row, 3, new QTableWidgetItem(record.borrowDate.toString("dd/MM/yyyy")));
        ui->borrowTable->setItem(row, 4, new QTableWidgetItem(record.returnDate.toString("dd/MM/yyyy")));
        
        // Add status and actions
        QTableWidgetItem* statusItem = new QTableWidgetItem(record.isReturned ? "Returned" : "Borrowed");
        ui->borrowTable->setItem(row, 5, statusItem);

        QWidget* actionWidget = new QWidget(this);
        QHBoxLayout* layout = new QHBoxLayout(actionWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(5);

        if (!record.isReturned) {
            QPushButton* returnButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogYesButton), "Return");
            returnButton->setToolTip("Return this book");
            returnButton->setProperty("recordId", record.id);
            connect(returnButton, &QPushButton::clicked, this, &ClientDetailDialog::on_returnBookButton_clicked);
            layout->addWidget(returnButton);

            QPushButton* extendButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_ArrowRight), "Extend");
            extendButton->setToolTip("Extend the borrow time");
            extendButton->setProperty("recordId", record.id);
            connect(extendButton, &QPushButton::clicked, this, &ClientDetailDialog::on_extendBorrowButton_clicked);
            layout->addWidget(extendButton);
        }

        actionWidget->setLayout(layout);
        ui->borrowTable->setCellWidget(row, 6, actionWidget);
    }
}

void ClientDetailDialog::on_newBorrowButton_clicked()
{
    NewBorrowDialog newBorrowDialog(m_library->getAvailableBooks(), this);
    int res = WindowManager::instance().startNewWindow(&newBorrowDialog);
    if (res == Accepted) {
        Book selectedBook = newBorrowDialog.getSelectedBook();
        BorrowRecord newRecord;
        newRecord.bookId = selectedBook.id();
        newRecord.clientId = m_client.id();
        newRecord.borrowDate = QDate::currentDate();
        newRecord.returnDate = newBorrowDialog.getReturnDate(); // Default 2
        newRecord.isReturned = false;
        if  (const TransactionResult result=m_library->borrowBook(m_client.id(), newRecord); result == TransactionResult::Success) {
            QMessageBox::information(this, "Success", "Book borrowed successfully!");
            // Reload and update the table after a successful borrow
            loadBorrowRecords();
            WindowManager::instance().handleEvent(EventType::BooksUpdated);
            emit updateBorrowTable();
        } else {
            QString errorMsg;
            QString level = "Error";
            switch (result)
            {
            case  TransactionResult::Failure_NotAvailableBook:
                errorMsg = "The selected book is not available.";
                level = "Warning";
                break;
            case  TransactionResult::Failure_NoCopies:
                errorMsg = "No copies of the selected book are available.";
                level="Error";
                break;
            case TransactionResult::Failure_AlreadyBorrowed:
                errorMsg = "You have already borrowed this book and not returned it yet.";
                level="Warning";
                break;
            case TransactionResult::Failure_BookNotFound:
                errorMsg = "The selected book was not found.";
                break;
            case TransactionResult::Failure_ClientNotFound:
                errorMsg = "Client not found.";
                break;
            case TransactionResult::Failure_DBFailed:
                errorMsg = "A database error occurred while processing the request.";
                break;
            default:
                errorMsg=" An unknown error occurred.";
                break;
            }
            QMessageBox::warning(this, level, errorMsg);
        }
    }
}

void ClientDetailDialog::on_editInfoButton_clicked()
{
    if (EditClientDialog editClientDialog(m_client, this->m_library->allFamilies(),this); editClientDialog.exec() == Accepted) {
        Client updatedClient = this->m_client;
        if (m_library->updateClient(m_client.id(), updatedClient.name(), updatedClient.surname(), updatedClient.family())) {
            m_client = updatedClient;
            setupUI();
            QMessageBox::information(this, "Success", "Client information updated successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to update client information.");
        }
    }
}

void ClientDetailDialog::on_saveCloseButton_clicked()
{
    this->accept();
}

void ClientDetailDialog::on_returnBookButton_clicked()
{
    auto* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    if (int recordId = button->property("recordId").toInt(); m_library->returnBook(recordId) == TransactionResult::Success) {
        QMessageBox::information(this, "Success", "Book returned successfully!");
        // Reload and update the table after a successful return
        loadBorrowRecords();
        emit updateBorrowTable();
    } else {
        QMessageBox::warning(this, "Error", "Failed to return book. It may have already been returned.");
    }
}

void ClientDetailDialog::on_extendBorrowButton_clicked()
{
    const QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int recordId = button->property("recordId").toInt();

    // Find the book record for context
    const BorrowRecord* record = nullptr;
    for (const BorrowRecord r : m_borrowRecords) {
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
            QMessageBox::warning(this, "Error", "Failed to extend borrow time.");
        }
    }
}

void ClientDetailDialog::on_infoTable_cellDoubleClicked(int row, int column)
{
    if (row != 0 || column != 2) return; // Only respond to family name cell
    FamilyViewDialog family(this);
    family.setFamilyInfo(m_client.family(), m_library->getClientsByFamilyName(m_client.family()));
    family.exec();
}

void ClientDetailDialog::onBooksUpdated()
{
    loadBorrowRecords();
}
