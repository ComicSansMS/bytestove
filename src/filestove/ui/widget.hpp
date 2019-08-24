#ifndef FILESTOVE_INCLUDE_GUARD_UI_WIDGET_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_WIDGET_HPP_

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QWidget>
#pragma warning(pop)

namespace filestove::ui {

class StoveWidget : public QWidget {
    Q_OBJECT;
public:
    StoveWidget();

};

}

#endif
