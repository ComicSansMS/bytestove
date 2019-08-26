#include <filestove/ui/pathlist_widget.hpp>

#pragma warning(push)
#pragma warning(disable: 4251)
#include <QKeyEvent>
#pragma warning(pop)

namespace filestove::ui {

PathlistWidget::PathlistWidget(QWidget* parent)
    :QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::InternalMove);
}

void PathlistWidget::keyPressEvent(QKeyEvent* evt)
{
    if (evt->key() == Qt::Key_Delete) {
        removeSelectedItems();
    }
    return QListWidget::keyPressEvent(evt);
}

void PathlistWidget::removeSelectedItems()
{
    for (QListWidgetItem* to_remove : selectedItems()) {
        delete to_remove;
    }
}

}
