#include <allegro.h>

char ncd_joy_enabled = 0;

void ncd_joy_install(void)
{
    if(!ncd_joy_enabled)
    {
        if(install_joystick(JOY_TYPE_AUTODETECT) < 0)
        {
            ncd_joy_enabled = 0;
        }
        else
        {
            ncd_joy_enabled = 1;
        }
    }
}

void ncd_joy_remove(void)
{
    if(ncd_joy_enabled)
    {
        remove_joystick();
        ncd_joy_enabled = 0;
    }
}

void ncd_joy_poll(void)
{
    if(ncd_joy_enabled)
    {
        poll_joystick();
    }
}

int ncd_joy_left(int controller)
{
    if(ncd_joy_enabled)
    {
        return joy[controller].stick[0].axis[0].d1;
    }
    else
    {
        return 0;
    }
}

int ncd_joy_right(int controller)
{
    if(ncd_joy_enabled)
    {
        return joy[controller].stick[0].axis[0].d2;
    }
    else
    {
        return 0;
    }
}

int ncd_joy_up(int controller)
{
    if(ncd_joy_enabled)
    {
        return joy[controller].stick[0].axis[1].d1;
    }
    else
    {
        return 0;
    }
}

int ncd_joy_down(int controller)
{
    if(ncd_joy_enabled)
    {
        return joy[controller].stick[0].axis[1].d2;
    }
    else
    {
        return 0;
    }
}

int ncd_joy_button(int controller, int button)
{
    if(ncd_joy_enabled)
    {
        return joy[controller].button[button].b;
    }
    else
    {
        return 0;
    }
}
