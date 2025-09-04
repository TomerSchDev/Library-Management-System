//
// Created by Tomer on 04/09/2025.
//

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H
#include <qobject.h>

#include "windows/abstractWindow.h"


class WindowManager : public QObject
{
    Q_OBJECT

public:
        static WindowManager& instance()
        {
            static WindowManager instance;
            return instance;
        }
        void startNewWindow(AbstractWindow* window);
        void handleEvent(EventType event);
private:
        WindowManager();
        QList<AbstractWindow*> m_windows;
        void onWindowClosed(AbstractWindow* window);
};



#endif //WINDOWMANAGER_H
