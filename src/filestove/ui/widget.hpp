#ifndef FILESTOVE_INCLUDE_GUARD_UI_WIDGET_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_WIDGET_HPP_

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QWidget>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4251)
#include <QBoxLayout>
#include <QListWidget>
#include <QPushButton>
#pragma warning(pop)

namespace filestove::ui {

class StoveWidget : public QWidget {
    Q_OBJECT;
private:
    QVBoxLayout m_layout;
    QListWidget* m_list;
    QHBoxLayout m_buttonLayout;
    QPushButton* m_buttonAddFiles;
    QPushButton* m_buttonAddDirectory;
public:
    StoveWidget();

public slots:
    void onAddFiles();
    void onAddDirectory();
};

}

#endif
