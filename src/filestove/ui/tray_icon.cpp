#include <filestove/ui/tray_icon.hpp>

#include <filestove/ui/about_dialog.hpp>

namespace filestove::ui {

TrayIcon::TrayIcon()
    :m_iconBase(":/filestove_base.ico"),
     m_iconBurning(":/filestove_burning.ico"),
     m_iconDone(":/filestove_done.ico"),
     m_menu(std::make_unique<QMenu>())
{
    m_menu->addAction("About", this, &TrayIcon::showAboutDialog);
    m_menu->addSeparator();
    m_menu->addAction("Quit", this, [this]() { emit requestQuit(); });

    setIcon(m_iconBase);
    setContextMenu(m_menu.get());
    show();

    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::onActivation);
}

void TrayIcon::ignite()
{
    setIcon(m_iconBurning);
}

void TrayIcon::extinguish()
{
    setIcon(m_iconBase);
}

void TrayIcon::finished()
{
    setIcon(m_iconDone);
}

void TrayIcon::showAboutDialog()
{
    AboutDialog dialog;
    dialog.exec();
}

void TrayIcon::onActivation(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        emit showStoveWidget();
    }
}

}
