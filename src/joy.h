#ifndef JOY_H
#define JOY_H

#ifdef __cplusplus
extern "C"{
#endif

extern char ncd_joy_enabled;

void ncd_joy_install(void);
void ncd_joy_remove(void);
void ncd_joy_poll(void);
int ncd_joy_left(int controller);
int ncd_joy_right(int controller);
int ncd_joy_up(int controller);
int ncd_joy_down(int controller);
int ncd_joy_button(int controller, int button);

#ifdef __cplusplus
}
#endif

#endif
