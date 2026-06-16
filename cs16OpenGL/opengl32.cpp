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
offset_s	offset[10];	// custom offset stuff
team_s		team[2];	// containing all vertcounts and team defs

GLuint base; // for bitmap font
HDC hDC;
void CountOffset();		// get the number of added offsets
void SetOffset(int x);	// set offset for aiming
void SetOffsetNames();	// sets offsets names (cuz they consist of 5 parts/words)
void UnvalidVertex();	// turn all vert counts invalid
int GetVertexMin();		// get lowest vert count
int GetVertexMax();		// get highest vert count
float curcolor[4];

// default on-screen panel positions + move-mode step (used by HookInit / menu / HandleKey)
#define MENU_DEF_X   40
#define MENU_DEF_Y   50
#define CHECK_DEF_X  40
#define CHECK_DEF_Y  40
#define RADAR_DEF_X  84		// radar center default (pre-ui_scale): ~14px margin + 70px radius
#define RADAR_DEF_Y  84
#define MOVE_STEP    2		// px per polled frame while in move mode (held = continuous)

// rounded-rectangle helpers (defined lower, used by the menu / F11 panels above them)
void DrawBox2D(float x0,float y0,float x1,float y1,float rad);			// rounded outline
void FillRoundRect2D(float x0,float y0,float x1,float y1,float rad);		// rounded fill
void SetToast(const char *fmt, ...);									// toast notification (defined lower)

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
					sscanf(str, "trigger %i;"	,&cvar.trigger);
					sscanf(str, "trigger_delay %i;",&cvar.trigger_delay);
					sscanf(str, "autofire %i;"	,&cvar.autofire);
					sscanf(str, "autofire_rate %i;",&cvar.autofire_rate);
					sscanf(str, "notify %i;"	,&cvar.notify);
					sscanf(str, "esp_log %i;"	,&cvar.esp_log);
					sscanf(str, "aimthru %i;"	,&cvar.aimthru);
					sscanf(str, "target %i;"	,&cvar.target);
					sscanf(str, "recoil %i;"	,&cvar.recoil);
					sscanf(str, "esp_engine %i;",&cvar.esp_engine);
					sscanf(str, "esp_name %i;"	,&cvar.esp_name);
					sscanf(str, "esp_box %i;"	,&cvar.esp_box);
					sscanf(str, "esp_dist %i;"	,&cvar.esp_dist);
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
					sscanf(str, "radar_zoom %i;",&cvar.radar_zoom);
					sscanf(str, "radar_rotate %i;",&cvar.radar_rotate);
					sscanf(str, "radar_names %i;",&cvar.radar_names);
					sscanf(str, "radar_rings %i;",&cvar.radar_rings);
					sscanf(str, "hud_pad %i;"	,&cvar.hud_pad);
					sscanf(str, "esp_box_pad %i;"	,&cvar.esp_box_pad);
					sscanf(str, "esp_box_radius %i;",&cvar.esp_box_radius);
					sscanf(str, "esp_box_width %i;",&cvar.esp_box_width);
					sscanf(str, "esp_head %i;"	,&cvar.esp_head);
					sscanf(str, "esp_snap %i;"	,&cvar.esp_snap);
					sscanf(str, "esp_vischeck %i;",&cvar.esp_vischeck);
					sscanf(str, "esp_arrow %i;"	,&cvar.esp_arrow);
					sscanf(str, "esp_maxdist %i;",&cvar.esp_maxdist);
					sscanf(str, "esp_fade %i;"	,&cvar.esp_fade);
					sscanf(str, "esp_team %i;"	,&cvar.esp_team);
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
					sscanf(str, "stand_h %f;"	,&cvar.stand_h);
					sscanf(str, "duck_h %f;"	,&cvar.duck_h);
					sscanf(str, "pronefix %f;"	,&cvar.pronefix);
					sscanf(str, "aimkey %i;"	,&cvar.aimkey);
					//offset scan
					sscanf(str, "offset0 s %f d %f %s %s %s %s %s;",&offset[0].s,&offset[0].d,&offset[0].npart1,&offset[0].npart2,&offset[0].npart3,&offset[0].npart4,&offset[0].npart5);
					sscanf(str, "offset1 s %f d %f %s %s %s %s %s;",&offset[1].s,&offset[1].d,&offset[1].npart1,&offset[1].npart2,&offset[1].npart3,&offset[1].npart4,&offset[1].npart5);
					sscanf(str, "offset2 s %f d %f %s %s %s %s %s;",&offset[2].s,&offset[2].d,&offset[2].npart1,&offset[2].npart2,&offset[2].npart3,&offset[2].npart4,&offset[2].npart5);
					sscanf(str, "offset3 s %f d %f %s %s %s %s %s;",&offset[3].s,&offset[3].d,&offset[3].npart1,&offset[3].npart2,&offset[3].npart3,&offset[3].npart4,&offset[3].npart5);
					sscanf(str, "offset4 s %f d %f %s %s %s %s %s;",&offset[4].s,&offset[4].d,&offset[4].npart1,&offset[4].npart2,&offset[4].npart3,&offset[4].npart4,&offset[4].npart5);
					sscanf(str, "offset5 s %f d %f %s %s %s %s %s;",&offset[5].s,&offset[5].d,&offset[5].npart1,&offset[5].npart2,&offset[5].npart3,&offset[5].npart4,&offset[5].npart5);
					sscanf(str, "offset6 s %f d %f %s %s %s %s %s;",&offset[6].s,&offset[6].d,&offset[6].npart1,&offset[6].npart2,&offset[6].npart3,&offset[6].npart4,&offset[6].npart5);
					sscanf(str, "offset7 s %f d %f %s %s %s %s %s;",&offset[7].s,&offset[7].d,&offset[7].npart1,&offset[7].npart2,&offset[7].npart3,&offset[7].npart4,&offset[7].npart5);
					sscanf(str, "offset8 s %f d %f %s %s %s %s %s;",&offset[8].s,&offset[8].d,&offset[8].npart1,&offset[8].npart2,&offset[8].npart3,&offset[8].npart4,&offset[8].npart5);
					sscanf(str, "offset9 s %f d %f %s %s %s %s %s;",&offset[9].s,&offset[9].d,&offset[9].npart1,&offset[9].npart2,&offset[9].npart3,&offset[9].npart4,&offset[9].npart5);
					//scan for model file
					sscanf(str, "modelfile %s;"	,&modelfile);
				}
			}
			fclose(file);

		}
	}
	else if(ftype==1)	// a model vert count file
	{
		if(!file)	// if file doesnt exist
		{
			mdlfail=true;	// turn bool true (later you can say whats wrong)
			return;			// and leave the function
		}
		else
		{
			strcpy(modelpath,filename);	// copy path to use it later error checking (F11)
			while(!feof(file))
			{
				fgets(str, 256, file);
				if(!strstr(str,"//"))
				{
					//team 0
					sscanf(str, "team 0 name %s;",&team[0].name);		// get team 0 name
					sscanf(str, "team 0 add 01 %i;",&team[0].vert01);	// and vertex counts
					sscanf(str, "team 0 add 02 %i;",&team[0].vert02);
					sscanf(str, "team 0 add 03 %i;",&team[0].vert03);
					sscanf(str, "team 0 add 04 %i;",&team[0].vert04);
					sscanf(str, "team 0 add 05 %i;",&team[0].vert05);
					sscanf(str, "team 0 add 06 %i;",&team[0].vert06);
					sscanf(str, "team 0 add 07 %i;",&team[0].vert07);
					sscanf(str, "team 0 add 08 %i;",&team[0].vert08);
					sscanf(str, "team 0 add 09 %i;",&team[0].vert09);
					sscanf(str, "team 0 add 10 %i;",&team[0].vert10);
					sscanf(str, "team 0 add 11 %i;",&team[0].vert11);
					sscanf(str, "team 0 add 12 %i;",&team[0].vert12);
					// team 1
					sscanf(str, "team 1 name %s;",&team[1].name);		// get team 1 name
					sscanf(str, "team 1 add 01 %i;",&team[1].vert01);	// and vertex counts
					sscanf(str, "team 1 add 02 %i;",&team[1].vert02);
					sscanf(str, "team 1 add 03 %i;",&team[1].vert03);
					sscanf(str, "team 1 add 04 %i;",&team[1].vert04);
					sscanf(str, "team 1 add 05 %i;",&team[1].vert05);
					sscanf(str, "team 1 add 06 %i;",&team[1].vert06);
					sscanf(str, "team 1 add 07 %i;",&team[1].vert07);
					sscanf(str, "team 1 add 08 %i;",&team[1].vert08);
					sscanf(str, "team 1 add 09 %i;",&team[1].vert09);
					sscanf(str, "team 1 add 10 %i;",&team[1].vert10);
					sscanf(str, "team 1 add 11 %i;",&team[1].vert11);
					sscanf(str, "team 1 add 12 %i;",&team[1].vert12);
				}
			}
			fclose(file);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Persistent user settings.
//   * oglconf.cfg stays the shipped DEFAULTS (+ offsets / modelfile).
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
	fprintf(f,"trigger %i\n",cvar.trigger);
	fprintf(f,"trigger_delay %i\n",cvar.trigger_delay);
	fprintf(f,"autofire %i\n",cvar.autofire);
	fprintf(f,"autofire_rate %i\n",cvar.autofire_rate);
	fprintf(f,"notify %i\n",cvar.notify);
	fprintf(f,"esp_log %i\n",cvar.esp_log);
	fprintf(f,"aimthru %i\n",cvar.aimthru);
	fprintf(f,"target %i\n",cvar.target);
	fprintf(f,"shoot %i\n",cvar.shoot);
	fprintf(f,"fov %i\n",cvar.fov);
	fprintf(f,"recoil %i\n",cvar.recoil);
	fprintf(f,"wall %i\n",cvar.wall);
	fprintf(f,"nosky %i\n",cvar.sky);
	fprintf(f,"noflash %i\n",cvar.flash);
	fprintf(f,"nosmoke %i\n",cvar.smoke);
	fprintf(f,"lambert %i\n",cvar.lambert);
	fprintf(f,"crosshair %i\n",cvar.cross);
	fprintf(f,"esp_engine %i\n",cvar.esp_engine);
	fprintf(f,"esp_name %i\n",cvar.esp_name);
	fprintf(f,"esp_box %i\n",cvar.esp_box);
	fprintf(f,"esp_box_pad %i\n",cvar.esp_box_pad);
	fprintf(f,"esp_box_radius %i\n",cvar.esp_box_radius);
	fprintf(f,"esp_box_width %i\n",cvar.esp_box_width);
	fprintf(f,"esp_dist %i\n",cvar.esp_dist);
	fprintf(f,"esp_head %i\n",cvar.esp_head);
	fprintf(f,"esp_snap %i\n",cvar.esp_snap);
	fprintf(f,"esp_vischeck %i\n",cvar.esp_vischeck);
	fprintf(f,"esp_arrow %i\n",cvar.esp_arrow);
	fprintf(f,"esp_maxdist %i\n",cvar.esp_maxdist);
	fprintf(f,"esp_fade %i\n",cvar.esp_fade);
	fprintf(f,"esp_team %i\n",cvar.esp_team);
	fprintf(f,"esp_hud %i\n",cvar.esp_hud);
	fprintf(f,"hud_hp %i\n",cvar.hud_hp);
	fprintf(f,"hud_ammo %i\n",cvar.hud_ammo);
	fprintf(f,"hud_die %i\n",cvar.hud_die);
	fprintf(f,"hud_pad %i\n",cvar.hud_pad);
	fprintf(f,"chams %i\n",cvar.chams);
	fprintf(f,"chams_wire %i\n",cvar.chams_wire);
	fprintf(f,"radar %i\n",cvar.radar);
	fprintf(f,"radar_x %i\n",cvar.radar_x);
	fprintf(f,"radar_y %i\n",cvar.radar_y);
	fprintf(f,"radar_shape %i\n",cvar.radar_shape);
	fprintf(f,"radar_zoom %i\n",cvar.radar_zoom);
	fprintf(f,"radar_rotate %i\n",cvar.radar_rotate);
	fprintf(f,"radar_names %i\n",cvar.radar_names);
	fprintf(f,"radar_rings %i\n",cvar.radar_rings);
	fprintf(f,"aimkey %i\n",cvar.aimkey);
	fprintf(f,"menu_x %i\n",cvar.menu_x);
	fprintf(f,"menu_y %i\n",cvar.menu_y);
	fprintf(f,"check_x %i\n",cvar.check_x);
	fprintf(f,"check_y %i\n",cvar.check_y);
	fprintf(f,"curoffset %i\n",curoffset);
	fprintf(f,"customoffset %i\n",customoffset?1:0);
	fprintf(f,"stand_h %f\n",cvar.stand_h);
	fprintf(f,"duck_h %f\n",cvar.duck_h);
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
	int ci=customoffset?1:0;			// keeps current value if the line is absent
	while(!feof(f))
	{
		str[0]=0;
		if(!fgets(str,256,f)) break;
		if(strstr(str,"//")) continue;
		sscanf(str,"aim %i"			,&cvar.aim);
		sscanf(str,"aim_smooth %i"	,&cvar.aim_smooth);
		sscanf(str,"trigger %i"		,&cvar.trigger);
		sscanf(str,"trigger_delay %i",&cvar.trigger_delay);
		sscanf(str,"autofire %i"	,&cvar.autofire);
		sscanf(str,"autofire_rate %i",&cvar.autofire_rate);
		sscanf(str,"notify %i"		,&cvar.notify);
		sscanf(str,"esp_log %i"		,&cvar.esp_log);
		sscanf(str,"aimthru %i"		,&cvar.aimthru);
		sscanf(str,"target %i"		,&cvar.target);
		sscanf(str,"shoot %i"		,&cvar.shoot);
		sscanf(str,"fov %i"			,&cvar.fov);
		sscanf(str,"recoil %i"		,&cvar.recoil);
		sscanf(str,"wall %i"		,&cvar.wall);
		sscanf(str,"nosky %i"		,&cvar.sky);
		sscanf(str,"noflash %i"		,&cvar.flash);
		sscanf(str,"nosmoke %i"		,&cvar.smoke);
		sscanf(str,"lambert %i"		,&cvar.lambert);
		sscanf(str,"crosshair %i"	,&cvar.cross);
		sscanf(str,"esp_engine %i"	,&cvar.esp_engine);
		sscanf(str,"esp_name %i"	,&cvar.esp_name);
		sscanf(str,"esp_box %i"		,&cvar.esp_box);
		sscanf(str,"esp_box_pad %i"	,&cvar.esp_box_pad);
		sscanf(str,"esp_box_radius %i",&cvar.esp_box_radius);
		sscanf(str,"esp_box_width %i",&cvar.esp_box_width);
		sscanf(str,"esp_dist %i"	,&cvar.esp_dist);
		sscanf(str,"esp_head %i"	,&cvar.esp_head);
		sscanf(str,"esp_snap %i"	,&cvar.esp_snap);
		sscanf(str,"esp_vischeck %i",&cvar.esp_vischeck);
		sscanf(str,"esp_arrow %i"	,&cvar.esp_arrow);
		sscanf(str,"esp_maxdist %i"	,&cvar.esp_maxdist);
		sscanf(str,"esp_fade %i"	,&cvar.esp_fade);
		sscanf(str,"esp_team %i"	,&cvar.esp_team);
		sscanf(str,"esp_hud %i"		,&cvar.esp_hud);
		sscanf(str,"hud_hp %i"		,&cvar.hud_hp);
		sscanf(str,"hud_ammo %i"	,&cvar.hud_ammo);
		sscanf(str,"hud_die %i"		,&cvar.hud_die);
		sscanf(str,"hud_pad %i"		,&cvar.hud_pad);
		sscanf(str,"chams %i"		,&cvar.chams);
		sscanf(str,"chams_wire %i"	,&cvar.chams_wire);
		sscanf(str,"radar %i"		,&cvar.radar);
		sscanf(str,"radar_x %i"		,&cvar.radar_x);
		sscanf(str,"radar_y %i"		,&cvar.radar_y);
		sscanf(str,"radar_shape %i"	,&cvar.radar_shape);
		sscanf(str,"radar_zoom %i"	,&cvar.radar_zoom);
		sscanf(str,"radar_rotate %i",&cvar.radar_rotate);
		sscanf(str,"radar_names %i"	,&cvar.radar_names);
		sscanf(str,"radar_rings %i"	,&cvar.radar_rings);
		sscanf(str,"aimkey %i"		,&cvar.aimkey);
		sscanf(str,"menu_x %i"		,&cvar.menu_x);
		sscanf(str,"menu_y %i"		,&cvar.menu_y);
		sscanf(str,"check_x %i"		,&cvar.check_x);
		sscanf(str,"check_y %i"		,&cvar.check_y);
		sscanf(str,"curoffset %i"	,&curoffset);
		sscanf(str,"customoffset %i",&ci);
		sscanf(str,"stand_h %f"		,&cvar.stand_h);
		sscanf(str,"duck_h %f"		,&cvar.duck_h);
	}
	customoffset=(ci!=0);
	fclose(f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void HookInit(bool activate)
{
	if(activate)					// if hack is activated once:
	{
		UnvalidVertex();			// turn all vert counts invalid
		menu_move_mode=0;
		cvar.menu_x=MENU_DEF_X;   cvar.menu_y=MENU_DEF_Y;	// position defaults (oglconf/save override)
		cvar.check_x=CHECK_DEF_X; cvar.check_y=CHECK_DEF_Y;
		cvar.radar_x=RADAR_DEF_X; cvar.radar_y=RADAR_DEF_Y;
		LoadFile("oglconf.cfg",0);	// read DEFAULT cvar settings and modelfile
		LoadFile(modelfile,1);		// read modelfile, store all verts and team name
		CountOffset();				// count number of custom offsets
		SetOffsetNames();			// set the names
		LoadSettings();				// override defaults with the user's saved settings (if any)
		if(curoffset<0||curoffset>9) curoffset=0;	// hard bound (offset[] has 10 slots)
		if(offsetcount>0 && curoffset>offsetcount-1) curoffset=offsetcount-1;
		if(customoffset)			// keep the custom stand/duck heights restored by LoadSettings
			strcpy(offsetname, offset[(curoffset>=0&&curoffset<offsetcount)?curoffset:0].name);
		else
			SetOffset(curoffset);	// non-custom: take stand/duck from the selected offset
		oldtarget=cvar.target;		// sync so the line below keeps the restored target
		cvar.target=oldtarget;		// set current target (if hack was turned off and on again, it kept old target)
		cvar.scope=0;				// cvar which i didnt add into menu, removes sniper crosshair
		hookactive=true;			// turn bool true, e.g. to allow menu etc.
		message=true;				// turn true to get status text (F11)
		if(cvar.pronefix)
			player_height_min=0;	// fix ducked player check for mods like DoD
		else if(!cvar.pronefix)
			player_height_min=40;	// if no pronefix set min. player height to be recognized as "not dead"
		player_vertex_min=GetVertexMin();	// set lowest vert count
		player_vertex_max=GetVertexMax();	// set highest vert count
		SetToast("Hack: ON");				// greet (respects cvar.notify)
	}
	else if(!activate) // hack turned off, set all things to 0 (not activated)
	{
		cvar.aim=0;
		cvar.aim_smooth=0;
		cvar.trigger=0;
		cvar.trigger_delay=0;
		cvar.autofire=0;
		cvar.autofire_rate=0;
		cvar.notify=0;
		cvar.esp_log=0;
		cvar.aimthru=0;
		cvar.esp=0;
		cvar.esp_line=0;
		cvar.esp_engine=0;
		cvar.esp_name=0;
		cvar.esp_box=0;
		cvar.esp_box_pad=0;
		cvar.esp_box_radius=0;
		cvar.esp_box_width=0;
		cvar.esp_dist=0;
		cvar.esp_head=0;
		cvar.esp_snap=0;
		cvar.esp_vischeck=0;
		cvar.esp_arrow=0;
		cvar.esp_maxdist=0;
		cvar.esp_fade=0;
		cvar.esp_team=0;
		cvar.esp_hud=0;
		cvar.hud_hp=0;
		cvar.hud_ammo=0;
		cvar.hud_die=0;
		cvar.chams=0;
		cvar.chams_wire=0;
		cvar.radar=0;
		cvar.radar_shape=0;
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
void CountOffset()
{
	for (int x=0;x<10;x++)
	{	
		if((offset[x].s==0.0f) && (offset[x].d==0.0f))
			offsetcount=x;
		else
			offsetcount=10;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void SetOffsetNames()
{
	for (int r=0;r<10;r++)
		sprintf(offset[r].name,"%s %s %s %s %s",offset[r].npart1,offset[r].npart2,offset[r].npart3,offset[r].npart4,offset[r].npart5);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void SetOffset(int x) // called on hack load and whenever a custom offset is chosen
{
	strcpy(offsetname,offset[x].name);
	cvar.stand_h= offset[x].s;
	cvar.duck_h=  offset[x].d;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void UnvalidVertex() // called before vertices read from modelfile
{
	for(int vx=0;vx<2;vx++)
	{
		team[vx].vert01=-1;
		team[vx].vert02=-1;
		team[vx].vert03=-1;
		team[vx].vert04=-1;
		team[vx].vert05=-1;
		team[vx].vert06=-1;
		team[vx].vert07=-1;
		team[vx].vert08=-1;
		team[vx].vert09=-1;
		team[vx].vert10=-1;
		team[vx].vert11=-1;
		team[vx].vert12=-1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int GetVertexMin()
{
	int min=26666;
	for (int gx=0;gx<2;gx++)
	{
		if((team[gx].vert01>0) && (team[gx].vert01<min)) { min=team[gx].vert01; }
		if((team[gx].vert02>0) && (team[gx].vert02<min)) { min=team[gx].vert02; }
		if((team[gx].vert03>0) && (team[gx].vert03<min)) { min=team[gx].vert03; }
		if((team[gx].vert04>0) && (team[gx].vert04<min)) { min=team[gx].vert04; }
		if((team[gx].vert05>0) && (team[gx].vert05<min)) { min=team[gx].vert05; }
		if((team[gx].vert06>0) && (team[gx].vert06<min)) { min=team[gx].vert06; }
		if((team[gx].vert07>0) && (team[gx].vert07<min)) { min=team[gx].vert07; }
		if((team[gx].vert08>0) && (team[gx].vert08<min)) { min=team[gx].vert08; }
		if((team[gx].vert09>0) && (team[gx].vert09<min)) { min=team[gx].vert09; }
		if((team[gx].vert10>0) && (team[gx].vert10<min)) { min=team[gx].vert10; }
		if((team[gx].vert11>0) && (team[gx].vert11<min)) { min=team[gx].vert11; }
		if((team[gx].vert12>0) && (team[gx].vert12<min)) { min=team[gx].vert12; }
	}
	min=min-5;
	return min;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int GetVertexMax()
{
	int max=0;
	for (int fx=0;fx<2;fx++)
	{
		if(team[fx].vert01>max) { max=team[fx].vert01; }
		if(team[fx].vert02>max) { max=team[fx].vert02; }
		if(team[fx].vert03>max) { max=team[fx].vert03; }
		if(team[fx].vert04>max) { max=team[fx].vert04; }
		if(team[fx].vert05>max) { max=team[fx].vert05; }
		if(team[fx].vert06>max) { max=team[fx].vert06; }
		if(team[fx].vert07>max) { max=team[fx].vert07; }
		if(team[fx].vert08>max) { max=team[fx].vert08; }
		if(team[fx].vert09>max) { max=team[fx].vert09; }
		if(team[fx].vert10>max) { max=team[fx].vert10; }
		if(team[fx].vert11>max) { max=team[fx].vert11; }
		if(team[fx].vert12>max) { max=team[fx].vert12; }
	}
	max=max+5;
	return max;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/*
bool PlayerIsTeam(int team,int vert)
{
	if(
	   (vert==team[team].vert01) ||
	   (vert==team[team].vert02) ||
	   (vert==team[team].vert03) ||
	   (vert==team[team].vert04) ||
	   (vert==team[team].vert05) ||
	   (vert==team[team].vert06) ||
	   (vert==team[team].vert07) ||
	   (vert==team[team].vert08) ||
	   (vert==team[team].vert09) ||
	   (vert==team[team].vert10) ||
	   (vert==team[team].vert11) ||
	   (vert==team[team].vert12)
	  )
		return true;
	else
		return false;
}*/

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
		int rr=70;	// radar radius in pre-scale units -> keep the whole disc on screen
		int minx=rr, maxx=(int)((float)vp[2]/sc)-rr;
		int miny=rr, maxy=(int)((float)vp[3]/sc)-rr;
		cvar.radar_x+=dx; cvar.radar_y+=dy;
		if(cvar.radar_x<minx) cvar.radar_x=minx; if(cvar.radar_x>maxx) cvar.radar_x=maxx;
		if(cvar.radar_y<miny) cvar.radar_y=miny; if(cvar.radar_y>maxy) cvar.radar_y=maxy;
	}
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
				aimkeychanged=true;
				cvar.aimkey++;
				if(cvar.aimkey>3) { cvar.aimkey=0; }
				else if(cvar.aimkey<0) { cvar.aimkey=3; }
				SaveSettings();	// persist the aim-key choice too
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
			else if(menu.active && !keyp.up)
			{
				keyp.up=true;
				menu.count-=1;
			}
			break;
		case VK_DOWN:
			if(menu_move_mode) MoveActivePanel(0,MOVE_STEP);
			else if(menu.active && !keyp.down)
			{
				keyp.down=true;
				menu.count+=1;
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
	HFONT	font;										
	HFONT	oldfont;									

	// scale the bitmap font to the screen so text isn't tiny on 4K. Baseline is
	// 1080p; clamp so it never gets smaller than the original or absurdly big.
	GLint vpf[4]; (*orig_glGetIntegerv)(GL_VIEWPORT,vpf);
	ui_scale = (vpf[3]>0) ? (float)vpf[3]/1080.0f : 1.0f;
	if(ui_scale<1.0f) ui_scale=1.0f;
	if(ui_scale>2.5f) ui_scale=2.5f;
	int fh = (int)(10.0f*ui_scale + 0.5f);	// font height in px (10 at 1080p)

	base = (*orig_glGenLists)(96);								
	font = CreateFont(-fh,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_DONTCARE|DEFAULT_PITCH,
	"Verdana");
	oldfont = (HFONT)SelectObject(hDC, font);           
	wglUseFontBitmaps(hDC, 32, 96, base);				
	GetCharWidth32(hDC, 32, 127, g_fontw);	// record per-char widths for accurate text centering
	SelectObject(hDC, oldfont);							
	DeleteObject(font);									
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
	(*orig_glDeleteLists)(base, 96);
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

//////////////////////////////////////////////////////////////////////////////////////////////////
// Data-driven, animated menu:
//   * fade in/out (menu_alpha) and a highlight bar that smoothly slides to the
//     selected row (both time-based, so they look the same at 60 or 240 fps)
//   * dependency hiding: a row with a non-null "dep" pointer is shown only while
//     that parent cvar is on (e.g. Shoot/Aimthru/FOV hide unless Aimbot is on),
//     and the cursor navigates only over visible rows.
enum { IT_TOGGLE, IT_INT, IT_FLOAT, IT_OFFSET, IT_TARGET, IT_MOVE, IT_ACTION };
typedef struct {
	const char *label;
	int   type;
	void *p;			// int* or float* cvar this row controls
	float mn, mx, step;	// range/step for IT_INT / IT_FLOAT
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
		{"Offset",      IT_OFFSET, 0,                0,0,0,       0, 0,          0},
		{"Stand_h",     IT_FLOAT,  &cvar.stand_h,    10,26,0.25f, 1, 0,          0},
		{"Duck_h",      IT_FLOAT,  &cvar.duck_h,     10,28,0.25f, 1, 0,          0},
		{"Aimbot",      IT_TOGGLE, &cvar.aim,        0,0,0,       0, 0,          0},
		{"Aim smooth",  IT_INT,    &cvar.aim_smooth, 0,10,1,      0, &cvar.aim,  1},
		{"Target",      IT_TARGET, &cvar.target,     0,0,0,       0, &cvar.aim,  1},
		{"Shoot",       IT_TOGGLE, &cvar.shoot,      0,0,0,       0, &cvar.aim,  1},
		{"Aimthru",     IT_TOGGLE, &cvar.aimthru,    0,0,0,       0, &cvar.aim,  1},
		{"FOV",         IT_INT,    &cvar.fov,        0,1000,10,   1, &cvar.aim,  1},
		{"Triggerbot",  IT_TOGGLE, &cvar.trigger,    0,0,0,       0, 0,          0},
		{"Trigger delay",IT_INT,   &cvar.trigger_delay,0,500,10,  0, &cvar.trigger,1},
		{"Auto-fire",   IT_TOGGLE, &cvar.autofire,   0,0,0,       0, 0,          0},
		{"Auto-fire rate",IT_INT,  &cvar.autofire_rate,20,300,10, 0, &cvar.autofire,1},
		{"Recoil",      IT_INT,    &cvar.recoil,     0,5,1,       1, 0,          0},
		{"Wallhack",    IT_INT,    &cvar.wall,       0,3,1,       1, 0,          0},
		{"No Sky",      IT_TOGGLE, &cvar.sky,        0,0,0,       0, 0,          0},
		{"No Flash",    IT_TOGGLE, &cvar.flash,      0,0,0,       0, 0,          0},
		{"No Smoke",    IT_TOGGLE, &cvar.smoke,      0,0,0,       0, 0,          0},
		{"Lambert",     IT_TOGGLE, &cvar.lambert,    0,0,0,       0, 0,          0},
		{"ESP Engine",  IT_TOGGLE, &cvar.esp_engine, 0,0,0,       0, 0,                0},
		{"Player Name", IT_TOGGLE, &cvar.esp_name,   0,0,0,       0, &cvar.esp_engine, 1},
		{"Box",         IT_TOGGLE, &cvar.esp_box,    0,0,0,       0, &cvar.esp_engine, 1},
		{"Box padding", IT_INT,    &cvar.esp_box_pad,   -10,40,2, 0, &cvar.esp_engine, 1},
		{"Box radius",  IT_INT,    &cvar.esp_box_radius, 0,20,2,  0, &cvar.esp_engine, 1},
		{"Box width",   IT_INT,    &cvar.esp_box_width,  1,8,1,   0, &cvar.esp_engine, 1},
		{"Distance",    IT_TOGGLE, &cvar.esp_dist,   0,0,0,       0, &cvar.esp_engine, 1},
		{"Head dot",    IT_TOGGLE, &cvar.esp_head,   0,0,0,       0, &cvar.esp_engine, 1},
		{"Snaplines",   IT_INT,    &cvar.esp_snap,   0,3,1,       1, &cvar.esp_engine, 1},
		{"Vis check",   IT_TOGGLE, &cvar.esp_vischeck,0,0,0,      0, &cvar.esp_engine, 1},
		{"Off-screen arrow",IT_TOGGLE,&cvar.esp_arrow,0,0,0,      0, &cvar.esp_engine, 1},
		{"Max distance",IT_INT,    &cvar.esp_maxdist, 0,200,5,    0, &cvar.esp_engine, 1},
		{"Distance fade",IT_TOGGLE,&cvar.esp_fade,    0,0,0,      0, &cvar.esp_engine, 1},
		{"Show team",   IT_INT,    &cvar.esp_team,    0,2,1,      1, &cvar.esp_engine, 1},
		{"HUD HP/Ammo", IT_TOGGLE, &cvar.esp_hud,    0,0,0,       0, 0,                0},
		{"HP",          IT_TOGGLE, &cvar.hud_hp,     0,0,0,       0, &cvar.esp_hud,    1},
		{"Ammo",        IT_TOGGLE, &cvar.hud_ammo,   0,0,0,       0, &cvar.esp_hud,    1},
		{"Show when die",IT_TOGGLE,&cvar.hud_die,    0,0,0,       0, &cvar.esp_hud,    1},
		{"Padding",     IT_INT,    &cvar.hud_pad,   -20,80,4,     0, &cvar.esp_hud,    1},
		{"Chams",       IT_TOGGLE, &cvar.chams,      0,0,0,       0, 0,                0},
		{"Chams Wire",  IT_TOGGLE, &cvar.chams_wire, 0,0,0,       0, &cvar.chams,      1},
		{"Radar",       IT_TOGGLE, &cvar.radar,      0,0,0,       0, 0,                0},
		{"Move radar",  IT_MOVE,   0,               3,0,0,       0, &cvar.radar,      1},
		{"Dot shape",   IT_INT,    &cvar.radar_shape,0,1,1,       1, &cvar.radar,      1},
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
	int vis[64], nvis=0;
	for(int i=0;i<N;i++)
		if(items[i].dep==0 || *(items[i].dep)!=0) vis[nvis++]=i;
	if(nvis==0) return;
	if(menu.count>=nvis) menu.count=0;				// wrap the cursor
	if(menu.count<0)     menu.count=nvis-1;

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
		case IT_FLOAT:
			{
				float *v=(float*)it->p; *v += dir*it->step; customoffset=true;
				if(*v>it->mx) *v=it->mn;
				if(*v<it->mn) *v=it->mx;
			}
			break;
		case IT_OFFSET:
			if(dir>0) curoffset++; else if(dir<0) curoffset--;
			SetOffset(curoffset); customoffset=false;
			if(curoffset>offsetcount-1){ curoffset=0; SetOffset(curoffset); }
			else if(curoffset<0){ curoffset=offsetcount-1; SetOffset(curoffset); }
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
		else if(it->type==IT_INT)    SetToast("%s: %i", it->label, *(int*)it->p);
		else if(it->type==IT_TARGET) SetToast("Target: %s", (*(int*)it->p)?team[1].name:team[0].name);
		else if(it->type==IT_FLOAT)  SetToast("%s: %.2f", it->label, *(float*)it->p);
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
		float bb=y0+nvis*line-2*sc + (menu_move_mode? 12.0f*sc : 0.0f);	// room for the move hint
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

	// sliding highlight bar behind the selected row
	{
		float hy=y0+menu_sel_anim*line;
		(*orig_glPushAttrib)(GL_ALL_ATTRIB_BITS);
		(*orig_glDisable)(GL_TEXTURE_2D);
		(*orig_glEnable)(GL_BLEND);
		(*orig_glBlendFunc)(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		(*orig_glColor4f)(0.1f,0.6f,1.0f,0.30f*menu_alpha);
		(*orig_glBegin)(GL_QUADS);
		(*orig_glVertex2f)(mx-5*sc,   hy-line+3*sc);
		(*orig_glVertex2f)(mx+160*sc, hy-line+3*sc);
		(*orig_glVertex2f)(mx+160*sc, hy+3*sc);
		(*orig_glVertex2f)(mx-5*sc,   hy+3*sc);
		(*orig_glEnd)();
		(*orig_glPopAttrib)();
	}

	char buf[96];
	for(int r=0;r<nvis;r++)
	{
		mitem_t *it=&items[vis[r]];
		float ry=y0+r*line, ix=mx+(it->child?10.0f*sc:0.0f);
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
			else sprintf(buf,"%s%s: %i", pre,it->label,*(int*)it->p);
			break;
		case IT_FLOAT:  sprintf(buf,"%s%s: %.2f", pre,it->label,*(float*)it->p);          break;
		case IT_TARGET: sprintf(buf,"%s: %s", it->label,(*(int*)it->p)?team[1].name:team[0].name); break;
		case IT_OFFSET: if(!customoffset) sprintf(buf,"%s: %s",it->label,offsetname);
		                else              sprintf(buf,"%s: <custom>",it->label);          break;
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

	if(menu_move_mode)		// footer hint shown only while repositioning a panel
		DrawText(mx, y0+nvis*line+1.0f*sc, 1.0f,0.9f,0.4f, "arrows move, Insert=done");

	gTextAlpha=1.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// LEGACY menu below - unused, replaced by the data-driven DrawMenu above.
// Kept for reference; safe to delete.
void DrawMenu_legacy(int x, int y)
{
	DrawText(x,y-28,0.7f,0.7f,1.0f,"--------------------------");
	DrawText(x,y-17,0.85f,0.9f,1.0f,"- Mod by maitrungduc1410 -");
	DrawText(x,y+2 ,0.7f,0.7f,1.0f,"--------------------------");

	if(menu.count==0)
	{
		if(menu.select)
		{
			menu.select=false;
			if(menu.select_r)
			{
				curoffset++;
				SetOffset(curoffset);
				customoffset=false;
				menu.select_r=false;
			}
			else if(menu.select_l)
			{
				curoffset--;
				SetOffset(curoffset);
				customoffset=false;
				menu.select_l=false;
			}
			if(curoffset>offsetcount-1) { curoffset=0; SetOffset(curoffset); }
			else if(curoffset<0) { curoffset=offsetcount-1; SetOffset(curoffset); }
		}
		if(!customoffset)
			DrawText(x,y+7,1.0f,1.0f,1.0f,"Offset: %s",offsetname);
		else if(customoffset)
			DrawText(x,y+7,1.0f,1.0f,1.0f,"Offset: <custom>");

	}
	else if(menu.count!=0)
	{
		if(!customoffset)
			DrawText(x,y+7,0.7f,0.7f,1.0f,"Offset: %s",offsetname);
		else if(customoffset)
			DrawText(x,y+7,0.7f,0.7f,1.0f,"Offset: <custom>");

	}

	if(menu.count==1)
	{
		if(menu.select)
		{
			menu.select=false;
			if(menu.select_r)
			{
				cvar.stand_h+=0.25f;
				customoffset=true;
				menu.select_r=false;
			}
			else if(menu.select_l)
			{
				cvar.stand_h-=0.25f;
				customoffset=true;
				menu.select_l=false;
			}
			if(cvar.stand_h>26) { cvar.stand_h=10; }
			else if(cvar.stand_h<10) { cvar.stand_h=26; }
		}
		DrawText(x,y+20,1.0f,1.0f,1.0f,"Stand_h: %.2f",cvar.stand_h);
	}
	else if(menu.count!=1)
		DrawText(x,y+20,0.7f,0.7f,1.0f,"Stand_h: %.2f",cvar.stand_h);

	if(menu.count==2)
	{
		if(menu.select)
		{
			menu.select=false;
			if(menu.select_r)
			{
				cvar.duck_h+=0.25f;
				customoffset=true;
				menu.select_r=false;
			}
			else if(menu.select_l)
			{
				cvar.duck_h-=0.25f;
				customoffset=true;
				menu.select_l=false;
			}
			if(cvar.duck_h>28) { cvar.duck_h=10; }
			else if(cvar.duck_h<10) { cvar.duck_h=28; }
		}
		DrawText(x,y+33,1.0f,1.0f,1.0f,"Duck_h: %.2f",cvar.duck_h);
	}
	else if(menu.count!=2)
		DrawText(x,y+33,0.7f,0.7f,1.0f,"Duck_h: %.2f",cvar.duck_h);

	if(menu.count==3)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.target=change(cvar.target);
		}
		if(cvar.target) { DrawText(x,y+46,1.0f,1.0f,1.0f,"Target: %s",team[1].name); }
		else if(!cvar.target) { DrawText(x,y+46,1.0f,1.0f,1.0f,"Target: %s",team[0].name); }
	}
	else if(menu.count!=3)
	{
		if(cvar.target) { DrawText(x,y+46,0.7f,0.7f,1.0f,"Target: %s",team[1].name); }
		else if(!cvar.target) { DrawText(x,y+46,0.7f,0.7f,1.0f,"Target: %s",team[0].name); }
	}

	if(menu.count==4)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.aim=change(cvar.aim);
		}
		if(cvar.aim) { DrawText(x,y+59,1.0f,1.0f,1.0f,"Aimbot: On"); }
		else if(!cvar.aim) { DrawText(x,y+59,1.0f,1.0f,1.0f,"Aimbot: Off"); }
	}
	else if(menu.count!=4)
	{
		if(cvar.aim) { DrawText(x,y+59,0.7f,0.7f,1.0f,"Aimbot: On"); }
		else if(!cvar.aim) { DrawText(x,y+59,0.7f,0.7f,1.0f,"Aimbot: Off"); }
	}

	if(menu.count==5)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.shoot=change(cvar.shoot);
		}
		if(cvar.shoot) { DrawText(x,y+72,1.0f,1.0f,1.0f,"Shoot: On"); }
		else if(!cvar.shoot) { DrawText(x,y+72,1.0f,1.0f,1.0f,"Shoot: Off"); }
	}
	else if(menu.count!=5)
	{
		if(cvar.shoot) { DrawText(x,y+72,0.7f,0.7f,1.0f,"Shoot: On"); }
		else if(!cvar.shoot) { DrawText(x,y+72,0.7f,0.7f,1.0f,"Shoot: Off"); }
	}

	if(menu.count==6)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.aimthru=change(cvar.aimthru);
		}
		if(cvar.aimthru) { DrawText(x,y+85,1.0f,1.0f,1.0f,"Aimthru: On"); }
		else if(!cvar.aimthru) { DrawText(x,y+85,1.0f,1.0f,1.0f,"Aimthru: Off"); }
	}
	else if(menu.count!=6)
	{
		if(cvar.aimthru) { DrawText(x,y+85,0.7f,0.7f,1.0f,"Aimthru: On"); }
		else if(!cvar.aimthru) { DrawText(x,y+85,0.7f,0.7f,1.0f,"Aimthru: Off"); }
	}

	if(menu.count==7)
	{
		if(menu.select)
		{
			menu.select=false;
			if(menu.select_r)
			{
				cvar.fov+=50;
				menu.select_r=false;
			}
			else if(menu.select_l)
			{
				cvar.fov-=50;
				menu.select_l=false;
			}
			if(cvar.fov>1000) { cvar.fov=0; }
		}
		DrawText(x,y+98,1.0f,1.0f,1.0f,"FOV: %i",cvar.fov);
	}
	else if(menu.count!=7)
		DrawText(x,y+98,0.7f,0.7f,1.0f,"FOV: %i",cvar.fov);

	if(menu.count==8)
	{
		if(menu.select)
		{
			menu.select=false;
			if(menu.select_r)
			{
				cvar.recoil+=1;
				menu.select_r=false;
			}
			else if(menu.select_l)
			{
				cvar.recoil-=1;
				menu.select_l=false;
			}
			if(cvar.recoil>5) { cvar.recoil=0; }
			else if(cvar.recoil<0) { cvar.recoil=5; }
		}
		DrawText(x,y+111,1.0f,1.0f,1.0f,"Recoil: %i",cvar.recoil);
	}
	else if(menu.count!=8)
		DrawText(x,y+111,0.7f,0.7f,1.0f,"Recoil: %i",cvar.recoil);

	if(menu.count==9)
	{
		if(menu.select)
		{
			menu.select=false;
			if(menu.select_r)
			{
				cvar.wall+=1;
				menu.select_r=false;
			}
			else if(menu.select_l)
			{
				cvar.wall-=1;
				menu.select_l=false;
			}
			if(cvar.wall>3) { cvar.wall=0; }
			else if(cvar.wall<0) { cvar.wall=3; }
		}
		DrawText(x,y+124,1.0f,1.0f,1.0f,"Wallhack: %i",cvar.wall);
	}
	else if(menu.count!=9)
	DrawText(x,y+124,0.7f,0.7f,1.0f,"Wallhack: %i",cvar.wall);

	if(menu.count==10)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.sky=change(cvar.sky);
		}
		if(cvar.sky) { DrawText(x,y+137,1.0f,1.0f,1.0f,"No Sky: On"); }
		else if(!cvar.sky) { DrawText(x,y+137,1.0f,1.0f,1.0f,"NoSky: Off"); }
	}
	else if(menu.count!=10)
	{
		if(cvar.sky) { DrawText(x,y+137,0.7f,0.7f,1.0f,"No Sky: On"); }
		else if(!cvar.sky) { DrawText(x,y+137,0.7f,0.7f,1.0f,"NoSky: Off"); }
	}

	if(menu.count==11)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.flash=change(cvar.flash);
		}
		if(cvar.flash) { DrawText(x,y+150,1.0f,1.0f,1.0f,"No Flash: On"); }
		else if(!cvar.flash) { DrawText(x,y+150,1.0f,1.0f,1.0f,"No Flash: Off"); }
	}
	else if(menu.count!=11)
	{
		if(cvar.flash) { DrawText(x,y+150,0.7f,0.7f,1.0f,"No Flash: On"); }
		else if(!cvar.flash) { DrawText(x,y+150,0.7f,0.7f,1.0f,"No Flash: Off"); }
	}

	if(menu.count==12)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.smoke=change(cvar.smoke);
		}
		if(cvar.smoke) { DrawText(x,y+163,1.0f,1.0f,1.0f,"No Smoke: On"); }
		else if(!cvar.smoke) { DrawText(x,y+163,1.0f,1.0f,1.0f,"No Smoke: Off"); }
	}
	else if(menu.count!=12)
	{
		if(cvar.smoke) { DrawText(x,y+163,0.7f,0.7f,1.0f,"No Smoke: On"); }
		else if(!cvar.smoke) { DrawText(x,y+163,0.7f,0.7f,1.0f,"No Smoke: Off"); }
	}

	if(menu.count==13)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.lambert=change(cvar.lambert);
		}
		if(cvar.lambert) { DrawText(x,y+176,1.0f,1.0f,1.0f,"Lambert: On"); }
		else if(!cvar.lambert) { DrawText(x,y+176,1.0f,1.0f,1.0f,"Lambert: Off"); }
	}
	else if(menu.count!=13)
	{
		if(cvar.lambert) { DrawText(x,y+176,0.7f,0.7f,1.0f,"Lambert: On"); }
		else if(!cvar.lambert) { DrawText(x,y+176,0.7f,0.7f,1.0f,"Lambert: Off"); }
	}

	if(menu.count==14)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.esp=change(cvar.esp);
		}
		if(cvar.esp) { DrawText(x,y+189,1.0f,1.0f,1.0f,"ESP: On"); }
		else if(!cvar.esp) { DrawText(x,y+189,1.0f,1.0f,1.0f,"ESP: Off"); }
	}
	else if(menu.count!=14)
	{
		if(cvar.esp) { DrawText(x,y+189,0.7f,0.7f,1.0f,"ESP: On"); }
		else if(!cvar.esp) { DrawText(x,y+189,0.7f,0.7f,1.0f,"ESP: Off"); }
	}

	if(menu.count==15)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.cross=change(cvar.cross);
		}
		if(cvar.cross) { DrawText(x,y+202,1.0f,1.0f,1.0f,"Crosshair: On"); }
		else if(!cvar.cross) { DrawText(x,y+202,1.0f,1.0f,1.0f,"Crosshair: Off"); }
	}
	else if(menu.count!=15)
	{
		if(cvar.cross) { DrawText(x,y+202,0.7f,0.7f,1.0f,"Crosshair: On"); }
		else if(!cvar.cross) { DrawText(x,y+202,0.7f,0.7f,1.0f,"Crosshair: Off"); }
	}

	if(menu.count==16)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.esp_box=change(cvar.esp_box);
		}
		if(cvar.esp_box) { DrawText(x,y+215,1.0f,1.0f,1.0f,"ESP Box: On"); }
		else { DrawText(x,y+215,1.0f,1.0f,1.0f,"ESP Box: Off"); }
	}
	else if(menu.count!=16)
	{
		if(cvar.esp_box) { DrawText(x,y+215,0.7f,0.7f,1.0f,"ESP Box: On"); }
		else { DrawText(x,y+215,0.7f,0.7f,1.0f,"ESP Box: Off"); }
	}

	if(menu.count==17)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.esp_dist=change(cvar.esp_dist);
		}
		if(cvar.esp_dist) { DrawText(x,y+228,1.0f,1.0f,1.0f,"ESP Dist: On"); }
		else { DrawText(x,y+228,1.0f,1.0f,1.0f,"ESP Dist: Off"); }
	}
	else if(menu.count!=17)
	{
		if(cvar.esp_dist) { DrawText(x,y+228,0.7f,0.7f,1.0f,"ESP Dist: On"); }
		else { DrawText(x,y+228,0.7f,0.7f,1.0f,"ESP Dist: Off"); }
	}

	if(menu.count==18)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.esp_line=change(cvar.esp_line);
		}
		if(cvar.esp_line) { DrawText(x,y+241,1.0f,1.0f,1.0f,"ESP Line: On"); }
		else { DrawText(x,y+241,1.0f,1.0f,1.0f,"ESP Line: Off"); }
	}
	else if(menu.count!=18)
	{
		if(cvar.esp_line) { DrawText(x,y+241,0.7f,0.7f,1.0f,"ESP Line: On"); }
		else { DrawText(x,y+241,0.7f,0.7f,1.0f,"ESP Line: Off"); }
	}

	if(menu.count==19)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.esp_engine=change(cvar.esp_engine);
		}
		if(cvar.esp_engine) { DrawText(x,y+254,1.0f,1.0f,1.0f,"ESP Engine: On"); }
		else { DrawText(x,y+254,1.0f,1.0f,1.0f,"ESP Engine: Off"); }
	}
	else if(menu.count!=19)
	{
		if(cvar.esp_engine) { DrawText(x,y+254,0.7f,0.7f,1.0f,"ESP Engine: On"); }
		else { DrawText(x,y+254,0.7f,0.7f,1.0f,"ESP Engine: Off"); }
	}

	if(menu.count==20)
	{
		if(menu.select)
		{
			menu.select=false;
			cvar.esp_hud=change(cvar.esp_hud);
		}
		if(cvar.esp_hud) { DrawText(x,y+267,1.0f,1.0f,1.0f,"HUD HP/Ammo: On"); }
		else { DrawText(x,y+267,1.0f,1.0f,1.0f,"HUD HP/Ammo: Off"); }
	}
	else if(menu.count!=20)
	{
		if(cvar.esp_hud) { DrawText(x,y+267,0.7f,0.7f,1.0f,"HUD HP/Ammo: On"); }
		else { DrawText(x,y+267,0.7f,0.7f,1.0f,"HUD HP/Ammo: Off"); }
	}

	if(menu.count>20) { menu.count=0; }
	else if(menu.count<0) { menu.count=20; }
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
	if(mdlfail)
		DrawText(x,y,1.0f,0.5f,0.5f,"> Could not load model file: <%s> !!!",modelpath);
	else
		DrawText(x,y,1.0f,1.0f,1.0f,"> Model file: %s",modelpath);
	y=y+(int)(13*ui_scale);
	DrawText(x,y,0.5f,1.0f,0.5f,"> Your screen resolution is: %ix%i",vp[2],vp[3]);
	y=y+(int)(26*ui_scale);
	DrawText(x,y,0.7f,0.7f,1.0f,"%i valid offsets found:",offsetcount);
	y=y+(int)(13*ui_scale);
	for (int o=0;o<offsetcount;o++)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> [%i] %s| Stand: %0.2f | Duck: %0.2f",o+1,offset[o].name,offset[o].s,offset[o].d);
		y=y+(int)(13*ui_scale);
	}
	y=y+(int)(13*ui_scale);
	DrawText(x,y,0.7f,0.7f,1.0f,"Team 0: %s",team[0].name);
	y=y+(int)(13*ui_scale);
	if(team[0].vert01!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert01);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert02!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert02);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert03!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert03);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert04!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert04);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert05!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert05);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert06!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert06);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert07!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert07);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert08!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert08);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert09!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert09);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert10!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert10);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert11!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert11);
		y=y+(int)(13*ui_scale);
	}
	if(team[0].vert12!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert12);
		y=y+(int)(13*ui_scale);
	}
	//
	DrawText(x,y,0.7f,0.7f,1.0f,"Team 1: %s",team[1].name);
	y=y+(int)(13*ui_scale);
	if(team[1].vert01!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert01);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert02!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert02);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert03!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert03);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert04!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert04);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert05!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert05);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert06!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert06);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert07!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert07);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert08!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert08);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert09!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert09);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert10!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert10);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert11!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert11);
		y=y+(int)(13*ui_scale);
	}
	if(team[1].vert12!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert12);
		y=y+(int)(13*ui_scale);
	}
	y=y+(int)(13*ui_scale);
	DrawText(x,y,1.0f,1.0f,1.0f,"> lowest vertex count: %i",player_vertex_min+5);
	y=y+(int)(13*ui_scale);
	DrawText(x,y,1.0f,1.0f,1.0f,"> highest vertex count %i",player_vertex_max-5);

	check_h=(float)(y-startY)+13.0f*ui_scale;	// size next frame's panel to fit the text
	gTextAlpha=1.0f;							// restore for anything drawn after us
}

void DrawKeyInfo() // just drawn if 'aimkeychanged' is true
{
	if(cvar.aimkey==0)
		DrawText((vp[2]/2)-120,(vp[3]/2)-30,0.9f,0.9f,1.0f,"Aim changed to <Autoaim>");
	else if(cvar.aimkey==1)
		DrawText((vp[2]/2)-120,(vp[3]/2)-30,0.9f,0.9f,1.0f,"Aim changed to <Mouse1>");
	else if(cvar.aimkey==2)
		DrawText((vp[2]/2)-120,(vp[3]/2)-30,0.9f,0.9f,1.0f,"Aim changed to <Mouse2>");
	else if(cvar.aimkey==3)
		DrawText((vp[2]/2)-120,(vp[3]/2)-30,0.9f,0.9f,1.0f,"Aim changed to <Mouse3>");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
bool IsVertTeam(long vertcount,int t)	// helper: does vertcount belong to team t?
{
	return (
		(vertcount==team[t].vert01) || (vertcount==team[t].vert02) ||
		(vertcount==team[t].vert03) || (vertcount==team[t].vert04) ||
		(vertcount==team[t].vert05) || (vertcount==team[t].vert06) ||
		(vertcount==team[t].vert07) || (vertcount==team[t].vert08) ||
		(vertcount==team[t].vert09) || (vertcount==team[t].vert10) ||
		(vertcount==team[t].vert11) || (vertcount==team[t].vert12) );
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
#define ES_ORIGIN			0x010	// entity_state_t::origin (vec3)
#define ES_USEHULL			0x0C8	// entity_state_t::usehull (0 stand, 1 duck)

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
	if(!cvar.esp_engine && !cvar.esp_hud && !cvar.radar && !need_scan) return;
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
		if(cvar.esp_engine)
			DrawText(16.0f*ui_scale,24.0f*ui_scale,1.0f,0.7f,0.2f,"ENGINE ESP: searching engine table (start a game)...");
		(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPopMatrix)();
		(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPopMatrix)();
		(*orig_glPopAttrib)();
		return;
	}

	if(cvar.esp_engine || cvar.radar || need_scan)
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
	}

	// ---- 2D radar frame. Center is freely positioned (radar_x/radar_y, move mode 3). ----
	float rcx=0,rcy=0,rrad=0; float rcos=1.0f,rsin=0.0f; bool radar_on=false;
	if(cvar.radar)
	{
		rrad=70.0f*ui_scale;
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
			if(info.spectator) continue;
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
		if(EngDead(idx) || stale) continue;

		float o[3];
		o[0]=ReadFlt(ent+ENT_ORIGIN); o[1]=ReadFlt(ent+ENT_ORIGIN+4); o[2]=ReadFlt(ent+ENT_ORIGIN+8);
		if(o[0]==0&&o[1]==0&&o[2]==0)	// fallback: entity_state origin
		{
			DWORD cs=ent+ENT_CURSTATE;
			o[0]=ReadFlt(cs+ES_ORIGIN); o[1]=ReadFlt(cs+ES_ORIGIN+4); o[2]=ReadFlt(cs+ES_ORIGIN+8);
			if(o[0]==0&&o[1]==0&&o[2]==0) continue;
		}

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
			float headA[3]={o[0],o[1],o[2]+halfhA+zoffA-2.0f};	// just below head top
			float feetA[3]={o[0],o[1],o[2]-halfhA+zoffA};
			float sHe[3], sFe[3];
			bool okHe=EngWorldToScreen(headA,sHe);
			bool okFe=EngWorldToScreen(feetA,sFe);

			// --- aimbot: nearest head to the crosshair within FOV ---
			if(need_aim && okHe)
			{
				float ax=(sHe[0]*0.5f+0.5f)*sw, ay=sh-(sHe[1]*0.5f+0.5f)*sh;
				if(ax>=0 && ay>=0 && ax<sw && ay<sh)
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

		// head dot (small filled circle just above the head)
		if(cvar.esp_head)
		{
			(*orig_glColor4f)(vr,vg,vb,esp_a);
			FillCircle2D(hx, hy-3.0f*ui_scale, 3.0f*ui_scale);
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
			DrawText(cx-(float)strlen(namebuf)*4.0f*ui_scale, y0-14.0f*ui_scale, vr,vg,vb, "%s", namebuf);
			gTextAlpha=ta;
		}

		if(cvar.esp_dist)
		{
			float ta=gTextAlpha; gTextAlpha=esp_a;
			DrawText(cx-12.0f*ui_scale, y1+2.0f, 1.0f,1.0f,1.0f, "%.0fm", distM);
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
	if(cvar.esp_engine)
		DrawText(dbgx,dbgy,0.2f,1.0f,0.4f,"ENGINE ESP: %i players  team=%s",
			eng_players, eng_have_extra?"extra":"model");

	if(cvar.esp_log)
		DrawText(dbgx,dbgy+dbgline,0.6f,0.9f,1.0f,"PVS/detect: cur=%i  peak=%i  avg=%.1f",
			det_cur, det_peak, det_avg);

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
	// No Flash: bFlash must only suppress vertices of the ONE white fullscreen quad the
	// engine draws for a flashbang. It used to leak past that quad (it was never cleared
	// for non-QUADS primitives), so once any white quad set it, every later glVertex2f
	// draw - including the AWP scope rings and crosshair - got dropped too. Clear it at
	// the start of every primitive; the GL_QUADS block below re-arms it when needed.
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
			if(flashcol[0]==1.0 && flashcol[1]==1.0 && flashcol[2]==1.0)
				bFlash=true;
			else
				bFlash=false;
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

		if(aimkeychanged) // F10 pressed
		{
			if(pTimer(5)) // draw key change info for 5 seconds
				DrawKeyInfo();
			else
				aimkeychanged=false;
		}
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
		if (y==vp[3]) // if this matches we know, hl draws a white fullscreen (flashbang)
		{
			GLfloat flashcol[4]; 
			(*orig_glGetFloatv)(GL_CURRENT_COLOR, flashcol); // we store the color and ...
			(*orig_glColor4f)(flashcol[0],flashcol[1],flashcol[2],0.01f);	// call the color but with very low alpha (trans)
			bFlash=false;
			gotflashed=true; // draw flash warning message
		}
	}
	else (*orig_glVertex2f) (x, y);
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

	// ---- engine-based aimbot ----
	// Consumes the screen target computed by DrawEngineEsp's player loop on the
	// PREVIOUS frame (DrawEngineEsp runs at the end of each frame in
	// wglSwapBuffers, this viewport hook runs during the next frame). We clear
	// eng_aim_have to ensure exactly one mouse nudge per frame even though
	// sys_glViewport is called many times.
	if(cvar.aim && eng_aim_have && hookactive && enabledraw)
	{
		eng_aim_have=false;			// consume the per-frame pick
		bool keyok = (cvar.aimkey==0)
			|| (cvar.aimkey==1 && GetAsyncKeyState(VK_LBUTTON))
			|| (cvar.aimkey==2 && GetAsyncKeyState(VK_RBUTTON))
			|| (cvar.aimkey==3 && GetAsyncKeyState(VK_MBUTTON));
		if(keyok)
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

	// ---- auto-pistol / auto-knife (cvar.autofire) ----
	// While mouse1 is physically held, spam clicks at autofire_rate ms intervals.
	// Turns semi-auto pistols and the knife into rapid fire via mouse_event.
	if(cvar.autofire && hookactive && enabledraw && (GetAsyncKeyState(VK_LBUTTON)&0x8000))
	{
		static DWORD af_last=0;
		DWORD now=GetTickCount();
		int rate=cvar.autofire_rate; if(rate<20) rate=20;	// clamp to a sane max rate
		if((now-af_last)>=(DWORD)rate)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			af_last=now;
		}
	}

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

// Centered toast near the bottom of the screen. Drawn in its own 2D pass so it
// shows regardless of whether the ESP/menu passes ran. Fades out over the last
// 400 ms of its lifetime.
void DrawToast()
{
	if(toast_until==0 || toast_msg[0]==0) return;
	DWORD now=GetTickCount();
	if(now>=toast_until){ toast_until=0; return; }

	float a=(float)(toast_until-now)/400.0f; if(a>1.0f) a=1.0f;	// fade last 400ms

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

	float tw=TextWidthPx(toast_msg);						// exact text width -> true centering
	float padX=16.0f*ui_scale, padY=10.0f*ui_scale;			// generous padding on all 4 sides
	float charH=10.0f*ui_scale;								// font cap height (~10px @1080p)
	float cx=sw*0.5f, cyc=sh*0.80f;							// horizontal + vertical center of the pill
	float x0=cx-tw*0.5f-padX, x1=cx+tw*0.5f+padX;
	float y0=cyc-charH*0.5f-padY, y1=cyc+charH*0.5f+padY;
	float rad=6.0f*ui_scale;

	(*orig_glColor4f)(0.0f,0.0f,0.0f,0.55f*a);				// translucent fill
	FillRoundRect2D(x0,y0,x1,y1,rad);
	(*orig_glColor4f)(1.0f,0.9f,0.4f,a);					// yellow border to match the text
	(*orig_glLineWidth)(2.0f*ui_scale);						// same stroke width as the hack menu
	DrawBox2D(x0,y0,x1,y1,rad);

	float ta=gTextAlpha; gTextAlpha=a;
	DrawText(cx-tw*0.5f, cyc+charH*0.5f, 1.0f,0.9f,0.4f, "%s", toast_msg);	// centered
	gTextAlpha=ta;

	(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPopMatrix)();
	(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPopMatrix)();
	(*orig_glPopAttrib)();
}

void sys_wglSwapBuffers(HDC hDC)
{
	if(hookactive)
	{
		DrawEngineEsp();	// radar + engine ESP + own HUD (bottom overlay layer)
		DrawToast();		// feature toggle notifications (middle layer)
		DrawOverlayUI();	// hack menu + F11 check (top overlay layer)
	}
	viewportcount=0;		// reset viewport count, cuz this is the last function called every frame
	(*orig_wglSwapBuffers) (hDC);
}

BOOL __stdcall DllMain (HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls (hOriginalDll);
			return Init();

		case DLL_PROCESS_DETACH:
			if ( hOriginalDll != NULL )
			{
				FreeLibrary(hOriginalDll);
				hOriginalDll = NULL;
			}
			break;
	}
	return TRUE;
}
