#include <QApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QSignalMapper>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFrame>
#include <QToolButton>
#include <QHBoxLayout>
#include <QImageReader>
#include <QDir>
#include <QDebug>

#include "../PathUtils.h"
#include "Property.h"

//=================================================================================================
PropertyEditor::PropertyEditor( QWidget *parent ):
  QTableWidget( parent ),
  currentProperty(0)
{
  setColumnCount(2);
  setSelectionMode( QAbstractItemView::NoSelection );
  setEditTriggers( QAbstractItemView::NoEditTriggers );
  setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  horizontalHeader()->setStretchLastSection( true );
  setAlternatingRowColors( true );
  setTabKeyNavigation( true );
  setHorizontalHeaderLabels( QStringList() << tr("Property") << tr("Value") );

  #ifdef Qt5
    horizontalHeader()->setSectionsClickable( false );
    horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch );
  #else
    horizontalHeader()->setClickable( false );
    horizontalHeader()->setResizeMode( 1, QHeaderView::Stretch );
  #endif

  verticalHeader()->hide();

  QImageReader reader;

  for (int i = 0; i < reader.supportedImageFormats().count(); ++i)
    imageFormats << QString("*.%1").arg( reader.supportedImageFormats().at(i).data() );

  sm = new QSignalMapper( this );
  connect( sm, SIGNAL(mapped(QWidget*)), this, SLOT(mapped(QWidget*)) );
}
//=================================================================================================
PropertyEditor::~PropertyEditor()
{
}
//=================================================================================================
void PropertyEditor::setCurrentProperty( CPropertyList *p )
{
  currentProperty = p;
}
//=================================================================================================
void PropertyEditor::showCurrent()
{
  if (!currentProperty)
    return;

  blockSignals( true );

  int rows = rowCount();

  if (rows > 0)
  {
    for (int i = rows - 1; i >= 0; --i)
    {
      QWidget *wgt = cellWidget( i, 1 );
      disconnect( wgt, 0, 0, 0 );

      delete wgt;
      wgt = 0;

      removeRow(i);
    }
  }

  clear();
  setHorizontalHeaderLabels( QStringList() << tr("Property") << tr("Value") );

  for (QMap <QString, CProperty>::const_iterator it = currentProperty->PropertyList.constBegin(); it != currentProperty->PropertyList.constEnd(); ++it)
  {
    int row = addRow();

    QTableWidgetItem *property = new QTableWidgetItem( it.key() );
    setItem( row, 0, property );

    CProperty p = it.value();

    switch (p.Type)
    {
      case PropInt:
      {
        QSpinBox *sb = new QSpinBox( this );
        sb->setButtonSymbols( QAbstractSpinBox::NoButtons );
        sb->setSingleStep(1);
        sb->setRange( 0, 9999999 );
        sb->setValue( *p.Int );
        sb->setObjectName( it.key() );
        connect( sb, SIGNAL(valueChanged(int)), sm, SLOT(map()) );
        sm->setMapping( sb, sb );
        setCellWidget( row, 1, sb );
      }
      break;

      case PropString:
      {
        QLineEdit *le = new QLineEdit( this );
        le->setText( *p.String );
        le->setObjectName( it.key() );
        le->setWhatsThis("string");
        connect( le, SIGNAL(textChanged(QString)), sm, SLOT(map()) );
        sm->setMapping( le, le );
        setCellWidget( row, 1, le );
      }
      break;

      case PropFloat:
      {
        QDoubleSpinBox *sb = new QDoubleSpinBox( this );
        sb->setButtonSymbols( QAbstractSpinBox::NoButtons );
        sb->setSingleStep( 0.1 );
        sb->setRange( 0.0, 9999999.0 );
        sb->setValue( *p.Float );
        sb->setObjectName( it.key() );
        connect( sb, SIGNAL(valueChanged(double)), sm, SLOT(map()) );
        sm->setMapping( sb, sb );
        setCellWidget( row, 1, sb );
      }
      break;

      case PropFile:
      {
        QFrame *fr = new QFrame( this );
        QHBoxLayout *hl = new QHBoxLayout( fr );

        QLineEdit *le = new QLineEdit( fr );
        QString text = *(p.File);
        le->setText( text.replace( "\"", "" ) );
        le->setObjectName( it.key() );
        le->setWhatsThis("file");
        connect( le, SIGNAL(textChanged(QString)), sm, SLOT(map()) );
        sm->setMapping( le, le );

        QToolButton *tb = new QToolButton( fr );
        tb->setText("...");

        QSignalMapper *s = new QSignalMapper( fr );
        connect( tb, SIGNAL(clicked()), s, SLOT(map()) );

        s->setMapping( tb, le );
        connect( s, SIGNAL(mapped(QWidget*)), this, SLOT(selectFile(QWidget*)) );

        hl->addWidget( le, 1 );
        hl->addWidget( tb );
        hl->setMargin(0);
        hl->setSpacing(0);

        setCellWidget( row, 1, fr );
      }
      break;

      case PropEnum:
      {
        QComboBox *cb = new QComboBox( this );
        cb->setWhatsThis("cubemap");
        cb->addItem( "", -1 );
        cb->addItem( "FRONT", 0 );
        cb->addItem( "RIGHT", 1 );
        cb->addItem( "BACK", 2 );
        cb->addItem( "LEFT", 3 );
        cb->addItem( "TOP", 4 );
        cb->addItem( "BOTTOM", 5 );
        cb->setEditable( false );
        cb->setCurrentIndex( *p.Enum );
        cb->setObjectName( it.key() );
        connect( cb, SIGNAL(currentIndexChanged(int)), sm, SLOT(map()) );
        sm->setMapping( cb, cb );
        setCellWidget( row, 1, cb );
      }
      break;

      case PropCursors:
      {
        QComboBox *cb = new QComboBox( this );
        cb->setWhatsThis("cursors");

        int curIndex = -1;
        QString basePath = PathUtils::nativePath( qApp->applicationDirPath() + "/Data/resources/" );

        if (QDir( basePath ).exists())
        {
          cb->addItem( "", "" );

          QDir cursorsDir( basePath );
          QStringList entriesCursors = cursorsDir.entryList( QStringList() << "cursor_*.png", QDir::Files );

          for (int i = 0; i < entriesCursors.size(); ++i)
          {
            QString imageFile = basePath + entriesCursors.at(i);
            cb->addItem( QIcon( imageFile ),
                         entriesCursors.at(i).mid( 7, entriesCursors.at(i).length() - 11 ),
                         QString("pipmak.%1").arg( entriesCursors.at(i).mid( 7, entriesCursors.at(i).length() - 11 ) ) );

            if (!QString("pipmak.%1").arg( entriesCursors.at(i).mid( 7, entriesCursors.at(i).length() - 11 ) ).compare( *p.Cursors ))
              curIndex = i + 1;
          }
        }

        cb->setEditable( false );
        cb->setCurrentIndex( curIndex );
        cb->setObjectName( it.key() );
        connect( cb, SIGNAL(currentIndexChanged(int)), sm, SLOT(map()) );
        sm->setMapping( cb, cb );
        setCellWidget( row, 1, cb );
      }
      break;
    }
  }

  blockSignals( false );
}
//=================================================================================================
int PropertyEditor::addRow()
{
  int row = rowCount();
  insertRow( row );
  setRowHeight( row, static_cast<int>( rowHeight( row ) * 0.7 ) );

  return row;
}
//=================================================================================================
void PropertyEditor::mapped( QWidget *wgt )
{
  QString name = wgt->objectName();
  QString className = wgt->metaObject()->className();

  if (!className.compare("QLineEdit"))
  {
    emit propertyChanged( currentProperty->getData(), name, ((QLineEdit *)wgt)->text() );

    if (!((QLineEdit *)wgt)->whatsThis().compare("string"))
    {
      CProperty &p = currentProperty->PropertyList[ name ];
      *p.String = ((QLineEdit *)wgt)->text();
    }
    else
    {
      CProperty &p = currentProperty->PropertyList[ name ];
      *p.File = ((QLineEdit *)wgt)->text();
    }
  }
  else if (!className.compare("QComboBox"))
  {
    emit propertyChanged( currentProperty->getData(), name, ((QComboBox *)wgt)->currentIndex() );

    CProperty &p = currentProperty->PropertyList[ name ];

    if (!((QComboBox *)wgt)->whatsThis().compare("cubemap"))
      *p.Enum = ((QComboBox *)wgt)->currentIndex();
    else
      *p.Cursors = ((QComboBox *)wgt)->itemData( ((QComboBox *)wgt)->currentIndex() ).toString();
  }
  else if (!className.compare("QSpinBox"))
  {
    emit propertyChanged( currentProperty->getData(), name, ((QSpinBox *)wgt)->value() );

    CProperty &p = currentProperty->PropertyList[ name ];
    *p.Int = ((QSpinBox *)wgt)->value();
  }
  else if (!className.compare("QDoubleSpinBox"))
  {
    emit propertyChanged( currentProperty->getData(), name, ((QDoubleSpinBox *)wgt)->value() );

    CProperty &p = currentProperty->PropertyList[ name ];
    *p.Float = ((QSpinBox *)wgt)->value();
  }
}
//=================================================================================================
void PropertyEditor::selectFile( QWidget *lineEdit )
{
  QString fileName = QFileDialog::getOpenFileName( this, tr("Select Image"), qApp->applicationDirPath(),
                                                   QString( tr("Image Files (%1)") ).arg( imageFormats.join(" ") ) );

  if (!fileName.isEmpty())
    ((QLineEdit *)lineEdit)->setText( PathUtils::getName( fileName ) );
}
//=================================================================================================
