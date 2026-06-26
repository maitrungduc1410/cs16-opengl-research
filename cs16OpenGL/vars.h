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
	int esp_engine;	// engine entity-list ESP master (real names/team/origin)
	int esp_name;	// ESP Engine sub-option: player name
	int esp_name_pad;	// ESP Engine sub-option: name text vertical offset (px, negative=closer to box)
	int esp_name_size;	// ESP Engine sub-option: name font size (1=small,2=normal,3=large,4=x-large)
	int esp_box;	// ESP Engine sub-option: 2D box
	int esp_box_pad;	// ESP Engine sub-option: box padding around the player (px)
	int esp_box_radius;	// ESP Engine sub-option: box corner radius (px, 0=sharp)
	int esp_box_width;	// ESP Engine sub-option: box stroke width (px, scaled)
	int esp_dist;	// ESP Engine sub-option: distance text
	int esp_dist_pad;	// ESP Engine sub-option: distance text vertical offset (px)
	int esp_dist_size;	// ESP Engine sub-option: distance font size (1=small,2=normal,3=large,4=x-large)
	int esp_team;	// ESP Engine sub-option: which team to draw (0=both,1=CT,2=T)
	int esp_dbg;	// ESP Engine sub-option: top-left "ENGINE ESP" debug readout text
	// ---- new ESP Engine sub-options (#2,#3,#4,#5,#6) ----
	int esp_snap;	// snapline from a fixed anchor to each enemy (0=off,1=bottom,2=top,3=crosshair)
	int esp_vischeck;	// 1 = dim/recolor when target is occluded (depth-buffer test)
	int esp_maxdist;	// max distance (meters) to draw ESP for; 0 = unlimited
	int esp_fade;	// 1 = fade ESP alpha by distance (closer = fully opaque)
	int esp_arrow;	// 1 = off-screen arrow pointing at enemies behind/outside view
	int esp_hud;	// own HP/ammo HUD master (arcs around the crosshair)
	int hud_hp;		// HUD sub-option: health arc
	int hud_ammo;	// HUD sub-option: ammo arc
	int hud_die;	// HUD sub-option: keep showing the HUD while dead (default off)
	int chams;		// model chams: flat color / wireframe player models
	int chams_wire;	// chams sub-option: wireframe instead of solid fill
	int radar;		// 2D radar built from the engine entity list
	int radar_x;	// radar center position x (pre-ui_scale units, free-moved like the menu)
	int radar_y;	// radar center position y (pre-ui_scale units)
	int radar_shape;// radar sub-option: 0=circle dots, 1=square dots
	int radar_size;	// radar disc radius in pre-ui_scale units (smaller = smaller radar)
	int radar_zoom;	// world units that map to the radar edge (200..5000); smaller = closer view
	int radar_rotate;// 1 = rotate with view yaw (north = facing dir); 0 = fixed north-up
	int radar_names;// 1 = draw short player name next to their dot
	int radar_rings;// 1 = draw concentric range rings inside the radar
	int hud_pad;	// HUD sub-option: extra arc padding around the crosshair (units)
	int	aimthru;
	int	aim;
	int	aim_smooth;	// 0=snap, 1..10 = smoothing strength (higher = smoother/slower)
	int	aim_dot;	// aimbot: draw a dot at the exact point the aimbot aims at
	int	aim_point;	// aimbot: vertical aim offset from head center (world units, +=up)
	int	trigger;	// triggerbot: auto-fire when crosshair is over an enemy (engine list)
	int	trigger_delay;	// reaction delay in ms before the triggerbot fires (humanize)
	int	autofire;	// auto-pistol / auto-knife: spam clicks while mouse1 is held
	int	autofire_rate;	// ms between auto-fire clicks (lower = faster)
	int	notify;		// toast notifications when a feature is toggled in the menu
	int	esp_log;	// detection logging: on-screen per-frame enemy/PVS counters
	int	fov;
	int lambert;
	int	recoil;
	int	norecoil;	// zero the client-side (cosmetic) view punch -> stable screen
	int target;
	int shoot;
	int wall;
	int smoke;
	int flash;
	int scope;
	int sky;
	int menu_x;
	int menu_y;
	int check_x;	// F11 check-screen panel position (x)
	int check_y;	// F11 check-screen panel position (y)
	int menu_vis_rows;	// rows shown at once in the hack menu before scrolling (default 20)
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

typedef struct {	// team display name (used by the "Target" option + F11)
	char name[52];
} team_s;

static GLint	vp[4];			// viewport info (2 and 3 holds screen resolution)
static GLdouble mm[16],pm[16];	// infos about different positions e.g. mouse
static HMODULE hOriginalDll = 0;

int	recoilnum=0;		// compares how much mouse is moved down by cvar.recoil
int oldtarget;			// hold the target selected when hack is turned off
int viewportcount=0;	// counts viewport calls
int t_count=0;			// for timer

bool t_get=false;	// timer
bool bFlash=false;	// flags . . . 
bool bSmoke=false;
bool bScope=false;
bool bWall=false;
bool bSky=false;
bool ch=false;
bool key_init=false;
bool key_check=false;
bool key_ften=false;
bool hookactive=false;
bool FirstInit=false;
bool modelviewport=false;
bool enabledraw=false;		// when viewport is called 5th time its true
bool message=false;
bool checktext=false;		// F11, check text
bool gotflashed=false;
bool cfgfail=false;			// true if config couldnt be find
bool saveloaded=false;		// true if oglsave.cfg (user settings) was loaded this session

char filename[256]="";
char dllpath[256]="";
char dllfile[14]="\\opengl32.dll";
char configpath[256]="";
char savepath[256]="";		// full path to oglsave.cfg (for the F11 check screen)

// ---- tier2: engine entity-list ESP ----------------------------------------
// Cached pointers/addresses we discover at runtime (0 = not found yet).
DWORD	eng_table		=0;	// address of cl_enginefunc_t table (in client.dll data)
DWORD	norec_fn		=0;	// address of client.dll!V_CalcRefdef (0 = not located yet)
bool	norec_hooked	=false;	// is our V_CalcRefdef detour currently installed?
float	norec_peak		=0;	// debug: largest |punch| the engine tried to apply (F11)
float	norec_last[3]	={0,0,0};	// debug: last punch values seen before we cleared them (F11)
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
int		me_health		=100;	// local player health   (Health msg, 0..100). Defaults to
								// 100 because the spawn "Health=100" message is sent once and
								// may be missed before our hook installs; you always spawn at
								// 100, so this shows the correct value until the first update.
bool	me_dead			=false;	// set by the DeathMsg hook (victim == us) and cleared by
								// the ResetHUD hook (respawn). Independent of the Health
								// value so spectating a teammate can't fake "alive".
int		me_armor		=0;		// local player armor    (Battery msg, 0..100)
int		me_clip			=-1;	// current weapon clip   (CurWeapon msg, -1 = no clip)
int		me_weaponid		=-1;	// active weapon id      (CurWeapon msg)
int		me_maxclip[64]	={0};	// biggest clip ever seen per weapon id (for the %)
bool	msg_hooked		=false;	// did we install the user-message hooks yet?
int		eng_msg_tries	=0;		// how many times we tried to find the msg nodes

// ---- UI scaling + menu animation -------------------------------------------
float	ui_scale		=1.0f;	// text/menu scale vs 1080p baseline (set in BuildFont)
int		g_fontw[96]		={0};	// pixel advance width of ASCII 32..127 (filled in BuildFont)
float	gTextAlpha		=1.0f;	// global alpha multiplier for DrawText (menu fade)
float	menu_alpha		=0.0f;	// 0..1 fade state of the menu
float	check_alpha		=0.0f;	// 0..1 fade state of the F11 check screen
float	menu_sel_anim	=0.0f;	// animated (smoothly sliding) selected-row index
DWORD	menu_last_tick	=0;		// last GetTickCount() for time-based animation
float	g_menu_dt		=0.0f;	// per-frame anim delta (seconds), updated every frame
int		ui_open_seq		=0;		// monotonic counter: who (menu/F11) was opened last
int		menu_open_seq	=0;		// ui_open_seq stamp when the menu was last opened
int		check_open_seq	=0;		// ui_open_seq stamp when the F11 check was last opened
int		menu_move_mode	=0;		// 0=off, 1=moving hack menu, 2=moving F11 panel
DWORD	menu_rep_t		=0;		// GetTickCount() when a held up/down may step next (auto-repeat)
int		menu_scroll		=0;		// index of the first visible row (scroll window top)

// ---- engine-based aimbot state --------------------------------------------
// Computed once per frame inside DrawEngineEsp (which has the entity list +
// WorldToScreen available), then consumed by sys_glViewport which actually
// nudges the OS mouse. Decoupling avoids re-reading the engine table twice.
bool	eng_aim_have	=false;	// did we pick a target this frame?
float	eng_aim_sx		=0.0f;	// target screen x (px, 0..vp[2])
float	eng_aim_sy		=0.0f;	// target screen y (px, 0..vp[3])
bool	eng_aim_visible	=true;	// false if blocked by a wall (depth-buffer test)

// ---- triggerbot state (cvar.trigger) --------------------------------------
// eng_trig_active is refreshed each frame by DrawEngineEsp (true while the
// crosshair sits on an enemy box and the target is visible). sys_glViewport
// then fires once trigger_delay ms have elapsed since acquisition.
bool	eng_trig_active	=false;	// crosshair currently on a valid enemy
DWORD	eng_trig_acq	=0;		// GetTickCount() when the crosshair first landed on it
DWORD	eng_trig_fire	=0;		// last triggerbot shot tick (refractory period)

// ---- toast notifications (cvar.notify) ------------------------------------
char	toast_msg[64]	="";	// current toast text ("" = none)
DWORD	toast_until		=0;		// GetTickCount() when the toast disappears (0 = none)

// ---- detection logging / PVS counters (cvar.esp_log) ----------------------
int		det_cur			=0;		// enemy players received from the server this frame
int		det_peak		=0;		// highest enemy count seen this session
float	det_avg			=0.0f;	// exponential moving average of det_cur

// Low-level mouse hook: tracks the TRUE PHYSICAL mouse1 state, ignoring our own
// injected clicks (via LLMHF_INJECTED). This is the only injection-immune signal
// we have, so autofire uses it to know when YOU are really holding the button.
HHOOK			g_ll_hook		=NULL;	// WH_MOUSE_LL handle (0 until installed)
volatile bool	g_phys_lb		=false;	// true while the physical left button is down
HINSTANCE		g_self_inst		=NULL;	// this proxy DLL's own module handle (for the hook)
bool			g_hook_started	=false;	// did we already spin up the dedicated hook thread?