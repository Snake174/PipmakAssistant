#ifndef LUAPARSER_H
#define LUAPARSER_H

//=================================================================================================
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QVector>

#include "../PropertyEditor/Property.h"
//=================================================================================================
// hotspot { target = 2, effect = { pipmak.dissolve, 2 }, cursor = pipmak.hand_forward }
class HotSpot : public CPropertyList
{
  int x, y, az, el, w, h, target;
  QString effect, cursor, enabled, dont_pan, onmousedown, onmouseup, onmouseenter, onmouseleave,
  onmousestilldown, onmousewithin, onhilite, onenddrag;

public:
  QMap<QString, QVariant> fields;

  HotSpot()
  {
    x = y = az = el = w = h = 0;
    target = -1;
    effect = cursor = enabled = dont_pan = onmousedown = onmouseup = onmouseenter = onmouseleave
      = onmousestilldown = onmousewithin = onhilite = onenddrag = "";

    Register( "x", &x );
    Register( "y", &y );
    Register( "az", &az );
    Register( "el", &el );
    Register( "w", &w );
    Register( "h", &h );
    Register( "target", &target );
    Register( "effect", &effect );
    Register( "cursor", &cursor, PropEnumType::Cursors );
    Register( "enabled", &enabled );
    Register( "dont_pan", &dont_pan );
    Register( "onmousedown", &onmousedown );
    Register( "onmouseup", &onmouseup );
    Register( "onmouseenter", &onmouseenter );
    Register( "onmouseleave", &onmouseleave );
    Register( "onmousestilldown", &onmousestilldown );
    Register( "onmousewithin", &onmousewithin );
    Register( "onhilite", &onhilite );
    Register( "onenddrag", &onenddrag );

    fields.insert( "x", 0 );
    fields.insert( "y", 0 );
    fields.insert( "az", 0 );
    fields.insert( "el", 0 );
    fields.insert( "w", 0 );
    fields.insert( "h", 0 );
    fields.insert( "target", -1 );
    fields.insert( "effect", "" );
    fields.insert( "cursor", "" );
    fields.insert( "enabled", "" );
    fields.insert( "dont_pan", "" );
    fields.insert( "onmousedown", "" );
    fields.insert( "onmouseup", "" );
    fields.insert( "onmouseenter", "" );
    fields.insert( "onmouseleave", "" );
    fields.insert( "onmousestilldown", "" );
    fields.insert( "onmousewithin", "" );
    fields.insert( "onhilite", "" );
    fields.insert( "onenddrag", "" );
  }

  void setValue( const QString &valueName, QVariant value )
  {
    if (!valueName.compare("x"))
      setProp( PropInt, "x", value );
    else if (!valueName.compare("y"))
      setProp( PropInt, "y", value );
    else if (!valueName.compare("az"))
      setProp( PropInt, "az", value );
    else if (!valueName.compare("el"))
      setProp( PropInt, "el", value );
    else if (!valueName.compare("w"))
      setProp( PropInt, "w", value );
    else if (!valueName.compare("h"))
      setProp( PropInt, "h", value );
    else if (!valueName.compare("target"))
      setProp( PropInt, "target", value );
    else if (!valueName.compare("effect"))
      setProp( PropString, "effect", value );
    else if (!valueName.compare("cursor"))
      setProp( PropCursors, "cursor", value );
    else if (!valueName.compare("enabled"))
      setProp( PropString, "enabled", value );
    else if (!valueName.compare("dont_pan"))
      setProp( PropString, "dont_pan", value );
    else if (!valueName.compare("onmousedown"))
      setProp( PropString, "onmousedown", value );
    else if (!valueName.compare("onmouseup"))
      setProp( PropString, "onmouseup", value );
    else if (!valueName.compare("onmouseenter"))
      setProp( PropString, "onmouseenter", value );
    else if (!valueName.compare("onmouseleave"))
      setProp( PropString, "onmouseleave", value );
    else if (!valueName.compare("onmousestilldown"))
      setProp( PropString, "onmousestilldown", value );
    else if (!valueName.compare("onmousewithin"))
      setProp( PropString, "onmousewithin", value );
    else if (!valueName.compare("onhilite"))
      setProp( PropString, "onhilite", value );
    else if (!valueName.compare("onenddrag"))
      setProp( PropString, "onenddrag", value );

    fields[ valueName ] = value;
  }

  QString getDescription()
  {
    return "";
  }
};
//=================================================================================================
// patch { face = 1, x = 72, y = 198, visible = state.doorisopen, image = "door.jpeg" }
class Patch : public CPropertyList
{
  int face, x, y, nx, ny, nz, w, h, nw, nh, anchorh, anchorv, angle, anglex, angley, leftmargin, rightmargin;
  QString visible, image;

public:
  QMap<QString, QVariant> fields;

  Patch()
  {
    face = x = y = nx = ny = nz = w = h = nw = nh = anchorh = anchorv = angle = anglex = angley
      = leftmargin = rightmargin = 0;
    visible = image = "";

    Register( "face", &face, 0 );
    Register( "x", &x );
    Register( "y", &y );
    Register( "nx", &nx );
    Register( "ny", &ny );
    Register( "nz", &nz );
    Register( "w", &w );
    Register( "h", &h );
    Register( "nw", &nw );
    Register( "nh", &nh );
    Register( "anchorh", &anchorh );
    Register( "anchorv", &anchorv );
    Register( "angle", &angle );
    Register( "anglex", &anglex );
    Register( "angley", &angley );
    Register( "leftmargin", &leftmargin );
    Register( "rightmargin", &rightmargin );
    Register( "visible", &visible );
    Register( "image", &image, PropEnumType::File );

    fields.insert( "face", 0 );
    fields.insert( "x", 0 );
    fields.insert( "y", 0 );
    fields.insert( "nx", 0 );
    fields.insert( "ny", 0 );
    fields.insert( "nz", 0 );
    fields.insert( "w", 0 );
    fields.insert( "h", 0 );
    fields.insert( "nw", 0 );
    fields.insert( "nh", 0 );
    fields.insert( "anchorh", 0 );
    fields.insert( "anchorv", 0 );
    fields.insert( "angle", 0 );
    fields.insert( "anglex", 0 );
    fields.insert( "angley", 0 );
    fields.insert( "leftmargin", 0 );
    fields.insert( "rightmargin", 0 );
    fields.insert( "visible", "" );
    fields.insert( "image", "" );
  }

  void setValue( const QString &valueName, QVariant value )
  {
    if (!valueName.compare("face"))
      setProp( PropEnum, "face", value );
    else if (!valueName.compare("x"))
      setProp( PropInt, "x", value );
    else if (!valueName.compare("y"))
      setProp( PropInt, "y", value );
    else if (!valueName.compare("nx"))
      setProp( PropInt, "nx", value );
    else if (!valueName.compare("ny"))
      setProp( PropInt, "ny", value );
    else if (!valueName.compare("nz"))
      setProp( PropInt, "nz", value );
    else if (!valueName.compare("w"))
      setProp( PropInt, "w", value );
    else if (!valueName.compare("h"))
      setProp( PropInt, "h", value );
    else if (!valueName.compare("nw"))
      setProp( PropInt, "nw", value );
    else if (!valueName.compare("nh"))
      setProp( PropInt, "nh", value );
    else if (!valueName.compare("anchorh"))
      setProp( PropInt, "anchorh", value );
    else if (!valueName.compare("anchorv"))
      setProp( PropInt, "anchorv", value );
    else if (!valueName.compare("angle"))
      setProp( PropInt, "angle", value );
    else if (!valueName.compare("anglex"))
      setProp( PropInt, "anglex", value );
    else if (!valueName.compare("angley"))
      setProp( PropInt, "angley", value );
    else if (!valueName.compare("leftmargin"))
      setProp( PropInt, "leftmargin", value );
    else if (!valueName.compare("rightmargin"))
      setProp( PropInt, "rightmargin", value );
    else if (!valueName.compare("visible"))
      setProp( PropString, "visible", value );
    else if (!valueName.compare("image"))
      setProp( PropFile, "image", value );

    fields[ valueName ] = value;
  }

  QString getDescription()
  {
    return "";
  }
};
//=================================================================================================
// sound { "background.ogg", loop = true }
struct Sound
{
  QString fileName, loop, autoplay;
  float volume /* [0.0..1.0] default 1.0 */,
  pitch  /* default 1.0 */;
  int az, el;
  QMap<QString, QVariant> fields;

  Sound()
  {
    fileName = loop = autoplay = "";
    volume = pitch = 1.0f;
    az = el = 0;

    fields.insert( "filename", "" );
    fields.insert( "loop", "" );
    fields.insert( "autoplay", "" );
    fields.insert( "volume", 1.0f );
    fields.insert( "pitch", 1.0f );
    fields.insert( "az", 0 );
    fields.insert( "el", 0 );
  }

  void setValue( const QString &valueName, QVariant value )
  {
    if (!valueName.compare("filename"))
      fileName = value.toString();
    else if (!valueName.compare("loop"))
      loop = value.toString();
    if (!valueName.compare("autoplay"))
      autoplay = value.toString();
    else if (!valueName.compare("volume"))
      volume = value.toFloat();
    if (!valueName.compare("pitch"))
      pitch = value.toFloat();
    else if (!valueName.compare("az"))
      az = value.toInt();
    else if (!valueName.compare("el"))
      el = value.toInt();

    fields[ valueName ] = value;
  }
};
//=================================================================================================
// cubic { "01.jpeg", "02.jpeg", "03.jpeg", "04.jpeg", "05.jpeg", "06.jpeg" }
class CubeMaps : public CPropertyList
{
  QString front, right, back, left, top, bottom, cursor, mousemode;

public:
  QMap<QString, QVariant> fields;

  CubeMaps()
  {
    front = right = back = left = top = bottom = cursor = mousemode = "";

    Register( "front", &front, PropEnumType::File );
    Register( "right", &right, PropEnumType::File );
    Register( "back", &back, PropEnumType::File );
    Register( "left", &left, PropEnumType::File );
    Register( "top", &top, PropEnumType::File );
    Register( "bottom", &bottom, PropEnumType::File );
    Register( "cursor", &cursor, PropEnumType::Cursors );
    Register( "mousemode", &mousemode );

    fields.insert( "front", "" );
    fields.insert( "right", "" );
    fields.insert( "back", "" );
    fields.insert( "left", "" );
    fields.insert( "top", "" );
    fields.insert( "bottom", "" );
    fields.insert( "cursor", "" );
    fields.insert( "mousemode", "" );
  }

  void setValue( const QString &valueName, QVariant value )
  {
    if (!valueName.compare("front"))
      setProp( PropFile, "front", value );
    else if (!valueName.compare("right"))
      setProp( PropFile, "right", value );
    else if (!valueName.compare("back"))
      setProp( PropFile, "back", value );
    else if (!valueName.compare("left"))
      setProp( PropFile, "left", value );
    else if (!valueName.compare("top"))
      setProp( PropFile, "top", value );
    else if (!valueName.compare("bottom"))
      setProp( PropFile, "bottom", value );
    else if (!valueName.compare("cursor"))
      setProp( PropCursors, "cursor", value );
    else if (!valueName.compare("mousemode"))
      setProp( PropString, "mousemode", value );

    fields[ valueName ] = value;
  }

  QString getDescription()
  {
    QString res = "cubic { ";
    res.append( "\"" + front + "\", " );
    res.append( "\"" + right + "\", " );
    res.append( "\"" + back + "\", " );
    res.append( "\"" + left + "\", " );
    res.append( "\"" + top + "\", " );
    res.append( "\"" + bottom + "\"" );
    cursor.isEmpty() ? 0 : res.append( ", cursor = " + cursor );
    mousemode.isEmpty() ? 0 : res.append( ", mousemode = " + mousemode );
    res.append(" }");

    return res;
  }
};
//=================================================================================================
// slide "welcome.png"
class Slide : public CPropertyList
{
  QString fileName, border, cursor, mousemode;

public:
  QMap<QString, QVariant> fields;

  Slide()
  {
    fileName = border = cursor = mousemode = "";

    Register( "filename", &fileName, PropEnumType::File );
    Register( "border", &border );
    Register( "cursor", &cursor, PropEnumType::Cursors );
    Register( "mousemode", &mousemode );

    fields.insert( "filename", "" );
    fields.insert( "border", "" );
    fields.insert( "cursor", "" );
    fields.insert( "mousemode", "" );
  }

  void setValue( const QString &valueName, QVariant value )
  {
    if (!valueName.compare("filename"))
      setProp( PropFile, "filename", value );
    else if (!valueName.compare("border"))
      setProp( PropString, "border", value );
    else if (!valueName.compare("cursor"))
      setProp( PropCursors, "cursor", value );
    else if (!valueName.compare("mousemode"))
      setProp( PropString, "mousemode", value );

    fields[ valueName ] = value;
  }

  QString getDescription()
  {
    return "";
  }
};
//=================================================================================================
// panel "welcome.png"
class Panel : public CPropertyList
{
  QString fileName, cursor, mousemode;
  int w, h, leftmargin, rightmargin, topmargin, bottommargin, relx, rely, absx, absy;

public:
  QMap<QString, QVariant> fields;

  Panel()
  {
    fileName = cursor = mousemode = "";
    w = h = leftmargin = rightmargin = topmargin = bottommargin = relx = rely = absx = absy = 0;

    Register( "filename", &fileName, PropEnumType::File );
    Register( "cursor", &cursor, PropEnumType::Cursors );
    Register( "mousemode", &mousemode );

    Register( "w", &w );
    Register( "h", &h );
    Register( "leftmargin", &leftmargin );
    Register( "rightmargin", &rightmargin );
    Register( "topmargin", &topmargin );
    Register( "bottommargin", &bottommargin );
    Register( "relx", &relx );
    Register( "rely", &rely );
    Register( "absx", &absx );
    Register( "absy", &absy );

    fields.insert( "filename", "" );
    fields.insert( "cursor", "" );
    fields.insert( "mousemode", "" );
    fields.insert( "w", 0 );
    fields.insert( "h", 0 );
    fields.insert( "leftmargin", 0 );
    fields.insert( "rightmargin", 0 );
    fields.insert( "topmargin", 0 );
    fields.insert( "bottommargin", 0 );
    fields.insert( "relx", 0 );
    fields.insert( "rely", 0 );
    fields.insert( "absx", 0 );
    fields.insert( "absy", 0 );
  }

  void setValue( const QString &valueName, QVariant value )
  {
    if (!valueName.compare("filename"))
      setProp( PropFile, "filename", value );
    else if (!valueName.compare("cursor"))
      setProp( PropCursors, "cursor", value );
    else if (!valueName.compare("mousemode"))
      setProp( PropString, "mousemode", value );
    else if (!valueName.compare("w"))
      setProp( PropInt, "w", value );
    else if (!valueName.compare("h"))
      setProp( PropInt, "h", value );
    else if (!valueName.compare("leftmargin"))
      setProp( PropInt, "leftmargin", value );
    else if (!valueName.compare("rightmargin"))
      setProp( PropInt, "rightmargin", value );
    else if (!valueName.compare("topmargin"))
      setProp( PropInt, "topmargin", value );
    else if (!valueName.compare("bottommargin"))
      setProp( PropInt, "bottommargin", value );
    else if (!valueName.compare("relx"))
      setProp( PropInt, "relx", value );
    else if (!valueName.compare("rely"))
      setProp( PropInt, "rely", value );
    else if (!valueName.compare("absx"))
      setProp( PropInt, "absx", value );
    else if (!valueName.compare("absy"))
      setProp( PropInt, "absy", value );

    fields[ valueName ] = value;
  }

  QString getDescription()
  {
    return "";
  }
};
//=================================================================================================
// hotspotmap "hotspots.png"
struct HotSpotMap
{
  QString fileName;
  QMap<QString, QVariant> fields;

  HotSpotMap()
  {
    fileName = "";
    fields.insert( "filename", "" );
  }

  void setValue( const QString &valueName, QVariant value )
  {
    if (!valueName.compare("filename"))
      fileName = value.toString();

    fields[ valueName ] = value;
  }
};
//=================================================================================================
class LuaParser : public QObject
{
  Q_OBJECT

  QVector<HotSpot *> hotspots;
  QVector<Patch *> patches;
  QVector<Sound> sounds;
  CubeMaps *cubemap;
  Slide *slide;
  Panel *panel;
  HotSpotMap hotspotmap;
  bool endNode;

  public:
    explicit LuaParser( const QString &fileName, QObject *parent = 0 );
    ~LuaParser();

    QVector<HotSpot *> getHotSpots();
    QVector<Patch *> getPatches();
    QVector<Sound> getSounds();
    CubeMaps *getCubeMap();
    Slide *getSlide();
    Panel *getPanel();
    HotSpotMap getHotSpotMap();
    bool isEndNode();

  private:
    QMap<QString, QString> parseObject( const QString &obj );
};
//=================================================================================================

#endif // LUAPARSER_H
