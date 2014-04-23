#include <QApplication>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QRadioButton>
#include <QStyleFactory>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QKeyEvent>

#include "../Singleton.h"
#include "../PathUtils.h"
#include "SettingsDialog.h"

//=================================================================================================
SettingsDialog::SettingsDialog( QWidget *parent ):
  QDialog( parent )
{
  QTabWidget *tabs = new QTabWidget( this );

  tabs->addTab( sceneWidget( tabs ), QIcon(":res/scene.png"), tr("Scene") );
  tabs->addTab( keyboardWidget( tabs ), QIcon(":res/keyboard.png"), tr("Keyboard") );
  tabs->addTab( resourcesWidget( tabs ), QIcon(":res/resource.png"), tr("Resources") );
  tabs->addTab( updateWidget( tabs ), QIcon(":res/update.png"), tr("Update") );
  tabs->addTab( styleWidget( tabs ), QIcon(":res/style.png"), tr("Style") );

  QPushButton *pbOK = new QPushButton( QIcon(":res/apply.png"), tr("OK"), this );
  pbOK->setFocus();
  connect( pbOK, SIGNAL(clicked()), this, SLOT(accept()) );

  QPushButton *pbCancel = new QPushButton( QIcon(":res/cancel.png"), tr("Cancel"), this );
  connect( pbCancel, SIGNAL(clicked()), this, SLOT(reject()) );

  QHBoxLayout *hl = new QHBoxLayout();
  hl->addWidget( pbOK );
  hl->addWidget( pbCancel );
  hl->setAlignment( Qt::AlignRight );

  QVBoxLayout *vl = new QVBoxLayout( this );
  vl->addWidget( tabs );
  vl->addLayout( hl );

  setWindowIcon( QIcon(":res/settings.png") );
  setWindowTitle( tr("Settings") );
}
//=================================================================================================
SettingsDialog::~SettingsDialog()
{
}
//=================================================================================================
bool SettingsDialog::isRotationEnabled()
{
  return enableRotation->isChecked();
}
//=================================================================================================
int SettingsDialog::getTimeToRotate()
{
  return timeToRotate->value();
}
//=================================================================================================
int SettingsDialog::getRotateSpeed()
{
  return rotateSpeed->value();
}
//=================================================================================================
QString SettingsDialog::getProxyHost() const
{
  return proxyHost->text().trimmed();
}
//=================================================================================================
int SettingsDialog::getProxyPort()
{
  return proxyPort->value();
}
//=================================================================================================
bool SettingsDialog::isProxyEnabled()
{
  return enableProxy->isChecked();
}
//=================================================================================================
QMap<QString, QString> SettingsDialog::getShortcuts() const
{
  QMap<QString, QString> res;

  for (int i = 0; i < twItems.count(); ++i)
  {
    QTreeWidgetItem *it = twItems.at(i);
    res.insert( it->whatsThis(0), it->text(1) );
  }

  return res;
}
//=================================================================================================
QWidget *SettingsDialog::sceneWidget( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QVBoxLayout *vl = new QVBoxLayout( wgt );

  enableRotation = new QCheckBox( tr("Enable scene rotation on idle") );
  enableRotation->setChecked( Singleton<Settings>::Ptr().getValue( "Scene", "Rotation" ).toInt() == 1 ? true : false );

  QGroupBox *gb = new QGroupBox( tr("Rotation Parameters"), wgt );
  QGridLayout *glGB = new QGridLayout( gb );

  timeToRotate = new QSpinBox( gb );
  timeToRotate->setButtonSymbols( QAbstractSpinBox::NoButtons );
  timeToRotate->setRange( 1, 3600 );
  timeToRotate->setValue( Singleton<Settings>::Ptr().getValue( "Scene", "Time" ).toInt() );

  rotateSpeed = new QSlider( gb );
  rotateSpeed->setOrientation( Qt::Horizontal );
  rotateSpeed->setRange( 1, 10 );
  rotateSpeed->setSingleStep(1);
  rotateSpeed->setValue( Singleton<Settings>::Ptr().getValue( "Scene", "Speed" ).toInt() );

  glGB->addWidget( new QLabel( tr("Idle Time (sec)"), gb ), 0, 0 );
  glGB->addWidget( timeToRotate, 0, 1 );
  glGB->addWidget( new QLabel( tr("Rotation Speed"), gb ), 1, 0 );
  glGB->addWidget( rotateSpeed, 1, 1 );
  glGB->setAlignment( Qt::AlignTop );

  gb->setEnabled( enableRotation->isChecked() );

  connect( enableRotation, SIGNAL(toggled(bool)), gb, SLOT(setEnabled(bool)) );

  vl->addWidget( enableRotation );
  vl->addWidget( gb );

  return wgt;
}
//=================================================================================================
QWidget *SettingsDialog::keyboardWidget( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QVBoxLayout *vl = new QVBoxLayout( wgt );

  // Begin Keyboard Shortcuts
  QGroupBox *keyboardShortcuts = new QGroupBox( wgt );
  keyboardShortcuts->setTitle( tr("Keyboard Shortcuts") );

  twKeyShortcuts = new QTreeWidget( keyboardShortcuts );
  twKeyShortcuts->setColumnCount(2);
  twKeyShortcuts->headerItem()->setText( 0, tr("Action") );
  twKeyShortcuts->headerItem()->setText( 1, tr("Key Sequence") );
  connect( twKeyShortcuts, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
           this, SLOT(setEditText(QTreeWidgetItem*,int)) );

  QFont font;
  font.setBold( true );

  QTreeWidgetItem *projectOperations = new QTreeWidgetItem( twKeyShortcuts );
  projectOperations->setText( 0, tr("Project") );
  projectOperations->setFont( 0, font );
  QTreeWidgetItem *po_New = new QTreeWidgetItem( projectOperations );
  po_New->setWhatsThis( 0, "NewProject" );
  po_New->setIcon( 0, QIcon(":res/new.png") );
  po_New->setText( 0, tr("New Project") );
  po_New->setText( 1, Singleton<Settings>::Ptr().getValue( "Keyboard", "NewProject" ) );
  QTreeWidgetItem *po_Open = new QTreeWidgetItem( projectOperations );
  po_Open->setWhatsThis( 0, "OpenProjects" );
  po_Open->setIcon( 0, QIcon(":res/open.png") );
  po_Open->setText( 0, tr("OpenProjects") );
  po_Open->setText( 1, Singleton<Settings>::Ptr().getValue( "Keyboard", "OpenProjects" ) );
  QTreeWidgetItem *po_Run = new QTreeWidgetItem( projectOperations );
  po_Run->setWhatsThis( 0, "RunProject" );
  po_Run->setIcon( 0, QIcon(":res/start.png") );
  po_Run->setText( 0, tr("Run Project") );
  po_Run->setText( 1, Singleton<Settings>::Ptr().getValue( "Keyboard", "RunProject" ) );
  QTreeWidgetItem *po_Legend = new QTreeWidgetItem( projectOperations );
  po_Legend->setWhatsThis( 0, "ProjectLegend" );
  po_Legend->setIcon( 0, QIcon(":res/legend.png") );
  po_Legend->setText( 0, tr("Project Legend") );
  po_Legend->setText( 1, Singleton<Settings>::Ptr().getValue( "Keyboard", "ProjectLegend" ) );

  QTreeWidgetItem *other = new QTreeWidgetItem( twKeyShortcuts );
  other->setText( 0, QObject::tr("Other") );
  other->setFont( 0, font );
  QTreeWidgetItem *o_Settings = new QTreeWidgetItem( other );
  o_Settings->setWhatsThis( 0, "Settings" );
  o_Settings->setIcon( 0, QIcon(":res/settings.png") );
  o_Settings->setText( 0, tr("Settings") );
  o_Settings->setText( 1, Singleton<Settings>::Ptr().getValue( "Keyboard", "Settings" ) );
  QTreeWidgetItem *o_Update = new QTreeWidgetItem( other );
  o_Update->setWhatsThis( 0, "Update" );
  o_Update->setIcon( 0, QIcon(":res/update.png") );
  o_Update->setText( 0, tr("Update") );
  o_Update->setText( 1, Singleton<Settings>::Ptr().getValue( "Keyboard", "Update" ) );

  twKeyShortcuts->expandAll();
  twKeyShortcuts->setColumnWidth( 0, 300 );
  twKeyShortcuts->setItemsExpandable( false );
  twItems << po_New << po_Open << po_Run << po_Legend << o_Settings << o_Update;

  QVBoxLayout *keyboardLayout = new QVBoxLayout;
  keyboardLayout->addWidget( twKeyShortcuts );
  keyboardShortcuts->setLayout( keyboardLayout );
  // End Keyboard Shortcuts


  // Begin Shortcuts
  QGroupBox *shortcuts = new QGroupBox( wgt );
  shortcuts->setTitle( tr("Shortcuts") );
  QLabel *shLabel = new QLabel( tr("Key Sequence"), shortcuts );
  shEdit = new LineEdit( shortcuts );
  shEdit->setReadOnly( true );
  connect( shEdit, SIGNAL(textChanged(QString)), this, SLOT(setShortcut(QString)) );
  QPushButton *shButton = new QPushButton( tr("Clear"), shortcuts );
  connect( shButton, SIGNAL(clicked()), shEdit, SLOT(clear()) );
  QHBoxLayout *shLayout = new QHBoxLayout;
  shLayout->addWidget( shLabel );
  shLayout->addWidget( shEdit );
  shLayout->addWidget( shButton );
  shortcuts->setLayout( shLayout );
  // End Shortcuts

  vl->addWidget( keyboardShortcuts );
  vl->addWidget( shortcuts );
  vl->setAlignment( Qt::AlignTop );

  return wgt;
}
//=================================================================================================
QWidget *SettingsDialog::resourcesWidget( QWidget *parent )
{
  // Размер иконок
  QSize size;
  size.setWidth( 64 );
  size.setHeight( 64 );

  QWidget *wgt = new QWidget( parent );
  QHBoxLayout *hl = new QHBoxLayout( wgt );

  QTabWidget *tabs = new QTabWidget( wgt );

  // Пользовательский интерфейс
  QListWidget *gui = new QListWidget( tabs );
  gui->setSelectionMode( QAbstractItemView::NoSelection );
  gui->setViewMode( QListWidget::IconMode );
  gui->setIconSize( size );
  gui->setSpacing( 10 );
  gui->setDragDropMode( QAbstractItemView::NoDragDrop );
  gui->setResizeMode( QListWidget::Adjust );
  connect( gui, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editResource(QListWidgetItem*)) );

  // Курсоры
  QListWidget *cursors = new QListWidget( tabs );
  cursors->setSelectionMode( QAbstractItemView::SingleSelection );
  cursors->setViewMode( QListWidget::IconMode );
  cursors->setIconSize( size );
  cursors->setSpacing( 10 );
  cursors->setDragDropMode( QAbstractItemView::NoDragDrop );
  cursors->setResizeMode( QListWidget::Adjust );
  connect( cursors, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editResource(QListWidgetItem*)) );

  // Отсутствующий ресурс
  QListWidget *missing = new QListWidget( tabs );
  missing->setSelectionMode( QAbstractItemView::SingleSelection );
  missing->setViewMode( QListWidget::IconMode );
  missing->setIconSize( size );
  missing->setSpacing( 10 );
  missing->setDragDropMode( QAbstractItemView::NoDragDrop );
  missing->setResizeMode( QListWidget::Adjust );
  connect( missing, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editResource(QListWidgetItem*)) );

  tabs->addTab( gui, QIcon(":res/resource.png"), tr("GUI") );
  tabs->addTab( cursors, QIcon(":res/cursor.png"), tr("Cursors") );
  tabs->addTab( missing, QIcon(":res/missing.png"), tr("Missing Resource") );

  hl->addWidget( tabs );

  QString basePath = qApp->applicationDirPath() + "/Data/resources/";

  // Загрузка GUI
  QDir guiDir( basePath + "gui" );
  QStringList entriesGUI = guiDir.entryList( QStringList() << "*.png", QDir::Files );

  for (int i = 0; i < entriesGUI.size(); ++i)
  {
    QString imageFile = PathUtils::nativePath( basePath + "gui/" + entriesGUI.at(i) );

    QListWidgetItem *guiItem = new QListWidgetItem( QIcon( imageFile ), 0, gui );
    guiItem->setToolTip( QString("<table border='0'><tr><td align='center'><img src='%1'></td></tr><tr><td align='center'>%2</td></tr></table>")
                         .arg( imageFile ).arg( entriesGUI.at(i).left( entriesGUI.at(i).length() - 4 ) ) );
    guiItem->setData( Qt::UserRole, imageFile );
    guiItem->setData( Qt::SizeHintRole, size );

    gui->addItem( guiItem );
  }

  // Загрузка курсоров
  QDir cursorsDir( basePath );
  QStringList entriesCursors = cursorsDir.entryList( QStringList() << "cursor_*.png", QDir::Files );

  for (int i = 0; i < entriesCursors.size(); ++i)
  {
    QString imageFile = basePath + entriesCursors.at(i);

    QListWidgetItem *guiItem = new QListWidgetItem( QIcon( imageFile ), 0, cursors );
    guiItem->setToolTip( QString("<table border='0'><tr><td align='center'><img src='%1'></td></tr><tr><td align='center'>%2</td></tr></table>")
                         .arg( imageFile ).arg( entriesCursors.at(i).left( entriesCursors.at(i).length() - 4 ) ) );
    guiItem->setData( Qt::UserRole, imageFile );
    guiItem->setData( Qt::SizeHintRole, size );

    cursors->addItem( guiItem );
  }

  // Отсутствующий ресурс
  QListWidgetItem *guiItem = new QListWidgetItem( QIcon( basePath + "missing.png" ), 0, missing );
  guiItem->setToolTip( QString("<table border='0'><tr><td align='center'><img src='%1'></td></tr><tr><td align='center'>missing</td></tr></table>")
                       .arg( basePath + "missing.png" ) );
  guiItem->setData( Qt::UserRole, basePath + "missing.png" );
  guiItem->setData( Qt::SizeHintRole, size );

  missing->addItem( guiItem );

  return wgt;
}
//=================================================================================================
QWidget *SettingsDialog::updateWidget( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QVBoxLayout *vl = new QVBoxLayout( wgt );

  enableProxy = new QCheckBox( tr("Use proxy"), wgt );
  enableProxy->setChecked( Singleton<Settings>::Ptr().getValue( "Proxy", "Enable" ).toInt() == 1 ? true : false );

  QGroupBox *gb = new QGroupBox( tr("Proxy"), wgt );
  QGridLayout *gl = new QGridLayout( gb );

  proxyHost = new QLineEdit( gb );
  proxyHost->setText( Singleton<Settings>::Ptr().getValue( "Proxy", "Host" ) );

  proxyPort = new QSpinBox( gb );
  proxyPort->setButtonSymbols( QAbstractSpinBox::NoButtons );
  proxyPort->setRange( 0, 99999 );
  proxyPort->setValue( Singleton<Settings>::Ptr().getValue( "Proxy", "Port" ).toInt() );

  gl->addWidget( new QLabel( tr("Host"), gb ), 0, 0 );
  gl->addWidget( proxyHost, 0, 1 );
  gl->addWidget( new QLabel( tr("Port"), gb ), 1, 0 );
  gl->addWidget( proxyPort, 1, 1 );

  vl->addWidget( enableProxy );
  vl->addWidget( gb );
  vl->setAlignment( Qt::AlignTop );

  gb->setEnabled( enableProxy->isChecked() );

  connect( enableProxy, SIGNAL(toggled(bool)), gb, SLOT(setEnabled(bool)) );

  return wgt;
}
//=================================================================================================
QWidget *SettingsDialog::styleWidget( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QVBoxLayout *vl = new QVBoxLayout( wgt );

  QGroupBox *gb = new QGroupBox( tr("Styles"), wgt );
  QVBoxLayout *vlGb = new QVBoxLayout( gb );

  QStringList lst = QStyleFactory::keys();

  for (int i = 0; i < lst.count(); ++i)
  {
    QRadioButton *rb = new QRadioButton( lst.at(i), gb );
    connect( rb, SIGNAL(clicked()), this, SLOT(styleChanged()) );
    vlGb->addWidget( rb );

    if (!Singleton<Settings>::Ptr().getValue( "Style", "Current" ).compare( lst.at(i), Qt::CaseInsensitive ))
    {
      rb->setChecked( true );
      style = lst.at(i);
    }
  }

  vlGb->setAlignment( Qt::AlignTop );
  vl->addWidget( gb );

  return wgt;
}
//=================================================================================================
void SettingsDialog::editResource( QListWidgetItem *item )
{
  // Исходный файл
  QString srcFileName = item->data( Qt::UserRole ).toString();
  // Файл, которым будем заменять
  QString dstFileName = QFileDialog::getOpenFileName( this, QString( tr("Load Image") ),
                                                      qApp->applicationDirPath(), QString( tr("Image Files (*.png)") ) );

  if (!dstFileName.isEmpty())
  {
    // Удаляем исходный файл
    QFile( srcFileName ).remove();
    // Копируем выбранный файл на место удалённого
    QFile::copy( dstFileName, srcFileName );
    // Изменяем иконку на выбранный файл
    item->setIcon( QIcon( srcFileName ) );
  }
}
//=================================================================================================
void SettingsDialog::styleChanged()
{
  QRadioButton *rb = dynamic_cast<QRadioButton *>( sender() );

  if (rb)
  {
    qApp->setStyle( QStyleFactory::create( rb->text() ) );
    style = rb->text();
  }
}
//=================================================================================================
void SettingsDialog::setShortcut( const QString &text )
{
  if (twKeyShortcuts->selectedItems().count() > 0)
  {
    QTreeWidgetItem *item = twKeyShortcuts->selectedItems().at(0);

    if (item)
    {
      if (item->childCount() == 0)
        item->setText( 1, text );
    }
  }
}
//=================================================================================================
void SettingsDialog::setEditText( QTreeWidgetItem *item, int )
{
  shEdit->setText( item->text(1) );
}
//=================================================================================================



//=================================================================================================
LineEdit::LineEdit( QWidget *parent ):
  QLineEdit( parent )
{
  keysList.insert( "0", 0x30 );
  keysList.insert( "1", 0x31 );
  keysList.insert( "2", 0x32 );
  keysList.insert( "3", 0x33 );
  keysList.insert( "4", 0x34 );
  keysList.insert( "5", 0x35 );
  keysList.insert( "6", 0x36 );
  keysList.insert( "7", 0x37 );
  keysList.insert( "8", 0x38 );
  keysList.insert( "9", 0x39 );

  keysList.insert( "F1", 0x01000030 );
  keysList.insert( "F2", 0x01000031 );
  keysList.insert( "F3", 0x01000032 );
  keysList.insert( "F4", 0x01000033 );
  keysList.insert( "F5", 0x01000034 );
  keysList.insert( "F6", 0x01000035 );
  keysList.insert( "F7", 0x01000036 );
  keysList.insert( "F8", 0x01000037 );
  keysList.insert( "F9", 0x01000038 );
  keysList.insert( "F10", 0x01000039 );
  keysList.insert( "F11", 0x0100003a );
  keysList.insert( "F12", 0x0100003b );

  keysList.insert( "Ins", 0x01000006 );
  keysList.insert( "Del", 0x01000007 );
  keysList.insert( "Home", 0x01000010 );
  keysList.insert( "End", 0x01000011 );
  keysList.insert( "PgUp", 0x01000016 );
  keysList.insert( "PgDown", 0x01000017 );

  keysList.insert( "A", 0x41 );
  keysList.insert( "B", 0x42 );
  keysList.insert( "C", 0x43 );
  keysList.insert( "D", 0x44 );
  keysList.insert( "E", 0x45 );
  keysList.insert( "F", 0x46 );
  keysList.insert( "G", 0x47 );
  keysList.insert( "H", 0x48 );
  keysList.insert( "I", 0x49 );
  keysList.insert( "J", 0x4a );
  keysList.insert( "K", 0x4b );
  keysList.insert( "L", 0x4c );
  keysList.insert( "M", 0x4d );
  keysList.insert( "N", 0x4e );
  keysList.insert( "O", 0x4f );
  keysList.insert( "P", 0x50 );
  keysList.insert( "Q", 0x51 );
  keysList.insert( "R", 0x52 );
  keysList.insert( "S", 0x53 );
  keysList.insert( "T", 0x54 );
  keysList.insert( "U", 0x55 );
  keysList.insert( "V", 0x56 );
  keysList.insert( "W", 0x57 );
  keysList.insert( "X", 0x58 );
  keysList.insert( "Y", 0x59 );
  keysList.insert( "Z", 0x5a );
}
//=================================================================================================
void LineEdit::keyPressEvent( QKeyEvent *e )
{
  QStringList list = text().split(";");

  if (list.count() < 2)
  {
    if (!text().endsWith(";") && !text().isEmpty())
      setText( text() + ";" );

    if ((e->key() >= 0x30 && e->key() <= 0x39) || (e->key() >= 0x01000030 && e->key() <= 0x0100003b)
      || (e->key() >= 0x41 && e->key() <= 0x5a) || e->key() == 0x01000006 || e->key() == 0x01000007 || e->key() == 0x01000010
      || e->key() == 0x01000011 || e->key() == 0x01000016 || e->key() == 0x01000017)
    {
      if (e->modifiers() & Qt::CTRL)
        setText( text() + "Ctrl+" + keysList.key( e->key() ) );
      else if (e->modifiers() & Qt::SHIFT)
        setText( text() + "Shift+" + keysList.key( e->key() ) );
      else
        setText( text() + keysList.key( e->key() ) );
    }
    else
    {
      switch (e->key())
      {
        case 33: setText( text() + "Shift+1" ); break;
        case 64: setText( text() + "Shift+2" ); break;
        case 35: setText( text() + "Shift+3" ); break;
        case 36: setText( text() + "Shift+4" ); break;
        case 37: setText( text() + "Shift+5" ); break;
        case 94: setText( text() + "Shift+6" ); break;
        case 38: setText( text() + "Shift+7" ); break;
        case 42: setText( text() + "Shift+8" ); break;
        case 40: setText( text() + "Shift+9" ); break;
        case 41: setText( text() + "Shift+0" ); break;
      }
    }

    if (text().endsWith(";") && !text().isEmpty())
      setText( text().left( text().length() - 1 ) );
  }
}
//=================================================================================================
