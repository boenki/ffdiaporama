#ifndef CCUSTOMBLOCKTABLE_H
#define CCUSTOMBLOCKTABLE_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"
#include "../sharedfiles/_QCustomDialog.h"

// Specific inclusions
#include "_Diaporama.h"
#include "_ApplicationDefinitions.h"

#include <QTableWidget>

class cCustomBlockTable : public QTableWidget {
Q_OBJECT
public:
    cBaseApplicationConfig  *ApplicationConfig;
    cCompositionList        *CompositionList;           // Link to Composition List
    QImage                  TextIcon;

    explicit                cCustomBlockTable(QWidget *parent = 0);

    virtual void            resizeEvent(QResizeEvent *);
    virtual void            mouseDoubleClickEvent(QMouseEvent *);
    virtual void            mouseReleaseEvent(QMouseEvent *);

signals:
    void                    DoubleClickEvent(QMouseEvent *ev);
    void                    RightClickEvent(QMouseEvent *ev);

public slots:

};

#endif // CCUSTOMBLOCKTABLE_H
