#ifndef LOGO_H
#define LOGO_H

#define PP_MAX_CREDIT_NAMES 16
#define PP_MAX_CREDITS      64

typedef struct
{

	char * description;
	char * name[PP_MAX_CREDIT_NAMES];
	int names;

} PP_CREDIT;

extern int logo_step;
extern int credits_done;
extern int intro_times;

void pp_make_credits(void);
void pp_logo_logic(void);
void pp_logo_draw(void);

#endif
