/*
 * Code to turn an existing mouse input device into a touchscreen of sorts
 *
 * Copyright 2005 Openedhand Ltd.
 *
 * Author: Richard Purdie <rpurdie@openedhand.com>
 *
 * Based on bits of evtext.c by Vojtech Pavlik and kbdd by Nils Faerber
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "input.h"
#include "tsemu.h"

#define X_AXIS_MAX		1200
#define X_AXIS_MIN		0
#define Y_AXIS_MAX		1600
#define Y_AXIS_MIN		0
#define PRESSURE_MIN		0
#define PRESSURE_MAX		15000


char *events[EV_MAX + 1] = { "Reset", "Key", "Relative", "Absolute", "MSC", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, "LED", "Sound", NULL, "Repeat", "ForceFeedback", NULL, "ForceFeedbackStatus"};
char *keys[KEY_MAX + 1] = { "Reserved", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "Minus", "Equal", "Backspace",
"Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "LeftBrace", "RightBrace", "Enter", "LeftControl", "A", "S", "D", "F", "G",
"H", "J", "K", "L", "Semicolon", "Apostrophe", "Grave", "LeftShift", "BackSlash", "Z", "X", "C", "V", "B", "N", "M", "Comma", "Dot",
"Slash", "RightShift", "KPAsterisk", "LeftAlt", "Space", "CapsLock", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
"NumLock", "ScrollLock", "KP7", "KP8", "KP9", "KPMinus", "KP4", "KP5", "KP6", "KPPlus", "KP1", "KP2", "KP3", "KP0", "KPDot", "103rd",
"F13", "102nd", "F11", "F12", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "KPEnter", "RightCtrl", "KPSlash", "SysRq",
"RightAlt", "LineFeed", "Home", "Up", "PageUp", "Left", "Right", "End", "Down", "PageDown", "Insert", "Delete", "Macro", "Mute",
"VolumeDown", "VolumeUp", "Power", "KPEqual", "KPPlusMinus", "Pause", "F21", "F22", "F23", "F24", "KPComma", "LeftMeta", "RightMeta",
"Compose", "Stop", "Again", "Props", "Undo", "Front", "Copy", "Open", "Paste", "Find", "Cut", "Help", "Menu", "Calc", "Setup",
"Sleep", "WakeUp", "File", "SendFile", "DeleteFile", "X-fer", "Prog1", "Prog2", "WWW", "MSDOS", "Coffee", "Direction",
"CycleWindows", "Mail", "Bookmarks", "Computer", "Back", "Forward", "CloseCD", "EjectCD", "EjectCloseCD", "NextSong", "PlayPause",
"PreviousSong", "StopCD", "Record", "Rewind", "Phone", "ISOKey", "Config", "HomePage", "Refresh", "Exit", "Move", "Edit", "ScrollUp",
"ScrollDown", "KPLeftParenthesis", "KPRightParenthesis",
"International1", "International2", "International3", "International4", "International5",
"International6", "International7", "International8", "International9",
"Language1", "Language2", "Language3", "Language4", "Language5", "Language6", "Language7", "Language8", "Language9",
NULL, 
"PlayCD", "PauseCD", "Prog3", "Prog4", "Suspend", "Close",
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
"Btn0", "Btn1", "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8", "Btn9",
NULL, NULL,  NULL, NULL, NULL, NULL,
"LeftBtn", "RightBtn", "MiddleBtn", "SideBtn", "ExtraBtn", "ForwardBtn", "BackBtn",
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
"Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn",
"BaseBtn", "BaseBtn2", "BaseBtn3", "BaseBtn4", "BaseBtn5", "BaseBtn6",
NULL, NULL, NULL, "BtnDead",
"BtnA", "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ", "BtnTL", "BtnTR", "BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", "BtnMode",
"BtnThumbL", "BtnThumbR", NULL,
"ToolPen", "ToolRubber", "ToolBrush", "ToolPencil", "ToolAirbrush", "ToolFinger", "ToolMouse", "ToolLens", NULL, NULL,
"Touch", "Stylus", "Stylus2" };

char *absval[5] = { "Value", "Min  ", "Max  ", "Fuzz ", "Flat " };
char *relatives[REL_MAX + 1] = { "X", "Y", "Z", NULL, NULL, NULL, "HWheel", "Dial", "Wheel" };
char *absolutes[ABS_MAX + 1] = { "X", "Y", "Z", "Rx", "Ry", "Rz", "Throttle", "Rudder", "Wheel", "Gas", "Brake",
NULL, NULL, NULL, NULL, NULL,
"Hat0X", "Hat0Y", "Hat1X", "Hat1Y", "Hat2X", "Hat2Y", "Hat3X", "Hat 3Y", "Pressure", "Distance", "XTilt", "YTilt", "Misc"};
char *leds[LED_MAX + 1] = { "NumLock", "CapsLock", "ScrollLock", "Compose", "Kana", "Sleep", "Suspend", "Mute" };
char *repeats[REP_MAX + 1] = { "Delay", "Period" };
char *sounds[SND_MAX + 1] = { "Bell", "Click" };

char **names[EV_MAX + 1] = { events, keys, relatives, absolutes, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, leds, sounds, NULL, repeats, NULL, NULL, NULL };

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

int dev_uinput_init(void)
{
	struct uinput_dev dev;
	int fd, aux;

	fd = open("/dev/uinput", O_RDWR);
	if (fd < 0)
		fd = open("/dev/misc/uinput", O_RDWR);
	if (fd < 0)
		fd = open("/dev/devfs/misc/uinput", O_RDWR);
	if (fd < 0)
		fd = open("/dev/input/uinput", O_RDWR);
	if (fd < 0) {
		perror("failed to open uinput device");
		return -1;
	}

	memset(&dev, 0, sizeof(dev));
	strncpy(dev.name, "EmuTS", UINPUT_MAX_NAME_SIZE);
	dev.idbus = BUS_HOST;
	dev.idvendor = 0x0001;
	dev.idproduct = 0x0002;
	dev.idversion = 0x0100;
	dev.absmax[ABS_X] = X_AXIS_MAX;
	dev.absmin[ABS_X] = X_AXIS_MIN;
	dev.absmax[ABS_Y] = Y_AXIS_MAX;
	dev.absmin[ABS_Y] = Y_AXIS_MIN;
	dev.absmax[ABS_PRESSURE] = PRESSURE_MAX;
	dev.absmin[ABS_PRESSURE] = PRESSURE_MIN;

	if (write(fd, &dev, sizeof(dev)) < 0) {
		fprintf(stderr,"failed to write uinputdev: %s\n", strerror(errno));
		close(fd);
		return -1;
	}

	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) != 0) {
		close(fd);
		return -1;
	}

	if (ioctl(fd, UI_SET_EVBIT, EV_ABS) != 0) {
		close(fd);
		return -1;
	}

	if (ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH) != 0) {
		close(fd);
		return -1;
	}

	if (ioctl(fd, UI_SET_ABSBIT, ABS_X) != 0) {
		close(fd);
		return -1;
	}


	if (ioctl(fd, UI_SET_ABSBIT, ABS_Y) != 0) {
		close(fd);
		return -1;
	}

	if (ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE) != 0) {
		close(fd);
		return -1;
	}


	if (ioctl(fd, UI_DEV_CREATE) != 0) {
		close(fd);
		return -1;
	}

	return fd;
}

int dev_uinput_key(int fd, unsigned short code, int pressed, struct timeval *time)
{
	struct uinput_event event;

	memset(&event, 0, sizeof(event));
	event.time.tv_sec=time->tv_sec;
	event.time.tv_usec=time->tv_usec;
	event.type = EV_KEY;
	event.code = code;
	event.value = !!pressed;

	return (write(fd, &event, sizeof(event)));
}

int dev_uinput_abs(int fd, unsigned short code, int value, struct timeval *time)
{
	struct uinput_event event;

	memset(&event, 0, sizeof(event));
	event.time.tv_sec=time->tv_sec;
	event.time.tv_usec=time->tv_usec;
	event.type = EV_ABS;
	event.code = code;
	event.value = value;

	return (write(fd, &event, sizeof(event)));
}

int dev_uinput_sync(int fd, struct timeval *time)
{
	struct uinput_event event;

	memset(&event, 0, sizeof(event));
	event.time.tv_sec=time->tv_sec;
	event.time.tv_usec=time->tv_usec;
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;

	return (write(fd, &event, sizeof(event)));
}

void dev_uinput_close(int fd)
{
	ioctl(fd, UI_DEV_DESTROY);
	close(fd);
}

int main (int argc, char **argv)
{
	int fd, rd, i, j, k;
	struct input_event ev[64];
	int version;
	unsigned short id[4];
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	char name[256] = "Unknown";
	int abs[5];
	int uindev;
	int stroke_active=0;
	signed long xpos=0, ypos=0;

	if (argc < 2) {
		printf ("Usage: tsemnu /dev/inputX\n");
		printf ("Where X = mouse input device to emulate ts with\n");
		exit (1);
	}

	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("evtest");
		exit(1);
	}

	/*ioctl(fd, EVIOCGVERSION, &version);
	printf("Input driver version is %d.%d.%d\n",
		version >> 16, (version >> 8) & 0xff, version & 0xff);*/

	/*ioctl(fd, EVIOCGID, id);
	printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
		id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]); */

	/*ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	printf("Input device name: \"%s\"\n", name);*/

	/*memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
	printf("Supported events:\n");*/

	/*for (i = 0; i < EV_MAX; i++)
	{
		if (test_bit(i, bit[0])) 
		{
			printf("    Event type %d (%s)\n", i, events[i] ? events[i] : "?");
			ioctl(fd, EVIOCGBIT(i, KEY_MAX), bit[i]);
			for (j = 0; j < KEY_MAX; j++) 
			{
				if (test_bit(j, bit[i])) 
				{
					printf("    Event code %d (%s)\n", j, names[i] ? (names[i][j] ? names[i][j] : "?") : "?");
					if (i == EV_ABS) 
					{
						ioctl(fd, EVIOCGABS(j), abs);
						for (k = 0; k < 5; k++)
						{
							if ((k < 3) || abs[k])
							{
								printf("    %s %6d\n", absval[k], abs[k]);
							}
						}
					}
				}
			}
		}
	}*/	


	uindev = dev_uinput_init();


	while (1) 
	{
		int pressed=0;
		rd = read(fd, ev, sizeof(struct input_event) * 64);

		if (rd < (int) sizeof(struct input_event)) 
		{
			printf("yyy\n");
			perror("\nevtest: error reading");
			exit (1);
		}

		for (i = 0; i < rd / sizeof(struct input_event); i++)
		{
			if ((ev[i].type == 1) && (ev[i].code == 272) && (ev[i].value == 1))
				pressed = 1;
			if ((ev[i].type == 1) && (ev[i].code == 272) && (ev[i].value == 0))
				pressed = 2;

			if ((ev[i].type == 2) && (ev[i].code == 0)) 
				xpos += ev[i].value;
			if ((ev[i].type == 2) && (ev[i].code == 1))
				ypos += ev[i].value;

			if (xpos < X_AXIS_MIN)
				xpos=X_AXIS_MIN;
			if (xpos > X_AXIS_MAX)
				xpos=X_AXIS_MAX;
			if (ypos < Y_AXIS_MIN)
				ypos=Y_AXIS_MIN;
			if (ypos > Y_AXIS_MAX)
				ypos=Y_AXIS_MAX;

			if (stroke_active || (pressed == 1) ) {
				stroke_active=1;
				dev_uinput_abs(uindev, ABS_X, xpos, &ev[i].time);
				dev_uinput_abs(uindev, ABS_Y, ypos, &ev[i].time);
				dev_uinput_abs(uindev, ABS_PRESSURE, 10000, &ev[i].time);
				dev_uinput_key(uindev, BTN_TOUCH, 1, &ev[i].time);
				dev_uinput_sync(uindev, &ev[i].time);
			}
			
			if ((pressed == 2) && stroke_active) {
				stroke_active=0;
				dev_uinput_abs(uindev, ABS_X, xpos, &ev[i].time);
				dev_uinput_abs(uindev, ABS_Y, ypos, &ev[i].time);
				dev_uinput_abs(uindev, ABS_PRESSURE, 0, &ev[i].time);
				dev_uinput_key(uindev, BTN_TOUCH, 0, &ev[i].time);
				dev_uinput_sync(uindev, &ev[i].time);
			}
/*			printf("Event: time %ld.%06ld, type %d (%s), code %d (%s), value %d\n",
				ev[i].time.tv_sec, 
				ev[i].time.tv_usec, 
				ev[i].type,
				events[ev[i].type] ? events[ev[i].type] : "?",
				ev[i].code,
				names[ev[i].type] ? (names[ev[i].type][ev[i].code] ? names[ev[i].type][ev[i].code] : "?") : "?",
				ev[i].value);
*/
		}
	}

	dev_uinput_close(uindev);
}
