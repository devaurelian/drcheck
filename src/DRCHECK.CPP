/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Disk Routine Check Version 0.1 (pre-official release)                 */
/*  Disk DOS-level surface scanner                                        */
/*                                                                        */
/*  DRCHECK.CPP: Program source code                                      */
/*                                                                        */
/*  Created in May 1997                                                   */
/*  Copyright (c) 1997 Devliber (https://devliber.com)                    */
/*                                                                        */
/*  Written and compiled in Borland C++ 3.1.                              */
/*                                                                        */
/*  Licensed under the MIT License. See LICENSE file in the project       */
/*  root for full license information.                                    */
/*                                                                        */
/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Revision history:                                                     */
/*  0.1     May 1997    Initial coding.                                   */
/*                                                                        */
/*------------------------------------------------------------------------*/

#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <direct.h>

#include "fancytd.h"

void biosSetBlink(unsigned char blink)
{
    asm     push    bp
    asm     mov     ax, 0x1003
    asm     mov     bl, blink
    asm     int     0x10
    asm     pop     bp
}

char buf[8192];
int logx = 1, logy = 1;

int main(void)
{
    struct fatinfo diskinfo;

    printf("Disk Routine Check. Version 0.1 (pre-official release). May 29 1997\n");
    printf("Copyright (c) 1997 Devliber\n\n");
    if (_argc < 2)
    {
        puts("Usage is: DRCHECK <drive>");
        return 0;
    }

    int drive;
    if (_argv[1][1] != ':' || _argv[1][2])
    {
        printf("Invalid drive specification - %s\n", _argv[1]);
        return (-1);
    }

    drive = toupper(_argv[1][0]) - 'A' + 1;
    int startdrive = _getdrive();
    if (_chdrive(drive))
    {
        printf("Cannot access drive %s\n\n", _argv[1]);
        return (-1);
    }
    getfat(drive, &diskinfo); /* get drive information */

    textmode(C80);
    directvideo = 1;                    // use Direct Video by default, for speed
    biosSetBlink(0);                    // disable blinking (use light background colors)
    _setcursortype(_NOCURSOR);          // hide cursor
    win(1, 1, 80, 25, MAKEATTR(8, 15)); // clear screen

    win(1, 1, 80, 1, MAKEATTR(9, 15));
    ccputs("Disk Routine Check", 1);
    win(1, 2, 80, 2, MAKEATTR(15, 0));
    cputs(" Version 0.1 ³ Copyright (c) 1997 Devliber");

    win(1, 4, 80, 4, MAKEATTR(9, 15));
    cprintf(" Scanning drive %s (%u clusters)",
            _argv[1], diskinfo.fi_nclus);

    win(1, 25, 80, 25, MAKEATTR(15, 0));
    cputs(" Press any key to interrupt scan");
    win(2, 7, 79, 7, MAKEATTR(9, 15));
    ccputs("Error log", 1);
    win(2, 8, 79, 23, MAKEATTR(0, 15));
    win(1, 5, 80, 5, MAKEATTR(15, 0));

    for (unsigned j = 0; j < diskinfo.fi_nclus; j++)
    {
        gotoxy(1, 1);
        cprintf(" %2.0lf%% Cluster %u", (double)j * 100 / (diskinfo.fi_nclus), j);
        if (absread(drive - 1, diskinfo.fi_sclus, (long)j * diskinfo.fi_sclus, &buf))
        {
            window(1, 8, 80, 25);
            gotoxy(logx, logy);
            cprintf("Error encountered at cluster %u\n", j);
            logx = wherex();
            logy = wherey();
            window(1, 5, 80, 5);
        }
        if (kbhit())
            break;
    }

    _setcursortype(_NORMALCURSOR); // hide cursor
    _chdrive(startdrive);
    win(1, 1, 80, 25, MAKEATTR(0, 7));
    return 0;
}