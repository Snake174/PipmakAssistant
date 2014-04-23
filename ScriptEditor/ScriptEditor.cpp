#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCodec>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QMimeData>
#include <QDrag>
#include <QSplitter>
#include <QScrollBar>
#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QDebug>

#include "../PathUtils.h"
#include "../Node/Node.h"
#include "ScriptViewer.h"
#include "ScriptEditor.h"

//=================================================================================================
ScriptEditor::ScriptEditor( Node *node, QWidget *parent ):
  QWidget( parent ),
  node( node ),
  scriptName(""),
  isChanged( false )
{
  QMap<QString, QString> engineSettingsF;
  engineSettingsF.insert( "pipmak.getmousemode()",
                          tr("Returns the current standard mouse mode, which is either <b><i>pipmak.joystick</i></b> "
                             "or <b><i>pipmak.direct</i></b>. See <b><i>pipmak.setmousemode</i></b>. This may be different "
                             "from the mouse mode that is currently in effect, if it has been overridden by "
                             "<b><i>pipmak.pushmousemode.</i></b>") );
  engineSettingsF.insert( "pipmak.setmousemode(mode)",
                          tr("Sets the standard mouse mode to <b><i>mode</i></b>, which must be either <b><i>pipmak.joystick</i></b> "
                            "or <b></i>pipmak.direct</i></b>. The mouse mode determines how the mouse behaves on "
                            "panoramic nodes: In joystick mode (the default), the mouse pointer is freely movable. "
                            "Click and drag anywhere (except on draggable controls, see the <b><i>dont_pan</i></b> "
                            "property of controls) to pan around. The farther you drag, the faster panning is. "
                            "In direct mode, the pointer is fixed in the center of the screen, and moving the "
                            "mouse pans immediately. "
                            "You can switch between these two modes temporarily by holding down the "
                            "shift key, and pressing the right mouse button (or holding down the command "
                            "key on the Mac) will pan joystick-like regardless of the current mouse mode. "
                            "To temporarily change the mouse mode from Lua code, use "
                            "<b><i>pipmak.pushmousemode</i></b> and <b><i>pipmak.popmousemode</i></b>. The standard mouse "
                            "mode is only used when <b><i>pipmak.pushmousemode</i></b> has never been called or "
                            "after all calls to <b><i>pipmak.pushmousemode</i></b> have been balanced by corresponding "
                            "calls to <b><i>pipmak.popmousemode</i></b>.") );
  engineSettingsF.insert( "pipmak.setwindowed()",
                          tr("Displays Pipmak in a resizable window, starting out at 640 x 480 pixels. This is "
                             "the default state when Pipmak is started.") );
  engineSettingsF.insert( "pipmak.setfullscreen([width, height]|[smaller])",
                          tr("Displays Pipmak on the whole screen. If two numbers <b><i>width</i></b> and <b><i>height</i></b> are "
                            "given, the display resolution is set to <b><i>width x height</i></b>, or if no mode of that exact "
                            "size is available, the next smaller one. Calling the function in this manner is useful "
                            "together with <b><i>pipmak.getscreenmodes</i></b> and <b><i>pipmak.desktopsize</i></b> — don't "
                            "assume that any particular resolution like 1024 x 768 is available. As a convenience, "
                            "the two numbers may be enclosed in a table, this allows you to "
                            "write <b><i>pipmak.setfullscreen(pipmak.getscreenmodes()[i])</i></b> instead of "
                            "<b><i>pipmak.setfullscreen(unpack(pipmak.getscreenmodes()[i]))</i></b>. If a "
                            "boolean argument <b><i>smaller</i></b> is given with value <b><i>true</i></b>, the next lower screen resolution, "
                            "compared to the previous fullscreen resolution or window size, is chosen. "
                            "If <b><i>smaller</i></b> is <b><i>false</i></b> or absent, the next higher resolution is chosen.") );
  engineSettingsF.insert( "pipmak.screensize()",
                          tr("Returns width and height in pixels of the current window or fullscreen mode.") );
  engineSettingsF.insert( "pipmak.desktopsize()",
                          tr("Returns width and height of the display resolution that was in effect before any "
                             "call to <b><i>pipmak.setfullscreen</i></b> changed it.") );
  engineSettingsF.insert( "pipmak.getscreenmodes()",
                          tr("Returns a list (i.e. a table with consecutive integers as keys, starting at 1) of available "
                             "fullscreen resolutions, sorted from largest to smallest. Every entry in the list "
                             "is another list containing two numbers, width and height.") );
  engineSettingsF.insert( "pipmak.getinterpolation()",
                          tr("Returns a boolean value indicating whether bilinear interpolation is on. See <b><i>pipmak.setinterpolation</i></b>.") );
  engineSettingsF.insert( "pipmak.setinterpolation(bool)",
                          tr("Changes the appearance of panoramas and of slides when the window size (or "
                             "fullscreen resolution) is bigger than the slide. If <b><i>bool</i></b> is <b><i>true</i></b>, the images are "
                             "scaled using bilinear interpolation (smooth). This is the default. If <b><i>bool</i></b> is <b><i>false</i></b>, "
                             "lower-quality nearest neighbor scaling is used (blocky). This can make redrawing "
                             "faster, particularly if you don’t have hardware acceleration.") );
  engineSettingsF.insert( "pipmak.getshowcontrols()",
                          tr("Returns a boolean value indicating whether controls and patches are highlighted. "
                             "See <b><i>pipmak.setshowcontrols</i></b>.") );
  engineSettingsF.insert( "pipmak.setshowcontrols(bool)",
                          tr("If <b><i>bool</i></b> is <b><i>true</i></b>, highlights hotspots using solid colors, handles using a diagonally "
                             "striped pattern, and patches using gray triangles at their corners. If <b><i>bool</i></b> is "
                             "<b><i>false</i></b>, returns to the default state of invisible controls and patch corners.") );


  QMap<QString, QString> navigationF;
  QMap<QString, QString> transitionEffectsF;

  QVBoxLayout *vl = new QVBoxLayout( this );
  QSplitter *mainSplitter = new QSplitter( this );
  mainSplitter->setOrientation( Qt::Horizontal );

  viewer = new ScriptViewer( mainSplitter );
  loadFile( PathUtils::nativePath( node->getNodeName() + "/node.lua" ) );
  connect( viewer, SIGNAL(textChanged()), this, SLOT(textChanged()) );

  QSplitter *otherSplitter = new QSplitter( mainSplitter );
  otherSplitter->setOrientation( Qt::Vertical );

  QFont font;
  font.setBold( true );

  funcTree = new QTreeWidget( otherSplitter );
  funcTree->setHeaderLabel( tr("Functions") );
  funcTree->setDragEnabled( true );
  funcTree->setDragDropMode( QAbstractItemView::DragOnly );
  funcTree->setSelectionMode( QAbstractItemView::SingleSelection );
  funcTree->installEventFilter( this );
  connect( funcTree, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(showFuncDescr(QTreeWidgetItem*,int)) );


  // Engine Settings Functions
  QTreeWidgetItem *engineSettings = new QTreeWidgetItem( funcTree );
  engineSettings->setText( 0, tr("Engine Settings") );
  engineSettings->setFont( 0, font );

  for (QMap<QString, QString>::const_iterator it = engineSettingsF.constBegin(); it != engineSettingsF.constEnd(); ++it)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem( engineSettings );
    item->setText( 0, it.key() );
    item->setData( 0, Qt::UserRole, it.value() );
  }

  // Navigation Functions
  QTreeWidgetItem *navigation = new QTreeWidgetItem( funcTree );
  navigation->setText( 0, tr("Navigation") );
  navigation->setFont( 0, font );

  for (QMap<QString, QString>::const_iterator it = navigationF.constBegin(); it != navigationF.constEnd(); ++it)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem( navigation );
    item->setText( 0, it.key() );
    item->setData( 0, Qt::UserRole, it.value() );
  }

  // Transition Effects Functions
  QTreeWidgetItem *transitionEffects = new QTreeWidgetItem( funcTree );
  transitionEffects->setText( 0, tr("Transition Effects") );
  transitionEffects->setFont( 0, font );

  for (QMap<QString, QString>::const_iterator it = transitionEffectsF.constBegin(); it != transitionEffectsF.constEnd(); ++it)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem( transitionEffects );
    item->setText( 0, it.key() );
    item->setData( 0, Qt::UserRole, it.value() );
  }

  funcDescr = new QPlainTextEdit( otherSplitter );
  funcDescr->setReadOnly( true );
  funcDescr->setWordWrapMode( QTextOption::WordWrap );

  otherSplitter->addWidget( funcTree );
  otherSplitter->addWidget( funcDescr );

  mainSplitter->addWidget( viewer );
  mainSplitter->addWidget( otherSplitter );

  vl->addWidget( mainSplitter );
}
//=================================================================================================
ScriptEditor::~ScriptEditor()
{
}
//=================================================================================================
void ScriptEditor::loadFile( const QString &fileName )
{
  scriptName = fileName;

  QFile f( fileName );
  f.open( QIODevice::ReadOnly );
  QTextStream ts( &f );
  //ts.setCodec( QTextCodec::codecForName("UTF-8") );
  ts.setAutoDetectUnicode( true );
  ts.setGenerateByteOrderMark( false );
  viewer->setText( ts.readAll().replace("\t", "  ") );
  viewer->verticalScrollBar()->triggerAction( QScrollBar::SliderToMinimum );
  viewer->ensureCursorVisible();
  f.close();

  isChanged = false;
}
//=================================================================================================
bool ScriptEditor::eventFilter( QObject *target, QEvent *e )
{
  if (target == funcTree)
  {
    if (e->type() == QEvent::MouseButtonPress)
    {
      QTreeWidgetItem *item = funcTree->currentItem();

      QByteArray itemData;
      QDataStream ds( &itemData, QIODevice::WriteOnly );
      ds << item->data( 0, Qt::DisplayRole ).toString();

      QMimeData *mime = new QMimeData;
      mime->setData( "func/text", itemData );

      QDrag *drag = new QDrag( funcTree );
      drag->setMimeData( mime );
      drag->exec( Qt::CopyAction );

      delete drag;
      drag = 0;

      return true;
    }
  }

  return QWidget::eventFilter( target, e );
}
//=================================================================================================
void ScriptEditor::apply()
{
  QFile f( scriptName );

  if (f.open( QIODevice::WriteOnly ))
  {
    QTextStream ts( &f );
    //ts.setCodec( QTextCodec::codecForName("UTF-8") );
    ts.setAutoDetectUnicode( true );
    ts.setGenerateByteOrderMark( false );
    ts << viewer->text();
    f.close();

    isChanged = false;
  }
}
//=================================================================================================
void ScriptEditor::cancel()
{
  if (isChanged)
  {
    QMessageBox mBox( this );
    mBox.setWindowTitle( tr("Save Script Project") );
    mBox.setText( QString( tr("Save current script?") ) );
    mBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    mBox.setDefaultButton( QMessageBox::Yes );
    mBox.setButtonText( QMessageBox::Yes, QObject::tr("Yes") );
    mBox.setButtonText( QMessageBox::No, QObject::tr("No") );
    mBox.setModal( true );

    if (mBox.exec() == QMessageBox::Yes)
      apply();
  }
}
//=================================================================================================
void ScriptEditor::textChanged()
{
  isChanged = true;
}
//=================================================================================================
void ScriptEditor::showFuncDescr( QTreeWidgetItem *it, int col )
{
  funcDescr->blockSignals( true );
  funcDescr->clear();
  funcDescr->appendHtml( it->data( col, Qt::UserRole ).toString() );
  funcDescr->verticalScrollBar()->triggerAction( QScrollBar::SliderToMinimum );
  funcDescr->moveCursor( QTextCursor::Start );
  funcDescr->ensureCursorVisible();
  funcDescr->blockSignals( false );
}
//=================================================================================================
