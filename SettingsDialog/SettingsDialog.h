#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//=================================================================================================
#include <QDialog>
#include <QTreeWidget>
#include <QLineEdit>
#include <QHash>
#include <QMap>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QListWidgetItem;
  class QSpinBox;
  class QSlider;
  class QCheckBox;
QT_END_NAMESPACE
//=================================================================================================
class LineEdit : public QLineEdit
{
  Q_OBJECT

  QHash<QString, int> keysList;

  public:
    explicit LineEdit( QWidget *parent = 0 );

  protected:
    void keyPressEvent( QKeyEvent *e );
};
//=================================================================================================
class SettingsDialog : public QDialog
{
  Q_OBJECT

  QString style;
  QSpinBox *timeToRotate;
  QSlider *rotateSpeed;
  QCheckBox *enableRotation;
  QCheckBox *enableProxy;
  QLineEdit *proxyHost;
  QSpinBox *proxyPort;
  QTreeWidget *twKeyShortcuts;
  LineEdit *shEdit;
  QList<QTreeWidgetItem *> twItems;

  public:
    explicit SettingsDialog( QWidget *parent = 0 );
    ~SettingsDialog();

    QString getStyle() const { return style; }
    bool isRotationEnabled();
    int getTimeToRotate();
    int getRotateSpeed();
    QString getProxyHost() const;
    int getProxyPort();
    bool isProxyEnabled();
    QMap<QString, QString> getShortcuts() const;

  private:
    QWidget *sceneWidget( QWidget *parent );
    QWidget *keyboardWidget( QWidget *parent );
    QWidget *resourcesWidget( QWidget *parent );
    QWidget *updateWidget( QWidget *parent );
    QWidget *styleWidget( QWidget *parent );

  private slots:
    void editResource( QListWidgetItem *item );
    void styleChanged();
    void setShortcut( const QString &text );
    void setEditText( QTreeWidgetItem *item, int );
};
//=================================================================================================

#endif // SETTINGSDIALOG_H
