#ifndef FILESTOVE_INCLUDE_GUARD_UI_PATHLIST_WIDGET_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_PATHLIST_WIDGET_HPP_

#pragma warning(push)
#pragma warning(disable: 4251 5054)
#include <QListWidget>
#pragma warning(pop)


namespace filestove::ui {

class PathlistWidget : public QListWidget {
    Q_OBJECT
private:
public:
    PathlistWidget(QWidget* parent);

    void keyPressEvent(QKeyEvent* evt) override;

    void removeSelectedItems();
};

}

#endif
