//
// Created by Tomer on 04/09/2025.
//

#ifndef ABSTRACTWINDOW_H
#define ABSTRACTWINDOW_H
#include <QDialog>


enum class EventType {
    None,
    BooksUpdated,
    ClientsUpdated,
    FamiliesUpdated,
    TransactionsUpdated
};

/**
 * @class AbstractWindow
 * @brief An abstract base class for all application windows (QDialogs).
 *
 * This class enforces a common interface for event handling across all
 * dialogs by requiring an implementation of the handleEvent function.
 */
class AbstractWindow : public QDialog
{

    Q_OBJECT

public:
    explicit AbstractWindow()  {}
    explicit AbstractWindow(QWidget* widget) : QDialog(widget) {}
    ~AbstractWindow() override {}

    /**
     * @brief A pure virtual function to handle events.
     * @param event The type of event to handle, from the EventType enum.
     *
     * Subclasses must provide an implementation for this function.
     */
    virtual void handleEvent(EventType event) = 0;
};





#endif //ABSTRACTWINDOW_H
