#ifndef STRUCTURES_H
#define STRUCTURES_H

//=================================================================================================
#include <QObject>
#include <QMap>
#include <QVariant>
//=================================================================================================
namespace CubeMap
{
  enum
  {
    FRONT = 0,
    RIGHT = 1,
    BACK = 2,
    LEFT = 3,
    TOP = 4,
    BOTTOM = 5
  };
}
//=================================================================================================
namespace Roles
{
  enum
  {
    FileName = Qt::UserRole,
    ObjectType = Qt::UserRole + 1
  };
}
//=================================================================================================
namespace ObjectType
{
  enum
  {
    Project = 0,
    Location = 1,
    Image = 2,
    Script = 3,
    Sound = 4,
    Folder = 5
  };
}
//=================================================================================================
struct Keys
{
  QStringList keys;

  Keys()
  {
    keys << "key_backspace" << "key_tab" << "key_clear" << "key_return" << "key_pause" << "key_esc"
         << "key_delete" << "key_kp_0" << "key_kp_1" << "key_kp_2" << "key_kp_3" << "key_kp_4"
         << "key_kp_5" << "key_kp_6" << "key_kp_7" << "key_kp_8" << "key_kp_9" << "key_kp_period" << "key_kp_divide"
         << "key_kp_multiply" << "key_kp_minus" << "key_kp_plus" << "key_kp_enter" << "key_kp_equals"
         << "key_arrow_up" << "key_arrow_down" << "key_arrow_right" << "key_arrow_left" << "key_insert"
         << "key_home" <<  "key_end" << "key_pageup" << "key_pagedown" << "key_f1" << "key_f2" << "key_f3"
         << "key_f4" << "key_f5" << "key_f6" << "key_f7" << "key_f8" << "key_f9" << "key_f10"
         << "key_f11" << "key_f12" << "key_f13" << "key_f14" << "key_f15";

  }
};
//=================================================================================================

#endif // STRUCTURES_H
