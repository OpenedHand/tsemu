/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define UINPUT_MAX_NAME_SIZE	80


struct uinput_dev {
	char name[UINPUT_MAX_NAME_SIZE];
	unsigned short idbus;
	unsigned short idvendor;
	unsigned short idproduct;
	unsigned short idversion;
	int ff_effects_max;
	int absmax[ABS_MAX + 1];
	int absmin[ABS_MAX + 1];
	int absfuzz[ABS_MAX + 1];
	int absflat[ABS_MAX + 1];
};

/* User input interface */

#define UI_DEV_CREATE		_IO('U', 1)
#define UI_DEV_DESTROY		_IO('U', 2)
#define UI_SET_EVBIT		_IOW('U', 100, int)
#define UI_SET_KEYBIT		_IOW('U', 101, int)
#define UI_SET_RELBIT		_IOW('U', 102, int)
#define UI_SET_ABSBIT		_IOW('U', 103, int)
#define UI_SET_MSCBIT		_IOW('U', 104, int)
#define UI_SET_LEDBIT		_IOW('U', 105, int)
#define UI_SET_SNDBIT		_IOW('U', 106, int)
#define UI_SET_FFBIT		_IOW('U', 107, int)

struct uinput_event {
	struct timeval time;
	unsigned short type;
	unsigned short code;
	unsigned int value;
};

