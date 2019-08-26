#ifndef FILESTOVE_INCLUDE_GUARD_UI_WIDGET_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_WIDGET_HPP_

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QWidget>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4251)
#include <QBoxLayout>
#include <QPushButton>
#pragma warning(pop)

namespace filestove::ui {

class PathlistWidget;

class StoveWidget : public QWidget {
    Q_OBJECT;
private:
    QVBoxLayout m_layout;
    PathlistWidget* m_list;
    QHBoxLayout m_buttonLayout;
    QPushButton* m_buttonAddFiles;
    QPushButton* m_buttonAddDirectory;
    QPushButton* m_buttonRemoveEntry;
public:
    StoveWidget();

public slots:
    void onAddFiles();
    void onAddDirectory();
    void onRemoveEntry();
};

}

#endif
