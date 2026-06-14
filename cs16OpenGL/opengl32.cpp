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
					sscanf(str, "aimthru %i;"	,&cvar.aimthru);
					sscanf(str, "target %i;"	,&cvar.target);
					sscanf(str, "recoil %i;"	,&cvar.recoil);
					sscanf(str, "esp %i;"		,&cvar.esp);
					sscanf(str, "esp_box %i;"	,&cvar.esp_box);
					sscanf(str, "esp_dist %i;"	,&cvar.esp_dist);
					sscanf(str, "esp_line %i;"	,&cvar.esp_line);
					sscanf(str, "esp_engine %i;",&cvar.esp_engine);
					sscanf(str, "lambert %i;"	,&cvar.lambert);
					sscanf(str, "crosshair %i;"	,&cvar.cross);
					sscanf(str, "fov %i;"		,&cvar.fov);
					sscanf(str, "wall %i;"		,&cvar.wall);
					sscanf(str, "nosky %i;"		,&cvar.sky);
					sscanf(str, "noflash %i;"	,&cvar.flash);
					sscanf(str, "nosmoke %i;"	,&cvar.smoke);
					sscanf(str, "menu_x %i;"	,&cvar.menu_x);
					sscanf(str, "menu_y %i;"	,&cvar.menu_y);
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
void HookInit(bool activate)
{
	if(activate)					// if hack is activated once:
	{
		UnvalidVertex();			// turn all vert counts invalid
		LoadFile("oglconf.cfg",0);	// read cvar settings and modelfile
		LoadFile(modelfile,1);		// read modelfile, store all verts and team name
		CountOffset();				// count number of custom offsets
		SetOffsetNames();			// set the names
		SetOffset(curoffset);		// set start offset
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
	}
	else if(!activate) // hack turned off, set all things to 0 (not activated)
	{
		cvar.aim=0;
		cvar.aimthru=0;
		cvar.esp=0;
		cvar.esp_box=0;
		cvar.esp_dist=0;
		cvar.esp_line=0;
		cvar.esp_engine=0;
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
			}
			break;
		case VK_INSERT:
			if(!keyp.active)
			{
				keyp.active=true;
				menu.active=!menu.active;
			}
			break;
		case VK_UP:
			if(menu.active && !keyp.up)
			{
				keyp.up=true;
				menu.count-=1;
			}
			break;
		case VK_DOWN:
			if(menu.active && !keyp.down)
			{
				keyp.down=true;
				menu.count+=1;
			}
			break;
		case VK_RIGHT:
			if(menu.active && !keyp.right)
			{
				keyp.right=true;
				menu.select=true;
				menu.select_r=true;
			}
			break;
		case VK_LEFT:
			if(menu.active && !keyp.left)
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
	base = (*orig_glGenLists)(96);								
	font = CreateFont(-10,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_DONTCARE|DEFAULT_PITCH,
	"Verdana");
	oldfont = (HFONT)SelectObject(hDC, font);           
	wglUseFontBitmaps(hDC, 32, 96, base);				
	SelectObject(hDC, oldfont);							
	DeleteObject(font);									
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
	(*orig_glColor4f)(0.0f,0.0f,0.0f,1.0f);
	(*orig_glRasterPos2f)(x+1,y+1);

	//glPrint(text) - shadow
	(*orig_glPushAttrib)(GL_LIST_BIT);							
	(*orig_glListBase)(base - 32);								
	(*orig_glCallLists)(strlen(text), GL_UNSIGNED_BYTE, text);	
	(*orig_glPopAttrib)();										
	(*orig_glEnable)(GL_TEXTURE_2D); 

	(*orig_glDisable)(GL_TEXTURE_2D); 
	(*orig_glColor4f)(r,g,b,1.0f);
	(*orig_glRasterPos2f)(x,y);
	(*orig_glColor4f)(r,g,b,1.0f);

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

void DrawMenu(int x, int y)  // maybe a struct would have been easier but when i started it just had 4 menu options :P
{
    SYSTEMTIME SysTime;
    GetLocalTime(&SysTime);
	char *timestring = "Now the time is: %02d:%02d:%02d";

	DrawText(x,y-39,1.0f,1.0f,1.0f,timestring,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
	DrawText(x,y-28,0.7f,0.7f,1.0f,"--------------------------");
	DrawText(x,y-17,0.7f,0.7f,1.0f,"-       panzerGL 2.2        -");
	DrawText(x,y-6,0.7f,0.7f,1.0f,"-       James34602        -");
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

	if(menu.count>19) { menu.count=0; }
	else if(menu.count<0) { menu.count=19; }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void DrawCheckText(int x,int y) // bad way of doing this
{
	DrawText(x,y,0.7f,0.7f,1.0f,"panzerGL 2.2 MultiMod Hack - Check");
	y=y+13;
	DrawText(x,y,1.0f,1.0f,1.0f,"> Hack file: %s",dllpath);
	y=y+13;
	if(cfgfail)
		DrawText(x,y,1.0f,0.5f,0.5f,"> Could not load config file: <%s> !!!",configpath);
	else
		DrawText(x,y,1.0f,1.0f,1.0f,"> Config file: %s",configpath);
	y=y+13;
	if(mdlfail)
		DrawText(x,y,1.0f,0.5f,0.5f,"> Could not load model file: <%s> !!!",modelpath);
	else
		DrawText(x,y,1.0f,1.0f,1.0f,"> Model file: %s",modelpath);
	y=y+13;
	DrawText(x,y,0.5f,1.0f,0.5f,"> Your screen resolution is: %ix%i",vp[2],vp[3]);
	y=y+26;
	DrawText(x,y,0.7f,0.7f,1.0f,"%i valid offsets found:",offsetcount);
	y=y+13;
	for (int o=0;o<offsetcount;o++)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> [%i] %s| Stand: %0.2f | Duck: %0.2f",o+1,offset[o].name,offset[o].s,offset[o].d);
		y=y+13;
	}
	y=y+13;
	DrawText(x,y,0.7f,0.7f,1.0f,"Team 0: %s",team[0].name);
	y=y+13;
	if(team[0].vert01!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert01);
		y=y+13;
	}
	if(team[0].vert02!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert02);
		y=y+13;
	}
	if(team[0].vert03!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert03);
		y=y+13;
	}
	if(team[0].vert04!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert04);
		y=y+13;
	}
	if(team[0].vert05!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert05);
		y=y+13;
	}
	if(team[0].vert06!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert06);
		y=y+13;
	}
	if(team[0].vert07!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert07);
		y=y+13;
	}
	if(team[0].vert08!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert08);
		y=y+13;
	}
	if(team[0].vert09!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert09);
		y=y+13;
	}
	if(team[0].vert10!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert10);
		y=y+13;
	}
	if(team[0].vert11!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert11);
		y=y+13;
	}
	if(team[0].vert12!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[0].vert12);
		y=y+13;
	}
	//
	DrawText(x,y,0.7f,0.7f,1.0f,"Team 1: %s",team[1].name);
	y=y+13;
	if(team[1].vert01!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert01);
		y=y+13;
	}
	if(team[1].vert02!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert02);
		y=y+13;
	}
	if(team[1].vert03!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert03);
		y=y+13;
	}
	if(team[1].vert04!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert04);
		y=y+13;
	}
	if(team[1].vert05!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert05);
		y=y+13;
	}
	if(team[1].vert06!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert06);
		y=y+13;
	}
	if(team[1].vert07!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert07);
		y=y+13;
	}
	if(team[1].vert08!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert08);
		y=y+13;
	}
	if(team[1].vert09!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert09);
		y=y+13;
	}
	if(team[1].vert10!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert10);
		y=y+13;
	}
	if(team[1].vert11!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert11);
		y=y+13;
	}
	if(team[1].vert12!=-1)
	{
		DrawText(x,y,1.0f,1.0f,1.0f,"> vertice %i",team[1].vert12);
		y=y+13;
	}
	y=y+13;
	DrawText(x,y,1.0f,1.0f,1.0f,"> lowest vertex count: %i",player_vertex_min+5);
	y=y+13;
	DrawText(x,y,1.0f,1.0f,1.0f,"> highest vertex count %i",player_vertex_max-5);
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
int IsPointVisible(GLdouble x, GLdouble y, GLdouble z) // if aimthru is off, check for walls
{	
	GLdouble	winX, winY, winZ;
	GLfloat		pix;

	(*orig_glGetDoublev)(GL_MODELVIEW_MATRIX,mm);
	(*orig_glGetDoublev)(GL_PROJECTION_MATRIX ,pm);

	if (gluProject (x, y, z, mm, pm, vp, &winX, &winY, &winZ) == GL_TRUE)
	{
		(*orig_glReadPixels)((int)winX,(int)winY,1,1,GL_DEPTH_COMPONENT ,GL_FLOAT,&pix);
		if (pix>winZ) 
			return 1;
		else 
			return 0;
	}
	else
		return 0;
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
#define ENG_SLOT_GETLOCALPLAYER		51
#define ENG_SLOT_GETENTITYBYINDEX	53
#define ENG_SLOT_PTRIAPI			82

#define ENT_INDEX			0x000	// cl_entity_t: int  entity index
#define ENT_PLAYER			0x004	// cl_entity_t: int  "is player" flag
#define ENT_CURSTATE		0x2B0	// cl_entity_t: entity_state_t curstate
#define ENT_ORIGIN			0xB48	// cl_entity_t: vec3 interpolated origin
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

// Draw ESP for every player in the engine entity list. Called each frame from
// the wglSwapBuffers hook, in its own 2D pixel-space pass.
void DrawEngineEsp()
{
	eng_players=0;
	if(!cvar.esp_engine) return;

	GLint vpe[4];
	(*orig_glGetIntegerv)(GL_VIEWPORT,vpe);
	float sw=(float)vpe[2], sh=(float)vpe[3];
	if(sw<=0||sh<=0) return;

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
	if(!ready || fnLocal<0x10000 || fnEnt<0x10000)
	{
		DrawText(8.0f,16.0f,1.0f,0.7f,0.2f,"ENGINE ESP: searching engine table (start a game)...");
		(*orig_glMatrixMode)(GL_PROJECTION); (*orig_glPopMatrix)();
		(*orig_glMatrixMode)(GL_MODELVIEW);  (*orig_glPopMatrix)();
		(*orig_glPopAttrib)();
		return;
	}

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

	for(int idx=1; idx<=32; idx++)
	{
		if(idx==eng_local_idx) continue;

		char namebuf[64]="";
		if(fnInfo>=0x10000)
		{
			hud_player_info_t info; memset(&info,0,sizeof(info));
			((eng_GetPlayerInfo_t)fnInfo)(idx,&info);
			if(info.name==0 || !IsReadable((DWORD)info.name,1)) continue;	// empty slot
			if(info.spectator) continue;
			strncpy(namebuf,info.name,63); namebuf[63]=0;
		}

		if(EngDead(idx)) continue;

		DWORD ent=(DWORD)((eng_GetEntityByIndex_t)fnEnt)(idx);
		if(!ent || ReadInt(ent+ENT_PLAYER)==0) continue;

		float o[3];
		o[0]=ReadFlt(ent+ENT_ORIGIN); o[1]=ReadFlt(ent+ENT_ORIGIN+4); o[2]=ReadFlt(ent+ENT_ORIGIN+8);
		if(o[0]==0&&o[1]==0&&o[2]==0)	// fallback: entity_state origin
		{
			DWORD cs=ent+ENT_CURSTATE;
			o[0]=ReadFlt(cs+ES_ORIGIN); o[1]=ReadFlt(cs+ES_ORIGIN+4); o[2]=ReadFlt(cs+ES_ORIGIN+8);
			if(o[0]==0&&o[1]==0&&o[2]==0) continue;
		}

		int usehull=ReadInt(ent+ENT_CURSTATE+ES_USEHULL);
		float halfh=(usehull==1)?18.0f:36.0f;	// duck vs stand half-height (units)
		float zoff =(usehull==1)?6.0f :0.0f;

		float feet[3]={o[0],o[1],o[2]-halfh+zoff};
		float head[3]={o[0],o[1],o[2]+halfh+zoff};
		float sfeet[3],shead[3];
		if(!EngWorldToScreen(feet,sfeet)) continue;
		if(!EngWorldToScreen(head,shead)) continue;

		float fx=(sfeet[0]*0.5f+0.5f)*sw, fy=sh-(sfeet[1]*0.5f+0.5f)*sh;
		float hx=(shead[0]*0.5f+0.5f)*sw, hy=sh-(shead[1]*0.5f+0.5f)*sh;

		float y0=(hy<fy)?hy:fy, y1=(hy<fy)?fy:hy;	// top / bottom
		float bxh=y1-y0; if(bxh<4.0f) bxh=4.0f;
		float bxw=bxh*0.45f;
		float cx=(fx+hx)*0.5f;
		float x0=cx-bxw*0.5f, x1=cx+bxw*0.5f;

		int team=EngTeam(idx);					// team color
		float r,g,b;
		if(team==1)      { r=1.0f; g=0.25f; b=0.25f; }	// T  = red
		else if(team==2) { r=0.25f;g=0.55f; b=1.0f;  }	// CT = blue
		else             { r=0.25f;g=1.0f;  b=0.25f; }	// unknown = green

		(*orig_glLineWidth)(1.5f);
		(*orig_glColor3f)(r,g,b);
		(*orig_glBegin)(GL_LINE_LOOP);
		(*orig_glVertex2f)(x0,y0); (*orig_glVertex2f)(x1,y0);
		(*orig_glVertex2f)(x1,y1); (*orig_glVertex2f)(x0,y1);
		(*orig_glEnd)();

		if(cvar.esp_line)
		{
			(*orig_glLineWidth)(1.0f);
			(*orig_glBegin)(GL_LINES);
			(*orig_glVertex2f)(sw*0.5f,sh);
			(*orig_glVertex2f)(cx,y1);
			(*orig_glEnd)();
		}

		DrawText(cx-(float)strlen(namebuf)*4.0f, y0-14.0f, r,g,b, "%s", namebuf);

		float dx=lo[0]-o[0], dy=lo[1]-o[1], dz=lo[2]-o[2];
		float dist=(float)sqrt(dx*dx+dy*dy+dz*dz)/39.37f;	// units -> meters
		DrawText(cx-12.0f, y1+2.0f, 1.0f,1.0f,1.0f, "%.0fm", dist);

		eng_players++;
	}

	DrawText(8.0f,16.0f,0.2f,1.0f,0.4f,"ENGINE ESP: %i players  team=%s",
		eng_players, eng_have_extra?"on":"off");

	(*orig_glMatrixMode)(GL_PROJECTION);
	(*orig_glPopMatrix)();
	(*orig_glMatrixMode)(GL_MODELVIEW);
	(*orig_glPopMatrix)();
	(*orig_glPopAttrib)();
}

// x,y  = projected head point in NDC (-1..1)
// fx,fy = projected feet point in NDC (-1..1)
// dist  = distance to player (raster distance units)
bool DrawPlayerEsp(float x,float y,float fx,float fy,double dist,long vertcount)
{
	GLint cm;
	GLfloat color[4];

	bool isT1 = IsVertTeam(vertcount,1);
	bool isT0 = IsVertTeam(vertcount,0);
	if(!isT0 && !isT1)
		return false;

	// team color: team1 = blue, team0 = red
	float r = isT1 ? 0.0f : 1.0f;
	float g = 0.0f;
	float b = isT1 ? 1.0f : 0.0f;

	(*orig_glGetIntegerv)(GL_MATRIX_MODE,&cm);
	(*orig_glGetDoublev)(GL_MODELVIEW_MATRIX,mm);
	(*orig_glGetDoublev)(GL_PROJECTION_MATRIX ,pm);
	(*orig_glEnable)(GL_POINT_SMOOTH);
	(*orig_glDisable)(GL_TEXTURE_2D);
	(*orig_glMatrixMode)(GL_PROJECTION);
	(*orig_glPushMatrix)();
	(*orig_glLoadIdentity)();
	(*orig_glMatrixMode)(GL_MODELVIEW);
	(*orig_glPushMatrix)();
	(*orig_glLoadIdentity)();
	(*orig_glGetFloatv)(GL_CURRENT_COLOR, color);

	float cx  = (x + fx) * 0.5f;	// horizontal center of the model
	float top = (y > fy) ? y  : fy;	// head (higher on screen)
	float bot = (y > fy) ? fy : y;	// feet

	// --- original ESP: head triangle + target rectangle ---
	if(cvar.esp)
	{
		if( (isT1 && cvar.target==1) || (isT0 && cvar.target==0) )	// target marker
		{
			(*orig_glLineWidth)(2.0f);
			(*orig_glBegin)(GL_LINE_STRIP);
			(*orig_glColor3f)(r,g,b);
			(*orig_glVertex2f)(x-0.015,y-0.02);
			(*orig_glVertex2f)(x+0.015,y-0.02);
			(*orig_glVertex2f)(x+0.015,y-0.05);
			(*orig_glVertex2f)(x-0.015,y-0.05);
			(*orig_glVertex2f)(x-0.015,y-0.02);
			(*orig_glEnd)();
		}
		(*orig_glBegin)(GL_TRIANGLES);
		(*orig_glColor3f)(r,g,b);
		(*orig_glVertex2f)(x    ,y+0.06);
		(*orig_glVertex2f)(x-0.02,y+0.1);
		(*orig_glVertex2f)(x+0.02,y+0.1);
		(*orig_glEnd)();
	}

	// --- tier1: 2D bounding box (width derived from on-screen height) ---
	if(cvar.esp_box)
	{
		float h  = top - bot;
		float hw = h * 0.22f;		// human-ish aspect ratio
		if(hw < 0.008f) hw = 0.008f;
		(*orig_glLineWidth)(1.5f);
		(*orig_glColor3f)(r,g,b);
		(*orig_glBegin)(GL_LINE_LOOP);
		(*orig_glVertex2f)(cx-hw, top+0.02f);
		(*orig_glVertex2f)(cx+hw, top+0.02f);
		(*orig_glVertex2f)(cx+hw, bot);
		(*orig_glVertex2f)(cx-hw, bot);
		(*orig_glEnd)();
	}

	// --- tier1: snapline from bottom-center of screen to the feet ---
	if(cvar.esp_line)
	{
		(*orig_glLineWidth)(1.0f);
		(*orig_glColor3f)(r,g,b);
		(*orig_glBegin)(GL_LINES);
		(*orig_glVertex2f)(0.0f,-1.0f);
		(*orig_glVertex2f)(cx, bot);
		(*orig_glEnd)();
	}

	// --- tier1: distance text (raster pos uses the identity matrices set above) ---
	if(cvar.esp_dist)
		DrawText(cx-0.02f, top+0.05f, 1.0f,1.0f,1.0f, "%.0f", dist);

	(*orig_glMatrixMode)(GL_PROJECTION);
	(*orig_glPopMatrix)();   
	(*orig_glMatrixMode)(GL_MODELVIEW);
	(*orig_glPopMatrix)();
	(*orig_glMatrixMode)(cm);
	(*orig_glDisable)(GL_POINT_SMOOTH);
	(*orig_glEnable)(GL_TEXTURE_2D);
	(*orig_glColor4f)(color[0],color[1],color[2],color[3]);

	return true;
}

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

		if(menu.active)	{ DrawMenu(cvar.menu_x,cvar.menu_y); }	// draws menu
		if(checktext)	{ DrawCheckText(40,40); }				// draws check stuff (F11)
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
		player.get=false;

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

	GLdouble wx,wy,wz;
	GLdouble wx2,wy2,wz2;
	GLfloat     color[4];
	(*orig_glGetFloatv)(GL_CURRENT_COLOR, color);
	
	(*orig_glGetDoublev)(GL_MODELVIEW_MATRIX,mm);
	(*orig_glGetDoublev)(GL_PROJECTION_MATRIX ,pm);
	(*orig_glDisable)(GL_TEXTURE_2D);	
	
	//HL model drawing begins here
	if ((mode==GL_SMOOTH) && !(player.get))
	{
		player.get=true; 
		player.highest_z=-99999;
		player.lowest_z=-99999;
		player.vertices=0;
	}
	else
	{
		if (player.get 	&& (player.vertices>player_vertex_min) && (player.height<80)) 
		{
			if ((player.height>player_height_min)	&& (player.height<55))  // player ducked?
				roffset=(26.0f-cvar.duck_h)*(-1.0f);						// if so use duck offset
			else 
				roffset=(24.0f-cvar.stand_h)*(-1.0f);
			
			gluProject(player.highest_x,player.highest_y,player.highest_z+roffset,mm,pm,vp,&wx ,&wy ,&wz );
			gluProject(player.lowest_x ,player.lowest_y ,player.lowest_z         ,mm,pm,vp,&wx2,&wy2,&wz2);
			
			GLfloat rasdist[1];
			
			(*orig_glRasterPos3f)( player.highest_x,player.highest_y,player.highest_z+roffset);
			
			(*orig_glGetFloatv)(GL_CURRENT_RASTER_DISTANCE,rasdist);
			player.distance=fabs(rasdist[0]);

			if ( 
				(player.vertices==team[1].vert01) || // check if its one of the vert counts
				(player.vertices==team[1].vert02) || // so we know its a player model
				(player.vertices==team[1].vert03) ||
				(player.vertices==team[1].vert04) ||
				(player.vertices==team[1].vert05) ||
				(player.vertices==team[1].vert06) ||
				(player.vertices==team[1].vert07) ||
				(player.vertices==team[1].vert08) ||
				(player.vertices==team[1].vert09) ||
				(player.vertices==team[1].vert10) ||
				(player.vertices==team[1].vert11) ||
				(player.vertices==team[1].vert12) ||
				(player.vertices==team[0].vert01) ||
				(player.vertices==team[0].vert02) ||
				(player.vertices==team[0].vert03) ||
				(player.vertices==team[0].vert04) ||
				(player.vertices==team[0].vert05) ||
				(player.vertices==team[0].vert06) ||
				(player.vertices==team[0].vert07) ||
				(player.vertices==team[0].vert08) ||
				(player.vertices==team[0].vert09) ||
				(player.vertices==team[0].vert10) ||
				(player.vertices==team[0].vert11) ||
				(player.vertices==team[0].vert12)  )
			{
				if ((player.height<player_height_min)) // when pronefix is 1, its always false
					player.iscorpse=true;
				else
				{
					player.iscorpse=false;
					if(cvar.esp || cvar.esp_box || cvar.esp_dist || cvar.esp_line)	// if player is alive draw ESP
					{
						float px = (float)(wx/(vp[2]/2))-1;		// head (highest point)
						float py = (float)(wy/(vp[3]/2))-1;
						float pfx = (float)(wx2/(vp[2]/2))-1;	// feet (lowest point)
						float pfy = (float)(wy2/(vp[3]/2))-1;
						DrawPlayerEsp(px,py,pfx,pfy,player.distance,player.vertices);
					}
				}
			}

			float delx = ((float)((vp[2]/2)-(wx)));
			float dely = ((float)((vp[3]/2)-(vp[3]-wy)));
			float lastx2 = ((vp[2]/2)-((player.vector_x*vp[2])/65535));
			float lasty2 = ((vp[3]/2)-(((player.vector_y*vp[3])/65535)+vp[3]));

			if (aimat==-1 && player.height>player_height_min)
			{
				if ((player.vertices < player_vertex_max) && (player.vertices > player_vertex_min) && (wx>=vp[2]/2-cvar.fov/2) //dont display own model
					&& (wx<=vp[2]/2+cvar.fov/2) && (wy>=vp[3]/2-cvar.fov/2) && (wy<=vp[3]/2+cvar.fov/2))
				{	// if player is in scan area/field of view (fov) we check for his team
					if	((
						(player.vertices==team[0].vert01) ||
						(player.vertices==team[0].vert02) ||
						(player.vertices==team[0].vert03) ||
						(player.vertices==team[0].vert04) ||
						(player.vertices==team[0].vert05) ||
						(player.vertices==team[0].vert06) ||
						(player.vertices==team[0].vert07) ||
						(player.vertices==team[0].vert08) ||
						(player.vertices==team[0].vert09) ||
						(player.vertices==team[0].vert10) ||
						(player.vertices==team[0].vert11) ||
						(player.vertices==team[0].vert12)) &&
						(cvar.target==0)) // if its a player of team 0
					{
						if ((( delx+dely ) < ( lastx2+lasty2 )) || (player.vector_x + player.vector_y)==0)
						{
							//set and calculate dimensions for mouse_event
							player.vector_x=(float)((wx*65535)/vp[2]);
							player.vector_y=(float)(((vp[3]-wy)*65535)/vp[3]);
						}
						// targetpos for visibility pixels
						player.origin_x=player.highest_x;
						player.origin_y=player.highest_y;
						player.origin_z=player.highest_z+roffset;
						aimat=0; // current model/player is team 0
						
					}
					else if	(( // other way, if its a player of team 1
						(player.vertices==team[1].vert01) ||
						(player.vertices==team[1].vert02) ||
						(player.vertices==team[1].vert03) ||
						(player.vertices==team[1].vert04) ||
						(player.vertices==team[1].vert05) ||
						(player.vertices==team[1].vert06) ||
						(player.vertices==team[1].vert07) ||
						(player.vertices==team[1].vert08) ||
						(player.vertices==team[1].vert09) ||
						(player.vertices==team[1].vert10) ||
						(player.vertices==team[1].vert11) ||
						(player.vertices==team[1].vert12)) &&
						(cvar.target==1))
					{
						if ((( delx+dely ) < ( lastx2+lasty2 )) || (player.vector_x + player.vector_y)==0) 
						{
							//set and calculate dimensions for mouse_event
							player.vector_x=(float)((wx*65535)/vp[2]);
							player.vector_y=(float)(((vp[3]-wy)*65535)/vp[3]);
						}
						// targetpos for visibility pixels
						player.origin_x=player.highest_x;
						player.origin_y=player.highest_y;
						player.origin_z=player.highest_z+roffset;
						aimat=1; // yes its a model of team 1
					}
				}
			}
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
	if (player.get)
	{
		player.vertices++; // count all vertices
		//get highest point of entity
		if ( (z>player.highest_z) || (player.highest_z==-99999))
		{
			player.highest_x=x;
			player.highest_y=y;
			player.highest_z=z;
		}
		//get lowest point of entity
		if ( (z<player.lowest_z) || (player.lowest_z==-99999)) {
			player.lowest_x=x;
			player.lowest_y=y;
			player.lowest_z=z;
		}

		player.height=player.highest_z-player.lowest_z;
		
		(*orig_glEnable)(GL_TEXTURE_2D);
	}

	if (cvar.lambert)
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

	if (modelviewport)
	{					// if the player is in target team and one of the selected aim method is chosen ...
		if ((aimat==cvar.target) && (cvar.aim) && ((cvar.aimkey==0) || ((cvar.aimkey==1) && (GetAsyncKeyState(VK_LBUTTON))) || ((cvar.aimkey==2) && (GetAsyncKeyState(VK_RBUTTON))) || ((cvar.aimkey==3) && (GetAsyncKeyState(VK_MBUTTON)))))
		{
			if (!cvar.aimthru)	// ...and aimthru is off
			{					// we check if player is behind a wall
				if (IsPointVisible(player.origin_x+0,player.origin_y+0,player.origin_z+5) || IsPointVisible(player.origin_x+10,player.origin_y+10,player.origin_z+5) ||	IsPointVisible(player.origin_x+10,player.origin_y-10,player.origin_z+5) || IsPointVisible(player.origin_x-10,player.origin_y-10,player.origin_z-30) || IsPointVisible(player.origin_x+10,player.origin_y-10,player.origin_z-30) || IsPointVisible(player.origin_x+10,player.origin_y+10,player.origin_z-55) || IsPointVisible(player.origin_x-10,player.origin_y+10,player.origin_z-55))
				{	// move the mouse to the aim point
					mouse_event(MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE,player.vector_x,player.vector_y,0,0);
					HandleKey(VK_LBUTTON); // prevent recoil

					if (cvar.shoot)
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);	// press mouse1 key if autoshoot is on
						mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);	// and release key if autoshoot is on
					}
				}
			}
			else // same here without checking for wall (aimthru is enabled)
			{
				mouse_event(MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE,player.vector_x,player.vector_y,0,0);
				HandleKey(VK_LBUTTON);

				if (cvar.shoot)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
					mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
				}
			}
		}
		// reset to start again next frame
		player.highest_z=-99999;
		player.lowest_z=-99999;
		aimat = -1;
		player.vector_x=0;
		player.vector_y=0;
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

void sys_wglSwapBuffers(HDC hDC)
{
	if(hookactive)			// tier2: draw engine entity-list ESP as the last thing each frame
		DrawEngineEsp();
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
