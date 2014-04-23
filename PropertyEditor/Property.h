#ifndef _PROPERTY_H_
#define _PROPERTY_H_

// An elaborated implementation of the simple but elegant
// property scheme suggested by Charles Cafrelli.
// Any object which wishes to expose any variables it has
// can do so by registering their name as a string.  This
// makes a bridge between the name of the variable at
// compile time (which is obviously gone once compiled)
// and a run-time queryable format of that format.
// Perfect for writing editors etc which pop up editing
// boxes for any possible object without hardcoded
// 1:1 matching between the editor and the core object.
//
// Bert Peers rat@larian.com (c) 2000 Larian Studios
// 01/02/2000 Created
// 02/02/2000 Wrote an article about it :
//				http://www.larian.com/rat/objectproperty.html
//
// Example of how to use it :
//
// class CMyStuff : public CPropertyList
// {
//		long	SomeVar;
//		CMyStuff () { static std::string SomeVar_S ("SomeVar"); Register (SomeVar_S, &SomeVar); }
// };
//
// CProperty Prop;
// CMyStuff Stuff;
// Prop = Stuff.PropertyList ["SomeVar"];
// if (Prop.Type != PropLong) Error ("Expected a long"); else *(Prop.Long) = 1234;

//=================================================================================================
namespace PropEnumType
{
  enum
  {
    File = 0,
    Cursors = 1
  };
}
//=================================================================================================
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QTableWidget>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class CPropertyList;
  class QSignalMapper;
QT_END_NAMESPACE
//=================================================================================================
class PropertyEditor : public QTableWidget
{
  Q_OBJECT

  CPropertyList *currentProperty;
  QSignalMapper *sm;
  QStringList imageFormats;

  public:
    PropertyEditor( QWidget *parent = 0 );
    ~PropertyEditor();

    void setCurrentProperty( CPropertyList *p );
    void showCurrent();

  private:
    int addRow();

  private slots:
    void mapped( QWidget *wgt );
    void selectFile( QWidget *lineEdit );

  signals:
    void propertyChanged( const QMap<int, QVariant> &data, const QString &name, const QVariant &value );
};
//=================================================================================================
typedef enum
{
  PropString,
  PropFloat,
  PropInt,
  PropFile,
  PropEnum,
  PropCursors
} TProperty;
//=================================================================================================
class CProperty
{
  public:
    TProperty Type;

    union
    {
      QString	*String;
      float *Float;
      int *Int;
      QString *File;
      int *Enum;
      QString *Cursors;
    };
};
//=================================================================================================
class CPropertyList
{
  PropertyEditor *pe;
  QMap<int, QVariant> data;

  public:
    QMap <QString, CProperty>	PropertyList;

    void Register( const QString &Name, QString *String )
    {
      CProperty &P = PropertyList[ Name ];
      P.Type = PropString;
      P.String = String;
    }

    void Register( const QString &Name, float *Float )
    {
      CProperty &P = PropertyList[ Name ];
      P.Type = PropFloat;
      P.Float = Float;
    }

    void Register( const QString &Name, int *Int )
    {
      CProperty &P = PropertyList[ Name ];
      P.Type = PropInt;
      P.Int = Int;
    }

    void Register( const QString &Name, QString *File, int type )
    {
      CProperty &P = PropertyList[ Name ];
      type == PropEnumType::File ? P.Type = PropFile : P.Type = PropCursors;
      type == PropEnumType::File ? P.File = File : P.Cursors = File;
    }

    void Register( const QString &Name, int *Enum, int )
    {
      CProperty &P = PropertyList[ Name ];
      P.Type = PropEnum;
      P.Enum = Enum;
    }

    void Unregister( const QString &Name )
    {
      QMap <QString, CProperty>::iterator it = PropertyList.find( Name );

      if (it != PropertyList.end())
        PropertyList.erase( it );
    }

    void setEditor( PropertyEditor *editor )
    {
      pe = editor;
    }

    void setProp( TProperty type, const QString &name, const QVariant &value )
    {
      CProperty &p = PropertyList[ name ];

      switch (type)
      {
        case PropEnum:          p.Type = PropEnum;          *p.Enum = value.toInt();             break;
        case PropCursors:       p.Type = PropCursors;       *p.Cursors = value.toString();       break;
        case PropFile:          p.Type = PropFile;          *p.File = value.toString();          break;
        case PropFloat:         p.Type = PropFloat;         *p.Float = value.toFloat();          break;
        case PropInt:           p.Type = PropInt;           *p.Int = value.toInt();              break;
        case PropString:        p.Type = PropString;        *p.String = value.toString();        break;
      }
    }

    QVariant getProp( const QString &name ) const
    {
      CProperty p = PropertyList.value( name );

      switch (p.Type)
      {
        case PropEnum:          return *p.Enum;
        case PropCursors:       return *p.Cursors;
        case PropFile:          return *p.File;
        case PropFloat:         return *p.Float;
        case PropInt:           return *p.Int;
        case PropString:        return *p.String;
      }

      return 0;
    }

    void setData( int type, const QVariant &value )
    {
      data.insert( type, value );
    }

    QMap<int, QVariant> getData() const
    {
      return data;
    }

    void setCurrent()
    {
      pe->setCurrentProperty( this );
      pe->showCurrent();
    }

    virtual QString getDescription() = 0;
};
//=================================================================================================

#endif // Property
