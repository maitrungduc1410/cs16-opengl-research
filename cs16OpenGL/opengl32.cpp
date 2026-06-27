/*

	Game-Deception Blank Wrapper v2
	Copyright (c) Crusader 2002

	panzerGL 2.2 (MultiMod)
	credits: Kenbabutz (oC Hack Source)

	If you decide to use this source or parts of it credit Kenbabutz for the
	original aimbot and model recognition, Crusader for the wrapper and of course me
	for all the shit i have	done of it ;).

	-> added a lot of comments before releasing

*/

#pragma warning (disable:4100)
#pragma warning (disable:4211)
#pragma warning (disable:4244)
#pragma warning (disable:4711)

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <gl/gl.h>
#include <gl/glu.h>
//#include <gl/glaux.h>
#include <math.h>
#include "opengl32.h"
#include <Winbase.h>
#include <Stdlib.h>
#include "vars.h"	// containing all variables and cvars used by the hack
// used structs
player_s	player;		// player coords, vectors, height, ...
cvar_s		cvar;		// cvars
menu_s		menu;		// menu bools
key_s		keyp;		// key handler bools
team_s		team[2]={ {"Terrorists"}, {"Counters"} };	// hardcoded team display names (team[0]=T side, team[1]=CT side)

#define FONT_SIZES  4
GLuint g_font_base[FONT_SIZES];		// font display list bases (sizes 1-4: 7/10/13/16px at 1080p)
int    g_fontw_sz[FONT_SIZES][96];	// per-char widths per font size
GLuint base; // for bitmap font (= g_font_base[1], the default size)
HDC hDC;
float curcolor[4];

// default on-screen panel positions + move-mode step (used by HookInit / menu / HandleKey)
#define MENU_DEF_X   40
#define MENU_DEF_Y   50
#define CHECK_DEF_X  40
#define CHECK_DEF_Y  40
#define RADAR_DEF_X  84		// radar center default (pre-ui_scale): ~14px margin + 70px radius
#define RADAR_DEF_Y  84
#define MOVE_STEP    2		// px per polled frame while in move mode (held = continuous)

// Aimbot aim height: the engine bounding-hull top sits a few units ABOVE the
// skull, so drop by this many world units to land at the CENTER of the head.
// cvar.aim_point is then added on top to let the user fine-tune the aim height.
#define AIM_HEAD_CENTER 5.0f

// hack-menu scrolling: keep the panel a fixed height (MENU_VIS_ROWS rows) and
// scroll the list when there are more entries; arrows pick the row, the view
// follows. Holding up/down auto-repeats after REP_DELAY, then every REP_RATE ms.
#define MENU_VIS_ROWS   20		// rows shown at once before the list scrolls (default; overridden by menu_vis_rows in oglconf.cfg / oglsave.cfg)
#define MENU_REP_DELAY  350		// ms a key must be held before it starts repeating
#define MENU_REP_RATE   70		// ms between auto-repeat steps while held

// rounded-rectangle helpers (defined lower, used by the menu / F11 panels above them)
void DrawBox2D(float x0,float y0,float x1,float y1,float rad);			// rounded outline
void FillRoundRect2D(float x0,float y0,float x1,float y1,float rad);		// rounded fill
void SetToast(const char *fmt, ...);									// toast notification (defined lower)
void SetAimStatus(const char *fmt, ...);								// pink aim-key status (Toggle mode)
void DrawAimStatus();													// draws the pink aim-key status pill

// Pink aim-status color: the same tone we draw the T-team ESP box in (see the
// per-player team color below), so the aim feedback reads as a familiar accent.
#define AIM_NOTIFY_R 1.0f
#define AIM_NOTIFY_G 0.25f
#define AIM_NOTIFY_B 0.25f

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void LoadFile(char *thefile,int ftype)
{
	char temp[256]="";

	GetCurrentDirectory(_MAX_PATH, dllpath);	// get path and put it in dllpath, 
	strcat(dllpath,dllfile);					// to say where opengl32.dll is located

	GetCurrentDirectory(_MAX_PATH, filename);	// same for oglconf.cfg but
	sprintf(temp,"\\%s",thefile);				// to open the file
	strcat(filename,temp);
	FILE *file=fopen(filename, "r");			// open it to read
	char str[256]="";

	if(ftype==0)	// oglconf.cfg
	{
		if(!file)	// if file doesnt exist
		{
			cfgfail=true;	// turn bool true (later you can say whats wrong)
			return;			// and leave the function
		}
		else
		{
			strcpy(configpath,filename);	// copy path to use it later for error checking (F11)
			while(!feof(file))
			{
				fgets(str, 256, file);
				if(!strstr(str,"//"))
				{
					//cvar scan
					sscanf(str, "aim %i;"		,&cvar.aim);
					sscanf(str, "aim_smooth %i;",&cvar.aim_smooth);
					sscanf(str, "aim_dot %i;"	,&cvar.aim_dot);
					sscanf(str, "aim_point %i;"	,&cvar.aim_point);
					sscanf(str, "aim_mode %i;"	,&cvar.aim_mode);
					sscanf(str, "aim_key %i;"	,&cvar.aim_key);
					sscanf(str, "trigger %i;"	,&cvar.trigger);
					sscanf(str, "trigger_delay %i;",&cvar.trigger_delay);
					sscanf(str, "autofire %i;"	,&cvar.autofire);
					sscanf(str, "autofire_rate %i;",&cvar.autofire_rate);
					sscanf(str, "bhop %i;"		,&cvar.bhop);
					sscanf(str, "bhop_hold %i;"	,&cvar.bhop_hold);
					sscanf(str, "bhop_key %i;"	,&cvar.bhop_key);
					sscanf(str, "notify %i;"	,&cvar.notify);
					sscanf(str, "esp_log %i;"	,&cvar.esp_log);
					sscanf(str, "aimthru %i;"	,&cvar.aimthru);
					sscanf(str, "target %i;"	,&cvar.target);
					sscanf(str, "recoil %i;"	,&cvar.recoil);
					sscanf(str, "norecoil %i;"	,&cvar.norecoil);
					sscanf(str, "esp_engine %i;",&cvar.esp_engine);
					sscanf(str, "esp_name %i;"	,&cvar.esp_name);
					sscanf(str, "esp_name_pad %i;",&cvar.esp_name_pad);
					sscanf(str, "esp_name_size %i;",&cvar.esp_name_size);
					sscanf(str, "esp_box %i;"	,&cvar.esp_box);
					sscanf(str, "esp_dist %i;"	,&cvar.esp_dist);
					sscanf(str, "esp_dist_pad %i;",&cvar.esp_dist_pad);
					sscanf(str, "esp_dist_size %i;",&cvar.esp_dist_size);
					sscanf(str, "esp_hud %i;"	,&cvar.esp_hud);
					sscanf(str, "hud_hp %i;"	,&cvar.hud_hp);
					sscanf(str, "hud_ammo %i;"	,&cvar.hud_ammo);
					sscanf(str, "hud_die %i;"	,&cvar.hud_die);
					sscanf(str, "chams %i;"		,&cvar.chams);
					sscanf(str, "chams_wire %i;",&cvar.chams_wire);
					sscanf(str, "radar %i;"		,&cvar.radar);
					sscanf(str, "radar_x %i;"	,&cvar.radar_x);
					sscanf(str, "radar_y %i;"	,&cvar.radar_y);
					sscanf(str, "radar_shape %i;",&cvar.radar_shape);
					sscanf(str, "radar_size %i;",&cvar.radar_size);
					sscanf(str, "radar_zoom %i;",&cvar.radar_zoom);
					sscanf(str, "radar_rotate %i;",&cvar.radar_rotate);
					sscanf(str, "radar_names %i;",&cvar.radar_names);
					sscanf(str, "radar_rings %i;",&cvar.radar_rings);
					sscanf(str, "hud_pad %i;"	,&cvar.hud_pad);
					sscanf(str, "spec_warn %i;"	,&cvar.spec_warn);
					sscanf(str, "spec_pad %i;"	,&cvar.spec_pad);
					sscanf(str, "esp_box_pad %i;"	,&cvar.esp_box_pad);
					sscanf(str, "esp_box_radius %i;",&cvar.esp_box_radius);
					sscanf(str, "esp_box_width %i;",&cvar.esp_box_width);
					sscanf(str, "esp_snap %i;"	,&cvar.esp_snap);
					sscanf(str, "esp_vischeck %i;",&cvar.esp_vischeck);
					sscanf(str, "esp_arrow %i;"	,&cvar.esp_arrow);
					sscanf(str, "esp_maxdist %i;",&cvar.esp_maxdist);
					sscanf(str, "esp_fade %i;"	,&cvar.esp_fade);
					sscanf(str, "esp_team %i;"	,&cvar.esp_team);
					sscanf(str, "esp_dbg %i;"	,&cvar.esp_dbg);
					sscanf(str, "lambert %i;"	,&cvar.lambert);
					sscanf(str, "crosshair %i;"	,&cvar.cross);
					sscanf(str, "fov %i;"		,&cvar.fov);
					sscanf(str, "wall %i;"		,&cvar.wall);
					sscanf(str, "nosky %i;"		,&cvar.sky);
					sscanf(str, "noflash %i;"	,&cvar.flash);
					sscanf(str, "nosmoke %i;"	,&cvar.smoke);
					sscanf(str, "menu_x %i;"	,&cvar.menu_x);
					sscanf(str, "menu_y %i;"	,&cvar.menu_y);
					sscanf(str, "check_x %i;"	,&cvar.check_x);
					sscanf(str, "check_y %i;"	,&cvar.check_y);
					sscanf(str, "menu_vis_rows %i;",&cvar.menu_vis_rows);
				}
			}
			fclose(file);

		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Persistent user settings.
//   * oglconf.cfg stays the shipped DEFAULTS.
//   * oglsave.cfg holds whatever the user last tuned in the menu and is rewritten
//     on every change. On load, a missing file or a missing line just keeps the
//     default value of that option, so we always degrade gracefully.
static void GetSavePath(char *out)
{
	GetCurrentDirectory(_MAX_PATH, out);
	strcat(out,"\\oglsave.cfg");
}

void SaveSettings()
{
	if(!hookactive) return;				// never persist the all-zero "hack off" state
	char path[_MAX_PATH]=""; GetSavePath(path);
	FILE *f=fopen(path,"w");
	if(!f) return;
	fprintf(f,"// auto-saved settings (mtd) - edited from the in-game menu\n");
	fprintf(f,"aim %i\n",cvar.aim);
	fprintf(f,"aim_smooth %i\n",cvar.aim_smooth);
	fprintf(f,"aim_dot %i\n",cvar.aim_dot);
	fprintf(f,"aim_point %i\n",cvar.aim_point);
	fprintf(f,"aim_mode %i\n",cvar.aim_mode);
	fprintf(f,"aim_key %i\n",cvar.aim_key);
	fprintf(f,"trigger %i\n",cvar.trigger);
	fprintf(f,"trigger_delay %i\n",cvar.trigger_delay);
	fprintf(f,"autofire %i\n",cvar.autofire);
	fprintf(f,"autofire_rate %i\n",cvar.autofire_rate);
	fprintf(f,"bhop %i\n",cvar.bhop);
	fprintf(f,"bhop_hold %i\n",cvar.bhop_hold);
	fprintf(f,"bhop_key %i\n",cvar.bhop_key);
	fprintf(f,"notify %i\n",cvar.notify);
	fprintf(f,"esp_log %i\n",cvar.esp_log);
	fprintf(f,"aimthru %i\n",cvar.aimthru);
	fprintf(f,"target %i\n",cvar.target);
	fprintf(f,"shoot %i\n",cvar.shoot);
	fprintf(f,"fov %i\n",cvar.fov);
	fprintf(f,"recoil %i\n",cvar.recoil);
	fprintf(f,"norecoil %i\n",cvar.norecoil);
	fprintf(f,"wall %i\n",cvar.wall);
	fprintf(f,"nosky %i\n",cvar.sky);
	fprintf(f,"noflash %i\n",cvar.flash);
	fprintf(f,"nosmoke %i\n",cvar.smoke);
	fprintf(f,"lambert %i\n",cvar.lambert);
	fprintf(f,"crosshair %i\n",cvar.cross);
	fprintf(f,"esp_engine %i\n",cvar.esp_engine);
	fprintf(f,"esp_name %i\n",cvar.esp_name);
	fprintf(f,"esp_name_pad %i\n",cvar.esp_name_pad);
	fprintf(f,"esp_name_size %i\n",cvar.esp_name_size);
	fprintf(f,"esp_box %i\n",cvar.esp_box);
	fprintf(f,"esp_box_pad %i\n",cvar.esp_box_pad);
	fprintf(f,"esp_box_radius %i\n",cvar.esp_box_radius);
	fprintf(f,"esp_box_width %i\n",cvar.esp_box_width);
	fprintf(f,"esp_dist %i\n",cvar.esp_dist);
	fprintf(f,"esp_dist_pad %i\n",cvar.esp_dist_pad);
	fprintf(f,"esp_dist_size %i\n",cvar.esp_dist_size);
	fprintf(f,"esp_snap %i\n",cvar.esp_snap);
	fprintf(f,"esp_vischeck %i\n",cvar.esp_vischeck);
	fprintf(f,"esp_arrow %i\n",cvar.esp_arrow);
	fprintf(f,"esp_maxdist %i\n",cvar.esp_maxdist);
	fprintf(f,"esp_fade %i\n",cvar.esp_fade);
	fprintf(f,"esp_team %i\n",cvar.esp_team);
	fprintf(f,"esp_dbg %i\n",cvar.esp_dbg);
	fprintf(f,"esp_hud %i\n",cvar.esp_hud);
	fprintf(f,"hud_hp %i\n",cvar.hud_hp);
	fprintf(f,"hud_ammo %i\n",cvar.hud_ammo);
	fprintf(f,"hud_die %i\n",cvar.hud_die);
	fprintf(f,"hud_pad %i\n",cvar.hud_pad);
	fprintf(f,"spec_warn %i\n",cvar.spec_warn);
	fprintf(f,"spec_pad %i\n",cvar.spec_pad);
	fprintf(f,"chams %i\n",cvar.chams);
	fprintf(f,"chams_wire %i\n",cvar.chams_wire);
	fprintf(f,"radar %i\n",cvar.radar);
	fprintf(f,"radar_x %i\n",cvar.radar_x);
	fprintf(f,"radar_y %i\n",cvar.radar_y);
	fprintf(f,"radar_shape %i\n",cvar.radar_shape);
	fprintf(f,"radar_size %i\n",cvar.radar_size);
	fprintf(f,"radar_zoom %i\n",cvar.radar_zoom);
	fprintf(f,"radar_rotate %i\n",cvar.radar_rotate);
	fprintf(f,"radar_names %i\n",cvar.radar_names);
	fprintf(f,"radar_rings %i\n",cvar.radar_rings);
	fprintf(f,"menu_x %i\n",cvar.menu_x);
	fprintf(f,"menu_y %i\n",cvar.menu_y);
	fprintf(f,"check_x %i\n",cvar.check_x);
	fprintf(f,"check_y %i\n",cvar.check_y);
	fprintf(f,"menu_vis_rows %i\n",cvar.menu_vis_rows);
	fclose(f);
}

void LoadSettings()
{
	char path[_MAX_PATH]=""; GetSavePath(path);
	strcpy(savepath,path);				// remember it for the F11 check screen
	saveloaded=false;
	FILE *f=fopen(path,"r");
	if(!f) return;						// no save yet -> everything keeps its default
	saveloaded=true;					// file opened OK -> user settings are in effect
	char str[256]="";
	while(!feof(f))
	{
		str[0]=0;
		if(!fgets(str,256,f)) break;
		if(strstr(str,"//")) continue;
		sscanf(str,"aim %i"			,&cvar.aim);
		sscanf(str,"aim_smooth %i"	,&cvar.aim_smooth);
		sscanf(str,"aim_dot %i"		,&cvar.aim_dot);
		sscanf(str,"aim_point %i"	,&cvar.aim_point);
		sscanf(str,"aim_mode %i"	,&cvar.aim_mode);
		sscanf(str,"aim_key %i"		,&cvar.aim_key);
		sscanf(str,"trigger %i"		,&cvar.trigger);
		sscanf(str,"trigger_delay %i",&cvar.trigger_delay);
		sscanf(str,"autofire %i"	,&cvar.autofire);
		sscanf(str,"autofire_rate %i",&cvar.autofire_rate);
		sscanf(str,"bhop %i"		,&cvar.bhop);
		sscanf(str,"bhop_hold %i"	,&cvar.bhop_hold);
		sscanf(str,"bhop_key %i"	,&cvar.bhop_key);
		sscanf(str,"notify %i"		,&cvar.notify);
		sscanf(str,"esp_log %i"		,&cvar.esp_log);
		sscanf(str,"aimthru %i"		,&cvar.aimthru);
		sscanf(str,"target %i"		,&cvar.target);
		sscanf(str,"shoot %i"		,&cvar.shoot);
		sscanf(str,"fov %i"			,&cvar.fov);
		sscanf(str,"recoil %i"		,&cvar.recoil);
		sscanf(str,"norecoil %i"	,&cvar.norecoil);
		sscanf(str,"wall %i"		,&cvar.wall);
		sscanf(str,"nosky %i"		,&cvar.sky);
		sscanf(str,"noflash %i"		,&cvar.flash);
		sscanf(str,"nosmoke %i"		,&cvar.smoke);
		sscanf(str,"lambert %i"		,&cvar.lambert);
		sscanf(str,"crosshair %i"	,&cvar.cross);
		sscanf(str,"esp_engine %i"	,&cvar.esp_engine);
		sscanf(str,"esp_name %i"	,&cvar.esp_name);
		sscanf(str,"esp_name_pad %i",&cvar.esp_name_pad);
		sscanf(str,"esp_name_size %i",&cvar.esp_name_size);
		sscanf(str,"esp_box %i"		,&cvar.esp_box);
		sscanf(str,"esp_box_pad %i"	,&cvar.esp_box_pad);
		sscanf(str,"esp_box_radius %i",&cvar.esp_box_radius);
		sscanf(str,"esp_box_width %i",&cvar.esp_box_width);
		sscanf(str,"esp_dist %i"	,&cvar.esp_dist);
		sscanf(str,"esp_dist_pad %i",&cvar.esp_dist_pad);
		sscanf(str,"esp_dist_size %i",&cvar.esp_dist_size);
		sscanf(str,"esp_snap %i"	,&cvar.esp_snap);
		sscanf(str,"esp_vischeck %i",&cvar.esp_vischeck);
		sscanf(str,"esp_arrow %i"	,&cvar.esp_arrow);
		sscanf(str,"esp_maxdist %i"	,&cvar.esp_maxdist);
		sscanf(str,"esp_fade %i"	,&cvar.esp_fade);
		sscanf(str,"esp_team %i"	,&cvar.esp_team);
		sscanf(str,"esp_dbg %i"		,&cvar.esp_dbg);
		sscanf(str,"esp_hud %i"		,&cvar.esp_hud);
		sscanf(str,"hud_hp %i"		,&cvar.hud_hp);
		sscanf(str,"hud_ammo %i"	,&cvar.hud_ammo);
		sscanf(str,"hud_die %i"		,&cvar.hud_die);
		sscanf(str,"hud_pad %i"		,&cvar.hud_pad);
		sscanf(str,"spec_warn %i"	,&cvar.spec_warn);
		sscanf(str,"spec_pad %i"	,&cvar.spec_pad);
		sscanf(str,"chams %i"		,&cvar.chams);
		sscanf(str,"chams_wire %i"	,&cvar.chams_wire);
		sscanf(str,"radar %i"		,&cvar.radar);
		sscanf(str,"radar_x %i"		,&cvar.radar_x);
		sscanf(str,"radar_y %i"		,&cvar.radar_y);
		sscanf(str,"radar_shape %i"	,&cvar.radar_shape);
		sscanf(str,"radar_size %i"	,&cvar.radar_size);
		sscanf(str,"radar_zoom %i"	,&cvar.radar_zoom);
		sscanf(str,"radar_rotate %i",&cvar.radar_rotate);
		sscanf(str,"radar_names %i"	,&cvar.radar_names);
		sscanf(str,"radar_rings %i"	,&cvar.radar_rings);
		sscanf(str,"menu_x %i"		,&cvar.menu_x);
		sscanf(str,"menu_y %i"		,&cvar.menu_y);
		sscanf(str,"check_x %i"		,&cvar.check_x);
		sscanf(str,"check_y %i"		,&cvar.check_y);
		sscanf(str,"menu_vis_rows %i",&cvar.menu_vis_rows);
	}
	fclose(f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void HookInit(bool activate)
{
	if(activate)					// if hack is activated once:
	{
		menu_move_mode=0;
		cvar.menu_x=MENU_DEF_X;   cvar.menu_y=MENU_DEF_Y;	// position defaults (oglconf/save override)
		cvar.check_x=CHECK_DEF_X; cvar.check_y=CHECK_DEF_Y;
		cvar.radar_x=RADAR_DEF_X; cvar.radar_y=RADAR_DEF_Y;
		cvar.menu_vis_rows=MENU_VIS_ROWS;	// default, overridden by oglconf.cfg then oglsave.cfg
		LoadFile("oglconf.cfg",0);	// read DEFAULT cvar settings
		LoadSettings();				// override defaults with the user's saved settings (if any)
		if(cvar.menu_vis_rows<4)  cvar.menu_vis_rows=4;
		if(cvar.menu_vis_rows>55) cvar.menu_vis_rows=55;
		oldtarget=cvar.target;		// sync so the line below keeps the restored target
		cvar.target=oldtarget;		// set current target (if hack was turned off and on again, it kept old target)
		cvar.scope=0;				// cvar which i didnt add into menu, removes sniper crosshair
		hookactive=true;			// turn bool true, e.g. to allow menu etc.
		message=true;				// turn true to get status text (F11)
		SetToast("Hack: ON");				// greet (respects cvar.notify)
	}
	else if(!activate) // hack turned off, set all things to 0 (not activated)
	{
		cvar.aim=0;
		cvar.aim_smooth=0;
		cvar.aim_dot=0;
		cvar.aim_point=0;
		cvar.trigger=0;
		cvar.trigger_delay=0;
		cvar.autofire=0;
		cvar.autofire_rate=0;
		cvar.bhop=0;
		cvar.bhop_hold=0;
		cvar.notify=0;
		cvar.esp_log=0;
		cvar.aimthru=0;
		cvar.esp_engine=0;
		cvar.esp_name=0;
		cvar.esp_box=0;
		cvar.esp_box_pad=0;
		cvar.esp_box_radius=0;
		cvar.esp_box_width=0;
		cvar.esp_dist=0;
		cvar.esp_snap=0;
		cvar.esp_vischeck=0;
		cvar.esp_arrow=0;
		cvar.esp_maxdist=0;
		cvar.esp_fade=0;
		cvar.esp_team=0;
		cvar.esp_dbg=0;
		cvar.esp_hud=0;
		cvar.hud_hp=0;
		cvar.hud_ammo=0;
		cvar.hud_die=0;
		cvar.spec_warn=0;
		cvar.spec_pad=0;
		cvar.chams=0;
		cvar.chams_wire=0;
		cvar.radar=0;
		cvar.radar_shape=0;
		cvar.radar_size=0;
		cvar.radar_zoom=0;
		cvar.radar_rotate=0;
		cvar.radar_names=0;
		cvar.radar_rings=0;
		cvar.hud_pad=0;
		cvar.lambert=0;
		cvar.cross=0;
		cvar.wall=0;
		cvar.smoke=0;
		cvar.flash=0;
		cvar.scope=0;
		cvar.sky=0;
		cvar.recoil=0;
		cvar.norecoil=0;
		oldtarget=cvar.target; // save last chosen target team
		cvar.target=0;
		menu_move_mode=0;
		hookactive=false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int change(int a) // basic function to toggle things on (1) and off (0)
{
	int b=0;
	if(a==0) { b=1; }
	else if(a==1) { b=0; }
	return b;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Shared activation-key table used by the Aimbot "Aim key" and "Bhop key" menu
// options. The cvar stores a small index; these map it to a Win32 virtual key
// (for GetAsyncKeyState) and a human-readable label (for the menu / toast).
#ifndef VK_XBUTTON1
#define VK_XBUTTON1 0x05	// mouse button 4 (older SDK headers gate this behind _WIN32_WINNT)
#endif
#ifndef VK_XBUTTON2
#define VK_XBUTTON2 0x06	// mouse button 5
#endif
#define KEY_TABLE_COUNT 12
int KeyTableVK(int i)
{
	switch(i)
	{
	case 0:  return VK_RBUTTON;	case 1:  return VK_XBUTTON1;	case 2:  return VK_XBUTTON2;
	case 3:  return VK_MBUTTON;	case 4:  return VK_SHIFT;	case 5:  return VK_CONTROL;
	case 6:  return VK_MENU;	case 7:  return 'E';	case 8:  return 'F';
	case 9:  return 'C';	case 10: return 'V';	case 11: return 'X';
	default: return VK_RBUTTON;
	}
}
const char* KeyTableName(int i)
{
	static const char* n[KEY_TABLE_COUNT]={
		"Mouse R","Mouse 4","Mouse 5","Mouse M","Shift","Ctrl","Alt","E","F","C","V","X"};
	if(i<0) i=0; if(i>=KEY_TABLE_COUNT) i=KEY_TABLE_COUNT-1;
	return n[i];
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/*
void GetNearestPlayer()
{
	dist[dx]=player.distance;
	dx++;

	if(dx>5)
	{
		mindist=9999;
		for(int pd=0;pd<5;pd++)
		{
			if(dist[pd]<=mindist)
				mindist=dist[pd];
		}
		dx=0;
	}
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////
// Nudge whichever panel is currently being moved, clamped to stay on screen.
void MoveActivePanel(int dx,int dy)
{
	if(menu_move_mode==1)			// hack menu (coords are pre-ui_scale)
	{
		float sc = ui_scale>0.01f?ui_scale:1.0f;
		int maxx=(int)((float)vp[2]/sc)-40, maxy=(int)((float)vp[3]/sc)-30;
		cvar.menu_x+=dx; cvar.menu_y+=dy;
		if(cvar.menu_x<0) cvar.menu_x=0; if(cvar.menu_x>maxx) cvar.menu_x=maxx;
		if(cvar.menu_y<10) cvar.menu_y=10; if(cvar.menu_y>maxy) cvar.menu_y=maxy;
	}
	else if(menu_move_mode==2)		// F11 panel (x is raw px, y is raw px)
	{
		cvar.check_x+=dx; cvar.check_y+=dy;
		if(cvar.check_x<0) cvar.check_x=0; if(cvar.check_x>vp[2]-100) cvar.check_x=vp[2]-100;
		if(cvar.check_y<14) cvar.check_y=14; if(cvar.check_y>vp[3]-40) cvar.check_y=vp[3]-40;
	}
	else if(menu_move_mode==3)		// radar center (coords are pre-ui_scale, like the menu)
	{
		float sc = ui_scale>0.01f?ui_scale:1.0f;
		int rr=cvar.radar_size; if(rr<30)rr=70; if(rr>150)rr=150;	// radar radius in pre-scale units -> keep the whole disc on screen
		int minx=rr, maxx=(int)((float)vp[2]/sc)-rr;
		int miny=rr, maxy=(int)((float)vp[3]/sc)-rr;
		cvar.radar_x+=dx; cvar.radar_y+=dy;
		if(cvar.radar_x<minx) cvar.radar_x=minx; if(cvar.radar_x>maxx) cvar.radar_x=maxx;
		if(cvar.radar_y<miny) cvar.radar_y=miny; if(cvar.radar_y>maxy) cvar.radar_y=maxy;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Reset all config to defaults: reload oglconf.cfg, delete oglsave.cfg, notify the player.
void ResetConfig()
{
	// 1) zero all gameplay cvars so stale save values can't bleed through
	cvar.aim=0; cvar.aim_smooth=0; cvar.aim_dot=0; cvar.aim_point=0; cvar.aim_mode=0; cvar.aim_key=0;
	cvar.trigger=0; cvar.trigger_delay=0;
	cvar.autofire=0; cvar.autofire_rate=0; cvar.bhop=0; cvar.bhop_hold=0; cvar.bhop_key=0; cvar.notify=0; cvar.esp_log=0;
	cvar.aimthru=0; cvar.esp_engine=0; cvar.esp_name=0; cvar.esp_name_pad=0; cvar.esp_name_size=2; cvar.esp_box=0;
	cvar.esp_box_pad=0; cvar.esp_box_radius=0; cvar.esp_box_width=0;
	cvar.esp_dist=0; cvar.esp_dist_pad=0; cvar.esp_dist_size=2; cvar.esp_snap=0; cvar.esp_vischeck=0;
	cvar.esp_arrow=0; cvar.esp_maxdist=0; cvar.esp_fade=0; cvar.esp_team=0;
	cvar.esp_dbg=0; cvar.esp_hud=0; cvar.hud_hp=0; cvar.hud_ammo=0;
	cvar.hud_die=0; cvar.hud_pad=0; cvar.spec_warn=0; cvar.spec_pad=0; cvar.chams=0; cvar.chams_wire=0;
	cvar.radar=0; cvar.radar_shape=0; cvar.radar_size=0; cvar.radar_zoom=0;
	cvar.radar_rotate=0; cvar.radar_names=0; cvar.radar_rings=0;
	cvar.lambert=0; cvar.cross=0; cvar.wall=0; cvar.smoke=0; cvar.flash=0;
	cvar.scope=0; cvar.sky=0; cvar.recoil=0; cvar.norecoil=0;
	cvar.fov=0; cvar.shoot=0; cvar.target=0;
	// 2) restore UI position defaults
	cvar.menu_x=MENU_DEF_X; cvar.menu_y=MENU_DEF_Y;
	cvar.check_x=CHECK_DEF_X; cvar.check_y=CHECK_DEF_Y;
	cvar.radar_x=RADAR_DEF_X; cvar.radar_y=RADAR_DEF_Y;
	cvar.menu_vis_rows=MENU_VIS_ROWS;
	menu_move_mode=0;
	// 3) reload defaults from oglconf.cfg
	LoadFile("oglconf.cfg",0);
	if(cvar.menu_vis_rows<4) cvar.menu_vis_rows=4;
	if(cvar.menu_vis_rows>55) cvar.menu_vis_rows=55;
	// 4) delete oglsave.cfg
	char path[_MAX_PATH]=""; GetSavePath(path);
	DeleteFileA(path);
	saveloaded=false;
	// 5) toast (notify was just restored from oglconf.cfg so it's the default value)
	cvar.notify=1;			// force toast visible even if config had notify 0
	SetToast("Config reset to defaults!");
}

void HandleKey(int key) // keyhandler
{
	if(GetAsyncKeyState(key))
	{
		switch(key)
		{
		case VK_F12:
			if(!key_init)
			{
				key_init=true; 
				if(!hookactive)     { HookInit(true); }
				else if(hookactive) { HookInit(false); }
			}
			break;
		case VK_F11:
			if(!key_check)
			{
				key_check=true; 
				checktext=!checktext;
				if(checktext) check_open_seq=++ui_open_seq;	// opened last -> draws on top
			}
			break;
		case VK_F10:
			if(!key_ften)
			{
				key_ften=true;
				ResetConfig();
			}
			break;
		case VK_INSERT:
			if(!keyp.active)
			{
				keyp.active=true;
				if(menu_move_mode)			// in move mode: Insert = finish (don't close menu)
				{
					menu_move_mode=0;
					SaveSettings();			// persist the new position
				}
				else
				{
					menu.active=!menu.active;
					if(menu.active) menu_open_seq=++ui_open_seq;	// opened last -> on top
				}
			}
			break;
		case VK_UP:
			if(menu_move_mode) MoveActivePanel(0,-MOVE_STEP);	// continuous while held
			else if(menu.active)
			{
				DWORD now=GetTickCount();
				if(!keyp.up) { keyp.up=true; menu.count-=1; menu_rep_t=now+MENU_REP_DELAY; }	// first press
				else if(now>=menu_rep_t) { menu.count-=1; menu_rep_t=now+MENU_REP_RATE; }		// auto-repeat while held
			}
			break;
		case VK_DOWN:
			if(menu_move_mode) MoveActivePanel(0,MOVE_STEP);
			else if(menu.active)
			{
				DWORD now=GetTickCount();
				if(!keyp.down) { keyp.down=true; menu.count+=1; menu_rep_t=now+MENU_REP_DELAY; }
				else if(now>=menu_rep_t) { menu.count+=1; menu_rep_t=now+MENU_REP_RATE; }
			}
			break;
		case VK_RIGHT:
			if(menu_move_mode) MoveActivePanel(MOVE_STEP,0);
			else if(menu.active && !keyp.right)
			{
				keyp.right=true;
				menu.select=true;
				menu.select_r=true;
			}
			break;
		case VK_LEFT:
			if(menu_move_mode) MoveActivePanel(-MOVE_STEP,0);
			else if(menu.active && !keyp.left)
			{
				keyp.left=true;
				menu.select=true;
				menu.select_l=true;
			}
			break;
		case VK_LBUTTON:
			if (cvar.recoil > 0)
			{
				recoilnum++;
				if(recoilnum > 0)
				{
					mouse_event(MOUSEEVENTF_MOVE,0,cvar.recoil,0,0);
					recoilnum=0;
				}
			}
			break;
		}
	}
	else if(!GetAsyncKeyState(key))
	{
		switch(key)
		{
		case VK_INSERT: keyp.active=false; break;
		case VK_UP: keyp.up=false; break;
		case VK_DOWN: keyp.down=false; break;
		case VK_RIGHT: keyp.right=false; break;
		case VK_LEFT: keyp.left=false; break;
		case VK_F10: key_ften=false; break;
		case VK_F11: key_check=false; break;
		case VK_F12: key_init=false; break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool pTimer(int sec)	// a timer function
{
    SYSTEMTIME pTimer;
    GetLocalTime(&pTimer);
	if(!t_get)
	{
		t_count=pTimer.wSecond+sec;
		t_get=true;
		if(t_count>=60)
			t_count-=60;
	}
	if(t_count==pTimer.wSecond)
	{
		t_get=false;
		return false; // expired
	}
	else
		return true; // running
}

//////////////////////////////////////////////////////////////////////////////////////////////////
GLvoid BuildFont(GLvoid) // loads the opengl font into memory
{
	hDC=wglGetCurrentDC();

	// scale the bitmap font to the screen so text isn't tiny on 4K. Baseline is
	// 1080p; clamp so it never gets smaller than the original or absurdly big.
	GLint vpf[4]; (*orig_glGetIntegerv)(GL_VIEWPORT,vpf);
	ui_scale = (vpf[3]>0) ? (float)vpf[3]/1080.0f : 1.0f;
	if(ui_scale<1.0f) ui_scale=1.0f;
	if(ui_scale>2.5f) ui_scale=2.5f;

	// Build 4 font sizes: 7/10/13/16 px at 1080p (all scaled by ui_scale)
	static const float sz_px[FONT_SIZES]={7.0f,10.0f,13.0f,16.0f};
	for(int si=0;si<FONT_SIZES;si++)
	{
		int fh=(int)(sz_px[si]*ui_scale+0.5f);
		g_font_base[si]=(*orig_glGenLists)(96);
		HFONT font=CreateFont(-fh,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_DONTCARE|DEFAULT_PITCH,"Verdana");
		HFONT oldfont=(HFONT)SelectObject(hDC,font);
		wglUseFontBitmaps(hDC,32,96,g_font_base[si]);
		GetCharWidth32(hDC,32,127,g_fontw_sz[si]);
		SelectObject(hDC,oldfont);
		DeleteObject(font);
	}
	base=g_font_base[1];				// size 2 (10px) = default for all DrawText calls
	memcpy(g_fontw,g_fontw_sz[1],sizeof(g_fontw));	// sync g_fontw to the default size
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel width of a string in the current bitmap font (uses the widths captured
// in BuildFont). Already in scaled pixels, so no extra ui_scale is needed.
float TextWidthPx(const char *s)
{
	if(!s) return 0.0f;
	float w=0.0f;
	for(; *s; s++)
	{
		unsigned char c=(unsigned char)*s;
		if(c>=32 && c<128) w+=(float)g_fontw[c-32];
	}
	return w;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
GLvoid KillFont(GLvoid)									
{
	for(int si=0;si<FONT_SIZES;si++)
		(*orig_glDeleteLists)(g_font_base[si], 96);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void DrawText(float x, float y,float r, float g, float b, const char *fmt, ...)
{
	char		text[256];								
	va_list		ap;										
	
	if (fmt == NULL)									
		return;											
	
	va_start(ap, fmt);									
	vsprintf(text, fmt, ap);						
	va_end(ap);

	GLfloat  curcolor[4], position[4];
	(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
	(*orig_glGetFloatv)(GL_CURRENT_COLOR, curcolor);
	(*orig_glGetFloatv)(GL_CURRENT_RASTER_POSITION, position);
	(*orig_glDisable)(GL_TEXTURE_2D); 
	(*orig_glColor4f)(0.0f,0.0f,0.0f,gTextAlpha);
	(*orig_glRasterPos2f)(x+1,y+1);

	//glPrint(text) - shadow
	(*orig_glPushAttrib)(GL_LIST_BIT);							
	(*orig_glListBase)(base - 32);								
	(*orig_glCallLists)(strlen(text), GL_UNSIGNED_BYTE, text);	
	(*orig_glPopAttrib)();										
	(*orig_glEnable)(GL_TEXTURE_2D); 

	(*orig_glDisable)(GL_TEXTURE_2D); 
	(*orig_glColor4f)(r,g,b,gTextAlpha);
	(*orig_glRasterPos2f)(x,y);
	(*orig_glColor4f)(r,g,b,gTextAlpha);

	//glPrint(text);
	(*orig_glPushAttrib)(GL_LIST_BIT);							
	(*orig_glListBase)(base - 32);								
	(*orig_glCallLists)(strlen(text), GL_UNSIGNED_BYTE, text);	
	(*orig_glPopAttrib)();										
	(*orig_glEnable)(GL_TEXTURE_2D); 

    //restore ogl shiat
    (*orig_glPopAttrib)();
    (*orig_glColor4fv)(curcolor);
    (*orig_glRasterPos2f)(position[0],position[1]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel width of a string using a specific font size (1-4).
float TextWidthPxSz(const char *s, int sz)
{
	if(!s) return 0.0f;
	if(sz<1) sz=1; if(sz>FONT_SIZES) sz=FONT_SIZES;
	float w=0.0f;
	for(; *s; s++)
	{
		unsigned char c=(unsigned char)*s;
		if(c>=32 && c<128) w+=(float)g_fontw_sz[sz-1][c-32];
	}
	return w;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// DrawText variant that renders at a specific font size (1=small .. 4=x-large).
void DrawTextSz(float x, float y, float r, float g, float b, int sz, const char *fmt, ...)
{
	char		text[256];
	va_list		ap;
	if(fmt==NULL) return;
	va_start(ap,fmt); vsprintf(text,fmt,ap); va_end(ap);
	if(sz<1) sz=1; if(sz>FONT_SIZES) sz=FONT_SIZES;
	GLuint fsz=g_font_base[sz-1]; if(fsz==0) fsz=base;	// fallback to default if not built

	GLfloat  curcolor[4], position[4];
	(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
	(*orig_glGetFloatv)(GL_CURRENT_COLOR, curcolor);
	(*orig_glGetFloatv)(GL_CURRENT_RASTER_POSITION, position);
	(*orig_glDisable)(GL_TEXTURE_2D);
	(*orig_glColor4f)(0.0f,0.0f,0.0f,gTextAlpha);
	(*orig_glRasterPos2f)(x+1,y+1);
	(*orig_glPushAttrib)(GL_LIST_BIT);
	(*orig_glListBase)(fsz - 32);
	(*orig_glCallLists)(strlen(text), GL_UNSIGNED_BYTE, text);
	(*orig_glPopAttrib)();
	(*orig_glEnable)(GL_TEXTURE_2D);
	(*orig_glDisable)(GL_TEXTURE_2D);
	(*orig_glColor4f)(r,g,b,gTextAlpha);
	(*orig_glRasterPos2f)(x,y);
	(*orig_glColor4f)(r,g,b,gTextAlpha);
	(*orig_glPushAttrib)(GL_LIST_BIT);
	(*orig_glListBase)(fsz - 32);
	(*orig_glCallLists)(strlen(text), GL_UNSIGNED_BYTE, text);
	(*orig_glPopAttrib)();
	(*orig_glEnable)(GL_TEXTURE_2D);
	(*orig_glPopAttrib)();
	(*orig_glColor4fv)(curcolor);
	(*orig_glRasterPos2f)(position[0],position[1]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Queue a short on-screen toast (e.g. "Wallhack: On"). Respects cvar.notify so
// it can be disabled. The actual drawing/fade happens in DrawToast() each frame.
void SetToast(const char *fmt, ...)
{
	if(!cvar.notify) return;
	va_list ap; va_start(ap,fmt);
	vsprintf(toast_msg,fmt,ap);
	va_end(ap);
	toast_until=GetTickCount()+2000;	// visible for 2 seconds
}

// Queue the pink aim-status pill for Toggle mode (e.g. "Aimbot: ON"). Same 2s
// lifetime/fade as a toast; pressing the aim key again just resets the timeout.
// Respects cvar.notify like the regular toast.
void SetAimStatus(const char *fmt, ...)
{
	if(!cvar.notify) return;
	va_list ap; va_start(ap,fmt);
	vsprintf(aim_status_msg,fmt,ap);
	va_end(ap);
	aim_status_until=GetTickCount()+2000;	// same visible window as SetToast
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Data-driven, animated menu:
//   * fade in/out (menu_alpha) and a highlight bar that smoothly slides to the
//     selected row (both time-based, so they look the same at 60 or 240 fps)
//   * dependency hiding: a row with a non-null "dep" pointer is shown only while
//     that parent cvar is on (e.g. Shoot/Aimthru/FOV hide unless Aimbot is on),
//     and the cursor navigates only over visible rows.
enum { IT_TOGGLE, IT_INT, IT_TARGET, IT_MOVE, IT_ACTION };
typedef struct {
	const char *label;
	int   type;
	void *p;			// int* cvar this row controls
	float mn, mx, step;	// range/step for IT_INT
	int   wrap;			// wrap around at the bounds?
	int  *dep;			// non-null => row hidden while *dep == 0
	int   child;		// 1 = indented sub-item
} mitem_t;

// Advance the menu fade once per frame. Must be called EVERY frame (even while the
// menu is hidden) so the time delta never freezes - otherwise re-opening the menu
// sees a huge dt and snaps to full opacity with no fade-in.
void UpdateMenuAnim()
{
	DWORD now=GetTickCount();
	if(menu_last_tick==0) menu_last_tick=now;
	float dt=(now-menu_last_tick)/1000.0f; menu_last_tick=now;
	if(dt<0) dt=0; if(dt>0.1f) dt=0.1f;
	g_menu_dt=dt;

	float ka = dt*18.0f; if(ka>1.0f) ka=1.0f;		// snappy ~0.05s fade, both directions
	menu_alpha  += ((menu.active?1.0f:0.0f) - menu_alpha )*ka;	// hack menu
	check_alpha += ((checktext  ?1.0f:0.0f) - check_alpha)*ka;	// F11 check screen
}

void DrawMenu(int x, int y)
{
	static mitem_t items[] = {
		{"Aimbot",      IT_TOGGLE, &cvar.aim,        0,0,0,       0, 0,          0},
		{"Aim smooth",  IT_INT,    &cvar.aim_smooth, 0,10,1,      0, &cvar.aim,  1},
		{"Target",      IT_TARGET, &cvar.target,     0,0,0,       0, &cvar.aim,  1},
		{"Shoot",       IT_TOGGLE, &cvar.shoot,      0,0,0,       0, &cvar.aim,  1},
		{"Aimthru",     IT_TOGGLE, &cvar.aimthru,    0,0,0,       0, &cvar.aim,  1},
		{"FOV",         IT_INT,    &cvar.fov,        0,1000,10,   1, &cvar.aim,  1},
		{"Head dot",    IT_TOGGLE, &cvar.aim_dot,    0,0,0,       0, &cvar.aim,  1},
		{"Aim point",   IT_INT,    &cvar.aim_point,  -50,50,1,    0, &cvar.aim,  1},
		{"Aim mode",    IT_INT,    &cvar.aim_mode,   0,2,1,       1, &cvar.aim,  1},
		{"Aim key",     IT_INT,    &cvar.aim_key,    0,KEY_TABLE_COUNT-1,1, 1, &cvar.aim, 1},
		{"Triggerbot",  IT_TOGGLE, &cvar.trigger,    0,0,0,       0, 0,          0},
		{"Trigger delay",IT_INT,   &cvar.trigger_delay,0,500,10,  0, &cvar.trigger,1},
		{"Auto-fire",   IT_TOGGLE, &cvar.autofire,   0,0,0,       0, 0,          0},
		{"Auto-fire rate",IT_INT,  &cvar.autofire_rate,20,300,10, 0, &cvar.autofire,1},
		{"Bhop",        IT_TOGGLE, &cvar.bhop,       0,0,0,       0, 0,          0},
		{"Bhop hold",   IT_INT,    &cvar.bhop_hold,  0,1,1,       1, &cvar.bhop, 1},
		{"Bhop key",    IT_INT,    &cvar.bhop_key,   0,KEY_TABLE_COUNT-1,1, 1, &cvar.bhop, 1},
		{"Recoil",      IT_INT,    &cvar.recoil,     0,5,1,       1, 0,          0},
		{"No recoil",   IT_TOGGLE, &cvar.norecoil,   0,0,0,       0, 0,          0},
		{"Wallhack",    IT_INT,    &cvar.wall,       0,3,1,       1, 0,          0},
		{"No Sky",      IT_TOGGLE, &cvar.sky,        0,0,0,       0, 0,          0},
		{"No Flash",    IT_TOGGLE, &cvar.flash,      0,0,0,       0, 0,          0},
		{"No Smoke",    IT_TOGGLE, &cvar.smoke,      0,0,0,       0, 0,          0},
		{"Lambert",     IT_TOGGLE, &cvar.lambert,    0,0,0,       0, 0,          0},
		{"ESP Engine",  IT_TOGGLE, &cvar.esp_engine, 0,0,0,       0, 0,                0},
		{"Player name", IT_TOGGLE, &cvar.esp_name,     0,0,0,      0, &cvar.esp_engine, 1},
		{"Name size",   IT_INT,    &cvar.esp_name_size, 1,4,1,      1, &cvar.esp_name,   1},
		{"Name padding",IT_INT,    &cvar.esp_name_pad,  -20,40,2,   0, &cvar.esp_name,   1},
		{"Box",         IT_TOGGLE, &cvar.esp_box,       0,0,0,      0, &cvar.esp_engine, 1},
		{"Box padding", IT_INT,    &cvar.esp_box_pad,   -10,40,2, 0, &cvar.esp_engine, 1},
		{"Box radius",  IT_INT,    &cvar.esp_box_radius, 0,20,2,  0, &cvar.esp_engine, 1},
		{"Box width",   IT_INT,    &cvar.esp_box_width,  1,8,1,   0, &cvar.esp_engine, 1},
		{"Distance",    IT_TOGGLE, &cvar.esp_dist,     0,0,0,      0, &cvar.esp_engine, 1},
		{"Dist size",   IT_INT,    &cvar.esp_dist_size, 1,4,1,      1, &cvar.esp_dist,   1},
		{"Dist padding",IT_INT,    &cvar.esp_dist_pad,  -20,40,2,   0, &cvar.esp_dist,   1},
		{"Snaplines",   IT_INT,    &cvar.esp_snap,   0,3,1,       1, &cvar.esp_engine, 1},
		{"Vis check",   IT_TOGGLE, &cvar.esp_vischeck,0,0,0,      0, &cvar.esp_engine, 1},
		{"Off-screen arrow",IT_TOGGLE,&cvar.esp_arrow,0,0,0,      0, &cvar.esp_engine, 1},
		{"Max distance",IT_INT,    &cvar.esp_maxdist, 0,200,5,    0, &cvar.esp_engine, 1},
		{"Distance fade",IT_TOGGLE,&cvar.esp_fade,    0,0,0,      0, &cvar.esp_engine, 1},
		{"Show team",   IT_INT,    &cvar.esp_team,    0,2,1,      1, &cvar.esp_engine, 1},
		{"Debug text",  IT_TOGGLE, &cvar.esp_dbg,    0,0,0,      0, &cvar.esp_engine, 1},
		{"HUD HP/Ammo", IT_TOGGLE, &cvar.esp_hud,    0,0,0,       0, 0,                0},
		{"HP",          IT_TOGGLE, &cvar.hud_hp,     0,0,0,       0, &cvar.esp_hud,    1},
		{"Ammo",        IT_TOGGLE, &cvar.hud_ammo,   0,0,0,       0, &cvar.esp_hud,    1},
		{"Show when die",IT_TOGGLE,&cvar.hud_die,    0,0,0,       0, &cvar.esp_hud,    1},
		{"Padding",     IT_INT,    &cvar.hud_pad,   -20,80,4,     0, &cvar.esp_hud,    1},
		{"Spec warning",IT_TOGGLE, &cvar.spec_warn,  0,0,0,       0, 0,                0},
		{"Spec padding",IT_INT,    &cvar.spec_pad,  -40,200,4,    0, &cvar.spec_warn,  1},
		{"Chams",       IT_TOGGLE, &cvar.chams,      0,0,0,       0, 0,                0},
		{"Chams Wire",  IT_TOGGLE, &cvar.chams_wire, 0,0,0,       0, &cvar.chams,      1},
		{"Radar",       IT_TOGGLE, &cvar.radar,      0,0,0,       0, 0,                0},
		{"Move radar",  IT_MOVE,   0,               3,0,0,       0, &cvar.radar,      1},
		{"Dot shape",   IT_INT,    &cvar.radar_shape,0,1,1,       1, &cvar.radar,      1},
		{"Size",        IT_INT,    &cvar.radar_size, 30,150,5,    0, &cvar.radar,      1},
		{"Zoom (units)",IT_INT,    &cvar.radar_zoom, 200,5000,100,0, &cvar.radar,      1},
		{"Rotate view", IT_TOGGLE, &cvar.radar_rotate,0,0,0,      0, &cvar.radar,      1},
		{"Names",       IT_TOGGLE, &cvar.radar_names, 0,0,0,      0, &cvar.radar,      1},
		{"Range rings", IT_TOGGLE, &cvar.radar_rings, 0,0,0,      0, &cvar.radar,      1},
		{"Crosshair",   IT_TOGGLE, &cvar.cross,      0,0,0,       0, 0,                0},
		{"Notifications",IT_TOGGLE,&cvar.notify,     0,0,0,       0, 0,                0},
		{"Detect log",  IT_TOGGLE, &cvar.esp_log,    0,0,0,       0, 0,                0},
		{"Move hack menu", IT_MOVE,  0,             1,0,0,       0, 0,                0},
		{"Move F11 panel", IT_MOVE,  0,             2,0,0,       0, 0,                0},
		{"Reset positions",IT_ACTION,0,             0,0,0,       0, 0,                0},
	};
	const int N = sizeof(items)/sizeof(items[0]);

	// menu_alpha / g_menu_dt are advanced once per frame by UpdateMenuAnim() (from the
	// swap-buffers overlay pass), so here we only need to draw. The whole menu fades
	// together via menu_alpha (text through gTextAlpha, panel + highlight via alpha).
	if(menu_alpha<0.002f) return;					// fully hidden
	float dt=g_menu_dt;

	// visible rows only (skip those whose parent cvar is off)
	int vis[80], nvis=0;
	for(int i=0;i<N;i++)
		if(items[i].dep==0 || *(items[i].dep)!=0) vis[nvis++]=i;
	if(nvis==0) return;
	if(menu.count>=nvis) menu.count=nvis-1;			// clamp the cursor (no wrap-around now that we scroll)
	if(menu.count<0)     menu.count=0;

	// scroll window: keep a fixed height and slide so the cursor stays visible
	int visRows = cvar.menu_vis_rows; if(visRows<4) visRows=4; if(visRows>55) visRows=55;
	visRows = (nvis<visRows)?nvis:visRows;
	if(menu.count < menu_scroll)            menu_scroll = menu.count;				// scrolled above the view
	if(menu.count >= menu_scroll+visRows)   menu_scroll = menu.count-visRows+1;	// scrolled below the view
	if(menu_scroll > nvis-visRows)          menu_scroll = nvis-visRows;			// don't show empty space past the end
	if(menu_scroll < 0)                     menu_scroll = 0;

	// apply a left/right press to the current row
	if(menu.select)
	{
		menu.select=false;
		int dir = menu.select_r?1:(menu.select_l?-1:0);
		menu.select_r=false; menu.select_l=false;
		mitem_t *it=&items[vis[menu.count]];
		switch(it->type)
		{
		case IT_TOGGLE:
		case IT_TARGET:
			*(int*)it->p = change(*(int*)it->p);
			break;
		case IT_INT:
			{
				int *v=(int*)it->p; *v += dir*(int)it->step;
				if(*v>(int)it->mx) *v = it->wrap?(int)it->mn:(int)it->mx;
				if(*v<(int)it->mn) *v = it->wrap?(int)it->mx:(int)it->mn;
			}
			break;
		case IT_MOVE:
			menu_move_mode=(int)it->mn;			// enter move mode (arrows now move the panel)
			if(menu_move_mode==2) checktext=true;	// reveal F11 so you can position it
			if(menu_move_mode==3) cvar.radar=1;		// reveal radar so you can position it
			break;
		case IT_ACTION:							// "Reset positions"
			cvar.menu_x=MENU_DEF_X;   cvar.menu_y=MENU_DEF_Y;
			cvar.check_x=CHECK_DEF_X; cvar.check_y=CHECK_DEF_Y;
			cvar.radar_x=RADAR_DEF_X; cvar.radar_y=RADAR_DEF_Y;
			menu_move_mode=0;
			break;
		}
		// toast the change (skipped automatically if cvar.notify is off)
		if(it->type==IT_TOGGLE)      SetToast("%s: %s", it->label, (*(int*)it->p)?"On":"Off");
		else if(it->type==IT_INT)
		{
			if(it->p==&cvar.aim_mode)
			{
				static const char *am[3]={"Always","Hold","Toggle"};
				int v=*(int*)it->p; if(v<0)v=0; if(v>2)v=2; SetToast("%s: %s", it->label, am[v]);
			}
			else if(it->p==&cvar.bhop_hold)
			{
				static const char *bm[2]={"Always","Hold"};
				int v=*(int*)it->p; if(v<0)v=0; if(v>1)v=1; SetToast("%s: %s", it->label, bm[v]);
			}
			else if(it->p==&cvar.aim_key || it->p==&cvar.bhop_key)
				SetToast("%s: %s", it->label, KeyTableName(*(int*)it->p));
			else SetToast("%s: %i", it->label, *(int*)it->p);
		}
		else if(it->type==IT_TARGET) SetToast("Target: %s", (*(int*)it->p)?team[1].name:team[0].name);
		SaveSettings();		// persist the change so it survives the next game launch
	}

	// layout (everything scaled to the resolution via ui_scale)
	float sc=ui_scale, mx=x*sc, my=y*sc, line=13.0f*sc, y0=my+7.0f*sc;
	float ks=dt*16.0f; if(ks>1.0f) ks=1.0f;
	menu_sel_anim += ((float)menu.count-menu_sel_anim)*ks;	// slide highlight

	gTextAlpha=menu_alpha;

	// dark translucent panel behind the menu for readability (fades with the menu)
	{
		float bl=mx-8*sc, bt=my-33*sc, br=mx+182*sc;
		float bb=y0+visRows*line-2*sc + (menu_move_mode? 12.0f*sc : 0.0f);	// fixed height (scrolls); room for the move hint
		float rad=8.0f*sc;										// soft rounded corners
		(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
		(*orig_glDisable)(GL_TEXTURE_2D);
		(*orig_glEnable)(GL_BLEND);
		(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		(*orig_glColor4f)(0.0f,0.0f,0.0f,0.55f*menu_alpha);		// translucent fill
		FillRoundRect2D(bl,bt,br,bb,rad);
		(*orig_glColor4f)(0.1f,0.6f,1.0f,menu_alpha);			// bold accent border (opaque, fades w/ menu)
		(*orig_glLineWidth)(2.0f*sc);
		DrawBox2D(bl,bt,br,bb,rad);
		(*orig_glPopAttrib)();
	}

	DrawText(mx, my-20*sc, 0.85f,0.9f,1.0f, "Mod by maitrungduc1410");
	DrawText(mx, my-8 *sc, 0.55f,0.55f,0.75f, "----------------------");

	// sliding highlight bar behind the selected row (rounded corners).
	// menu_sel_anim is in absolute-row space; shift by the scroll and clamp to
	// the window so it parks at the edge instead of sliding off while scrolling.
	{
		float selDisp=menu_sel_anim-(float)menu_scroll;
		if(selDisp<0) selDisp=0; if(selDisp>(float)(visRows-1)) selDisp=(float)(visRows-1);
		float hy=y0+selDisp*line;
		float hrad=4.0f*sc;									// soft corners on the highlight
		(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
		(*orig_glDisable)(GL_TEXTURE_2D);
		(*orig_glEnable)(GL_BLEND);
		(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		(*orig_glColor4f)(0.1f,0.6f,1.0f,0.30f*menu_alpha);
		FillRoundRect2D(mx-5*sc, hy-line+3*sc, mx+160*sc, hy+3*sc, hrad);
		(*orig_glPopAttrib)();
	}

	char buf[96];
	for(int r=menu_scroll;r<menu_scroll+visRows && r<nvis;r++)
	{
		mitem_t *it=&items[vis[r]];
		float ry=y0+(r-menu_scroll)*line, ix=mx+(it->child?10.0f*sc:0.0f);
		const char *pre=it->child?"- ":"";
		switch(it->type)
		{
		case IT_TOGGLE: sprintf(buf,"%s%s: %s", pre,it->label,(*(int*)it->p)?"On":"Off"); break;
		case IT_INT:
			if(it->p==&cvar.radar_shape)
			{
				static const char *rs[2]={"Circle","Square"};
				int v=*(int*)it->p; if(v<0)v=0; if(v>1)v=1;
				sprintf(buf,"%s%s: %s", pre,it->label,rs[v]);
			}
			else if(it->p==&cvar.esp_team)
			{
				static const char *et[3]={"Both","CT","T"};
				int v=*(int*)it->p; if(v<0)v=0; if(v>2)v=2;
				sprintf(buf,"%s%s: %s", pre,it->label,et[v]);
			}
			else if(it->p==&cvar.esp_snap)
			{
				static const char *sn[4]={"Off","Bottom","Top","Crosshair"};
				int v=*(int*)it->p; if(v<0)v=0; if(v>3)v=3;
				sprintf(buf,"%s%s: %s", pre,it->label,sn[v]);
			}
			else if(it->p==&cvar.esp_maxdist)
			{
				int v=*(int*)it->p;
				if(v<=0) sprintf(buf,"%s%s: Unlimited", pre,it->label);
				else     sprintf(buf,"%s%s: %im", pre,it->label,v);
			}
			else if(it->p==&cvar.aim_mode)
			{
				static const char *am[3]={"Always","Hold","Toggle"};
				int v=*(int*)it->p; if(v<0)v=0; if(v>2)v=2;
				sprintf(buf,"%s%s: %s", pre,it->label,am[v]);
			}
			else if(it->p==&cvar.bhop_hold)
			{
				static const char *bm[2]={"Always","Hold"};
				int v=*(int*)it->p; if(v<0)v=0; if(v>1)v=1;
				sprintf(buf,"%s%s: %s", pre,it->label,bm[v]);
			}
			else if(it->p==&cvar.aim_key || it->p==&cvar.bhop_key)
			{
				sprintf(buf,"%s%s: %s", pre,it->label,KeyTableName(*(int*)it->p));
			}
			else sprintf(buf,"%s%s: %i", pre,it->label,*(int*)it->p);
			break;
		case IT_TARGET: sprintf(buf,"%s%s: %s", pre,it->label,(*(int*)it->p)?team[1].name:team[0].name); break;
		case IT_MOVE:
			if(menu_move_mode==(int)it->mn) sprintf(buf,"%s%s: [moving]", pre,it->label);
			else                            sprintf(buf,"%s%s", pre,it->label);
			break;
		case IT_ACTION: sprintf(buf,"%s%s", pre,it->label); break;
		default: buf[0]=0; break;
		}
		if(r==menu.count) DrawText(ix,ry, 1.0f,1.0f,1.0f, "%s", buf);
		else              DrawText(ix,ry, 0.7f,0.7f,1.0f, "%s", buf);
	}

	// scrollbar: a dim track + accent thumb on the right, only when the list overflows
	if(nvis>visRows)
	{
		float trackTop=y0-line+4.0f*sc, trackH=(float)visRows*line;
		float sbx=mx+168.0f*sc, sbw=3.0f*sc;
		float frac=(float)visRows/(float)nvis;					// portion of the list on screen
		float thumbH=trackH*frac; if(thumbH<8.0f*sc) thumbH=8.0f*sc;
		float sf=(float)menu_scroll/(float)(nvis-visRows);		// 0..1 scroll position
		float thumbTop=trackTop+(trackH-thumbH)*sf;
		(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
		(*orig_glDisable)(GL_TEXTURE_2D);
		(*orig_glEnable)(GL_BLEND);
		(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		(*orig_glColor4f)(1.0f,1.0f,1.0f,0.12f*menu_alpha);		// track
		FillRoundRect2D(sbx,trackTop,sbx+sbw,trackTop+trackH,sbw*0.5f);
		(*orig_glColor4f)(0.1f,0.6f,1.0f,0.85f*menu_alpha);		// thumb (accent)
		FillRoundRect2D(sbx,thumbTop,sbx+sbw,thumbTop+thumbH,sbw*0.5f);
		(*orig_glPopAttrib)();
	}

	if(menu_move_mode)		// footer hint shown only while repositioning a panel
		DrawText(mx, y0+visRows*line+1.0f*sc, 1.0f,0.9f,0.4f, "arrows move, Insert=done");

	gTextAlpha=1.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void DrawCheckText(int x,int y) // bad way of doing this
{
	static float check_h=300.0f;	// panel height (px), measured on the previous frame
	int startY=y;
	gTextAlpha=check_alpha;			// whole check screen fades in/out with check_alpha

	// dark translucent panel behind the F11 check screen for readability
	{
		float padL=10.0f*ui_scale, padT=14.0f*ui_scale;	// more top room so text isn't flush
		float pl=(float)x-padL, pt=(float)y-padT, pr=(float)x+540.0f*ui_scale, pb=(float)y+check_h+padL;
		float rad=8.0f*ui_scale;						// soft rounded corners
		(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
		(*orig_glDisable)(GL_TEXTURE_2D);
		(*orig_glEnable)(GL_BLEND);
		(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		(*orig_glColor4f)(0.0f,0.0f,0.0f,0.6f*check_alpha);	// translucent fill
		FillRoundRect2D(pl,pt,pr,pb,rad);
		(*orig_glColor4f)(0.1f,0.6f,1.0f,check_alpha);		// bold accent border (opaque, fades w/ F11)
		(*orig_glLineWidth)(2.0f*ui_scale);
		DrawBox2D(pl,pt,pr,pb,rad);
		(*orig_glPopAttrib)();
	}

	DrawText(x,y,0.7f,0.7f,1.0f,"Mod by maitrungduc1410 - Check");
	y=y+(int)(13*ui_scale);
	DrawText(x,y,1.0f,1.0f,1.0f,"> Hack file: %s",dllpath);
	y=y+(int)(13*ui_scale);
	if(cfgfail)
		DrawText(x,y,1.0f,0.5f,0.5f,"> Could not load config file: <%s> !!!",configpath);
	else
		DrawText(x,y,1.0f,1.0f,1.0f,"> Config file: %s",configpath);
	y=y+(int)(13*ui_scale);
	if(saveloaded)
		DrawText(x,y,0.5f,1.0f,0.5f,"> Saved settings: %s",savepath);
	else
		DrawText(x,y,1.0f,0.85f,0.4f,"> Saved settings: none yet (using defaults) <%s>",savepath);
	y=y+(int)(13*ui_scale);
	DrawText(x,y,0.5f,1.0f,0.5f,"> Your screen resolution is: %ix%i",vp[2],vp[3]);
	y=y+(int)(26*ui_scale);
	DrawText(x,y,0.7f,0.7f,1.0f,"Teams: %s vs %s",team[0].name,team[1].name);
	y=y+(int)(13*ui_scale);
	// ---- No-recoil diagnostics (so we can see WHY it does / doesn't work) ----
	y=y+(int)(13*ui_scale);
	if(!cvar.norecoil)
		DrawText(x,y,0.7f,0.7f,1.0f,"No recoil: OFF");
	else if(!norec_hooked)
		DrawText(x,y,1.0f,0.5f,0.5f,"No recoil: ON but V_CalcRefdef NOT hooked yet (waiting for client.dll)");
	else
		DrawText(x,y,0.5f,1.0f,0.5f,"No recoil: ON  hooked V_CalcRefdef @ 0x%08X",norec_fn);
	y=y+(int)(13*ui_scale);
	DrawText(x,y,1.0f,1.0f,1.0f,"> peak punch seen: %0.3f   last: %0.2f %0.2f %0.2f",
		norec_peak,norec_last[0],norec_last[1],norec_last[2]);
	y=y+(int)(13*ui_scale);

	check_h=(float)(y-startY)+13.0f*ui_scale;	// size next frame's panel to fit the text
	gTextAlpha=1.0f;							// restore for anything drawn after us
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//  TIER 2 - engine entity-list ESP
//
//  Instead of guessing players from vertex counts, we read the GoldSrc engine's
//  own entity list. We locate the engine function table (cl_enginefunc_t) that
//  the engine hands to client.dll, then call:
//      slot 21  pfnGetPlayerInfo   -> real name / spectator flag
//      slot 51  GetLocalPlayer     -> local cl_entity_t*
//      slot 53  GetEntityByIndex   -> cl_entity_t* for any slot
//      slot 82  pTriAPI            -> TriAPI; its slot 12 is WorldToScreen
//  Entity/struct byte-offsets below are for engine build 4554 (the common one).
//  Everything is wrapped in IsReadable() guards, so a wrong offset just yields
//  no box instead of a crash. Approach ported from EnurDev/goldsrc-esp-overlay.
//////////////////////////////////////////////////////////////////////////////////////////////////

#define ENG_SLOT_GETPLAYERINFO		21	// cl_enginefunc_t indices (DWORD slots)
#define ENG_SLOT_GETVIEWANGLES		34	// pfnGetViewAngles(float* -> pitch,yaw,roll)
#define ENG_SLOT_GETLOCALPLAYER		51
#define ENG_SLOT_GETENTITYBYINDEX	53
#define ENG_SLOT_PTRIAPI			82

#define ENT_INDEX			0x000	// cl_entity_t: int  entity index
#define ENT_PLAYER			0x004	// cl_entity_t: int  "is player" flag
#define ENT_CURSTATE		0x2B0	// cl_entity_t: entity_state_t curstate
#define ENT_CURPOS			0x404	// cl_entity_t: current_position (update counter)
#define ENT_ORIGIN			0xB48	// cl_entity_t: vec3 interpolated origin
#define ENG_STALE_MS		400		// ms without an update -> treat as dead/gone (fps-independent)
#define ENG_DEATH_HOLD_MAX_MS	1200	// safety cap on the DeathMsg latch: normally we hold a corpse until EngDead/stale/respawn confirms it, but never longer than this so a missed confirmation can't hide a live player. Kept short because in deathmatch the engine may never report the death (instant respawn keeps the slot alive+streaming), so this cap, not a confirmation, ends the hold.
#define ENG_RESPAWN_DIST		150.0f	// world units: an origin jump this large while latched means the player respawned (teleported to a spawn point), so release the latch immediately instead of waiting on the engine / safety cap. A corpse never moves this far.
#define ES_ORIGIN			0x010	// entity_state_t::origin (vec3)
#define ES_USEHULL			0x0C8	// entity_state_t::usehull (0 stand, 1 duck)
#define ES_ONGROUND			0x0D0	// entity_state_t::onground (-1 = airborne, else ground ent idx)
#define ES_IUSER1			0x104	// entity_state_t::iuser1 (spectator observer MODE; 0 = not observing)
#define ES_IUSER2			0x108	// entity_state_t::iuser2 (spectator observer TARGET entity index)
#define ENG_SPEC_MATCH_R	48.0f	// spec_warn fallback: a spectator whose camera origin is within this many world units of the local player is treated as watching us (catches in-eye spectators when the engine doesn't replicate iuser2)

#define EXTRA_STRIDE		0x68	// g_PlayerExtraInfo entry size
#define EXTRA_TEAMNUMBER	0x2A	// short: team (1=T, 2=CT)
#define EXTRA_DEAD			0x3C	// byte : 0=alive

typedef struct {					// what pfnGetPlayerInfo fills in
	char	*name;
	short	ping;
	BYTE	thisplayer;
	BYTE	spectator;
	BYTE	packetloss;
	char	*model;
	short	topcolor;
	short	bottomcolor;
	__int64	steamid;
} hud_player_info_t;

// engine fns are __cdecl while our DLL builds with /Gz (__stdcall), so the
// calling convention MUST be stated explicitly or the stack gets corrupted.
typedef void* (__cdecl *eng_GetLocalPlayer_t)(void);
typedef void* (__cdecl *eng_GetEntityByIndex_t)(int idx);
typedef void  (__cdecl *eng_GetPlayerInfo_t)(int idx, hud_player_info_t *info);
typedef int   (__cdecl *eng_WorldToScreen_t)(float *world, float *screen);
typedef void  (__cdecl *eng_GetViewAngles_t)(float *angles);

bool IsReadable(DWORD addr,DWORD len)	// is [addr,addr+len) committed & readable?
{
	if(addr==0 || len==0) return false;
	MEMORY_BASIC_INFORMATION mbi;
	if(!VirtualQuery((LPCVOID)addr,&mbi,sizeof(mbi))) return false;
	if(mbi.State!=MEM_COMMIT) return false;
	if(mbi.Protect & PAGE_GUARD) return false;
	DWORD p=mbi.Protect & 0xFF;
	if(!(p==PAGE_READONLY||p==PAGE_READWRITE||p==PAGE_WRITECOPY||
		 p==PAGE_EXECUTE_READ||p==PAGE_EXECUTE_READWRITE||p==PAGE_EXECUTE_WRITECOPY))
		return false;
	return (addr+len) <= ((DWORD)mbi.BaseAddress + mbi.RegionSize);
}

DWORD ReadDW   (DWORD a){ return IsReadable(a,4)?*(DWORD*)a:0; }
int   ReadInt  (DWORD a){ return IsReadable(a,4)?*(int*)a:0; }
float ReadFlt  (DWORD a){ return IsReadable(a,4)?*(float*)a:0.0f; }
short ReadShort(DWORD a){ return IsReadable(a,2)?*(short*)a:0; }
BYTE  ReadByte (DWORD a){ return IsReadable(a,1)?*(BYTE*)a:0; }

bool ModuleRange(const char *name,DWORD &base,DWORD &end)	// [base,end) via PE header
{
	HMODULE h=GetModuleHandleA(name);
	if(!h) return false;
	base=(DWORD)h;
	IMAGE_DOS_HEADER *dos=(IMAGE_DOS_HEADER*)base;
	if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
	IMAGE_NT_HEADERS *nt=(IMAGE_NT_HEADERS*)(base+dos->e_lfanew);
	if(nt->Signature!=IMAGE_NT_SIGNATURE) return false;
	end=base+nt->OptionalHeader.SizeOfImage;
	return true;
}

bool RegionReadable(MEMORY_BASIC_INFORMATION &mbi)
{
	DWORD p=mbi.Protect & 0xFF;
	return (mbi.State==MEM_COMMIT) && !(mbi.Protect&PAGE_GUARD) &&
		(p==PAGE_READONLY||p==PAGE_READWRITE||p==PAGE_WRITECOPY||
		 p==PAGE_EXECUTE_READ||p==PAGE_EXECUTE_READWRITE||p==PAGE_EXECUTE_WRITECOPY);
}

// scan client.dll's data for the engine table: a run of >=8 consecutive
// pointers that all land inside hw.dll, with slots 51 & 53 also valid.
DWORD FindEngineTable()
{
	DWORD cl_base,cl_end,hw_base,hw_end;
	if(!ModuleRange("client.dll",cl_base,cl_end)) return 0;
	if(!ModuleRange("hw.dll",hw_base,hw_end))     return 0;

	for(DWORD addr=cl_base; addr<cl_end; )
	{
		MEMORY_BASIC_INFORMATION mbi;
		if(!VirtualQuery((LPCVOID)addr,&mbi,sizeof(mbi))) break;
		DWORD region_end=(DWORD)mbi.BaseAddress+mbi.RegionSize;
		if(region_end>cl_end) region_end=cl_end;
		if(RegionReadable(mbi))
		{
			for(DWORD scan=addr; scan+4<=region_end; scan+=4)
			{
				int hits=0;
				for(int j=0; (scan+(DWORD)(j+1)*4)<=region_end && j<64; j++)
				{
					DWORD v=*(DWORD*)(scan+j*4);
					if(v>=hw_base && v<hw_end) hits++; else break;
				}
				if(hits>=8)
				{
					DWORD s51=*(DWORD*)(scan+ENG_SLOT_GETLOCALPLAYER*4);
					DWORD s53=*(DWORD*)(scan+ENG_SLOT_GETENTITYBYINDEX*4);
					if(s51>=hw_base&&s51<hw_end&&s53>=hw_base&&s53<hw_end)
						return scan;
				}
			}
		}
		addr=region_end;
	}
	return 0;
}

// masked code-pattern scan; on match returns the DWORD read at +ptr_off
DWORD ScanPattern(DWORD start,DWORD end,const BYTE *pat,const char *mask,int ptr_off)
{
	int len=(int)strlen(mask);
	for(DWORD addr=start; addr<end; )
	{
		MEMORY_BASIC_INFORMATION mbi;
		if(!VirtualQuery((LPCVOID)addr,&mbi,sizeof(mbi))) break;
		DWORD region_end=(DWORD)mbi.BaseAddress+mbi.RegionSize;
		if(region_end>end) region_end=end;
		if(RegionReadable(mbi))
		{
			for(DWORD scan=addr; scan+(DWORD)len<=region_end; scan++)
			{
				bool match=true;
				for(int i=0;i<len;i++)
					if(mask[i]=='x' && *(BYTE*)(scan+i)!=pat[i]) { match=false; break; }
				if(match)
				{
					DWORD pa=scan+ptr_off;
					if(IsReadable(pa,4))
					{
						DWORD arr=*(DWORD*)pa;
						if(arr>0x10000 && IsReadable(arr,EXTRA_STRIDE*33))
							return arr;
					}
				}
			}
		}
		addr=region_end;
	}
	return 0;
}

// locate client.dll's g_PlayerExtraInfo array (team/alive metadata)
DWORD FindExtraInfo()
{
	DWORD cl_base,cl_end;
	if(!ModuleRange("client.dll",cl_base,cl_end)) return 0;
	BYTE pat[]={0x0F,0xBF,0x87,0xCC,0xCC,0xCC,0xCC,0x8B,0x16,0x50,0x68,0xCC,0xCC,0xCC,0xCC,
		0x8B,0xCE,0xFF,0x52,0xCC,0x8D,0x4C,0xAD,0x00,0x66,0x8B,0x04,0x8D};
	const char *mask="xxx????xxxx????xxxx?xxxxxxxx";
	return ScanPattern(cl_base,cl_end,pat,mask,27);
}

bool EngineResolve()	// resolve & cache the engine table; true when usable
{
	if(eng_table==0 || ReadDW(eng_table+ENG_SLOT_GETLOCALPLAYER*4)==0)
		eng_table=FindEngineTable();
	if(eng_table==0) return false;
	if(!eng_have_extra)
	{
		eng_extrainfo=FindExtraInfo();
		eng_have_extra=(eng_extrainfo!=0);
	}
	return true;
}

DWORD EngFn(int slot){ return ReadDW(eng_table+slot*4); }

bool EngWorldToScreen(float *world,float *screen)
{
	DWORD tri=EngFn(ENG_SLOT_PTRIAPI);
	if(tri<0x10000) return false;
	DWORD w2s=ReadDW(tri+12*4);		// TriAPI slot 12 = WorldToScreen
	if(w2s==0) return false;
	return ((eng_WorldToScreen_t)w2s)(world,screen)==0;	// 0 = in front of camera
}

int EngTeam(int idx)
{
	if(!eng_have_extra || idx<0 || idx>32) return 0;
	return ReadShort(eng_extrainfo+idx*EXTRA_STRIDE+EXTRA_TEAMNUMBER);
}

// ---------------------------------------------------------------------------
//  No (visual) recoil  -  via a client.dll!V_CalcRefdef detour.
//
//  We first tried zeroing cl.punchangle from our wglSwapBuffers hook, but that
//  runs AFTER the engine already baked the punch into the view for the frame,
//  and client-side weapon prediction re-fills punch every frame - so it had no
//  visible effect (the F11 readout confirmed punch kept spiking, peak 5.75).
//
//  The correctly-timed place is the client's V_CalcRefdef(ref_params_s*), which
//  is where punch is ADDED to the view angles. We install a tiny inline detour
//  on it (client.dll exports the symbol by name) and clear pparams->punchangle
//  BEFORE the original runs. ref_params_s is a public, build-stable SDK ABI, so
//  punchangle always sits at +0xA0.  The detour uses unhook/call/re-hook (the
//  render runs on a single thread, so there is no race) to avoid needing a
//  length-disassembled trampoline.
// ---------------------------------------------------------------------------
#define REFP_PUNCHANGLE 0xA0			// ref_params_s::punchangle byte offset (SDK ABI, build-stable)
typedef void (__cdecl *VCalc_t)(void *pparams);
static BYTE g_vcalc_orig[5];			// original first 5 bytes of V_CalcRefdef (for unhook)

void __cdecl Hooked_VCalc(void *pparams)
{
	if(cvar.norecoil && pparams)
	{
		float *pa=(float*)((BYTE*)pparams+REFP_PUNCHANGLE);
		norec_last[0]=pa[0]; norec_last[1]=pa[1]; norec_last[2]=pa[2];	// debug snapshot
		float m=fabsf(pa[0])+fabsf(pa[1])+fabsf(pa[2]);
		if(m>norec_peak) norec_peak=m;
		pa[0]=0.0f; pa[1]=0.0f; pa[2]=0.0f;			// kill the kick BEFORE the view uses it
	}
	// run the real V_CalcRefdef: restore its bytes, call, then re-arm our jmp
	DWORD old;
	VirtualProtect((void*)norec_fn,5,PAGE_EXECUTE_READWRITE,&old);
	memcpy((void*)norec_fn,g_vcalc_orig,5);
	((VCalc_t)norec_fn)(pparams);
	BYTE jb[5]; jb[0]=0xE9; *(DWORD*)(jb+1)=(DWORD)&Hooked_VCalc-((DWORD)norec_fn+5);
	memcpy((void*)norec_fn,jb,5);
	VirtualProtect((void*)norec_fn,5,old,&old);
}

void EnsureNoRecoilHook()	// install / remove the detour so it matches cvar.norecoil
{
	bool want=(cvar.norecoil!=0);
	if(want && !norec_hooked)
	{
		if(norec_fn==0)
		{
			HMODULE cl=GetModuleHandleA("client.dll");
			if(!cl) return;								// not loaded yet -> retry next frame
			FARPROC f=GetProcAddress(cl,"V_CalcRefdef");
			if(!f) return;								// symbol missing -> can't hook this build
			norec_fn=(DWORD)f;
			memcpy(g_vcalc_orig,(void*)norec_fn,5);		// stash original prologue once
		}
		DWORD old;
		if(VirtualProtect((void*)norec_fn,5,PAGE_EXECUTE_READWRITE,&old))
		{
			BYTE jb[5]; jb[0]=0xE9; *(DWORD*)(jb+1)=(DWORD)&Hooked_VCalc-((DWORD)norec_fn+5);
			memcpy((void*)norec_fn,jb,5);
			VirtualProtect((void*)norec_fn,5,old,&old);
			norec_hooked=true;
		}
	}
	else if(!want && norec_hooked)
	{
		DWORD old;
		if(VirtualProtect((void*)norec_fn,5,PAGE_EXECUTE_READWRITE,&old))
		{
			memcpy((void*)norec_fn,g_vcalc_orig,5);		// restore original -> detour gone
			VirtualProtect((void*)norec_fn,5,old,&old);
		}
		norec_hooked=false;
	}
}

bool EngDead(int idx)
{
	if(!eng_have_extra || idx<0 || idx>32) return false;	// can't tell -> alive
	return ReadByte(eng_extrainfo+idx*EXTRA_STRIDE+EXTRA_DEAD)!=0;
}

// classify team from the player's model name (offset-free fallback when
// g_PlayerExtraInfo wasn't found). 1 = T, 2 = CT, 0 = unknown.
int TeamFromModel(const char *m)
{
	if(m==0 || !IsReadable((DWORD)m,1)) return 0;
	char s[64]; int i=0;
	for(; i<63 && IsReadable((DWORD)(m+i),1) && m[i]; i++)
	{
		char c=m[i];
		s[i]=(c>='A'&&c<='Z')?(c+32):c;	// lowercase
	}
	s[i]=0;
	if(strstr(s,"terror")||strstr(s,"leet")||strstr(s,"arctic")||
	   strstr(s,"guerilla")||strstr(s,"militia")) return 1;	// Terrorists
	if(strstr(s,"urban")||strstr(s,"gsg9")||strstr(s,"sas")||
	   strstr(s,"gign")||strstr(s,"vip")) return 2;			// Counter-Terrorists
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//  OWN HUD  -  health / armor / ammo via user-message hooks
//
//  GoldSrc never networks enemy/teammate health, but it DOES send the local
//  player his own "Health", "Battery" (armor) and "CurWeapon" (clip) messages.
//  The engine keeps a linked list of usermsg_t nodes, one per message name; each
//  node stores the client.dll handler pointer. We find the nodes for the three
//  messages we care about (by their inline name string, scanning only private
//  heap pages), save the original handler and overwrite it with our own. Our
//  handler reads the value, then forwards to the original so the vanilla HUD
//  keeps working exactly as before. Message names are stable across all builds,
//  so this avoids the signature fragility that broke g_PlayerExtraInfo.
//
//      struct usermsg_t { int iMsg; int iSize; char szName[16]; usermsg_t*next; pfn; }
//                          +0       +4         +8 (UM_NAME)      +24 (UM_NEXT) +28 (UM_PFN)
//////////////////////////////////////////////////////////////////////////////////////////////////

#define UM_NAME		8
#define UM_NEXT		24
#define UM_PFN		28

// usermsg handlers are __cdecl (engine ABI); state it so /Gz doesn't corrupt us.
typedef int (__cdecl *pfnUserMsgHook)(const char *name,int size,void *buf);

static pfnUserMsgHook um_org_health  = 0;	// saved original client.dll handlers
static pfnUserMsgHook um_org_battery = 0;
static pfnUserMsgHook um_org_curwpn  = 0;
static pfnUserMsgHook um_org_death   = 0;
static pfnUserMsgHook um_org_reset   = 0;
static DWORD um_node_health  = 0;			// usermsg_t node addresses we patched
static DWORD um_node_battery = 0;
static DWORD um_node_curwpn  = 0;
static DWORD um_node_death   = 0;
static DWORD um_node_reset   = 0;

int __cdecl Hk_Health(const char *n,int s,void *b)
{
	// Only updates the displayed value. We deliberately do NOT infer death from this:
	// while dead+spectating, the server feeds us the SPECTATED player's Health, so a
	// health>0 here would wrongly mark us alive. Death is driven by DeathMsg instead.
	if(b && s>=1) me_health=*(unsigned char*)b;
	return um_org_health ? um_org_health(n,s,b) : 1;
}
// DeathMsg layout (CS 1.6): byte killer, byte victim, byte headshot, string weapon.
// We mark ourselves dead when the victim index is our own player index.
int __cdecl Hk_DeathMsg(const char *n,int s,void *b)
{
	if(b && s>=2)
	{
		int victim=((unsigned char*)b)[1];
		if(victim==eng_local_idx && eng_local_idx>0) me_dead=true;
		// Instant death signal for ESP + aimbot + triggerbot: the moment ANYONE
		// dies, stamp the slot so the shared alive-gate drops them this same frame
		// (no waiting on the laggy scoreboard byte or the 400ms staleness timer).
		if(victim>0 && victim<=32) eng_dead_at[victim]=GetTickCount();
	}
	return um_org_death ? um_org_death(n,s,b) : 1;
}
// ResetHUD is sent to us on our own (re)spawn -> we are alive again.
int __cdecl Hk_ResetHUD(const char *n,int s,void *b)
{
	me_dead=false;
	return um_org_reset ? um_org_reset(n,s,b) : 1;
}
int __cdecl Hk_Battery(const char *n,int s,void *b)
{
	if(b && s>=1) me_armor=*(unsigned char*)b;		// armor fits a byte (0..100)
	return um_org_battery ? um_org_battery(n,s,b) : 1;
}
int __cdecl Hk_CurWeapon(const char *n,int s,void *b)
{
	if(b && s>=3)
	{
		unsigned char *p=(unsigned char*)b;
		int state=p[0], id=(signed char)p[1], clip=(signed char)p[2];
		if(state && id>0)							// state!=0 => this is the active weapon
		{
			me_weaponid=id; me_clip=clip;			// clip == -1 for knife/grenades
			if(id>=0 && id<64 && clip>me_maxclip[id]) me_maxclip[id]=clip;	// learn capacity
		}
	}
	return um_org_curwpn ? um_org_curwpn(n,s,b) : 1;
}

// Scan committed PRIVATE (heap) pages for a usermsg node whose szName == name.
// Image pages are skipped, so the many string literals in client.dll don't match.
DWORD FindUserMsgNode(const char *name)
{
	int nlen=(int)strlen(name);
	SYSTEM_INFO si; GetSystemInfo(&si);
	DWORD addr=(DWORD)si.lpMinimumApplicationAddress;
	DWORD maxa=(DWORD)si.lpMaximumApplicationAddress;
	while(addr<maxa)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if(!VirtualQuery((LPCVOID)addr,&mbi,sizeof(mbi))) break;
		DWORD rbase=(DWORD)mbi.BaseAddress, rend=rbase+mbi.RegionSize;
		if(rend<=rbase) break;
		if(mbi.State==MEM_COMMIT && mbi.Type==MEM_PRIVATE && RegionReadable(mbi))
		{
			DWORD limit=rend-(DWORD)(nlen+1);
			for(DWORD sp=rbase; sp<=limit; sp++)
			{
				if(*(char*)sp!=name[0]) continue;
				if(memcmp((void*)sp,name,nlen)!=0) continue;
				if(*(char*)(sp+nlen)!=0) continue;			// must be null-terminated
				DWORD node=sp-UM_NAME;
				if(node<rbase || !IsReadable(node,32)) continue;
				int   im =*(int*)(node+0);					// iMsg
				int   iz =*(int*)(node+4);					// iSize
				DWORD nx =*(DWORD*)(node+UM_NEXT);
				DWORD pfn=*(DWORD*)(node+UM_PFN);
				if(im<0||im>255) continue;
				if(iz<-1||iz>256) continue;
				if(pfn<0x10000 || !IsReadable(pfn,1)) continue;	// handler must be code
				if(nx!=0 && !IsReadable(nx,32)) continue;		// next must be node or null
				return node;
			}
		}
		addr=rend;
	}
	return 0;
}

void PatchPfn(DWORD node,DWORD newpfn)		// overwrite node->pfn (make page writable)
{
	DWORD a=node+UM_PFN, old;
	if(VirtualProtect((LPVOID)a,4,PAGE_EXECUTE_READWRITE,&old))
	{
		*(DWORD*)a=newpfn;
		VirtualProtect((LPVOID)a,4,old,&old);
	}
}

// Install (or re-install after a reconnect) our three message hooks. Cheap to
// call every frame: once hooked it just verifies the pointers are still ours.
void HookOwnMsgs()
{
	if(msg_hooked)
	{
		bool ok=true;	// reconnecting rebuilds the node list -> our hook is gone
		if(um_node_health  && ReadDW(um_node_health +UM_PFN)!=(DWORD)Hk_Health)    ok=false;
		if(um_node_battery && ReadDW(um_node_battery+UM_PFN)!=(DWORD)Hk_Battery)   ok=false;
		if(um_node_curwpn  && ReadDW(um_node_curwpn +UM_PFN)!=(DWORD)Hk_CurWeapon) ok=false;
		if(um_node_death   && ReadDW(um_node_death  +UM_PFN)!=(DWORD)Hk_DeathMsg)  ok=false;
		if(um_node_reset   && ReadDW(um_node_reset  +UM_PFN)!=(DWORD)Hk_ResetHUD)  ok=false;
		if(ok) return;
		msg_hooked=false; eng_msg_tries=0;
		um_node_health=um_node_battery=um_node_curwpn=um_node_death=um_node_reset=0;
	}
	// The Health/Battery/CurWeapon nodes only get registered AFTER you connect to a
	// server (HUD_Init), which can be far more than 60 frames after the HUD is first
	// enabled. Hooking too late means we miss the one-shot spawn messages, so HP shows
	// 0% and ammo shows nothing until the next update (taking damage / switching guns).
	// Keep retrying for a long time, but throttle the heap scan so it stays cheap.
	eng_msg_tries++;
	if(eng_msg_tries % 8 != 0) return;	// scan the heap ~every 8th frame
	if(eng_msg_tries > 8000)   return;	// give up only after a long while

	if(um_node_health==0)
	{ DWORD n=FindUserMsgNode("Health");
	  if(n){ um_node_health=n; um_org_health=(pfnUserMsgHook)ReadDW(n+UM_PFN); PatchPfn(n,(DWORD)Hk_Health); } }
	if(um_node_battery==0)
	{ DWORD n=FindUserMsgNode("Battery");
	  if(n){ um_node_battery=n; um_org_battery=(pfnUserMsgHook)ReadDW(n+UM_PFN); PatchPfn(n,(DWORD)Hk_Battery); } }
	if(um_node_curwpn==0)
	{ DWORD n=FindUserMsgNode("CurWeapon");
	  if(n){ um_node_curwpn=n; um_org_curwpn=(pfnUserMsgHook)ReadDW(n+UM_PFN); PatchPfn(n,(DWORD)Hk_CurWeapon); } }
	if(um_node_death==0)
	{ DWORD n=FindUserMsgNode("DeathMsg");
	  if(n){ um_node_death=n; um_org_death=(pfnUserMsgHook)ReadDW(n+UM_PFN); PatchPfn(n,(DWORD)Hk_DeathMsg); } }
	if(um_node_reset==0)
	{ DWORD n=FindUserMsgNode("ResetHUD");
	  if(n){ um_node_reset=n; um_org_reset=(pfnUserMsgHook)ReadDW(n+UM_PFN); PatchPfn(n,(DWORD)Hk_ResetHUD); } }

	if(um_node_health && um_node_battery && um_node_curwpn && um_node_death && um_node_reset) msg_hooked=true;
}

// Two 10-tick arcs flanking the crosshair: green (left) = health, yellow (right)
// = current clip. Each tick = 10%. Drawn inside the same 2D pass as DrawEngineEsp.
void DrawOwnHud(float sw,float sh)
{
	// "Show when die" ONLY controls visibility while dead. me_dead is driven by the
	// DeathMsg/ResetHUD hooks (not by the Health value), so spectating a live teammate
	// after death can't flip us back to "alive" and re-show the HUD.
	if(me_dead && !cvar.hud_die) return;

	// ui_scale is computed once in BuildFont() from the screen resolution (which is
	// fixed for the whole session), so the HUD scales the same way as the menu / ESP
	// text without any per-frame work.
	const float cx=sw*0.5f, cy=sh*0.5f, DEG=3.14159265f/180.0f;
	float R=(52.0f+(float)cvar.hud_pad)*ui_scale;	// arc radius (Padding pushes it out/in)
	if(R<14.0f*ui_scale) R=14.0f*ui_scale;			// keep it off the crosshair
	const int   TICKS=10;
	const float SPAN=96.0f;							// total arc per side (degrees) -> more curve
	const float SLOT=SPAN/TICKS;					// angular size of one slot
	const float FILL=0.66f;							// 66% bar + 34% gap -> CrossFire-style ticks
	const float HALFGAP=SLOT*(1.0f-FILL)*0.5f;		// half the gap, trimmed from each slot end

	int hp=me_health; if(hp<0)hp=0; if(hp>100)hp=100;
	int litH=(hp+9)/10;								// ceil to 0..10 ticks

	bool showA=false; int litA=0;
	if(me_clip>=0 && me_weaponid>=0 && me_weaponid<64 && me_maxclip[me_weaponid]>0)
	{
		int pct=me_clip*100/me_maxclip[me_weaponid];
		if(pct<0)pct=0; if(pct>100)pct=100;
		litA=(pct+9)/10; showA=true;				// hidden for knife/grenades (clip<0)
	}

	(*orig_glEnable)(GL_LINE_SMOOTH);				// rounded tick ends (restored by glPopAttrib)
	(*orig_glLineWidth)(5.5f*ui_scale);				// thicker bars

	if(cvar.hud_hp)
	{
		const float base=180.0f-SPAN*0.5f;			// health arc centered on the left (180 deg)
		(*orig_glBegin)(GL_LINES);
		for(int i=0;i<TICKS;i++)
		{
			float a0=(base+i*SLOT+HALFGAP)*DEG;		// trim both ends -> visible gap
			float a1=(base+(i+1)*SLOT-HALFGAP)*DEG;
			if(i<litH) (*orig_glColor4f)(0.10f,1.00f,0.20f,0.95f);
			else       (*orig_glColor4f)(0.10f,0.35f,0.10f,0.45f);
			(*orig_glVertex2f)(cx+cosf(a0)*R, cy-sinf(a0)*R);
			(*orig_glVertex2f)(cx+cosf(a1)*R, cy-sinf(a1)*R);
		}
		(*orig_glEnd)();
	}

	if(cvar.hud_ammo && showA)
	{
		const float base=-SPAN*0.5f;				// ammo arc centered on the right (0 deg)
		(*orig_glBegin)(GL_LINES);
		for(int i=0;i<TICKS;i++)
		{
			float a0=(base+i*SLOT+HALFGAP)*DEG;
			float a1=(base+(i+1)*SLOT-HALFGAP)*DEG;
			if(i<litA) (*orig_glColor4f)(1.00f,0.85f,0.10f,0.95f);
			else       (*orig_glColor4f)(0.40f,0.35f,0.10f,0.45f);
			(*orig_glVertex2f)(cx+cosf(a0)*R, cy-sinf(a0)*R);
			(*orig_glVertex2f)(cx+cosf(a1)*R, cy-sinf(a1)*R);
		}
		(*orig_glEnd)();
	}

	(*orig_glColor4f)(1,1,1,1);
}

// small filled disc in the current 2D pass (used for radar dots / local marker)
void FillCircle2D(float cx,float cy,float r)
{
	(*orig_glBegin)(GL_TRIANGLE_FAN);
	(*orig_glVertex2f)(cx,cy);
	for(int a=0;a<=12;a++){ float t=a*6.2831853f/12.0f; (*orig_glVertex2f)(cx+cosf(t)*r, cy+sinf(t)*r); }
	(*orig_glEnd)();
}

// radar dot: filled square or circle of half-size r
void DrawDot2D(float cx,float cy,float r,int square)
{
	if(square)
	{
		(*orig_glBegin)(GL_QUADS);
		(*orig_glVertex2f)(cx-r,cy-r);(*orig_glVertex2f)(cx+r,cy-r);
		(*orig_glVertex2f)(cx+r,cy+r);(*orig_glVertex2f)(cx-r,cy+r);
		(*orig_glEnd)();
	}
	else FillCircle2D(cx,cy,r);
}

// ESP box outline, optionally with rounded corners (rad in px, 0 = sharp)
void DrawBox2D(float x0,float y0,float x1,float y1,float rad)
{
	if(rad<0.5f)
	{
		(*orig_glBegin)(GL_LINE_LOOP);
		(*orig_glVertex2f)(x0,y0);(*orig_glVertex2f)(x1,y0);
		(*orig_glVertex2f)(x1,y1);(*orig_glVertex2f)(x0,y1);
		(*orig_glEnd)();
		return;
	}
	float w=x1-x0, h=y1-y0, m=(w<h?w:h)*0.5f; if(rad>m) rad=m;
	const float PI=3.14159265f; const int SEG=5;
	(*orig_glBegin)(GL_LINE_LOOP);
	for(int i=0;i<=SEG;i++){ float a=PI      +(PI*0.5f)*i/SEG; (*orig_glVertex2f)(x0+rad+cosf(a)*rad, y0+rad+sinf(a)*rad); }	// TL
	for(int i=0;i<=SEG;i++){ float a=1.5f*PI +(PI*0.5f)*i/SEG; (*orig_glVertex2f)(x1-rad+cosf(a)*rad, y0+rad+sinf(a)*rad); }	// TR
	for(int i=0;i<=SEG;i++){ float a=        (PI*0.5f)*i/SEG;  (*orig_glVertex2f)(x1-rad+cosf(a)*rad, y1-rad+sinf(a)*rad); }	// BR
	for(int i=0;i<=SEG;i++){ float a=0.5f*PI +(PI*0.5f)*i/SEG; (*orig_glVertex2f)(x0+rad+cosf(a)*rad, y1-rad+sinf(a)*rad); }	// BL
	(*orig_glEnd)();
}

// Filled rounded rectangle (rad in px, 0 = plain quad). Same corner layout as DrawBox2D.
void FillRoundRect2D(float x0,float y0,float x1,float y1,float rad)
{
	if(rad<0.5f)
	{
		(*orig_glBegin)(GL_QUADS);
		(*orig_glVertex2f)(x0,y0);(*orig_glVertex2f)(x1,y0);
		(*orig_glVertex2f)(x1,y1);(*orig_glVertex2f)(x0,y1);
		(*orig_glEnd)();
		return;
	}
	float w=x1-x0, h=y1-y0, m=(w<h?w:h)*0.5f; if(rad>m) rad=m;
	const float PI=3.14159265f; const int SEG=5;
	float cxm=(x0+x1)*0.5f, cym=(y0+y1)*0.5f;
	(*orig_glBegin)(GL_TRIANGLE_FAN);
	(*orig_glVertex2f)(cxm,cym);
	for(int i=0;i<=SEG;i++){ float a=PI      +(PI*0.5f)*i/SEG; (*orig_glVertex2f)(x0+rad+cosf(a)*rad, y0+rad+sinf(a)*rad); }	// TL
	for(int i=0;i<=SEG;i++){ float a=1.5f*PI +(PI*0.5f)*i/SEG; (*orig_glVertex2f)(x1-rad+cosf(a)*rad, y0+rad+sinf(a)*rad); }	// TR
	for(int i=0;i<=SEG;i++){ float a=        (PI*0.5f)*i/SEG;  (*orig_glVertex2f)(x1-rad+cosf(a)*rad, y1-rad+sinf(a)*rad); }	// BR
	for(int i=0;i<=SEG;i++){ float a=0.5f*PI +(PI*0.5f)*i/SEG; (*orig_glVertex2f)(x0+rad+cosf(a)*rad, y1-rad+sinf(a)*rad); }	// BL
	(*orig_glVertex2f)(x0+rad+cosf(PI)*rad, y0+rad+sinf(PI)*rad);	// close back to first corner pt
	(*orig_glEnd)();
}

// Filled triangle "arrow" pointing along (dx,dy) (assumed unit vector), centered at (cx,cy).
// size = half-length from tip to base; used by the off-screen enemy indicator.
void DrawArrow2D(float cx,float cy,float dx,float dy,float size)
{
	float nx=-dy, ny=dx;					// perpendicular for the base
	float tipx=cx+dx*size,     tipy=cy+dy*size;
	float blx =cx-dx*size*0.6f+nx*size*0.6f, bly=cy-dy*size*0.6f+ny*size*0.6f;
	float brx =cx-dx*size*0.6f-nx*size*0.6f, bry=cy-dy*size*0.6f-ny*size*0.6f;
	(*orig_glBegin)(GL_TRIANGLES);
	(*orig_glVertex2f)(tipx,tipy);
	(*orig_glVertex2f)(blx ,bly );
	(*orig_glVertex2f)(brx ,bry );
	(*orig_glEnd)();
}

// Depth-buffer visibility test for a 3D world point. Returns true if the point
// is NOT occluded (i.e. visible from the current camera). The caller passes the
// camera modelview/projection it snapshotted (mm_w/pm_w) BEFORE switching to the
// 2D ortho pass in DrawEngineEsp, so the projection still matches the 3D scene.
bool IsWorldVisible(float x,float y,float z,GLdouble *mm_in,GLdouble *pm_in,GLint *vp_in)
{
	GLdouble wx,wy,wz; GLfloat pix;
	if(gluProject(x,y,z,mm_in,pm_in,vp_in,&wx,&wy,&wz)!=GL_TRUE) return false;
	if(wz<=0.0||wz>=1.0) return false;
	if(wx<0||wy<0||wx>=vp_in[2]||wy>=vp_in[3]) return false;
	(*orig_glReadPixels)((int)wx,(int)wy,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&pix);
	return (pix>wz-0.0008f);		// tiny epsilon: head usually pokes ~through model surface
}

// Draw ESP for every player in the engine entity list. Called each frame from
// the wglSwapBuffers hook, in its own 2D pixel-space pass.
void DrawEngineEsp()
{
	eng_players=0;
	eng_aim_have=false;								// reset per-frame aim pick
	// We must enter the loop ALSO when engine aimbot / triggerbot / detection
	// logging are on (even if ESP/radar/HUD are off), since they all rely on the
	// per-frame entity walk below.
	bool need_aim  = (cvar.aim!=0);
	bool need_scan = need_aim || cvar.trigger || cvar.esp_log;
	if(!cvar.esp_engine && !cvar.esp_hud && !cvar.radar && !need_scan && !cvar.bhop && !cvar.spec_warn) return;
	eng_frame++;

	GLint vpe[4];
	(*orig_glGetIntegerv)(GL_VIEWPORT,vpe);
	float sw=(float)vpe[2], sh=(float)vpe[3];
	if(sw<=0||sh<=0) return;

	// Snapshot the engine's 3D MVP BEFORE we switch to 2D ortho, so the
	// per-player depth-buffer visibility test (esp_vischeck) can still
	// project world->screen using the actual game camera.
	GLdouble mm_w[16], pm_w[16];
	(*orig_glGetDoublev)(GL_MODELVIEW_MATRIX,mm_w);
	(*orig_glGetDoublev)(GL_PROJECTION_MATRIX,pm_w);

	bool ready=EngineResolve();

	// 2D pixel-space overlay (top-left origin, y down)
	(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
	(*orig_glDisable)(GL_DEPTH_TEST);
	(*orig_glDisable)(GL_TEXTURE_2D);
	(*orig_glEnable)(GL_BLEND);
	(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	(*orig_glMatrixMode)(GL_PROJECTION);
	(*orig_glPushMatrix)();
	(*orig_glLoadIdentity)();
	(*orig_glOrtho)(0,sw,sh,0,-1,1);
	(*orig_glMatrixMode)(GL_MODELVIEW);
	(*orig_glPushMatrix)();
	(*orig_glLoadIdentity)();

	DWORD fnLocal=ready?EngFn(ENG_SLOT_GETLOCALPLAYER):0;
	DWORD fnEnt  =ready?EngFn(ENG_SLOT_GETENTITYBYINDEX):0;
	DWORD fnInfo =ready?EngFn(ENG_SLOT_GETPLAYERINFO):0;

	// Keep our own player index fresh so the DeathMsg hook can tell when WE died,
	// even when ESP Engine / Radar are off and the loop below never runs.
	if(fnLocal>=0x10000)
	{
		DWORD lp=(DWORD)((eng_GetLocalPlayer_t)fnLocal)();
		if(lp) eng_local_idx=ReadInt(lp+ENT_INDEX);
	}

	// own HP / ammo arcs around the crosshair: driven by user messages, not by the
	// engine entity table, so draw them before the table-ready check below.
	if(cvar.esp_hud)
	{
		HookOwnMsgs();
		DrawOwnHud(sw,sh);
	}

	if(!ready || fnLocal<0x10000 || fnEnt<0x10000)
	{
		if(cvar.esp_engine && cvar.esp_dbg)
			DrawText(16.0f*ui_scale,24.0f*ui_scale,1.0f,0.7f,0.2f,"ENGINE ESP: searching engine table (start a game)...");
		(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPopMatrix)();
		(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPopMatrix)();
		(*orig_glPopAttrib)();
		return;
	}

	if(cvar.esp_engine || cvar.radar || need_scan || cvar.bhop || cvar.spec_warn)
	{

	float lo[3]={0,0,0};
	DWORD local=(DWORD)((eng_GetLocalPlayer_t)fnLocal)();
	if(local)
	{
		lo[0]=ReadFlt(local+ENT_ORIGIN);
		lo[1]=ReadFlt(local+ENT_ORIGIN+4);
		lo[2]=ReadFlt(local+ENT_ORIGIN+8);
		eng_local_idx =ReadInt(local+ENT_INDEX);
		eng_local_team=EngTeam(eng_local_idx);
		// auto-bhop: read the local player's ground state from curstate.onground
		// (-1 = airborne; any other value = standing on an entity). sys_glViewport
		// consumes this on the next frame to time the jump injection.
		int og=ReadInt(local+ENT_CURSTATE+ES_ONGROUND);
		eng_on_ground=(og!=-1)?1:0;

		// Bhop diagnostics (ESP debug toggle). want=1 means UpdateBhop is actively
		// spamming; pulses increments on every injected SPACE press. If want=1 and
		// pulses keeps rising but you still don't jump, the engine isn't accepting
		// the injected key (injection method) rather than us not sending it.
		// onground_raw is informational only (stays 0 even mid-air for the local
		// player), which is why we spam jump instead of gating on it.
		if(cvar.esp_dbg && cvar.bhop)
			DrawText(16.0f*ui_scale,40.0f*ui_scale,1.0f,0.7f,0.2f,
				"BHOP: want=%d pulses=%u down=%d  (onground_raw=%d, unreliable)",
				g_bhop_want?1:0, g_bhop_pulses, g_bhop_down?1:0, og);
	}

	// ---- 2D radar frame. Center is freely positioned (radar_x/radar_y, move mode 3). ----
	float rcx=0,rcy=0,rrad=0; float rcos=1.0f,rsin=0.0f; bool radar_on=false;
	if(cvar.radar)
	{
		{ int rs=cvar.radar_size; if(rs<30)rs=70; if(rs>150)rs=150; rrad=(float)rs*ui_scale; }	// user-tunable size
		rcx=(float)cvar.radar_x*ui_scale;			// pre-scale center -> pixels
		rcy=(float)cvar.radar_y*ui_scale;
		if(cvar.radar_rotate)
		{
			float va[3]={0,0,0};
			DWORD fnVA=EngFn(ENG_SLOT_GETVIEWANGLES);
			if(fnVA>=0x10000) ((eng_GetViewAngles_t)fnVA)(va);
			float yaw=va[1]*3.14159265f/180.0f;	// rotate so local forward points up
			rcos=cosf(yaw); rsin=sinf(yaw);
		}
		else { rcos=1.0f; rsin=0.0f; }				// fixed north-up
		radar_on=true;

		(*orig_glColor4f)(0.0f,0.0f,0.0f,0.45f);	// translucent disc
		(*orig_glBegin)(GL_TRIANGLE_FAN);
		(*orig_glVertex2f)(rcx,rcy);
		for(int a=0;a<=32;a++){ float t=a*6.2831853f/32.0f; (*orig_glVertex2f)(rcx+cosf(t)*rrad, rcy+sinf(t)*rrad); }
		(*orig_glEnd)();
		(*orig_glColor4f)(0.85f,0.85f,0.85f,0.7f);	// outer ring + cross-hairs
		(*orig_glLineWidth)(1.0f);
		(*orig_glBegin)(GL_LINE_LOOP);
		for(int a=0;a<32;a++){ float t=a*6.2831853f/32.0f; (*orig_glVertex2f)(rcx+cosf(t)*rrad, rcy+sinf(t)*rrad); }
		(*orig_glEnd)();
		(*orig_glBegin)(GL_LINES);
		(*orig_glVertex2f)(rcx-rrad,rcy);(*orig_glVertex2f)(rcx+rrad,rcy);
		(*orig_glVertex2f)(rcx,rcy-rrad);(*orig_glVertex2f)(rcx,rcy+rrad);
		(*orig_glEnd)();
		if(cvar.radar_rings)						// concentric range rings (1/3, 2/3 of zoom)
		{
			(*orig_glColor4f)(0.85f,0.85f,0.85f,0.3f);
			for(int k=1;k<=2;k++)
			{
				float rr=rrad*k/3.0f;
				(*orig_glBegin)(GL_LINE_LOOP);
				for(int a=0;a<24;a++){ float t=a*6.2831853f/24.0f; (*orig_glVertex2f)(rcx+cosf(t)*rr, rcy+sinf(t)*rr); }
				(*orig_glEnd)();
			}
		}
		(*orig_glColor4f)(1,1,1,1);					// local player marker
		DrawDot2D(rcx,rcy,3.0f*ui_scale,cvar.radar_shape);
	}

	// engine-aim per-frame best pick (closest to crosshair, in FOV, alive, target team)
	float aim_best_d2 = 1e18f;
	float aim_best_sx = 0.0f, aim_best_sy = 0.0f;
	bool  aim_best_vis = true;
	bool  aim_found = false;
	float aim_cx = sw*0.5f, aim_cy = sh*0.5f;		// screen center is the crosshair
	int want_team = (cvar.target==0)?1:2;			// engine team# of the targeted side (1=T,2=CT)
	int  det_seen = 0;								// enemy players received this frame (PVS counter)
	bool trig_hit = false;							// crosshair on an enemy this frame (triggerbot)

	// spec_warn ("who's watching me"): collected during the player walk below, drawn
	// after it as a compact block under the crosshair. spec_total counts ALL spectators
	// observing us; spec_show caps the names actually drawn at 3 (per the design).
	char spec_names[3][64];
	float spec_cols[3][3];
	int  spec_show = 0;								// names stored for display (0..3)
	int  spec_total = 0;							// total spectators targeting us
	int  spec_seen = 0;								// debug: spectators present in the list this frame
	int  spec_dbg_mode = -1, spec_dbg_tgt = -1;		// debug: first spectator's raw iuser1/iuser2

	for(int idx=1; idx<=32; idx++)
	{
		if(idx==eng_local_idx) continue;

		char namebuf[64]="";
		char modelbuf[64]="";
		if(fnInfo>=0x10000)
		{
			hud_player_info_t info; memset(&info,0,sizeof(info));
			((eng_GetPlayerInfo_t)fnInfo)(idx,&info);
			if(info.name==0 || !IsReadable((DWORD)info.name,1)) continue;	// empty slot
			if(info.spectator)
			{
				// "Who's watching me": a spectator's observer target lives in their
				// entity_state (iuser1=mode, iuser2=target index). We treat them as
				// watching us if iuser2 == our index, OR (fallback, for in-eye specs
				// when the engine doesn't replicate iuser2 to us) their camera origin
				// sits on top of ours. Either way the slot is still skipped from
				// ESP/aim below via the continue.
				if(cvar.spec_warn && eng_local_idx>0 && fnEnt>=0x10000)
				{
					spec_seen++;
					DWORD sent=(DWORD)((eng_GetEntityByIndex_t)fnEnt)(idx);
					if(sent)
					{
						int obsmode=ReadInt(sent+ENT_CURSTATE+ES_IUSER1);
						int obstgt =ReadInt(sent+ENT_CURSTATE+ES_IUSER2);
						if(spec_seen==1){ spec_dbg_mode=obsmode; spec_dbg_tgt=obstgt; }
						bool watch=(obsmode>0 && obstgt==eng_local_idx);
						if(!watch)
						{
							float so0=ReadFlt(sent+ENT_ORIGIN), so1=ReadFlt(sent+ENT_ORIGIN+4), so2=ReadFlt(sent+ENT_ORIGIN+8);
							if(so0||so1||so2)
							{
								float dxs=so0-lo[0], dys=so1-lo[1], dzs=so2-lo[2];
								if(dxs*dxs+dys*dys+dzs*dzs < ENG_SPEC_MATCH_R*ENG_SPEC_MATCH_R) watch=true;
							}
						}
						if(watch)
						{
							spec_total++;
							if(spec_show<3)
							{
								strncpy(spec_names[spec_show],info.name,63); spec_names[spec_show][63]=0;
								int st=EngTeam(idx);
								if(st==1)      { spec_cols[spec_show][0]=1.0f; spec_cols[spec_show][1]=0.25f; spec_cols[spec_show][2]=0.25f; }	// T  = red
								else if(st==2) { spec_cols[spec_show][0]=0.25f;spec_cols[spec_show][1]=0.55f; spec_cols[spec_show][2]=1.0f;  }	// CT = blue
								else           { spec_cols[spec_show][0]=0.25f;spec_cols[spec_show][1]=1.0f;  spec_cols[spec_show][2]=0.25f; }	// no team (pure spectator/admin) = green
								spec_show++;
							}
						}
					}
				}
				continue;
			}
			strncpy(namebuf,info.name,63); namebuf[63]=0;
			if(info.model && IsReadable((DWORD)info.model,1))
			{ strncpy(modelbuf,info.model,63); modelbuf[63]=0; }
		}

		DWORD ent=(DWORD)((eng_GetEntityByIndex_t)fnEnt)(idx);
		if(!ent || ReadInt(ent+ENT_PLAYER)==0) continue;

		// alive/stale check: current_position keeps incrementing while a player
		// receives network updates; it freezes on death / disconnect / round-end.
		int cur=ReadInt(ent+ENT_CURPOS);
		DWORD now=GetTickCount();
		if(cur!=eng_lastcurpos[idx]) { eng_lastcurpos[idx]=cur; eng_lastchange[idx]=now; }
		bool stale=(now-eng_lastchange[idx])>ENG_STALE_MS;	// time-based: same at 60 or 240 fps
		// origin first -- the death latch's respawn check needs it too.
		float o[3];
		o[0]=ReadFlt(ent+ENT_ORIGIN); o[1]=ReadFlt(ent+ENT_ORIGIN+4); o[2]=ReadFlt(ent+ENT_ORIGIN+8);
		if(o[0]==0&&o[1]==0&&o[2]==0)	// fallback: entity_state origin
		{
			DWORD cs=ent+ENT_CURSTATE;
			o[0]=ReadFlt(cs+ES_ORIGIN); o[1]=ReadFlt(cs+ES_ORIGIN+4); o[2]=ReadFlt(cs+ES_ORIGIN+8);
			if(o[0]==0&&o[1]==0&&o[2]==0) continue;
		}

		// DeathMsg instant-death latch: the moment a slot dies it is dropped from
		// EVERYTHING below (ESP box/name/radar/aim/trigger) that same frame, and it
		// STAYS dropped until we're sure what happened next. A fixed-time hold isn't
		// enough: the scoreboard 'dead' byte lags the kill by a variable amount, and
		// a fresh corpse can keep streaming position updates for a moment so the
		// staleness timer hasn't fired yet -- a fixed window expired during that gap
		// and the corpse briefly popped back as an aim/ESP target. The latch releases
		// on whichever happens first:
		//   - EngDead/stale : the engine confirmed the corpse -> normal gate hides it.
		//   - respawn       : the origin teleported a long way (to a spawn point) ->
		//                     it's a NEW life, show it now. This is the key one for
		//                     deathmatch: an instant respawn keeps the slot alive and
		//                     streaming, so EngDead/stale may NEVER fire and we'd
		//                     otherwise stay hidden all the way to the safety cap (the
		//                     multi-second "I respawned but I'm invisible" delay).
		//   - safety cap    : backstop so a missed signal can't hide a live player.
		if(eng_dead_at[idx])
		{
			if(!eng_dead_org_set[idx])		// remember the death spot on the first latched frame
			{ eng_dead_org[idx][0]=o[0]; eng_dead_org[idx][1]=o[1]; eng_dead_org[idx][2]=o[2]; eng_dead_org_set[idx]=true; }
			float jx=o[0]-eng_dead_org[idx][0], jy=o[1]-eng_dead_org[idx][1], jz=o[2]-eng_dead_org[idx][2];
			bool respawned=(jx*jx+jy*jy+jz*jz)>(ENG_RESPAWN_DIST*ENG_RESPAWN_DIST);
			if(EngDead(idx) || stale || respawned || (now-eng_dead_at[idx])>ENG_DEATH_HOLD_MAX_MS)
			{ eng_dead_at[idx]=0; eng_dead_org_set[idx]=false; }	// released; the normal gate below decides corpse(hide)/respawn(show)
			else continue;					// still in the post-death gap -> stay hidden
		}
		if(EngDead(idx) || stale) continue;

		// team color (shared by the radar dot and the on-screen ESP below)
		int team=EngTeam(idx);
		if(team==0) team=TeamFromModel(modelbuf);
		float r,g,b;
		if(team==1)      { r=1.0f; g=0.25f; b=0.25f; }	// T  = red
		else if(team==2) { r=0.25f;g=0.55f; b=1.0f;  }	// CT = blue
		else             { r=0.25f;g=1.0f;  b=0.25f; }	// unknown = green

		// distance once (meters) - used by radar name, distance text, fade and the max-dist filter
		float ddx3=lo[0]-o[0], ddy3=lo[1]-o[1], ddz3=lo[2]-o[2];
		float distM=(float)sqrt(ddx3*ddx3+ddy3*ddy3+ddz3*ddz3)/39.37f;

		// PVS / detection counter: this enemy is alive and was received from the server
		if(team==want_team) det_seen++;

		if(radar_on)								// plot this player on the radar
		{
			float ddx=o[0]-lo[0], ddy=o[1]-lo[1];
			float rx= ddx*rsin - ddy*rcos;		// distance to the right of local
			float ry= ddx*rcos + ddy*rsin;		// distance in front of local
			float zoomU=(float)cvar.radar_zoom; if(zoomU<200.0f) zoomU=200.0f;
			float sc=rrad/zoomU;					// user-tunable zoom (radar_zoom units = ring)
			float px=rcx + rx*sc, py=rcy - ry*sc;	// forward = up (screen y is down)
			float dd=sqrtf((px-rcx)*(px-rcx)+(py-rcy)*(py-rcy));
			if(dd>rrad){ px=rcx+(px-rcx)/dd*rrad; py=rcy+(py-rcy)/dd*rrad; }	// clamp to ring
			(*orig_glColor3f)(r,g,b);
			DrawDot2D(px,py,3.0f*ui_scale,cvar.radar_shape);	// circle/square per player

			if(cvar.radar_names && namebuf[0])
			{
				char shortn[10]; int sl=0;
				for(; sl<9 && namebuf[sl]; sl++) shortn[sl]=namebuf[sl];
				shortn[sl]=0;
				DrawText(px+4.0f*ui_scale, py-4.0f*ui_scale, r,g,b, "%s", shortn);
			}
		}

		// engine-aim + triggerbot candidate checks (independent of ESP being on).
		// Both target the side selected by cvar.target (mapped to engine team#).
		if((need_aim || cvar.trigger) && team==want_team)
		{
			int usehullA=ReadInt(ent+ENT_CURSTATE+ES_USEHULL);
			float halfhA=(usehullA==1)?18.0f:36.0f;
			float zoffA =(usehullA==1)?6.0f :0.0f;
			// Head geometry from the engine hull: XY = origin, top = origin + hull
			// half-height, feet = origin - hull half-height.
			float hxA=o[0], hyA=o[1];				// head XY
			float topZ =o[2]+halfhA+zoffA;			// crown / hull top
			float feetZ=o[2]-halfhA+zoffA;			// feet
			float aimscale = halfhA/36.0f;			// duck/stand ratio for the user offset
			// Aim point: the CENTER of the head (the top sits a few units above the
			// skull, so drop by AIM_HEAD_CENTER), plus the user's vertical offset
			// cvar.aim_point (positive = aim higher). The offset is scaled by the
			// duck ratio so "-12 = neck" stays at the neck instead of the thigh.
			float aimz  = topZ - AIM_HEAD_CENTER + (float)cvar.aim_point*aimscale;
			float aimA[3] ={hxA,hyA,aimz};
			float headA[3]={hxA,hyA,topZ-2.0f};		// head top (triggerbot box)
			float feetA[3]={hxA,hyA,feetZ};
			float sAi[3], sHe[3], sFe[3];
			bool okAi=EngWorldToScreen(aimA,sAi);
			bool okHe=EngWorldToScreen(headA,sHe);
			bool okFe=EngWorldToScreen(feetA,sFe);

			// screen-space aim point (shared by the aim dot and the aimbot pick)
			float ax=0,ay=0;
			if(okAi){ ax=(sAi[0]*0.5f+0.5f)*sw; ay=sh-(sAi[1]*0.5f+0.5f)*sh; }
			bool aim_on_screen = okAi && ax>=0 && ay>=0 && ax<sw && ay<sh;

			// aim-point dot: marks EXACTLY where the aimbot aims. Aimbot sub-option,
			// drawn for any on-screen target-team enemy and independent of ESP.
			if(cvar.aim && cvar.aim_dot && aim_on_screen)
			{
				(*orig_glColor3f)(r,g,b);
				FillCircle2D(ax, ay, 3.0f*ui_scale);
			}

			// --- aimbot: nearest aim point to the crosshair within FOV ---
			if(need_aim && aim_on_screen)
			{
				float ddx=ax-aim_cx, ddy=ay-aim_cy;
				float d2=ddx*ddx+ddy*ddy;
				float fovr=(float)cvar.fov*0.5f;	// FOV used as a px radius
				if(fovr<1.0f || d2<=fovr*fovr)
				{
					bool v=true;
					if(!cvar.aimthru)
						v=IsWorldVisible(o[0],o[1],o[2]+10.0f,mm_w,pm_w,vp);
					if((cvar.aimthru || v) && d2<aim_best_d2)
					{
						aim_best_d2=d2; aim_best_sx=ax; aim_best_sy=ay;
						aim_best_vis=v; aim_found=true;
					}
				}
			}

			// --- triggerbot: is the crosshair inside this enemy's 2D box? ---
			if(cvar.trigger && okHe && okFe)
			{
				float fxx=(sFe[0]*0.5f+0.5f)*sw, fyy=sh-(sFe[1]*0.5f+0.5f)*sh;
				float hxx=(sHe[0]*0.5f+0.5f)*sw, hyy=sh-(sHe[1]*0.5f+0.5f)*sh;
				float ty0=(hyy<fyy)?hyy:fyy, ty1=(hyy<fyy)?fyy:hyy;
				float tbh=ty1-ty0; if(tbh<4.0f) tbh=4.0f;
				float tbw=tbh*0.45f;
				float tcx=(fxx+hxx)*0.5f;
				float tx0=tcx-tbw*0.5f, tx1=tcx+tbw*0.5f;
				if(aim_cx>=tx0 && aim_cx<=tx1 && aim_cy>=ty0 && aim_cy<=ty1)
				{
					bool v=true;
					if(!cvar.aimthru)
						v=IsWorldVisible(o[0],o[1],o[2]+10.0f,mm_w,pm_w,vp);
					if(cvar.aimthru || v) trig_hit=true;
				}
			}
		}

		if(!cvar.esp_engine) continue;			// radar-only run: skip the on-screen ESP

		// team filter: 0=both, 1=CT only (team 2), 2=T only (team 1)
		if(cvar.esp_team==1 && team!=2) continue;
		if(cvar.esp_team==2 && team!=1) continue;

		// max-distance filter: skip if too far (0 = unlimited)
		if(cvar.esp_maxdist>0 && distM>(float)cvar.esp_maxdist) continue;

		int usehull=ReadInt(ent+ENT_CURSTATE+ES_USEHULL);
		float halfh=(usehull==1)?18.0f:36.0f;	// duck vs stand half-height (units)
		float zoff =(usehull==1)?6.0f :0.0f;

		float feet[3]={o[0],o[1],o[2]-halfh+zoff};
		float head[3]={o[0],o[1],o[2]+halfh+zoff};
		float sfeet[3],shead[3];
		bool on_feet = EngWorldToScreen(feet,sfeet);
		bool on_head = EngWorldToScreen(head,shead);

		// Off-screen arrow: when both projections are behind the camera or land
		// outside the viewport, draw an arrow on the screen edge pointing at the
		// world position. Uses a simple yaw-relative direction so it stays correct
		// even when the player is directly behind us (W2S returns garbage there).
		bool offscreen = !on_feet || !on_head;
		float fx=0,fy=0,hx=0,hy=0;
		if(on_feet && on_head)
		{
			fx=(sfeet[0]*0.5f+0.5f)*sw; fy=sh-(sfeet[1]*0.5f+0.5f)*sh;
			hx=(shead[0]*0.5f+0.5f)*sw; hy=sh-(shead[1]*0.5f+0.5f)*sh;
			float midx=(fx+hx)*0.5f, midy=(fy+hy)*0.5f;
			if(midx<0||midx>=sw||midy<0||midy>=sh) offscreen=true;
		}
		if(offscreen)
		{
			if(cvar.esp_arrow)
			{
				// Direction in world space relative to local yaw -> screen vector.
				// We reuse rcos/rsin (computed earlier in the radar block) ONLY if
				// the radar is on; otherwise compute yaw here.
				float vcos=rcos, vsin=rsin;
				if(!radar_on)
				{
					float va[3]={0,0,0};
					DWORD fnVA2=EngFn(ENG_SLOT_GETVIEWANGLES);
					if(fnVA2>=0x10000) ((eng_GetViewAngles_t)fnVA2)(va);
					float yaw=va[1]*3.14159265f/180.0f;
					vcos=cosf(yaw); vsin=sinf(yaw);
				}
				float wx=o[0]-lo[0], wy=o[1]-lo[1];
				float rx= wx*vsin - wy*vcos;		// right of local
				float ry= wx*vcos + wy*vsin;		// in front of local
				float ang=atan2f(rx,ry);			// 0 = directly ahead
				float cxs=sw*0.5f, cys=sh*0.5f;
				float dxs=sinf(ang), dys=-cosf(ang);	// screen-space direction
				float mar=40.0f*ui_scale;
				// clamp the arrow to the screen rim along that direction
				float tx = (dxs>0) ? (sw-mar-cxs)/dxs : (dxs<0) ? (mar-cxs)/dxs : 1e9f;
				float ty = (dys>0) ? (sh-mar-cys)/dys : (dys<0) ? (mar-cys)/dys : 1e9f;
				float t=(tx<ty)?tx:ty; if(t<0) t=0;
				float ax=cxs+dxs*t, ay=cys+dys*t;
				(*orig_glColor3f)(r,g,b);
				DrawArrow2D(ax,ay,dxs,dys,12.0f*ui_scale);
				eng_players++;
			}
			continue;			// no box / name / dot drawing while off-screen
		}

		float y0=(hy<fy)?hy:fy, y1=(hy<fy)?fy:hy;	// top / bottom
		float bxh=y1-y0; if(bxh<4.0f) bxh=4.0f;
		float bxw=bxh*0.45f;
		float cx=(fx+hx)*0.5f;
		float x0=cx-bxw*0.5f, x1=cx+bxw*0.5f;

		float pad=(float)cvar.esp_box_pad*ui_scale;	// grow/shrink the box around the player
		x0-=pad; x1+=pad; y0-=pad; y1+=pad;
		if(x1-x0<2.0f){ float m=(x0+x1)*0.5f; x0=m-1.0f; x1=m+1.0f; }
		if(y1-y0<2.0f){ float m=(y0+y1)*0.5f; y0=m-1.0f; y1=m+1.0f; }

		// distance fade: closer = full alpha; > esp_maxdist (or 60m default) = 0.35
		float esp_a = 1.0f;
		if(cvar.esp_fade)
		{
			float lim = (cvar.esp_maxdist>0)?(float)cvar.esp_maxdist:60.0f;
			float t = distM/lim; if(t<0) t=0; if(t>1) t=1;
			esp_a = 1.0f - t*0.65f;		// 1.0 .. 0.35
		}

		// visibility check via depth buffer (dim/recolor when occluded)
		bool visible = true;
		if(cvar.esp_vischeck)
		{
			float chest[3]={o[0],o[1],o[2]+10.0f};	// chest height is the most reliable test
			visible = IsWorldVisible(chest[0],chest[1],chest[2],mm_w,pm_w,vp);
		}
		float vr=r, vg=g, vb=b;
		if(cvar.esp_vischeck && !visible) { vr*=0.4f; vg*=0.4f; vb*=0.4f; }		// dim when hidden

		if(cvar.esp_box)
		{
			float bw=(float)cvar.esp_box_width*ui_scale; if(bw<1.0f) bw=1.0f;	// user-tunable thickness
			(*orig_glLineWidth)(bw);
			(*orig_glColor4f)(vr,vg,vb,esp_a);
			DrawBox2D(x0,y0,x1,y1,(float)cvar.esp_box_radius*ui_scale);
		}

		// snapline: 1=screen bottom-center, 2=top-center, 3=crosshair
		if(cvar.esp_snap)
		{
			float ax=sw*0.5f, ay=sh*0.5f;
			if(cvar.esp_snap==1) ay=sh-2.0f;
			else if(cvar.esp_snap==2) ay=2.0f;
			(*orig_glColor4f)(vr,vg,vb,esp_a*0.85f);
			(*orig_glLineWidth)(1.0f*ui_scale);
			(*orig_glBegin)(GL_LINES);
			(*orig_glVertex2f)(ax,ay);
			(*orig_glVertex2f)(cx,(y0+y1)*0.5f);
			(*orig_glEnd)();
		}

		if(cvar.esp_name)
		{
			float ta=gTextAlpha; gTextAlpha=esp_a;
			float ntw=TextWidthPxSz(namebuf,cvar.esp_name_size);
			DrawTextSz(cx-ntw*0.5f, y0-(14.0f+(float)cvar.esp_name_pad)*ui_scale, vr,vg,vb, cvar.esp_name_size, "%s", namebuf);
			gTextAlpha=ta;
		}

		if(cvar.esp_dist)
		{
			float ta=gTextAlpha; gTextAlpha=esp_a;
			char distbuf[32]; sprintf(distbuf,"%.0fm",distM);
			float dtw=TextWidthPxSz(distbuf,cvar.esp_dist_size);
			DrawTextSz(cx-dtw*0.5f, y1+(2.0f+(float)cvar.esp_dist_pad)*ui_scale, 1.0f,1.0f,1.0f, cvar.esp_dist_size, "%.0fm", distM);
			gTextAlpha=ta;
		}

		eng_players++;
	}

	if(aim_found)
	{
		eng_aim_have=true;
		eng_aim_sx=aim_best_sx; eng_aim_sy=aim_best_sy;
		eng_aim_visible=aim_best_vis;
	}

	// triggerbot: track acquisition time so sys_glViewport can honor trigger_delay
	if(trig_hit)
	{
		if(!eng_trig_active){ eng_trig_active=true; eng_trig_acq=GetTickCount(); }
	}
	else eng_trig_active=false;

	// detection logging: update PVS counters (current / peak / EMA average)
	det_cur=det_seen;
	if(det_cur>det_peak) det_peak=det_cur;
	det_avg += ((float)det_cur - det_avg)*0.05f;	// exponential moving average

	// top-left debug readouts: padded off the corner and spaced apart (scaled)
	float dbgx=16.0f*ui_scale, dbgy=24.0f*ui_scale, dbgline=20.0f*ui_scale;
	if(cvar.esp_engine && cvar.esp_dbg)
		DrawText(dbgx,dbgy,0.2f,1.0f,0.4f,"ENGINE ESP: %i players  team=%s",
			eng_players, eng_have_extra?"extra":"model");

	if(cvar.esp_log)
		DrawText(dbgx,dbgy+dbgline,0.6f,0.9f,1.0f,"PVS/detect: cur=%i  peak=%i  avg=%.1f",
			det_cur, det_peak, det_avg);

	// spec_warn debug: shows whether the engine actually gives us the observer target.
	// "specs" = spectators in the list, "watch" = those resolved to be watching us,
	// mode0/tgt0 = the first spectator's raw iuser1/iuser2 (tgt0 == me => iuser2 works).
	if(cvar.spec_warn && cvar.esp_dbg)
		DrawText(dbgx,dbgy+dbgline*2.0f,1.0f,0.7f,0.2f,"SPEC: specs=%i watch=%i  me=%i mode0=%i tgt0=%i",
			spec_seen, spec_total, eng_local_idx, spec_dbg_mode, spec_dbg_tgt);

	// "Who's watching me" block, centered just below the crosshair. Only present
	// while at least one spectator targets us (no fade: instant appear/disappear).
	// Line 1: a red dot + "N watching". Following lines: up to 3 names, each in its
	// team color (red T / blue CT / green = no team, i.e. a pure spectator/admin).
	if(cvar.spec_warn && spec_total>0)
	{
		float cxh=sw*0.5f, cyh=sh*0.5f;
		float lineh=14.0f*ui_scale;								// stack spacing per name
		float baseY=cyh + (52.0f+(float)cvar.spec_pad)*ui_scale;	// padding matches the HUD arcs' 52u baseline
		char hdr[32]; sprintf(hdr,"%i watching", spec_total);
		float hw=TextWidthPx(hdr);
		float dotr=3.0f*ui_scale, gap=6.0f*ui_scale;
		float startx=cxh-(dotr*2.0f+gap+hw)*0.5f;				// center the dot+text group
		(*orig_glColor4f)(1.0f,0.25f,0.25f,gTextAlpha);
		FillCircle2D(startx+dotr, baseY-3.0f*ui_scale, dotr);	// dot ~centered on the text row
		(*orig_glColor4f)(1,1,1,1);
		DrawText(startx+dotr*2.0f+gap, baseY, 1.0f,0.25f,0.25f, "%s", hdr);
		for(int si=0;si<spec_show;si++)
		{
			float nw=TextWidthPx(spec_names[si]);
			DrawText(cxh-nw*0.5f, baseY+lineh*(float)(si+1),
				spec_cols[si][0],spec_cols[si][1],spec_cols[si][2], "%s", spec_names[si]);
		}
	}

	}	// end if(cvar.esp_engine || cvar.radar || need_aim)

	(*orig_glMatrixMode)(GL_PROJECTION);
	(*orig_glPopMatrix)();
	(*orig_glMatrixMode)(GL_MODELVIEW);
	(*orig_glPopMatrix)();
	(*orig_glPopAttrib)();
}

// (tier1 DrawPlayerEsp removed - replaced entirely by DrawEngineEsp)

//////////////////////////////////////////////////////////////////////////////////////////////////
/************************************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////////////////
void sys_glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)
{
	orig_glMultiTexCoord2fARB(target,s,t);
}

void sys_glActiveTextureARB(GLenum target)
{
	orig_glActiveTextureARB(target);
}

void sys_BindTextureEXT(GLenum target, GLuint texture)
{
	orig_BindTextureEXT(target,texture);
}

void sys_glAlphaFunc (GLenum func,  GLclampf ref)
{
	(*orig_glAlphaFunc) (func, ref);
}

void sys_glBegin (GLenum mode)
{
	// No Flash: bFlash is only ARMED for a bright GL_QUADS quad (re-armed in the
	// GL_QUADS block below). While armed, the quad's vertices are buffered and
	// sys_glEnd decides whether it's the real fullscreen flash (suppress) or an
	// innocent bright HUD quad (replay). Clear it at the start of every primitive so
	// a stale arm from a previous quad can never leak into a later draw.
	bFlash=false;

	if ((cvar.wall==1) && (bWall) && (mode==GL_TRIANGLE_FAN || mode==GL_TRIANGLE_STRIP))
	{
		(*orig_glDisable)(GL_DEPTH_TEST);
	}
	else if((cvar.wall==2) && (bWall))
	{
		(*orig_glGetFloatv)(GL_CURRENT_COLOR, curcolor);
		(*orig_glDisable)(GL_DEPTH_TEST);
		(*orig_glEnable)(GL_BLEND);
		(*orig_glBlendFunc)(GL_SRC_ALPHA, GL_ONE);
		(*orig_glColor4f)(curcolor[0], curcolor[1], curcolor[2], 255.0);
	}
	else if((cvar.wall==3) && (bWall))
	{
		(*orig_glGetFloatv)(GL_CURRENT_COLOR, curcolor);
		(*orig_glDisable)(GL_DEPTH_TEST);
		(*orig_glEnable)(GL_BLEND);
		(*orig_glBlendFunc)(GL_SRC_ALPHA, GL_SRC_ALPHA_SATURATE);
		(*orig_glColor4f)(curcolor[0], curcolor[1], curcolor[2], 1.0);
	}
	if (mode==GL_QUADS)
	{
		bSky=true;
		if(cvar.smoke)
		{
			GLfloat smokecol[4];
			(*orig_glGetFloatv)(GL_CURRENT_COLOR, smokecol);
			if((smokecol[0]==smokecol[1]) && (smokecol[0]==smokecol[2]) && (smokecol[0]!=0.0) && (smokecol[0]!=1.0))
				bSmoke=true;
			else 
				bSmoke=false;
		}
		if(cvar.flash)
		{
			GLfloat flashcol[4];
			(*orig_glGetFloatv)(GL_CURRENT_COLOR, flashcol);
			// The stock flashbang is a pure-white fullscreen quad, but modded
			// servers tint it (pink/green/random each round), so requiring exact
			// white missed those entirely. A flash is really just a BRIGHT, UNTEXTURED
			// overlay of any hue. We ARM on "bright + untextured" here, then in
			// sys_glEnd only suppress it if it truly spans the whole screen.
			//   - The UNTEXTURED check is essential: every bright HUD element (ammo
			//     digits, kill-feed weapon icons, buy-menu weapon image, buy-zone
			//     card icon) is a TEXTURED sprite, so it never arms and is drawn
			//     normally. Buffering a textured quad would also break it, because
			//     replaying its vertices in sys_glEnd loses the per-vertex texcoords.
			//   - The fullscreen gate then protects untextured bright quads that
			//     aren't the flash (e.g. the buy-menu panel).
			float mx=flashcol[0];
			if(flashcol[1]>mx) mx=flashcol[1];
			if(flashcol[2]>mx) mx=flashcol[2];
			// A genuine blinding flash -- white OR a modded/tinted one -- is drawn as
			// a near-OPAQUE fullscreen overlay (high alpha). A kill-confirm / damage /
			// respawn screen fade is the SAME bright untextured fullscreen quad but
			// TRANSLUCENT (low alpha), so the old "bright + untextured + fullscreen"
			// test misread it as a flash and popped "YOU HAVE BEEN FLASHED!" on every
			// kill. Adding the alpha gate keeps real flashes (they're opaque) while
			// letting these see-through fades render normally and stay silent.
			bFlash=(mx>=0.5f) && (flashcol[3]>=0.60f) && !(*orig_glIsEnabled)(GL_TEXTURE_2D);
			if(bFlash) flashVN=0;	// start buffering this (untextured) quad's vertices
		}
		if(cvar.scope)
		{
			GLfloat scopecol[4];
			(*orig_glGetFloatv)(GL_CURRENT_COLOR, scopecol);
			if(scopecol[0]==1.0f && scopecol[1]==1.0f && scopecol[2]==1.0f && scopecol[3]==1.0f)
			{
				(*orig_glEnable)(GL_BLEND);
				(*orig_glColor4f)(scopecol[0],scopecol[1],scopecol[2],0.0f);
				(*orig_glDisable)(GL_BLEND);
			}
		}
	}
	else
		bSky=false;

	(*orig_glBegin) (mode);
}

void sys_glBitmap (GLsizei width,  GLsizei height,  GLfloat xorig,  GLfloat yorig,  GLfloat xmove,  GLfloat ymove,  const GLubyte *bitmap)
{
	(*orig_glBitmap) (width, height, xorig, yorig, xmove, ymove, bitmap);
}

void sys_glBlendFunc (GLenum sfactor,  GLenum dfactor)
{
	(*orig_glBlendFunc) (sfactor, dfactor);
}

void sys_glClear (GLbitfield mask)
{
	if ((mask==GL_DEPTH_BUFFER_BIT) && bSky)	// clear buffer if nosky is 1 otherwise sky is fucked
	{											// also nescessary if u add e.g. asus wh
		(*orig_glClearColor)(0.0f, 0.0f, 0.0f, 0.0f);
		mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		(*orig_glClear)(mask);
	}

	(*orig_glClear)(mask); 
}

void sys_glClearAccum (GLfloat red,  GLfloat green,  GLfloat blue,  GLfloat alpha)
{
	(*orig_glClearAccum) (red, green, blue, alpha);
}

void sys_glClearColor (GLclampf red,  GLclampf green,  GLclampf blue,  GLclampf alpha)
{
	(*orig_glClearColor) (red, green, blue, alpha);
}

void sys_glColor3f (GLfloat red,  GLfloat green,  GLfloat blue)
{
	(*orig_glColor3f) (red, green, blue);
}

void sys_glColor3ub (GLubyte red,  GLubyte green,  GLubyte blue)
{
	(*orig_glColor3ub) (red, green, blue);
}

void sys_glColor3ubv (const GLubyte *v)
{
	(*orig_glColor3ubv) (v);
}

void sys_glColor4f (GLfloat red,  GLfloat green,  GLfloat blue,  GLfloat alpha)
{
	(*orig_glColor4f) (red, green, blue, alpha);
}

void sys_glColor4ub (GLubyte red,  GLubyte green,  GLubyte blue,  GLubyte alpha)
{
	(*orig_glColor4ub) (red, green, blue, alpha);
}

void sys_glCullFace (GLenum mode)
{
	(*orig_glCullFace) (mode);
}

void sys_glDeleteTextures (GLsizei n,  const GLuint *textures)
{
	(*orig_glDeleteTextures) (n, textures);
}

void sys_glDepthFunc (GLenum func)
{
	(*orig_glDepthFunc) (func);
}

void sys_glDepthMask (GLboolean flag)
{
	(*orig_glDepthMask) (flag);
}

void sys_glDepthRange (GLclampd zNear,  GLclampd zFar)
{
	(*orig_glDepthRange) (zNear, zFar);
}

void sys_glDisable (GLenum cap)
{
	(*orig_glDisable) (cap);
}

void sys_glEnable (GLenum cap)
{
	if(!FirstInit) // just called once
	{
		BuildFont();							// load bitmap font
		(*orig_glGetIntegerv)(GL_VIEWPORT,vp);	// get screen resolution
		FirstInit=true;
	}

	if(enabledraw && hookactive)	// if viewport is called 5th time (avoid fps drop)
	{
		enabledraw=false;

		// put all text stuff here:
		// NOTE: the menu + F11 check are drawn later (in wglSwapBuffers, via
		// DrawOverlayUI) so they sit ON TOP of the radar / engine ESP overlay.

		if(gotflashed)
		{
			if(pTimer(1)) // 1 second only cuz gotflashed turns true/false even if ur still supposed to be flashed
				DrawText(vp[2]/2-50,vp[3]/2-20,1.0f,0.7f,0.7f,"YOU HAVE BEEN FLASHED!");
			else 
				gotflashed=false;
		}
	}

	if ((cvar.cross) && (!ch))
	{
		(*orig_glPushMatrix)(); 
		(*orig_glLoadIdentity)(); 
		(*orig_glDisable)(GL_TEXTURE_2D);
		(*orig_glEnable)(GL_BLEND);

		GLint iDim[4];

		(*orig_glGetIntegerv)(GL_VIEWPORT, iDim); 
		(*orig_glColor4f)(0.0f, 0.56f, 1.0f, 1.0f); 
		(*orig_glLineWidth)(1.0f); 

		(*orig_glBegin)(GL_LINES); 
		(*orig_glVertex2i)(iDim[2]/2, (iDim[3]/2)-22); 
		(*orig_glVertex2i)(iDim[2]/2, (iDim[3]/2)-2); 

		(*orig_glVertex2i)(iDim[2]/2, (iDim[3]/2)+2); 
		(*orig_glVertex2i)(iDim[2]/2, (iDim[3]/2)+22); 

		(*orig_glVertex2i)((iDim[2]/2)-22, iDim[3]/2); 
		(*orig_glVertex2i)((iDim[2]/2)-2, iDim[3]/2); 

		(*orig_glVertex2i)((iDim[2]/2)+2, iDim[3]/2); 
		(*orig_glVertex2i)((iDim[2]/2)+22, iDim[3]/2); 
		(*orig_glEnd)(); 

		(*orig_glDisable)(GL_BLEND);
		(*orig_glEnable)(GL_TEXTURE_2D); 
		(*orig_glPopMatrix)();
	}

	ch=true;

	(*orig_glEnable) (cap);
}

void sys_glEnd (void)
{
	if(bFlash && cvar.flash)
	{
		// We buffered a bright GL_QUADS quad. Suppress it ONLY if it really covers
		// the whole screen corner-to-corner (the flashbang blind). Anything smaller
		// is a HUD/UI element and gets drawn exactly as the engine intended.
		bool fullscreen=false;
		if(flashVN==4)
		{
			float minx=flashVX[0],maxx=flashVX[0],miny=flashVY[0],maxy=flashVY[0];
			for(int i=1;i<flashVN;i++)
			{
				if(flashVX[i]<minx) minx=flashVX[i];
				if(flashVX[i]>maxx) maxx=flashVX[i];
				if(flashVY[i]<miny) miny=flashVY[i];
				if(flashVY[i]>maxy) maxy=flashVY[i];
			}
			float w=(float)vp[2], h=(float)vp[3];	// 1px slack for rounding
			if(minx<=1.0f && maxx>=w-1.0f && miny<=1.0f && maxy>=h-1.0f)
				fullscreen=true;
		}
		if(fullscreen)
			gotflashed=true;	// drop the overlay entirely + show the FLASHED text
		else	// not a flash -> draw the buffered quad untouched
			for(int i=0;i<flashVN;i++) (*orig_glVertex2f)(flashVX[i],flashVY[i]);
		bFlash=false; flashVN=0;
	}
	(*orig_glEnd) ();
}

void sys_glFrustum (GLdouble left,  GLdouble right,  GLdouble bottom,  GLdouble top,  GLdouble zNear,  GLdouble zFar)
{
	(*orig_glFrustum) (left, right, bottom, top, zNear, zFar);
}

void sys_glOrtho (GLdouble left,  GLdouble right,  GLdouble bottom,  GLdouble top,  GLdouble zNear,  GLdouble zFar)
{
	(*orig_glOrtho) (left, right, bottom, top, zNear, zFar);
}

void sys_glPopMatrix (void)
{
	if (player.get) // player was drawn
	{
		player.get=false;
		if (cvar.chams)							// undo chams render state after each model
		{										// (so the world keeps its fill + textures)
			(*orig_glPolygonMode)(GL_FRONT_AND_BACK, GL_FILL);
			(*orig_glEnable)(GL_TEXTURE_2D);
		}
	}

	if(cvar.wall)
		bWall=false;

	(*orig_glPopMatrix) ();
}

void sys_glPopName (void)
{
	(*orig_glPopName) ();
}

void sys_glPrioritizeTextures (GLsizei n,  const GLuint *textures,  const GLclampf *priorities)
{
	(*orig_glPrioritizeTextures) (n, textures, priorities);
}

void sys_glPushAttrib (GLbitfield mask)
{
	(*orig_glPushAttrib) (mask);
}

void sys_glPushClientAttrib (GLbitfield mask)
{
	(*orig_glPushClientAttrib) (mask);
}

void sys_glPushMatrix (void)
{
	if(cvar.wall)
		bWall=true;

	(*orig_glPushMatrix) ();
}

void sys_glRotatef (GLfloat angle,  GLfloat x,  GLfloat y,  GLfloat z)
{
	(*orig_glRotatef) (angle, x, y, z);
}

void sys_glShadeModel (GLenum mode)
{
	(*orig_glShadeModel) (mode);

	GLfloat color[4];
	(*orig_glGetFloatv)(GL_CURRENT_COLOR, color);
	(*orig_glDisable)(GL_TEXTURE_2D);

	// A player model starts drawing on its first GL_SMOOTH. We only use this to
	// drive chams now - the legacy vertex-count aimbot was removed in favor of
	// the engine entity-list aimbot (see DrawEngineEsp / sys_glViewport).
	if ((mode==GL_SMOOTH) && !(player.get))
	{
		player.get=true;
		if (cvar.chams && cvar.chams_wire)		// wireframe ("spider") chams; restored in glPopMatrix
		{
			(*orig_glPolygonMode)(GL_FRONT_AND_BACK, GL_LINE);
			(*orig_glLineWidth)(1.0f);
		}
	}

	(*orig_glEnable)(GL_TEXTURE_2D);
	(*orig_glColor4f)(color[0],color[1],color[2],color[3]);
}

void sys_glTexCoord2f (GLfloat s,  GLfloat t)
{
	(*orig_glTexCoord2f) (s, t);
}

void sys_glTexEnvf (GLenum target,  GLenum pname,  GLfloat param)
{
	(*orig_glTexEnvf) (target, pname, param);
}

void sys_glTexImage2D (GLenum target,  GLint level,  GLint internalformat,  GLsizei width,  GLsizei height,  GLint border,  GLenum format,  GLenum type,  const GLvoid *pixels)
{
	(*orig_glTexImage2D) (target, level, internalformat, width, height, border, format, type, pixels);
}

void sys_glTexParameterf (GLenum target,  GLenum pname,  GLfloat param)
{
	(*orig_glTexParameterf) (target, pname, param);
}

void sys_glTranslated (GLdouble x,  GLdouble y,  GLdouble z)
{
	(*orig_glTranslated) (x, y, z);
}

void sys_glTranslatef (GLfloat x,  GLfloat y,  GLfloat z)
{
	(*orig_glTranslatef) (x, y, z);
}

void sys_glVertex2f (GLfloat x,  GLfloat y)
{
	if(bFlash && cvar.flash)
	{
		// Buffer this armed (bright) quad's corners; sys_glEnd decides whether it's
		// the real fullscreen flash (drop it) or an innocent bright HUD quad (replay
		// it untouched). We must NOT draw-or-drop per vertex here: dropping is what
		// used to make the crosshair / buy menu / weapon + grenade icons disappear.
		if(flashVN < 4)
		{
			flashVX[flashVN]=x; flashVY[flashVN]=y; flashVN++;
			return;
		}
		// More than 4 vertices in this batch -> it isn't the lone flash quad. Replay
		// what we buffered and stop intercepting so nothing in the batch is lost.
		// (The engine's current color is still the one it set for this quad, so the
		// replayed vertices keep their intended color.)
		for(int i=0;i<flashVN;i++) (*orig_glVertex2f)(flashVX[i],flashVY[i]);
		bFlash=false; flashVN=0;
	}
	(*orig_glVertex2f) (x, y);
}

void sys_glVertex3f (GLfloat x,  GLfloat y,  GLfloat z)
{
	if (player.get)								// inside a player model (set by sys_glShadeModel)
	{
		if (cvar.chams)							// flat silhouette / colored wireframe model
		{
			(*orig_glDisable)(GL_TEXTURE_2D);
			(*orig_glColor3f)(1.0f, 0.15f, 0.95f);	// chams color (magenta, easy to spot)
		}
		else
			(*orig_glEnable)(GL_TEXTURE_2D);
	}

	if (cvar.lambert && !(player.get && cvar.chams))	// chams color overrides lambert on models
		glColor3f(1.0f, 1.0f, 1.0f);

	(*orig_glVertex3f) (x, y, z);
}

void sys_glVertex3fv (const GLfloat *v)
{
	modelviewport=true;

	if(bSmoke && cvar.smoke) // leave this function if hl draws smoke
	{
		return;
	}
	if (cvar.sky)			// ... or sky, if the cvars are enabled
	{
		if (bSky==true)
		{
			return;
		}
	}

	(*orig_glVertex3fv) (v);
}

void sys_glViewport (GLint x,  GLint y,  GLsizei width,  GLsizei height)
{
	viewportcount++;
	if(viewportcount >= 5)
		enabledraw=true;	// enable drawing of text when viewport is called 5th time

	// ---- aim activation gate (Always / Hold key / Toggle key) ----
	// aim_mode 0 = always on; 1 = only while aim_key is held; 2 = key toggles a
	// latched state. Edge-detected here so the latch flips once per physical press
	// regardless of how many times sys_glViewport runs this frame.
	bool aim_active=true;
	aim_status_hold=false;		// reset each frame; only Hold-mode-while-held sets it true
	if(cvar.aim && hookactive && enabledraw)
	{
		bool keydn=(GetAsyncKeyState(KeyTableVK(cvar.aim_key))&0x8000)!=0;
		if(cvar.aim_mode==1)							// hold
		{
			aim_active=keydn;
			// Persistent pink status for the whole time the key is held.
			if(keydn && cvar.notify){ strcpy(aim_status_msg,"Aimbot: ON"); aim_status_hold=true; }
		}
		else if(cvar.aim_mode==2)							// toggle
		{
			if(keydn && !g_aim_key_prev)					// rising edge = one press
			{
				g_aim_toggle_on=!g_aim_toggle_on;
				// Timed pink status on each press (re-pressing resets the timeout).
				SetAimStatus("Aimbot: %s", g_aim_toggle_on?"ON":"OFF");
			}
			aim_active=g_aim_toggle_on;
		}
		g_aim_key_prev=keydn;
	}

	// ---- engine-based aimbot ----
	// Consumes the screen target computed by DrawEngineEsp's player loop on the
	// PREVIOUS frame (DrawEngineEsp runs at the end of each frame in
	// wglSwapBuffers, this viewport hook runs during the next frame). We clear
	// eng_aim_have to ensure exactly one mouse nudge per frame even though
	// sys_glViewport is called many times.
	if(cvar.aim && aim_active && eng_aim_have && hookactive && enabledraw)
	{
		eng_aim_have=false;			// consume the per-frame pick
		{
			// Convert screen target to absolute mouse coords (0..65535 in vp space).
			// Optional smoothing: divide the move into N steps, only apply 1/N this frame.
			POINT cur; GetCursorPos(&cur);
			float s=(float)cvar.aim_smooth; if(s<0)s=0; if(s>10)s=10;
			float fac=(s<=0.0f)?1.0f:(1.0f/(1.0f+s*0.8f));	// 1.0 .. ~0.11
			float tx=eng_aim_sx, ty=eng_aim_sy;
			// Blend cursor toward (tx,ty) in screen-space pixels.
			// NOTE: GetCursorPos returns desktop coords. We assume CS runs fullscreen
			// with viewport == desktop, which is the common case for build 4554.
			float gx = (float)cur.x + (tx-(float)cur.x)*fac;
			float gy = (float)cur.y + (ty-(float)cur.y)*fac;
			DWORD mvx = (DWORD)((gx*65535.0f)/(float)vp[2]);
			DWORD mvy = (DWORD)((gy*65535.0f)/(float)vp[3]);
			mouse_event(MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE,mvx,mvy,0,0);
			HandleKey(VK_LBUTTON);		// preserve recoil compensation
			if(cvar.shoot && eng_aim_visible)
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			}
		}
	}

	// ---- triggerbot (cvar.trigger) ----
	// eng_trig_active/eng_trig_acq are refreshed by DrawEngineEsp. Fire once the
	// crosshair has rested on the enemy for trigger_delay ms, with a short
	// refractory gap so it behaves like fast taps rather than a held button.
	if(cvar.trigger && hookactive && enabledraw && eng_trig_active)
	{
		DWORD now=GetTickCount();
		DWORD delay=(DWORD)((cvar.trigger_delay<0)?0:cvar.trigger_delay);
		if((now-eng_trig_acq)>=delay && (now-eng_trig_fire)>=120)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			HandleKey(VK_LBUTTON);		// keep recoil compensation in sync
			eng_trig_fire=now;
		}
	}

	// (auto-bunnyhop moved to UpdateBhop(), called once per frame from
	// wglSwapBuffers -- sys_glViewport runs several times per frame, which would
	// make the press/release toggle fire an unpredictable number of times.)

	modelviewport=false;
	ch=false;

	if (enabledraw) // check for pressed keys all >5th viewports (fast enough)
	{
		HandleKey(VK_F10);
		HandleKey(VK_F11);
		HandleKey(VK_F12);
		HandleKey(VK_INSERT);
		HandleKey(VK_LEFT);
		HandleKey(VK_RIGHT);
		HandleKey(VK_UP);
		HandleKey(VK_DOWN);
	}
	(*orig_glViewport) (x, y, width, height);
}

PROC sys_wglGetProcAddress(LPCSTR ProcName)
{
	return orig_wglGetProcAddress(ProcName);
}

// Top UI layer: the hack menu and the F11 check screen, drawn AFTER DrawEngineEsp
// so they sit above the radar / engine ESP. Between themselves, whichever was opened
// last draws on top. The menu fade is ticked here every frame so it stays smooth.
void DrawOverlayUI()
{
	UpdateMenuAnim();	// tick the fade every frame (even while hidden) -> smooth in/out

	bool showMenu  = (menu.active || menu_alpha>0.002f);
	bool showCheck = (checktext  || check_alpha>0.002f);
	if(!showMenu && !showCheck) return;

	GLint vpe[4];
	(*orig_glGetIntegerv)(GL_VIEWPORT,vpe);
	float sw=(float)vpe[2], sh=(float)vpe[3];
	if(sw<=0||sh<=0) return;

	(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
	(*orig_glDisable)(GL_DEPTH_TEST);
	(*orig_glDisable)(GL_TEXTURE_2D);
	(*orig_glEnable)(GL_BLEND);
	(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPushMatrix)(); (*orig_glLoadIdentity)();
	(*orig_glOrtho)(0,sw,sh,0,-1,1);
	(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPushMatrix)(); (*orig_glLoadIdentity)();

	// later-opened panel draws on top; but while repositioning, keep the hack menu
	// (and its move hint) on top so it's always readable.
	bool menuTop = (menu_open_seq>=check_open_seq) || (menu_move_mode>0);
	if(menuTop)
	{
		if(showCheck) DrawCheckText(cvar.check_x,cvar.check_y);
		if(showMenu)  DrawMenu(cvar.menu_x,cvar.menu_y);
	}
	else
	{
		if(showMenu)  DrawMenu(cvar.menu_x,cvar.menu_y);
		if(showCheck) DrawCheckText(cvar.check_x,cvar.check_y);
	}

	(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPopMatrix)();
	(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPopMatrix)();
	(*orig_glPopAttrib)();
}

// Draw a centered rounded "pill" of text at vertical fraction cyf of the screen,
// with the given accent color (used for both the border and the text) and alpha.
// Shared by the yellow feature-toast and the pink aim-status notification.
static void DrawPill(const char *msg, float cyf, float cr, float cg, float cb, float a)
{
	if(msg==0 || msg[0]==0) return;

	GLint vpe[4]; (*orig_glGetIntegerv)(GL_VIEWPORT,vpe);
	float sw=(float)vpe[2], sh=(float)vpe[3];
	if(sw<=0||sh<=0) return;

	(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
	(*orig_glDisable)(GL_DEPTH_TEST);
	(*orig_glDisable)(GL_TEXTURE_2D);
	(*orig_glEnable)(GL_BLEND);
	(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPushMatrix)(); (*orig_glLoadIdentity)();
	(*orig_glOrtho)(0,sw,sh,0,-1,1);
	(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPushMatrix)(); (*orig_glLoadIdentity)();

	float tw=TextWidthPx(msg);								// exact text width -> true centering
	float padX=16.0f*ui_scale, padY=10.0f*ui_scale;			// generous padding on all 4 sides
	float charH=10.0f*ui_scale;								// font cap height (~10px @1080p)
	float cx=sw*0.5f, cyc=sh*cyf;							// horizontal + vertical center of the pill
	float x0=cx-tw*0.5f-padX, x1=cx+tw*0.5f+padX;
	float y0=cyc-charH*0.5f-padY, y1=cyc+charH*0.5f+padY;
	float rad=6.0f*ui_scale;

	(*orig_glColor4f)(0.0f,0.0f,0.0f,0.55f*a);				// translucent fill
	FillRoundRect2D(x0,y0,x1,y1,rad);
	(*orig_glColor4f)(cr,cg,cb,a);							// accent border to match the text
	(*orig_glLineWidth)(2.0f*ui_scale);						// same stroke width as the hack menu
	DrawBox2D(x0,y0,x1,y1,rad);

	float ta=gTextAlpha; gTextAlpha=a;
	DrawText(cx-tw*0.5f, cyc+charH*0.5f, cr,cg,cb, "%s", msg);	// centered
	gTextAlpha=ta;

	(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPopMatrix)();
	(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPopMatrix)();
	(*orig_glPopAttrib)();
}

// Centered toast near the bottom of the screen. Drawn in its own 2D pass so it
// shows regardless of whether the ESP/menu passes ran. Fades out over the last
// 400 ms of its lifetime.
void DrawToast()
{
	if(toast_until==0 || toast_msg[0]==0) return;
	DWORD now=GetTickCount();
	if(now>=toast_until){ toast_until=0; return; }

	float a=(float)(toast_until-now)/400.0f; if(a>1.0f) a=1.0f;	// fade last 400ms
	DrawPill(toast_msg, 0.80f, 1.0f,0.9f,0.4f, a);				// yellow, bottom-centered
}

// Pink aim-key status pill, drawn just above the feature toast so the two never
// overlap. Hold mode: persistent (no fade) for as long as the key is held.
// Toggle mode: a timed pill (like the toast) that fades over its last 400 ms.
void DrawAimStatus()
{
	if(aim_status_hold)											// Hold mode, key currently down
	{
		DrawPill(aim_status_msg, 0.74f, AIM_NOTIFY_R,AIM_NOTIFY_G,AIM_NOTIFY_B, 1.0f);
		return;
	}

	if(aim_status_until==0 || aim_status_msg[0]==0) return;		// Toggle mode (timed)
	DWORD now=GetTickCount();
	if(now>=aim_status_until){ aim_status_until=0; return; }

	float a=(float)(aim_status_until-now)/400.0f; if(a>1.0f) a=1.0f;	// fade last 400ms
	DrawPill(aim_status_msg, 0.74f, AIM_NOTIFY_R,AIM_NOTIFY_G,AIM_NOTIFY_B, a);
}

// Low-level mouse hook callback. We ONLY care about the physical left button and
// must ignore our own injected clicks, otherwise the autofire would react to its
// own events. mouse_event/SendInput set LLMHF_INJECTED on the event flags, so we
// filter those out and record only genuine hardware presses/releases.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode==HC_ACTION)
	{
		MSLLHOOKSTRUCT *m=(MSLLHOOKSTRUCT*)lParam;
		if(!(m->flags & LLMHF_INJECTED))		// hardware event only
		{
			if(wParam==WM_LBUTTONDOWN) g_phys_lb=true;
			else if(wParam==WM_LBUTTONUP) g_phys_lb=false;
		}
	}
	return CallNextHookEx(g_ll_hook, nCode, wParam, lParam);
}

// The low-level hook MUST live on a thread that constantly pumps messages. We
// give it its OWN thread instead of the render thread: an LL hook on the render
// thread froze the game while click-spamming, and if the busy render thread let
// the hook time out Windows silently skipped events (missing the UP -> autofire
// stuck on). This thread does nothing but service the hook, so it stays reliable.
DWORD WINAPI MouseHookThread(LPVOID)
{
	g_ll_hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_self_inst, 0);
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0)		// keeps the hook serviced
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if(g_ll_hook){ UnhookWindowsHookEx(g_ll_hook); g_ll_hook=NULL; }
	return 0;
}

// Auto-pistol / auto-knife (cvar.autofire). Driven exactly once per frame from
// wglSwapBuffers.
//
// Firing model: GoldSrc fires a semi-auto shot only when IN_ATTACK goes 0 -> 1
// between two usercmds (one per frame). So a same-frame up+down nets to "down"
// and never fires while you physically hold. We therefore split each shot: one
// frame fully RELEASED (UP), the next frame PRESSED (DOWN) -> a real 0->1 edge.
//
// Hold detection: we cannot trust GetAsyncKeyState because our own injected
// clicks pollute it (causing the earlier "fires forever" latch). Instead we read
// g_phys_lb, the true physical button state from the low-level hook, which
// ignores injected events. So we start/stop strictly on YOUR real hold/release.
// ---- auto bunnyhop (cvar.bhop) -------------------------------------------
// Called ONCE per frame from wglSwapBuffers. Spams the jump key (SPACE) by
// alternating press/release every frame, exactly like the autofire feature
// alternates the mouse button (the engine samples input once per usercmd/frame,
// so a clean cross-frame 0->1 edge is what makes it act).
//
// Why spam instead of "hold while grounded": the engine predicts the LOCAL
// player's movement client-side and never writes the air state back into
// curstate, so curstate.onground reads "grounded" even mid-jump (confirmed via
// the BHOP debug line). Holding SPACE down based on that stale flag makes the
// engine jump exactly once and then refuse all further jumps -- GoldSrc's
// PM_Jump ignores +jump until it has seen a -jump (the anti-pogo rule), which
// also blocks your manual jumps.
//
// IMPORTANT: we inject with KEYEVENTF_SCANCODE (hardware scan code, bVk=0).
// CS 1.6 reads the keyboard through DirectInput, which keys off SCAN CODES; a
// plain virtual-key keybd_event often isn't seen by DirectInput at all. The
// scan-code form is picked up by both DirectInput and the Win32 message pump.
// Assumes jump is bound to SPACE (the default).
void UpdateBhop()
{
	// NOTE: do NOT gate on enabledraw here. It's a one-shot per-frame flag that
	// sys_glViewport raises and sys_glEnable consumes BEFORE wglSwapBuffers runs,
	// so it's always false by the time we're called. Match autofire's gating.
	bool want=false;
	if(cvar.bhop && hookactive && !menu.active)
		want = (cvar.bhop_hold==0) ||									// Always
		       ((GetAsyncKeyState(KeyTableVK(cvar.bhop_key))&0x8000)!=0);	// Hold key
	g_bhop_want=want;	// expose for the debug readout

	BYTE sc=(BYTE)MapVirtualKey(VK_SPACE,MAPVK_VK_TO_VSC);	// SPACE hardware scan code (0x39)

	if(want)
	{
		// Alternate down/up each frame -> +jump / -jump edges at the frame rate.
		if(g_bhop_down)
		{
			keybd_event(0,sc,KEYEVENTF_SCANCODE|KEYEVENTF_KEYUP,0);
			g_bhop_down=false;
		}
		else
		{
			keybd_event(0,sc,KEYEVENTF_SCANCODE,0);
			g_bhop_down=true;
			g_bhop_pulses++;								// count presses (debug)
		}
	}
	else if(g_bhop_down)	// not bhopping (or hack off) -> make sure SPACE is released
	{
		keybd_event(0,sc,KEYEVENTF_SCANCODE|KEYEVENTF_KEYUP,0);
		g_bhop_down=false;
	}
}

void UpdateAutofire()
{
	static bool  phase_press=false;		// next injected event is the DOWN (fire) half
	static bool  our_down=false;		// did WE leave the button injected-down?
	static bool  prev_phys=false;		// physical state last frame (for the press edge)
	static DWORD af_t=0;				// tick of the last shot (rate measured from here)

	bool enabled = cvar.autofire && hookactive && !menu.active;

	// Lazily spin up the dedicated hook thread ONCE, the first time autofire is
	// used. It then lives for the whole session (toggling autofire off/on does
	// NOT create more threads). We close the handle immediately since we never
	// join it; the thread self-manages and is reclaimed by the OS on exit.
	if(enabled && !g_hook_started)
	{
		g_phys_lb=(GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0;	// seed current state
		HANDLE h=CreateThread(NULL,0,MouseHookThread,NULL,0,NULL);
		if(h){ CloseHandle(h); g_hook_started=true; }
	}

	bool phys=g_phys_lb;
	if(phys && !prev_phys) af_t=GetTickCount();	// your manual press already fired once; wait a full rate
	prev_phys=phys;

	if(!enabled || !phys)				// not holding (or disabled) -> stop, release if needed
	{
		if(our_down){ mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0); our_down=false; }
		phase_press=false;
		return;
	}

	if(phase_press)						// previous frame was the release -> press now = fire
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		our_down=true;
		phase_press=false;
		af_t=GetTickCount();			// rate counts from the shot
		return;
	}

	DWORD now=GetTickCount();
	int rate=cvar.autofire_rate; if(rate<15) rate=15;	// ms between shots
	if((now-af_t)>=(DWORD)rate)			// time for the next shot: release this frame...
	{
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);	// ...DOWN comes next frame -> 0->1 edge
		our_down=false;
		phase_press=true;
	}
}

void sys_wglSwapBuffers(HDC hDC)
{
	if(hookactive)
	{
		UpdateAutofire();	// once-per-frame auto-pistol/auto-knife
		EnsureNoRecoilHook();	// (un)install the V_CalcRefdef detour for no visual recoil
		DrawEngineEsp();	// radar + engine ESP + own HUD (bottom overlay layer)
		DrawToast();		// feature toggle notifications (middle layer)
		DrawAimStatus();	// pink Hold/Toggle aim-key status (middle layer)
		DrawOverlayUI();	// hack menu + F11 check (top overlay layer)
	}
	UpdateBhop();			// once-per-frame jump spam (also releases SPACE when bhop/hack is off)
	viewportcount=0;		// reset viewport count, cuz this is the last function called every frame
	(*orig_wglSwapBuffers) (hDC);
}

BOOL __stdcall DllMain (HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			g_self_inst = hinstDll;			// remember our own module for the low-level hook
			DisableThreadLibraryCalls (hOriginalDll);
			return Init();

		case DLL_PROCESS_DETACH:
			if ( norec_hooked && norec_fn )		// restore V_CalcRefdef so a stale jmp can't crash
			{
				DWORD old;
				if(VirtualProtect((void*)norec_fn,5,PAGE_EXECUTE_READWRITE,&old))
				{
					memcpy((void*)norec_fn,g_vcalc_orig,5);
					VirtualProtect((void*)norec_fn,5,old,&old);
				}
				norec_hooked=false;
			}
			if ( g_ll_hook != NULL )			// remove the physical-button hook
			{
				UnhookWindowsHookEx(g_ll_hook);
				g_ll_hook = NULL;
			}
			if ( hOriginalDll != NULL )
			{
				FreeLibrary(hOriginalDll);
				hOriginalDll = NULL;
			}
			break;
	}
	return TRUE;
}