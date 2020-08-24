#include "includes.h"

int joy_key[MAX_JOY_KEYS];
int data_left_key[4] = {KEY_LEFT, KEY_A, KEY_G, KEY_L};
int data_right_key[4] = {KEY_RIGHT, KEY_D, KEY_J, KEY_QUOTE};
int data_up_key[4] = {KEY_UP, KEY_W, KEY_Y, KEY_P};
int data_down_key[4] = {KEY_DOWN, KEY_S, KEY_H, KEY_COLON};
int data_fire_key[4] = {KEY_ENTER, KEY_1, KEY_5, KEY_9};
int data_jump_key[4] = {KEY_RSHIFT, KEY_Q, KEY_T, KEY_O};
int data_option_key[4] = {KEY_BACKSLASH, KEY_X, KEY_N, KEY_SLASH};
int data_select_key[4] = {KEY_BACKSPACE, KEY_Z, KEY_B, KEY_STOP};
int data_left_joy[4] = {KEY_J0_LEFT, KEY_J1_LEFT, KEY_J2_LEFT, KEY_J3_LEFT};
int data_right_joy[4] = {KEY_J0_RIGHT, KEY_J1_RIGHT, KEY_J2_RIGHT, KEY_J3_RIGHT};
int data_up_joy[4] = {KEY_J0_UP, KEY_J1_UP, KEY_J2_UP, KEY_J3_UP};
int data_down_joy[4] = {KEY_J0_DOWN, KEY_J1_DOWN, KEY_J2_DOWN, KEY_J3_DOWN};
int data_fire_joy[4] = {KEY_J0_B0, KEY_J1_B0, KEY_J2_B0, KEY_J3_B0};
int data_jump_joy[4] = {KEY_J0_B1, KEY_J1_B1, KEY_J2_B1, KEY_J3_B1};
int data_option_joy[4] = {KEY_J0_B2, KEY_J1_B2, KEY_J2_B2, KEY_J3_B2};
int data_select_joy[4] = {KEY_J0_B3, KEY_J1_B3, KEY_J2_B3, KEY_J3_B3};

int cant_wait(void)
{
	int ret = 0;
	int i;

	if(key[KEY_ENTER] || key[KEY_ESC] || key[KEY_SPACE])
	{
		ret = 1;
		key[KEY_ENTER] = 0;
		key[KEY_SPACE] = 0;
		key[KEY_ESC] = 0;
	}
	for(i = 0; i < num_joysticks; i++)
	{
		if(ncd_joy_button(i, 0))
		{
			ret = 1;
		}
	}

	return ret;
}

void clear_keys(void)
{
	int i;

	for(i = 0; i < KEY_MAX; i++)
	{
		key[i] = 0;
	}
	ncd_joy_poll();
}

void read_joy_keys(void)
{
	int i;

	/* read the keyboard first */
	for(i = 0; i < KEY_MAX; i++)
	{
		joy_key[i] = key[i];
	}

	/* read first joystick */
	if(ncd_joy_left(0))
	{
		joy_key[KEY_J0_LEFT]++;
	}
	else
	{
		joy_key[KEY_J0_LEFT] = 0;
	}
	if(ncd_joy_right(0))
	{
		joy_key[KEY_J0_RIGHT]++;
	}
	else
	{
		joy_key[KEY_J0_RIGHT] = 0;
	}
	if(ncd_joy_up(0))
	{
		joy_key[KEY_J0_UP]++;
	}
	else
	{
		joy_key[KEY_J0_UP] = 0;
	}
	if(ncd_joy_down(0))
	{
		joy_key[KEY_J0_DOWN]++;
	}
	else
	{
		joy_key[KEY_J0_DOWN] = 0;
	}
	for(i = 0; i < 10; i++)
	{
		if(ncd_joy_button(0, i))
		{
			joy_key[KEY_J0_B0 + i]++;
		}
		else
		{
			joy_key[KEY_J0_B0 + i] = 0;
		}
	}

	/* read second joystick */
	if(ncd_joy_left(1))
	{
		joy_key[KEY_J1_LEFT]++;
	}
	else
	{
		joy_key[KEY_J1_LEFT] = 0;
	}
	if(ncd_joy_right(1))
	{
		joy_key[KEY_J1_RIGHT]++;
	}
	else
	{
		joy_key[KEY_J1_RIGHT] = 0;
	}
	if(ncd_joy_up(1))
	{
		joy_key[KEY_J1_UP]++;
	}
	else
	{
		joy_key[KEY_J1_UP] = 0;
	}
	if(ncd_joy_down(1))
	{
		joy_key[KEY_J1_DOWN]++;
	}
	else
	{
		joy_key[KEY_J1_DOWN] = 0;
	}
	for(i = 0; i < 10; i++)
	{
		if(ncd_joy_button(1, i))
		{
			joy_key[KEY_J1_B0 + i]++;
		}
		else
		{
			joy_key[KEY_J1_B0 + i] = 0;
		}
	}

	/* read third joystick */
	if(ncd_joy_left(2))
	{
		joy_key[KEY_J2_LEFT]++;
	}
	else
	{
		joy_key[KEY_J2_LEFT] = 0;
	}
	if(ncd_joy_right(2))
	{
		joy_key[KEY_J2_RIGHT]++;
	}
	else
	{
		joy_key[KEY_J2_RIGHT] = 0;
	}
	if(ncd_joy_up(2))
	{
		joy_key[KEY_J2_UP]++;
	}
	else
	{
		joy_key[KEY_J2_UP] = 0;
	}
	if(ncd_joy_down(2))
	{
		joy_key[KEY_J2_DOWN]++;
	}
	else
	{
		joy_key[KEY_J2_DOWN] = 0;
	}
	for(i = 0; i < 10; i++)
	{
		if(ncd_joy_button(2, i))
		{
			joy_key[KEY_J2_B0 + i]++;
		}
		else
		{
			joy_key[KEY_J2_B0 + i] = 0;
		}
	}

	/* read first joystick */
	if(ncd_joy_left(3))
	{
		joy_key[KEY_J3_LEFT]++;
	}
	else
	{
		joy_key[KEY_J3_LEFT] = 0;
	}
	if(ncd_joy_right(3))
	{
		joy_key[KEY_J3_RIGHT]++;
	}
	else
	{
		joy_key[KEY_J3_RIGHT] = 0;
	}
	if(ncd_joy_up(3))
	{
		joy_key[KEY_J3_UP]++;
	}
	else
	{
		joy_key[KEY_J3_UP] = 0;
	}
	if(ncd_joy_down(3))
	{
		joy_key[KEY_J3_DOWN]++;
	}
	else
	{
		joy_key[KEY_J3_DOWN] = 0;
	}
	for(i = 0; i < 10; i++)
	{
		if(ncd_joy_button(3, i))
		{
			joy_key[KEY_J3_B0 + i]++;
		}
		else
		{
			joy_key[KEY_J3_B0 + i] = 0;
		}
	}
//	joy_key[KEY_J0_LEFT] = ncd_joy_left(0);
//	joy_key[KEY_J0_RIGHT] = ncd_joy_right(0);
//	joy_key[KEY_J0_UP] = ncd_joy_up(0);
//	joy_key[KEY_J0_DOWN] = ncd_joy_down(0);
/*	for(i = 0; i < 10; i++)
	{
		joy_key[KEY_J0_B0 + i] = ncd_joy_button(0, i);
	}

	joy_key[KEY_J1_LEFT] = ncd_joy_left(1);
	joy_key[KEY_J1_RIGHT] = ncd_joy_right(1);
	joy_key[KEY_J1_UP] = ncd_joy_up(1);
	joy_key[KEY_J1_DOWN] = ncd_joy_down(1);
	for(i = 0; i < 10; i++)
	{
		joy_key[KEY_J1_B0 + i] = ncd_joy_button(1, i);
	}

	joy_key[KEY_J2_LEFT] = ncd_joy_left(2);
	joy_key[KEY_J2_RIGHT] = ncd_joy_right(2);
	joy_key[KEY_J2_UP] = ncd_joy_up(2);
	joy_key[KEY_J2_DOWN] = ncd_joy_down(2);
	for(i = 0; i < 10; i++)
	{
		joy_key[KEY_J2_B0 + i] = ncd_joy_button(2, i);
	}

	joy_key[KEY_J3_LEFT] = ncd_joy_left(3);
	joy_key[KEY_J3_RIGHT] = ncd_joy_right(3);
	joy_key[KEY_J3_UP] = ncd_joy_up(3);
	joy_key[KEY_J3_DOWN] = ncd_joy_down(3);
	for(i = 0; i < 10; i++)
	{
		joy_key[KEY_J3_B0 + i] = ncd_joy_button(3, i);
	} */
}

/* scan the key array for a pressed key (so all keys can be used) */
int get_key_all(void)
{
	int i, j;

	clear_keys();
	for(i = 0; i < KEY_MAX; i++)
	{
		joy_key[i] = 0;
	}
	while(1)
	{
		ncd_joy_poll();
		read_joy_keys();
		for(i = 0; i < MAX_JOY_KEYS; i++)
		{
			if((joy_key[i] && i < KEY_MAX) || joy_key[i] == 1)
			{
//				read_controller(&pp_player[0].controller);
				clear_keys();
				return i;
			}
		}
		ncds_update_music();
	}
}

/* need to scan the keyboard too (for 2 button joys) */
int get_joy_all(int jn)
{
	int i;


	while(!key[KEY_ESC])
	{
		ncd_joy_poll();
		for(i = 0; i < joy[jn].num_buttons; i++)
		{
			if(ncd_joy_button(jn, i))
			{
				return i;
			}
		}
	}
	return -1;
}

void reset_controller(PLAYER_CONTROLLER * cp, int p)
{
	int start;

	start = p - (num_joysticks > 4 ? 4 : num_joysticks);
	if(num_joysticks > p)
	{
		cp->left_key = data_left_joy[p];
		cp->right_key = data_right_joy[p];
		cp->up_key = data_up_joy[p];
		cp->down_key = data_down_joy[p];
    	cp->fire_key = data_fire_joy[p];
		cp->jump_key = data_jump_joy[p];
		cp->option_key = data_option_joy[p];
		cp->select_key = data_select_joy[p];
	}
	else
	{
		cp->left_key = data_left_key[start];
		cp->right_key = data_right_key[start];
		cp->up_key = data_up_key[start];
		cp->down_key = data_down_key[start];
	   	cp->fire_key = data_fire_key[start];
		cp->jump_key = data_jump_key[start];
		cp->option_key = data_option_key[start];
		cp->select_key = data_select_key[start];
	}
}

void junk_controller(PLAYER_CONTROLLER * cp)
{
	cp->left = 2;
	cp->right = 2;
	cp->up = 2;
	cp->down = 2;
	cp->fire = 2;
	cp->wjump = 1;
}

void clear_controller(PLAYER_CONTROLLER * cp)
{
	cp->left = 0;
	cp->right = 0;
	cp->up = 0;
	cp->down = 0;
	cp->fire = 0;
	cp->wjump = 0;
}

void get_key_state(PLAYER_CONTROLLER * cp)
{
	cp->button[PP_KEY_UP] = joy_key[cp->up_key];
	cp->button[PP_KEY_DOWN] = joy_key[cp->down_key];
	cp->button[PP_KEY_LEFT] = joy_key[cp->left_key];
	cp->button[PP_KEY_RIGHT] = joy_key[cp->right_key];
	cp->button[PP_KEY_FIRE] = joy_key[cp->fire_key];
	cp->button[PP_KEY_JUMP] = joy_key[cp->jump_key];
	cp->button[PP_KEY_OPTION] = joy_key[cp->option_key];
	cp->button[PP_KEY_SELECT] = joy_key[cp->select_key];
}

/* reads current state of player's controller */
void read_controller(PLAYER_CONTROLLER * cp)
{
	read_joy_keys();
	get_key_state(cp);

	/* left */
	if(cp->button[PP_KEY_LEFT])
	{
   		cp->left++;
	}
	else
	{
   		cp->left = 0;
	}

	/* right */
	if(cp->button[PP_KEY_RIGHT])
	{
   		cp->right++;
	}
	else
	{
   		cp->right = 0;
	}

	/* up */
	if(cp->button[PP_KEY_UP])
	{
   		cp->up++;
	}
	else
	{
   		cp->up = 0;
	}

	/* down */
	if(cp->button[PP_KEY_DOWN])
	{
   		cp->down++;
	}
	else
	{
   		cp->down = 0;
	}

	/* fire */
	if(cp->button[PP_KEY_FIRE])
	{
		cp->fire++;
	}
	else
	{
		cp->fire = 0;
	}

	/* jump */
	if(cp->button[PP_KEY_JUMP] && !cp->wjump)
	{
		cp->jump = 1;
		cp->wjump = 1;
	}
	else if(cp->button[PP_KEY_JUMP])
	{
		cp->jump = 0;
	}
	if(!cp->button[PP_KEY_JUMP])
	{
		cp->jump = 0;
		cp->wjump = 0;
	}

	/* option */
	if(cp->button[PP_KEY_OPTION] && !cp->woption)
	{
		cp->option = 1;
		cp->woption = 1;
	}
	else if(cp->button[PP_KEY_OPTION])
	{
		cp->option = 0;
	}
	if(!cp->button[PP_KEY_OPTION])
	{
		cp->option = 0;
		cp->woption = 0;
	}

	/* select */
	if(cp->button[PP_KEY_SELECT] && !cp->wselect)
	{
		cp->select = 1;
		cp->wselect = 1;
	}
	else if(cp->button[PP_KEY_SELECT])
	{
		cp->select = 0;
	}
	if(!cp->button[PP_KEY_SELECT])
	{
		cp->select = 0;
		cp->wselect = 0;
    }
}

/* reads current state of player's controller
   use this for internet play control
   key data must already be read into button */
void read_controller_net(PLAYER_CONTROLLER * cp)
{
	/* left */
	if(cp->button[PP_KEY_LEFT])
	{
   		cp->left++;
	}
	else
	{
   		cp->left = 0;
	}

	/* right */
	if(cp->button[PP_KEY_RIGHT])
	{
   		cp->right++;
	}
	else
	{
   		cp->right = 0;
	}

	/* up */
	if(cp->button[PP_KEY_UP])
	{
   		cp->up++;
	}
	else
	{
   		cp->up = 0;
	}

	/* down */
	if(cp->button[PP_KEY_DOWN])
	{
   		cp->down++;
	}
	else
	{
   		cp->down = 0;
	}

	/* fire */
	if(cp->button[PP_KEY_FIRE])
	{
		cp->fire++;
	}
	else
	{
		cp->fire = 0;
	}

	/* jump */
	if(cp->button[PP_KEY_JUMP] && !cp->wjump)
	{
		cp->jump = 1;
		cp->wjump = 1;
	}
	else if(cp->button[PP_KEY_JUMP])
	{
		cp->jump = 0;
	}
	if(!cp->button[PP_KEY_JUMP])
	{
		cp->jump = 0;
		cp->wjump = 0;
	}

	/* option */
	if(cp->button[PP_KEY_OPTION] && !cp->woption)
	{
		cp->option = 1;
		cp->woption = 1;
	}
	else if(cp->button[PP_KEY_OPTION])
	{
		cp->option = 0;
	}
	if(!cp->button[PP_KEY_OPTION])
	{
		cp->option = 0;
		cp->woption = 0;
	}

	/* select */
	if(cp->button[PP_KEY_SELECT] && !cp->wselect)
	{
		cp->select = 1;
		cp->wselect = 1;
	}
	else if(cp->button[PP_KEY_SELECT])
	{
		cp->select = 0;
	}
	if(!cp->button[PP_KEY_SELECT])
	{
		cp->select = 0;
		cp->wselect = 0;
    }
}

void copy_controller(PLAYER_CONTROLLER * cp1, PLAYER_CONTROLLER * cp2)
{
	cp1->left_key = cp2->left_key;
	cp1->right_key = cp2->right_key;
	cp1->up_key = cp2->up_key;
	cp1->down_key = cp2->down_key;
	cp1->fire_key = cp2->fire_key;
	cp1->jump_key = cp2->jump_key;
	cp1->option_key = cp2->option_key;
	cp1->select_key = cp2->select_key;
}

void compress_controller(PLAYER_CONTROLLER * cp, unsigned char * bit)
{
	unsigned char mask = 1;
	int i;
	char array[8] = {0};
	
	*bit = 0;
	for(i = 0; i < 8; i++)
	{
		*bit |= (cp->button[i] ? 1 << i : 0);
	}
}

void uncompress_controller(PLAYER_CONTROLLER * cp, unsigned char * bit)
{
	unsigned char mask = 1;
	int i;
	
	for(i = 0; i < 8; i++)
	{
		cp->button[i] = ((*bit) & mask) >> i;
		mask <<= 1;
	}
}
