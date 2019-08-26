#ifndef FILESTOVE_INCLUDE_GUARD_UI_TRAY_ICON_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_TRAY_ICON_HPP_

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QSystemTrayIcon>
#pragma warning(pop)

namespace filestove::ui {

class PathlistWidget;

class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT;
private:
    QIcon m_iconBase;
    QIcon m_iconBurning;
    QIcon m_iconDone;
public:
    TrayIcon();
public slots:
    void ignite();
    void extinguish();
    void finished();
};

}

#endif
