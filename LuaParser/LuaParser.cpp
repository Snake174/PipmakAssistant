#include <QFile>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>

#include "../PathUtils.h"
#include "LuaParser.h"

//=================================================================================================
#define SetValue( StructRef, name, value ) { StructRef.name = value; StructRef.fields[ #name ] = value; }
//=================================================================================================
LuaParser::LuaParser( const QString &fileName, QObject *parent ):
  QObject( parent ),
  cubemap(0),
  slide(0),
  panel(0),
  endNode( false )
{
  QFile f( PathUtils::nativePath( fileName ) );

  if (f.open( QIODevice::ReadOnly ))
  {
    QStringList src;

    QTextStream ts( &f );
    //ts.setCodec( QTextCodec::codecForName("UTF-8") );
    ts.setAutoDetectUnicode( true );
    ts.setGenerateByteOrderMark( false );

    while (!ts.atEnd())
      src << ts.readLine().trimmed();

    f.close();

    src.removeAll("");

    QString srcStr = src.join("\r\n");
    QStringList substr;

    // Убираем многострочные комментарии
    int begin = srcStr.indexOf("--[[");

    while (begin != -1)
    {
      int end = srcStr.indexOf( "]]", begin + 4, Qt::CaseSensitive );

      if (end != -1)
      {
        substr << srcStr.mid( begin, end - begin + 2 );
        begin = srcStr.indexOf("--[[", end + 2 );
      }
    }

    for (int i = 0; i < substr.count(); ++i)
      srcStr.replace( substr.at(i), "" );

    // Убираем однострочные комментарии
    substr.clear();

    begin = srcStr.indexOf("--");

    while (begin != -1)
    {
      int end = srcStr.indexOf( "\r\n", begin + 2, Qt::CaseSensitive );

      if (end != -1)
      {
        substr << srcStr.mid( begin, end - begin );
        begin = srcStr.indexOf("--", end );
      }
    }

    for (int i = 0; i < substr.count(); ++i)
      srcStr.replace( substr.at(i), "" );

    if (srcStr.contains("pipmak.quit()"))
      endNode = true;

    src.clear();
    src = srcStr.split("\r\n");
    src.removeAll("");

    cubemap = new CubeMaps();
    slide = new Slide();
    panel = new Panel();

    // Считывание всех объектов
    for (int i = 0; i < src.count(); ++i)
    {
      QString str = src.at(i);
      QString pattern;
      int patternCounter = 0;

      for (;;)
      {
        if (patternCounter >= str.length())
          break;

        if (str.at( patternCounter ) == ' ' || str.at( patternCounter ) == '"'
          || str.at( patternCounter ) == '{' || str.at( patternCounter ) == '(')
        {
          break;
        }

        pattern += str.at( patternCounter );
        ++patternCounter;
      }

      // Ищем кубическую карту
      if (!pattern.compare("cubic"))
      {
        QString findStr = str;
        int begin = findStr.indexOf("{");
        int end = -1;

        for (;;)
        {
          if (begin > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          begin = findStr.indexOf("{");
        }

        end = findStr.indexOf( "}", begin + 1 );

        for (;;)
        {
          if (end > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          end = findStr.indexOf( "}", begin + 1 );
        }

        findStr = findStr.mid( begin + 1, end - begin - 1 ).trimmed();
        findStr.replace( "\"", "" );
        findStr.replace( "\n", "" );
        findStr.replace( "\r", "" );
        findStr.replace( "\t", "" );

        QStringList params = findStr.split(",");

        if (params.count() >= 6)
        {
          cubemap->setValue( "front", QVariant( params.at(0).trimmed() ) );
          cubemap->setValue( "right", QVariant( params.at(1).trimmed() ) );
          cubemap->setValue( "back", QVariant( params.at(2).trimmed() ) );
          cubemap->setValue( "left", QVariant( params.at(3).trimmed() ) );
          cubemap->setValue( "top", QVariant( params.at(4).trimmed() ) );
          cubemap->setValue( "bottom", QVariant( params.at(5).trimmed() ) );

          if (params.count() > 6)
          {
            QString str;

            for (int i = 6; i < params.count(); ++i)
              str.append( params.at(i) );

            QMap<QString, QString> p = parseObject( str );

            for (QMap<QString, QString>::const_iterator it = p.constBegin(); it != p.constEnd(); ++it)
              cubemap->setValue( it.key(), QVariant( it.value() ) );
          }
        }
      }
      // Ищем слайд
      else if (!pattern.compare("slide"))
      {
        QString findStr = str;

        for (;;)
        {
          if (findStr.contains("{"))
            break;

          if (findStr.contains("\""))
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
        }

        if (findStr.contains("{"))
        {
          int begin = findStr.indexOf("\"");
          int end = -1;

          for (;;)
          {
            if (begin > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            begin = findStr.indexOf("\"");
          }

          end = findStr.indexOf( "\"", begin + 1 );

          for (;;)
          {
            if (end > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end = findStr.indexOf( "\"", begin + 1 );
          }

          int end2 = findStr.indexOf( "}", end + 1 );

          for (;;)
          {
            if (end2 > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end2 = findStr.indexOf( "}", end + 1 );
          }

          slide->setValue( "filename", findStr.mid( begin + 1, end - begin - 1 ) );

          int zapPos = findStr.indexOf( ",", end + 1 );

          if (zapPos > 0)
          {
            findStr = findStr.mid( zapPos + 1, end2 - zapPos - 1 ).trimmed();
            QMap<QString, QString> params = parseObject( findStr );

            for (QMap<QString, QString>::const_iterator it = params.constBegin(); it != params.constEnd(); ++it)
              slide->setValue( it.key(), QVariant( it.value() ) );
          }
        }
        else
        {
          int begin = findStr.indexOf("\"");
          int end = -1;

          for (;;)
          {
            if (begin > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            begin = findStr.indexOf("\"");
          }

          end = findStr.indexOf( "\"", begin + 1 );

          for (;;)
          {
            if (end > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end = findStr.indexOf( "\"", begin + 1 );
          }

          slide->setValue( "filename", findStr.mid( begin + 1, end - begin - 1 ) );
        }
      }
      // Ищем панель
      else if (!pattern.compare("panel"))
      {
        QString findStr = str;

        for (;;)
        {
          if (findStr.contains("{"))
            break;

          if (findStr.contains("\""))
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
        }

        if (findStr.contains("{"))
        {
          int begin = findStr.indexOf("\"");
          int end = -1;

          for (;;)
          {
            if (begin > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            begin = findStr.indexOf("\"");
          }

          end = findStr.indexOf( "\"", begin + 1 );

          for (;;)
          {
            if (end > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end = findStr.indexOf( "\"", begin + 1 );
          }

          int end2 = findStr.indexOf( "}", end + 1 );

          for (;;)
          {
            if (end2 > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end2 = findStr.indexOf( "}", end + 1 );
          }

          panel->setValue( "filename", findStr.mid( begin + 1, end - begin - 1 ) );

          int zapPos = findStr.indexOf( ",", end + 1 );

          if (zapPos > 0)
          {
            findStr = findStr.mid( zapPos + 1, end2 - zapPos - 1 ).trimmed();
            QMap<QString, QString> params = parseObject( findStr );

            for (QMap<QString, QString>::const_iterator it = params.constBegin(); it != params.constEnd(); ++it)
              panel->setValue( it.key(), QVariant( it.value() ) );
          }
        }
        else
        {
          int begin = findStr.indexOf("\"");
          int end = -1;

          for (;;)
          {
            if (begin > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            begin = findStr.indexOf("\"");
          }

          end = findStr.indexOf( "\"", begin + 1 );

          for (;;)
          {
            if (end > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end = findStr.indexOf( "\"", begin + 1 );
          }

          panel->setValue( "filename", findStr.mid( begin + 1, end - begin - 1 ) );
        }
      }
      // Ищем хотспоты
      else if (!pattern.compare("hotspot"))
      {
        QString findStr = str;
        int begin = str.indexOf("{");
        int end = -1;

        for (;;)
        {
          if (begin > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          begin = findStr.indexOf("{");
        }

        end = str.indexOf( "}", begin + 1 );

        for (;;)
        {
          if (end > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          end = findStr.indexOf( "}", begin + 1 );
        }

        findStr = findStr.mid( begin + 1, end - begin - 1 ).trimmed();
        QMap<QString, QString> params = parseObject( findStr );
        HotSpot *hs = new HotSpot();

        for (QMap<QString, QString>::const_iterator it = params.constBegin(); it != params.constEnd(); ++it)
          hs->setValue( it.key(), QVariant( it.value() ) );

        hotspots.push_back( hs );
      }
      // Ищем карту хотспотов
      else if (!pattern.compare("hotspotmap"))
      {
        QString findStr = str;

        int begin = findStr.indexOf("\"");
        int end = -1;

        for (;;)
        {
          if (begin > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          begin = findStr.indexOf("\"");
        }

        end = findStr.indexOf( "\"", begin + 1 );

        for (;;)
        {
          if (end > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          end = findStr.indexOf( "\"", begin + 1 );
        }

        hotspotmap.setValue( "filename", findStr.mid( begin + 1, end - begin - 1 ) );
      }
      // Ищем патчи
      else if (!pattern.compare("patch"))
      {
        QString findStr = str;
        int begin = str.indexOf("{");
        int end = -1;

        for (;;)
        {
          if (begin > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          begin = findStr.indexOf("{");
        }

        end = str.indexOf( "}", begin + 1 );

        for (;;)
        {
          if (end > 0)
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
          end = findStr.indexOf( "}", begin + 1 );
        }

        findStr = findStr.mid( begin + 1, end - begin - 1 ).trimmed();
        QMap<QString, QString> params = parseObject( findStr );
        Patch *patch = new Patch();

        for (QMap<QString, QString>::const_iterator it = params.constBegin(); it != params.constEnd(); ++it)
          patch->setValue( it.key(), QVariant( it.value() ) );

        patches.push_back( patch );
      }
      // Ищем звуки
      else if (!pattern.compare("sound"))
      {
        QString findStr = str;

        for (;;)
        {
          if (findStr.contains("{"))
            break;

          if (findStr.contains("\""))
            break;

          ++i;
          str = src.at(i);
          findStr += str + "\r\n";
        }

        if (findStr.contains("{"))
        {
          int begin = findStr.indexOf("\"");
          int end = -1;

          for (;;)
          {
            if (begin > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            begin = findStr.indexOf("\"");
          }

          end = findStr.indexOf( "\"", begin + 1 );

          for (;;)
          {
            if (end > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end = findStr.indexOf( "\"", begin + 1 );
          }

          int end2 = findStr.indexOf( "}", end + 1 );

          for (;;)
          {
            if (end2 > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end2 = findStr.indexOf( "}", end + 1 );
          }

          Sound snd;
          snd.setValue( "filename", findStr.mid( begin + 1, end - begin - 1 ) );

          int zapPos = findStr.indexOf( ",", end + 1 );

          if (zapPos > 0)
          {
            findStr = findStr.mid( zapPos + 1, end2 - zapPos - 1 ).trimmed();
            QMap<QString, QString> params = parseObject( findStr );

            for (QMap<QString, QString>::const_iterator it = params.constBegin(); it != params.constEnd(); ++it)
              snd.setValue( it.key(), QVariant( it.value() ) );
          }

          sounds.push_back( snd );
        }
        else
        {
          int begin = findStr.indexOf("\"");
          int end = -1;

          for (;;)
          {
            if (begin > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            begin = findStr.indexOf("\"");
          }

          end = findStr.indexOf( "\"", begin + 1 );

          for (;;)
          {
            if (end > 0)
              break;

            ++i;
            str = src.at(i);
            findStr += str + "\r\n";
            end = findStr.indexOf( "\"", begin + 1 );
          }

          Sound snd;
          snd.setValue( "filename", findStr.mid( begin + 1, end - begin - 1 ) );

          sounds.push_back( snd );
        }
      }
    }
  }
}
//=================================================================================================
LuaParser::~LuaParser()
{
}
//=================================================================================================
QVector<HotSpot *> LuaParser::getHotSpots()
{
  return hotspots;
}
//=================================================================================================
QVector<Patch *> LuaParser::getPatches()
{
  return patches;
}
//=================================================================================================
QVector<Sound> LuaParser::getSounds()
{
  return sounds;
}
//=================================================================================================
CubeMaps *LuaParser::getCubeMap()
{
  return cubemap;
}
//=================================================================================================
Slide *LuaParser::getSlide()
{
  return slide;
}
//=================================================================================================
Panel *LuaParser::getPanel()
{
  return panel;
}
//=================================================================================================
HotSpotMap LuaParser::getHotSpotMap()
{
  return hotspotmap;
}
//=================================================================================================
bool LuaParser::isEndNode()
{
  return endNode;
}
//=================================================================================================
// Функция парсит объекты типа: hotspot { target = 2, effect = { pipmak.dissolve, 2 }, cursor = pipmak.hand_forward }
// На вход подаётся строка: target = 2, effect = { pipmak.dissolve, 2 }, cursor = pipmak.hand_forward
// На выходе получается список с парами ключ-значение. В данном примере будет:
// QMap( "target", "2" )
// QMap( "effect", "{ pipmak.dissolve, 2 }" )
// QMap( "cursor", "pipmak.hand_forward" )
QMap<QString, QString> LuaParser::parseObject( const QString &obj )
{
  QMap<QString, QString> res;
  QString name, value;
  int i = 0, cntFigSkob = 0, cntSkob = 0, cntKav = 0;
  bool isOpenKav = false;

  for (;;)
  {
    if (obj.at(i) == '=')
    {
      ++i;

      for (;;)
      {
        if (i >= obj.length())
        {
          res.insert( name.trimmed(), value.trimmed() );
          name.clear();
          value.clear();

          break;
        }

        if (obj.at(i) == '{')
          ++cntFigSkob;
        else if (obj.at(i) == '(')
          ++cntSkob;
        else if (obj.at(i) == '\"' && !isOpenKav)
        {
          isOpenKav = true;
          ++cntKav;
        }
        else if (obj.at(i) == '}')
          --cntFigSkob;
        else if (obj.at(i) == ')')
          --cntSkob;
        else if (obj.at(i) == '\"' && isOpenKav)
        {
          isOpenKav = false;
          --cntKav;
        }

        if (obj.at(i) == ',' && !(cntFigSkob + cntKav + cntSkob))
        {
          res.insert( name.trimmed(), value.trimmed() );
          name.clear();
          value.clear();
          ++i;

          break;
        }

        value.append( obj.at(i) );
        ++i;
      }
    }

    if (i >= obj.length())
      break;

    name.append( obj.at(i) );
    ++i;
  }

  return res;
}
//=================================================================================================
