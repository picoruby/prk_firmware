#ifndef KEYBOARD_DEFINED_H_
#define KEYBOARD_DEFINED_H_

#ifdef __cplusplus
extern "C" {
#endif

enum
{
  AUTORELOAD_WAIT = 0,
  AUTORELOAD_READY,
  AUTORELOAD_NONE
};

extern int autoreload_state;

#ifdef __cplusplus
}
#endif

#endif /* KEYBOARD_DEFINED_H_ */

