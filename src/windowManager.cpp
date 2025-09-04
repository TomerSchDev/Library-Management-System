//
// Created by Tomer on 04/09/2025.
//

#include "windowManager.h"
#include <QObject>

#include <QApplication>


void WindowManager::startNewWindow(AbstractWindow* window)
{
    if (window && !m_windows.contains(window)) {
        m_windows.append(window);
        connect(window, &QObject::destroyed, this, [this, window]() {
        qDebug() << "Window destroyed. Removing from manager list.";
        m_windows.removeOne(window);
        });
        window->show();

    }
}

void WindowManager::handleEvent(const EventType event)
{
    for (auto& window : m_windows) {
        if (window) {
            window->handleEvent(event);
        }
    }
}

WindowManager::WindowManager()
{
    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        qDebug() << "Application is about to quit. Cleaning up all managed windows.";
        for (QWidget* window : m_windows) {
            window->close(); // This will trigger the deleteLater connection
        }
    });

}

void WindowManager::onWindowClosed(AbstractWindow* window)
{
    m_windows.removeAll(window);
    window->deleteLater();
}
