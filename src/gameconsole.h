/*
gameconsole.h

Copyright (C) 2012-2012 gbm

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef __GAMECONSOLE_H__
#define __GAMECONSOLE_H__

#define		MAX_CONSOLE_NUM				5	// 対応ゲーム機台数
#define		MAX_CONSOLE_NAME_SIZE		128	// ハード名サイズ

#define		CONSOLE_ERROR				-1

#define		CONSOLE_COLOR_COLOR			0
#define		CONSOLE_COLOR_MONOCHROME	1

// 各ゲーム機の番号
#define		CONSOLE_GAMEBOYCOLOR		0
#define		CONSOLE_NEOGEOPOCKETCOLOR	1
#define		CONSOLE_SWANCRYSTAL			2
#define		CONSOLE_ORIGINALGAMEBOY		3
#define		CONSOLE_PIECE				4

typedef struct consoledata{
	int		width;
	int		height;
	int		color;				// monochrome=1 color=0
	int		cbit;				// x bit color
	int		fps[4];
	char	name[MAX_CONSOLE_NAME_SIZE];
} CONSOLEDATA;

const static CONSOLEDATA consoledata[MAX_CONSOLE_NUM] = {
//	 w,		h,		color,					cbit,	fps[4],				name
	{160,	144,	CONSOLE_COLOR_COLOR,	6,		{60, 30, 20, 15},	"GAMEBOYCOLOR"},		// GBC
	{160,	152,	CONSOLE_COLOR_COLOR,	4,		{60, 30, 20, 15},	"NEOGEOPOCKETCOLOR"},	// NGPC
	{224,	144,	CONSOLE_COLOR_COLOR,	4,		{75, 60, 30, 15},	"SWANCRYSTAL"},			// SC
	{160,	144,	CONSOLE_COLOR_MONOCHROME,	2,	{60, 30, 20, 15},	"GAMEBOY"},				// GB
	{128,	88,		CONSOLE_COLOR_MONOCHROME,	2,	{60, 30, 20, 15},	"P/ECE"},				// P/ECE
};

class CCONSOLEGAMES
{
private:
	int m_console;

	int check_console(int console) {
		if ((console >= MAX_CONSOLE_NUM) || (console < 0)) return -1;
		return 0;
	}

public:
	CCONSOLEGAMES(int console = CONSOLE_GAMEBOYCOLOR) { if (SetConsole(console)) m_console = CONSOLE_GAMEBOYCOLOR; }

	int SetConsole(int console){
		if (check_console(console))	return CONSOLE_ERROR;
		m_console = console;
		return 0;
	}

	int GetWidth(int console = -1)	{
		if (check_console(console)) console = m_console;
		return consoledata[console].width;
	}
	int GetHeight(int console = -1)	{
		if (check_console(console)) console = m_console;
		return consoledata[console].height;
	}
	int	GetColor(int console = -1)	{
		if (check_console(console)) console = m_console;
		return consoledata[console].color;
	}
	int GetCbit(int console = -1)	{
		if (check_console(console)) console = m_console;
		return consoledata[console].cbit;
	}
	int GetFps(int fskip = 0, int console = -1)	{
		if (check_console(console)) console = m_console;
		if (fskip >= 0 && fskip <= 3) return consoledata[console].fps[fskip];
		else return consoledata[console].fps[0];
	}
	int	GetConsole()		{ return m_console; }

	int GetScreenSize(int console = -1)	{
		if (check_console(console)) console = m_console;
		return consoledata[console].width * consoledata[console].height * 3;
	}
	int GetCCapBufSize(int console = -1)	{
		if (check_console(console)) console = m_console;
		return consoledata[console].width * consoledata[console].height * 3 * 2;
	}
};
#endif