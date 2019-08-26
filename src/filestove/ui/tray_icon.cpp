#include <filestove/ui/tray_icon.hpp>

namespace filestove::ui {

TrayIcon::TrayIcon()
    :m_iconBase(":/filestove_base.ico"),
     m_iconBurning(":/filestove_burning.ico"),
     m_iconDone(":/filestove_done.ico")
{
    setIcon(m_iconBase);
    show();
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

}
