#ifndef SEASON_H
#define SEASON_H

#define SEASON_MAX_MATCHES            32
#define SEASON_TOURNAMENT_MAX_MATCHES  4

#define SEASON_MODE_REGULAR            0
#define SEASON_MODE_TOURNAMENT         1

typedef struct
{
	
	char fn[1024][1024];
	int count;
	
} SEASON_CHARACTERS;

typedef struct
{

	char fn[1024][1024];
	int count;

} SEASON_LEVELS;

typedef struct
{
	
	int mode[1024];
	int count;
	
} SEASON_MODES;

typedef struct
{
	
	int character;
	int level;
	int mode, players;
	int winner;
	
} SEASON_MATCH;

typedef struct
{
	
	int wins[4], losses[4];
	
} SEASON_STANDINGS;

typedef struct
{
	
	SEASON_CHARACTERS characters;
	SEASON_LEVELS levels;
	SEASON_MODES modes;
	SEASON_MATCH match[SEASON_MAX_MATCHES];
	SEASON_MATCH tournament_match[SEASON_TOURNAMENT_MAX_MATCHES];
	SEASON_STANDINGS standings;
	int matches, game, mode;
	
} SEASON;

/* create a season */
void season_create(SEASON * sp);
void season_add_character(SEASON * sp, char * fn);
void season_add_level(SEASON * sp, char * fn);
void season_add_mode(SEASON * sp, int mode);
void season_add_match(SEASON * sp, char * lf, char * cf1, char * cf2, char * cf3, char * cf4);

#endif
