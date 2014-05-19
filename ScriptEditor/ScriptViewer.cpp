#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QTextBlock>
#include <QRegExp>
#include <QMimeData>
#include <QDebug>

#include <Qsci/qscilexerlua.h>
#include <Qsci/qsciapis.h>

#include "ScriptViewer.h"

//=================================================================================================
ScriptViewer::ScriptViewer( QWidget *parent ):
  QsciScintilla( parent )
{
  setUtf8( true );

  // Текущая строка кода и ее подсветка
  setCaretLineVisible( true );
  setCaretLineBackgroundColor( Qt::black );
  setCaretForegroundColor( Qt::white );

  // Выравнивание
  setAutoIndent( true );
  setIndentationGuides( true );
  setIndentationsUseTabs( false );
  setIndentationWidth(2);

  // Авто-дополнение кода в зависимости от источника
  setAutoCompletionSource( QsciScintilla::AcsAll );
  setAutoCompletionCaseSensitivity( false );
  setAutoCompletionReplaceWord( true );
  setAutoCompletionUseSingle( QsciScintilla::AcusNever );
  setAutoCompletionThreshold(2);

  // Подсветка соответствий скобок
  setBraceMatching( QsciScintilla::SloppyBraceMatch );
  setMatchedBraceBackgroundColor( Qt::yellow );
  setUnmatchedBraceForegroundColor( Qt::blue );

  #ifndef WIN32
    QFont font( "Monospace", 10, QFont::Monospace );
  #else
    QFont font( "Lucida Console", 10, QFont::Monospace );
  #endif

  font.setFixedPitch( true );

  QFontMetrics fm( font );

  setFont( font );
  setMarginsFont( font );

  // margin это полоска слева, на которой обычно распологаются breakpoints
  setMarginsBackgroundColor( QColor("gainsboro") );
  setMarginLineNumbers( 0, true );
  setMarginWidth( 0, fm.width("0000") + 5 );
  setMarginsBackgroundColor( QColor("#333333") );
  setMarginsForegroundColor( QColor("#CCCCCC") );
  setMarginType( 0, QsciScintilla::NumberMargin );

  setFolding( QsciScintilla::BoxedTreeFoldStyle );

  QsciLexerLua *lexLua = new QsciLexerLua( this );
  lexLua->setFont( font );
  lexLua->setDefaultPaper( QColor("#333333") );
  lexLua->setDefaultColor( QColor("#CCCCCC") );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::Default );
  lexLua->setColor( QColor("#CCCCCC"), QsciLexerLua::Default );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::Keyword );
  lexLua->setColor( Qt::yellow, QsciLexerLua::Keyword );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::LineComment );
  lexLua->setColor( Qt::lightGray, QsciLexerLua::LineComment );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::Comment );
  lexLua->setColor( Qt::lightGray, QsciLexerLua::Comment );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::String );
  lexLua->setColor( Qt::green, QsciLexerLua::String );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::Identifier );
  lexLua->setColor( Qt::magenta, QsciLexerLua::Identifier );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::Identifier );
  lexLua->setColor( Qt::lightGray, QsciLexerLua::Identifier );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::Operator );
  lexLua->setColor( QColor("#CCCCCC"), QsciLexerLua::Operator );

  lexLua->setPaper( QColor("#333333"), QsciLexerLua::Number );
  lexLua->setColor( Qt::darkCyan, QsciLexerLua::Number );

  lexLua->setFoldCompact( false );

  QsciAPIs *api = new QsciAPIs( lexLua );
  api->add("pipmak");
  api->prepare();

  setLexer( lexLua );
}
//=================================================================================================
ScriptViewer::~ScriptViewer()
{
}
//=================================================================================================
void ScriptViewer::keyPressEvent( QKeyEvent *e )
{
  if ((e->modifiers() == Qt::CTRL) && (e->key() == Qt::Key_Space))
  {
    autoCompleteFromAll();
    return;
  }

  QsciScintilla::keyPressEvent(e);
}
//=================================================================================================
