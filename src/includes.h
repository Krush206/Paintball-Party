#ifndef INCLUDES_H
#define INCLUDES_H

/* include allegro and some add-ons */
#include <allegro.h>

/* disable AllegTTF by undefining this */
#ifdef USE_ALLEGTTF
	#include "allegttf/include/allegttf.h"
#endif

#include "gametime.h"
#include "ncds.h"
#include "animate.h"
#include "tilemap.h"
#include "collide.h"
#include "joy.h"
#include "font.h"
#include "message.h"

/* include the game-specific files */
#include "main.h"
#include "data.h"
#include "logo.h"
#include "title.h"
#include "game.h"
#include "stats.h"
#include "control.h"
#include "player.h"
#include "level.h"
#include "objects.h"
#include "particle.h"
#include "paintbal.h"
#include "replay.h"
#include "demo.h"
#include "season.h"

#endif
