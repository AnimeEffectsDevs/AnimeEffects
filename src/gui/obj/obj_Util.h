#ifndef GUI_OBJ_UTIL_H
#define GUI_OBJ_UTIL_H

#include "gui/ObjectTreeWidget.h"
#include "util/TreePos.h"
#include <QTreeWidgetItem>

namespace gui {
namespace obj {

    class Util {
    public:
        static QTreeWidgetItem* findItem(ObjectTreeWidget& aTree, const util::TreePos& aPos);
        static QTreeWidgetItem* removeItem(ObjectTreeWidget& aTree, const util::TreePos& aPos);
        static void insertItem(ObjectTreeWidget& aTree, const util::TreePos& aPos, QTreeWidgetItem& aItem);
    };

} // namespace obj
} // namespace gui

#endif // GUI_OBJ_UTIL_H
