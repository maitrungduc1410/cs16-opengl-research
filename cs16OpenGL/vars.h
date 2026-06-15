/*
	panzer GL 2.2 - vars.h
*/

typedef struct {		//player struct
	double highest_x;
	double highest_y;
	double highest_z;
	double lowest_x;
	double lowest_y;
	double lowest_z;
	double vector_x;
	double vector_y;
	double vector_z;
	double origin_x;
	double origin_y;
	double origin_z;
	double distance;
	double height;
	long vertices;
	bool iscorpse;
	bool get;
}player_s;

typedef struct { // cvars (of course ;P)
	int cross;
	int esp;		// (legacy tier1 master; only referenced by the unused DrawMenu_legacy)
	int esp_line;	// (legacy tier1 snapline; only referenced by DrawMenu_legacy)
	int esp_engine;	// engine entity-list ESP master (real names/team/origin)
	int esp_name;	// ESP Engine sub-option: player name
	int esp_box;	// ESP Engine sub-option: 2D box
	int esp_dist;	// ESP Engine sub-option: distance text
	int esp_hud;	// own HP/ammo HUD master (arcs around the crosshair)
	int hud_hp;		// HUD sub-option: health arc
	int hud_ammo;	// HUD sub-option: ammo arc
	int hud_die;	// HUD sub-option: keep showing the HUD while dead (default off)
	int	aimthru;
	int	aim;
	int	fov;
	int lambert;
	int	recoil;
	int target;
	int shoot;
	int wall;
	int smoke;
	int flash;
	int scope;
	int sky;
	int menu_x;
	int menu_y;
	int pronefix;
	int aimkey;
	float stand_h;
	float duck_h;
}cvar_s;

typedef struct { // for menu controlling
	int count;
	bool up;
	bool down;
	bool select;
	bool select_r;
	bool select_l;
	bool active;
}menu_s;

typedef struct {	// controlling keys, basicly for menu
	bool up;
	bool down;
	bool left;
	bool right;
	bool active;
}key_s;

typedef struct {	// storing custom offset infos
	float s;
	float d;
	char npart1[52];
	char npart2[52];
	char npart3[52];
	char npart4[52];
	char npart5[52];
	char name[256];
}offset_s;

typedef struct {	// custom vertex infos go here
	char name[52];
	int vert01;		// this should have been done in an array i know ;)
	int vert02;		// but i made it like this when i tested the shit...
	int vert03;
	int vert04;
	int vert05;
	int vert06;
	int vert07;
	int vert08;
	int vert09;
	int vert10;
	int vert11;	
	int vert12;
} team_s;

static GLint	vp[4];			// viewport info (2 and 3 holds screen resolution)
static GLdouble mm[16],pm[16];	// infos about different positions e.g. mouse
static HMODULE hOriginalDll = 0;

int	aimat=-1;			// turned -1, 0 and 1 to check if player is target
int	recoilnum=0;		// compares how much mouse is moved down by cvar.recoil
int oldtarget;			// hold the target selected when hack is turned off
int viewportcount=0;	// counts viewport calls
int t_count=0;			// for timer
int offsetcount=0;
int curoffset=0;
int player_height_min=0;
int player_vertex_min=0;
int player_vertex_max=0;

float roffset;	// active offset

bool t_get=false;	// timer
bool bFlash=false;	// flags . . . 
bool bSmoke=false;
bool bScope=false;
bool bWall=false;
bool bSky=false;
bool b2D=false;		// true while the engine is in its 2D/ortho pass (HUD, scope, sprites)
					// -> used to keep the wallhack from touching 2D draws like the AWP scope
bool ch=false;
bool key_init=false;
bool key_check=false;
bool key_ften=false;
bool aimkeychanged=false;
bool hookactive=false;
bool FirstInit=false;
bool modelviewport=false;
bool enabledraw=false;		// when viewport is called 5th time its true
bool message=false;
bool customoffset=false;	// for menu
bool checktext=false;		// F11, check text
bool gotflashed=false;
bool cfgfail=false;			// true if config couldnt be find
bool mdlfail=false;			// true if model file couldnt be find

char filename[256]="";
char dllpath[256]="";
char dllfile[14]="\\opengl32.dll";
char offsetname[256]="";
char modelfile[52]="";

char modelpath[256]="";
char configpath[256]="";

// ---- tier2: engine entity-list ESP ----------------------------------------
// Cached pointers/addresses we discover at runtime (0 = not found yet).
DWORD	eng_table		=0;	// address of cl_enginefunc_t table (in client.dll data)
DWORD	eng_extrainfo	=0;	// address of g_PlayerExtraInfo array (in client.dll)
int		eng_local_idx	=0;	// entity index of the local player
int		eng_local_team	=0;	// team number of the local player (0=unknown,1/2)
int		eng_players		=0;	// players drawn last frame (for the debug readout)
bool	eng_have_extra	=false;	// did we manage to locate g_PlayerExtraInfo?
int		eng_frame		=0;	// our own frame counter (kept for misc use)
int		eng_lastcurpos[33]={0};	// cl_entity current_position last seen, per slot
DWORD	eng_lastchange[33]={0};	// GetTickCount() when current_position last changed

// ---- own HUD: health / armor / ammo (via user-message hooks) ---------------
// We patch the engine's "Health"/"Battery"/"CurWeapon" user-message handlers so
// we get the same data the vanilla HUD does, then forward to the originals so
// the normal HUD keeps working.
int		me_health		=0;		// local player health   (Health msg, 0..100)
bool	me_dead			=false;	// true only after a Health msg of 0 (we actually died);
								// stays false until then, so "alive but HP not captured
								// yet" is NOT treated as dead.
int		me_armor		=0;		// local player armor    (Battery msg, 0..100)
int		me_clip			=-1;	// current weapon clip   (CurWeapon msg, -1 = no clip)
int		me_weaponid		=-1;	// active weapon id      (CurWeapon msg)
int		me_maxclip[64]	={0};	// biggest clip ever seen per weapon id (for the %)
bool	msg_hooked		=false;	// did we install the user-message hooks yet?
int		eng_msg_tries	=0;		// how many times we tried to find the msg nodes

// ---- UI scaling + menu animation -------------------------------------------
float	ui_scale		=1.0f;	// text/menu scale vs 1080p baseline (set in BuildFont)
float	gTextAlpha		=1.0f;	// global alpha multiplier for DrawText (menu fade)
float	menu_alpha		=0.0f;	// 0..1 fade state of the menu
float	menu_sel_anim	=0.0f;	// animated (smoothly sliding) selected-row index
DWORD	menu_last_tick	=0;		// last GetTickCount() for time-based animation
