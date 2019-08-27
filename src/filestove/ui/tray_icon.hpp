#ifndef FILESTOVE_INCLUDE_GUARD_UI_TRAY_ICON_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_TRAY_ICON_HPP_

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QSystemTrayIcon>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4251 5054)
#include <QMenu>
#pragma warning(pop)

#include <memory>

namespace filestove::ui {

class PathlistWidget;

class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT;
private:
    QIcon m_iconBase;
    QIcon m_iconBurning;
    QIcon m_iconDone;
    std::unique_ptr<QMenu> m_menu;
public:
    TrayIcon();
signals:
    void showStoveWidget();
    void requestQuit();
public slots:
    void ignite();
    void extinguish();
    void finished();
    void showAboutDialog();
    void onActivation(QSystemTrayIcon::ActivationReason reason);
};

}

#endif
