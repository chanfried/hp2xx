/*
   Copyright (C) 1992 Norbert Meyer.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of HP2xx.

HP2xx is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the GNU General Public License, Version 2 or later, for full details.

Everyone is granted permission to copy, modify and redistribute
HP2xx, but only under the conditions described in the GNU General Public
License.  A copy of this license is supposed to have been
given to you along with HP2xx so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/**
 ** TO_ATARI.C:  Zweite Version f�r einen Atari-Previewer im
 **              Rahmen des HP2xx-Projektes von Heinz Werntges.
 **
 **              Die erste Version f�r einen Atari-Previewer
 **              wertete jeweils die tempor�re Datei aus, in der
 **              zun�chst in einem HP2xx-spezifischem Code alle
 **              Plot-Anweisungen gesammelt werden (Diese
 **              Zwischendatei wird von HP2xx benutzt, um
 **              anschlie�end daraus zum einen die HP2xx-Bitmap zu
 **              erzeugen, und zum anderen um auf dieser Grundlage
 **              vektororientierte Datei-Formate wie das
 **              Postscript-Format zu errechnen).
 **
 **              Dieses Vorgehen hatte aber einen entscheidenden
 **              Nachteil: Da der Vektorteil von HP2xx von Zeit zu
 **              Zeit weiterentwickelt wird, mu�te dann jeweils
 **              auch der Atari-Previewer angepa�t werden.
 **
 **              Daher wertet die neue Version nun nicht mehr den
 **              Vektorteil des HP2xx aus. Stattdessen wird die
 **              von HP2xx zur Verf�gung gestellte Bitmap als
 **              Berechnungsgrundlage genommen. F�r die Bitmap ist
 **              ein festes Format garantiert, so da� der
 **              Previewer auf lange Sicht nicht mehr ge�ndert
 **              werden mu�.
 **
 **              Der Atari-Previewer nutzt nur VDI-Zeichenbefehle.
 **              Dadurch ist er zwar nicht gerade einer der
 **              schnellsten, sollte aber in jeder auf dem Atari
 **              verf�gbaren Graphikaufl�sung arbeiten.
 **
 **              Zudem bietet der Previewer bescheidenen
 **              Bedienungskomfort (Scrollen durch ein zu gro�
 **              geratenes Bild per Pfeiltasten, Hilfstext
 **              abrufbar). Auf eine komplette Fensterverwaltung
 **              wurde jedoch verzichtet. Dies h�tte den Previewer
 **              zum einen unn�tig aufgebl�ht und zum anderen w�re
 **              es schon irgendwie merkw�rdig gewesen, wenn nach
 **              einem rein buchstabenorientiertem Programmteil
 **              auf einmal ein typisches GEM-Programm auf dem
 **              Schirm erscheint.
 **
 **              Damit der Benutzer sich nicht so sehr mit den
 **              Besonderheiten seines Bildschirms herumplagen
 **              mu�, beachtet der Atari-Previewer Bildschirm-
 **              aufl�sungen mit sehr ungleich gro�en Pixeln (ein
 **              typischer Fall ist die mittlere Aufl�sung f�r den
 **              Atari ST). Ist also in der Bitmap f�r die x- und
 **              die y-Richtung jeweils die gleiche Aufl�sung
 **              gew�hlt, so erscheint im Preview ein Kreis
 **              (wenigstens so ungef�hr) auch auf dem Bildschirm
 **              als Kreis - unabh�ngig davon, ob man etwas
 **              merkw�rdige Graphikeigenschaften in seinem
 **              Rechner hat oder nicht.
 **
 **              Bisher wurde der Previewer mit folgenden
 **              Bildschirmaufl�sungen getestet (sollte aber - wie
 **              gesagt - auch bei anderen Bildschirmaufl�sungen
 **              laufen):
 **
 **              - ST niedrig ( 320 x 200, 16 Farben)
 **
 **              - ST mittel  ( 640 x 200, 4 Farben)
 **
 **              - ST hoch    ( 640 x 400, monochrom)
 **
 **              Trotz ausf�hrlicher Test mu� aber darauf
 **              hingewiesen werden, da� die Benutzung des
 **              Atari-Previewer auf eigene Gefahr geschieht.
 **/

/**  V. 0.00  16.05.92 NM Null-Version (nicht lauff�hig)
 **  V. 1.00  22.05.92 NM erste lauff�hige Version
 **/

/**
 **  Standard-Header f�r GEM-Programme:
 **/

#include <tos.h>
#include <vdi.h>
#include <aes.h>
#include <stdio.h>
#include <stdlib.h>

/**
 **  Header f�r HP2xx:
 **/

#include "bresnham.h"
#include "hp2xx.h"

/**
 **  erg�nzende Standard-Definitionen f�r GEM-Programme:
 **/

typedef enum {          /* boolean definieren   */
    FALSCH,
    WAHR
} boolean;

#define CON     2       /* Console (VT-52)  */


/*
 *  weitere Definitionen:
 */

#define CLS     printf("%cE",27);   /* Bildschirm l�schen   */

                                    /* Scancodes:           */
#define SC_H        35              /* H    \               */
#define SC_HELP     98              /* Help  >  gl. Fkt.    */
#define SC_F1       59              /* F1   /               */
#define SC_I        23              /* I f�r Info           */
#define SC_Q        16              /* Q    \ Programm-     */
#define SC_ESC      1               /* Esc  / abbruch       */
#define SC_PF_LKS   75              /* Pfeil links          */
#define SC_C_PF_LKS 115             /* Control Pfeil links  */
#define SC_PF_RTS   77              /* Pfeil rechts         */
#define SC_C_PF_RTS 116             /* Control Pfeil rechts */
#define SC_PF_OBN   72              /* Pfeil nach oben      */
#define SC_PF_UTN   80              /* Pfeil nach unten     */

                                    /* Sondertastenbits:    */
#define KB_SHIFT_RTS        1       /* Shift-rechts         */
#define KB_SHIFT_LKS        2       /* Shift-links          */
#define KB_CONTROL          4       /* Control              */
#define KB_ALTERNATE        8       /* Alternate            */


/*
 *  globale Variablen f�r GEM-Programme:
 */

int gl_apid;        /* Applikations-Identifikationsnummer   */

int phys_handle,    /* physikalisches Handle (GRAF_HANDLE)  */
    vdi_handle;     /* VDI-Handle (V_OPENVWK)               */

int gl_hchar,       /* H�he,                                */
    gl_wchar,       /* Breite eines Standard-Zeichens       */
    gl_hbox,        /* H�he, Breite der Box um ein          */
    gl_wbox;        /* Zeichen des Standard-Zeichensatzes   */

int w_text,         /* Anzahl Standard-Zeichen pro Zeile    */
    h_text;         /* Anzahl Zeilen                        */

int work_in[12],    /* Parameter�bergabe-Felder f�r         */
    work_out[57],   /* VDI-Aufrufe (inkl. V_OPENVWK)        */
    pxyarray[10];

int cntrl[12],      /* vom VDI und AES benutzte Parameter-  */
    initin[128],    /* �bergabefelder                       */
    ptsin[128],
    intout[128],
    ptsout[128];

int w_screen,       /* Gr��e des Schirms insgesamt          */
    h_screen,
    w_pixel,        /* Pixelbreite /-h�he in 1/1000 mm      */
    h_pixel,
    color_max,      /* gleichzeitig verf�gbare Farben       */
    color_palette;  /* Anzahl Farben insgesamt              */

/**
 **  Globale Variablen f�r den ATARI-Previewer
 **/

int rx,             /* Reale Schirmpositionen, Gr��e des    */
    rx_min,         /* Schirms und Korrekturfaktoren f�r    */
    rx_max,         /* nicht quadratische Pixel             */
    rx_factor,
    ry,
    ry_min,
    ry_max,
    ry_factor;

static Byte rx_reihe[3000]; /* eine Pixelreihe auf dem      */
                            /* Schirm (sollte auch f�r Gro�-*/
                            /* bildschirme reichen)         */

int sx_min,         /* Schirmausma�e nach Korrektur f�r     */
    sx_max,         /* nicht quadratische Pixel             */
    sy_min,
    sy_max;

int dx,             /* Darzustellender Bildausschnitt nach  */
    dx_min,         /* Korrektur f�r nicht quadratische     */
    dx_max,         /* Pixel                                */
    dy,
    dy_min,
    dy_max;

int px,             /* Position innerhalb und Gr��e der     */
    px_min,         /* Bitmap (in Pixeleinheiten)           */
    px_max,
    py,
    py_min,
    py_max;

int ox,             /* Offset zur Umrechnung vom p- ins d-  */
    oy;             /* System                               */

/**
 **  Funktionsprototypen f�r GEM-Initialisation:
 **/

boolean open_vwork  (void);             /* �ffnet virtuele Workstation  */
void    close_vwork (void);             /* Schlie�t virt. Workstation   */

/**
 **  HP2xx - Funktionsprototypen:
 **/

void    PicBuf_to_ATARI (PicBuf *, PAR *);  /* GEM-Initialisierung und  */
                                            /* Termination (Hauptprg.)  */

    void    preview (PicBuf *);             /* Vorbelegungen, Tasten-   */
                                            /* dr�cke auswerten         */

        void    hilfe (void);               /* Gibt Hilfstext aus       */

        void    info  (void);               /* Bildschirmparameter-Info */

        void    zeichne (PicBuf *);         /* F�hrt Graphik aus        */

            int  lese_pixel(PicBuf *);          /* Liest einzeln. Pixel */
            void zeichne_pixelreihe (void);     /* Zeichnet Pixelreihe  */


/*------------------------------------------------------------------*/


/**
 ** open_vwork:  �ffnet die Workstation, fragt wichtigste Kenndaten
 **              ab
 **/

boolean open_vwork(void)
{
    int i;

    if ((gl_apid = appl_init()) != -1) {

        /* phys. Handle und Standard-Zeichengr��e erfragen  */

        phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox,
                                  &gl_hbox);
        vdi_handle = phys_handle;

        /* work_in vorbesetzen, virtuelle Workstation auf   */
        /* Bildschirm �ffnen                                */

        work_in[0]  = phys_handle;                  /* Handle-Nr.       */
        for (i = 1; i < 10; work_in[i++] = 1);      /* alles Standard   */
        work_in[10] = 2;                            /* RC-Koordinaten   */
        v_opnvwk( work_in, &vdi_handle, work_out ); /* Bildschirm �ffnen*/

        /* Kenngr��en des Desktops abfragen */

        w_pixel  = work_out[3];             /* Pixelbreite /-h�he       */
        h_pixel  = work_out[4];             /*   in 1/1000 mm           */
        color_max = work_out[13];           /* gleichz.darstellb.Farb.  */
        color_palette = work_out[39];       /* verf�gbare Farben        */
        w_screen = work_out[0] + 1;         /* Bildschirmbreite /-h�he  */
        h_screen = work_out[1] + 1;         /*       in Pixeln          */
        vq_chcells(vdi_handle, &h_text, &w_text); /* in Stand.zeichen   */



        /* Maus abschalten (hier kein Maus-bedienbares Programm)    */

        graf_mouse(M_OFF, NULL);

        return(WAHR);

    } else
        return(FALSCH);
}


/*------------------------------------------------------------------*/


/**
 ** close_vwork: Schaltet die Maus wieder an,
 **              schlie�t die Workstation
 **              und die Applikation
 **/

void close_vwork(void)
{
   graf_mouse(M_ON, NULL);

   v_clsvwk(vdi_handle);

   appl_exit();
}


/*------------------------------------------------------------------*/

/**
 ** lese_pixel:  Liest ein einzelnes Pixel aus der Bitmap
 **
 **/
int  lese_pixel(PicBuf *picbuf)
{

    static int py_alt = -1; /* Vergleich, ob noch aktuelle Zeile    */
    static RowBuf *zeile;   /* Pointer auf Zeile in Bitmap          */

    /* int  byte_pos;            Welches Byte (nicht Bit !) ?           */
    /* int byte_inhalt;         Inhalt des Bytes in der Zeile       */
    /* int bit_pos;             Lage des ges. Bits (Pixels) im Byte */
    /* int bit_inhalt;           Bit (Pixel) gesetzt ?              */

    py = dy - dy_min + oy;
    px = dx - dx_min + ox;

    if (py != py_alt) {

        zeile = get_RowBuf(picbuf, picbuf->nr - (py + 1));

    } /* endif */

    /** Urspr�ngliche Version ...
     **  byte_pos = px >> 3;
     **  byte_inhalt = (int) zeile->buf[byte_pos];
     **
     **  bit_pos = px % 8;
     **  bit_inhalt = (byte_inhalt >> (7 - bit_pos)) & 1;
     **
     ** neue Version ...
     **/
    return((((int) zeile->buf[px >> 3]) >> (7 - (px % 8))) & 1);

}

/*------------------------------------------------------------------*/

/**
 ** zeichne_pixelreihe:  Gibt eine Pixelreihe auf dem Schirm aus
 **
 **/
void zeichne_pixelreihe (void)
{

    register int in_Arbeit = 0; /* Gerade Linienst�ck in Arbeit?*/
    register int start;         /* Beginn ...                   */
    register int stop;          /* ... Ende eines Linienst�cks  */
    register int pos = 0;       /* Position 0 ... n             */
    register int n = rx_max - rx_min;

    while (pos <= n) {

        if (rx_reihe[pos] != 0) {

            /* Pixel gesetzt */
            if (in_Arbeit == 0) {

                start = pos;
                in_Arbeit = 1;

            } /* endif */

            stop = pos;

        } else {

            /* Pixel nicht gesetzt */
            if (in_Arbeit == 1) {

                /* Linie ausgeben */
                pxyarray[0] = dx_min / rx_factor + start;   /* x1 */
                pxyarray[1] = ry;                           /* y1 */
                pxyarray[2] = dx_min / rx_factor + stop;    /* x2 */
                pxyarray[3] = ry;                           /* y2 */
                v_pline(vdi_handle, 2, pxyarray);

                in_Arbeit = 0;

            } /* endif */

        } /* endif */

        if (pos == n) {

            if (in_Arbeit == 1) {

                /* Linie ausgeben */
                pxyarray[0] = dx_min / rx_factor + start;   /* x1 */
                pxyarray[1] = ry;                           /* y1 */
                pxyarray[2] = dx_min / rx_factor + stop;    /* x2 */
                pxyarray[3] = ry;                           /* y2 */
                v_pline(vdi_handle, 2, pxyarray);

            } /* endif */

        } /* endif */

        pos += 1;

    } /* endwhile */

}


/*------------------------------------------------------------------*/

/**
 ** zeichne: Steuert das eigentliche Darstellen der Graphik
 **
 **/

void    zeichne (PicBuf *picbuf)
{

    register int i;     /* Z�hler f�r kleine Hilfsschleifen     */
    register int rx_n;  /* Z�hler zum "Sammeln" von Pixeln      */
    register int ry_n;  /* Z�hler zum "Sammeln" von Pixelreihen */
    int pixel;          /* Einzelnes Pixel                      */

    CLS     /* Bildschirm l�schen   */

    /* Ggf. graue / gr�ne R�nder am Bildschirmrand          */

    if (sx_max > px_max) {

        /* seitlichen Rand zeichnen */

        if (dx_min > 0) {

            /* linker Rand */
            pxyarray[0] = 0;                            /* x1   */
            pxyarray[1] = 0;                            /* y1   */
            pxyarray[2] = (dx_min - 1) / rx_factor;     /* x2   */
            pxyarray[3] = h_screen - 1;                 /* y2   */
            v_bar(vdi_handle, pxyarray);

        } /* endif */

        if (dx_max < w_screen * rx_factor - 1) {

            /* rechter Rand */
            pxyarray[0] = (dx_max + 1) / rx_factor;     /* x1   */
            pxyarray[1] = 0;                            /* y1   */
            pxyarray[2] = w_screen - 1;                 /* x2   */
            pxyarray[3] = h_screen - 1;                 /* y2   */
            v_bar(vdi_handle, pxyarray);

        } /* endif */

    } /* endif */

    if (sy_max > py_max) {

        /* Rand oben/unten zeichnen */

        if (dy_min > 0) {

            /* oberer Rand */
            pxyarray[0] = 0;                            /* x1   */
            pxyarray[1] = 0;                            /* y1   */
            pxyarray[2] = w_screen - 1;                 /* x2   */
            pxyarray[3] = (dy_min - 1) / ry_factor;     /* y2   */
            v_bar(vdi_handle, pxyarray);

        } /* endif */

        if (dy_max < h_screen * ry_factor - 1) {

            /* unterer Rand */
            pxyarray[0] = 0;                            /* x1   */
            pxyarray[1] = (dy_max + 1) / ry_factor;     /* y1   */
            pxyarray[2] = w_screen - 1;                 /* x2   */
            pxyarray[3] = h_screen - 1;                 /* y2   */
            v_bar(vdi_handle, pxyarray);

        } /* endif */

    } /* endif */


    /* Steuerung der Pixeldarstellung               */

    dy = dy_min;
    ry = ry_min;

    while (dy <= dy_max) {

        for (i = 0; i <= rx_max - rx_min; i++) {

            rx_reihe[i] = 0;

        } /* endfor */

        ry_n = 1;

        while (ry_n <= ry_factor && dy <= dy_max) {

            dx = dx_min;
            rx = rx_min;

            while (dx <= dx_max) {

                rx_n = 1;

                while (rx_n <= rx_factor && dx <= dx_max) {

                    pixel = lese_pixel(picbuf);
                    rx_reihe[rx - rx_min] = rx_reihe[rx - rx_min] | pixel;
                    dx += 1;
                    rx_n += 1;

                } /* endwhile */

                rx += 1;

            } /* endwhile */

                ry_n += 1;
                dy += 1;

        } /* endwhile */

            zeichne_pixelreihe();
            ry += 1;

    } /* endwhile */

}


/*------------------------------------------------------------------*/

/**
 ** hilfe:   Gibt Hilfstext aus
 **
 **/

void    hilfe (void)
{

    static char *hilfe80 =

        "                            ATARI PREVIEWER  H I L F E\n"
        "                            ==========================\n"
        "\n"
        "     <H>\n"
        "oder <Help>         Diesen Hilfstext anzeigen lassen\n"
        "oder <F1>\n"
        "\n"
        "     <I>            Information �ber wichtigste Kenngr��en des Bildschirms\n"
        "                    anzeigen lassen\n"
        "\n"
        "     <Esc>          Previewer verlassen, Programm beenden\n"
        "oder <Q>\n"
        "\n"
        "     <Pfeiltasten>  Verschieben des aktuellen Bildausschnittes in Richtung\n"
        "                    des Pfeils (wenn m�glich). Die Verschiebung kann durch\n"
        "                    gleichzeitiges Dr�cken weiterer Tasten variiert\n"
        "                    werden:\n"
        "\n"
        "                    <keine weitere Taste>   bildschirmweise verschieben\n"
        "                    <Control>               jeweils 1/8 Bildschirmbreite\n"
        "                    <Shift>                 pixelweise verschieben\n"
        "\n"
        "\n"
        ">>> Zur Programmfortsetzung bitte Taste dr�cken <<<";

    static char *hilfe40 =

        "ATARI PREVIEWER  H I L F E\n"
        "==========================\n"
        "<H> oder <Help> oder <F1>\n"
        "    Diesen Hilfstext anzeigen lassen\n"
        "<I>\n"
        "    Information �ber wichtigste Kenn-\n"
        "    gr��en des Bildschirms anzeigen\n"
        "    lassen\n"
        "<Esc> oder <Q>\n"
        "    Previewer verlassen, Programm\n"
        "    beenden\n"
        "<Pfeiltasten>\n"
        "    Verschieben des aktuellen Bildaus-\n"
        "    schnittes in Richtung des Pfeils\n"
        "    (wenn m�glich). Die Verschiebung\n"
        "    kann durch gleichzeitiges Dr�cken\n"
        "    weiterer Tasten variiert werden:\n"
        "    <keine weitere Taste>\n"
        "        bildschirmweise verschieben\n"
        "    <Control>\n"
        "        jeweils 1/8 Bildschirmbreite\n"
        "    <Shift>\n"
        "        pixelweise verschieben\n"
        "\n"
        ">>> Bitte Taste dr�cken <<<";


    CLS

    if (w_text < 80) {

        fprintf(stderr, "%s",hilfe40);

    } else {

        fprintf(stderr, "%s",hilfe80);

    } /* endif */

}

/*------------------------------------------------------------------*/

/**
 ** info:    Gibt Information �ber alle wichtigen Kenngr��en der
 **          aktuellen Bildschirmaufl�sung aus
 **
 **/

void    info  (void)
{

    CLS                             /* Bildschirm l�schen   */

    fprintf(stderr, "Bildschirmkenngr��en-Info\n");
    fprintf(stderr, "=========================\n\n");

    fprintf(stderr, "Bildschirmbreite:  %4d\n",w_screen);
    fprintf(stderr, "-h�he [Pixel]:     %4d\n",h_screen);
    fprintf(stderr, "\n");
    fprintf(stderr, "Pixelbreite [%cm]:  %4d\n",230,w_pixel);
    fprintf(stderr, "Pixelh�he   [%cm]:  %4d\n",230,h_pixel);
    fprintf(stderr, " ( Verh.(x / y) %c %4d\n",247,rx_factor);
    fprintf(stderr, "   Verh.(y / x) %c %4d )\n",247,ry_factor);
    fprintf(stderr, "\n");
    fprintf(stderr, "Buchstabenbreite:  %4d\n",gl_wchar);
    fprintf(stderr, "- h�he [Pixel]:    %4d\n",gl_hchar);
    fprintf(stderr, "\n");
    fprintf(stderr, "\"Box\"breite:       %4d\n",gl_wbox);
    fprintf(stderr, "\"Box\"h�he [Pixel]: %4d\n",gl_hbox);
    fprintf(stderr, "\n");
    fprintf(stderr, "Zeichen/Zeile:     %4d\n",w_text);
    fprintf(stderr, "Zeilen/Bildschirm: %4d\n",h_text);
    fprintf(stderr, "\n");
    fprintf(stderr, "Farbenzahl:        %4d\n",color_max);
    fprintf(stderr, "Farbennuancen:     %4d\n",color_palette);

    fprintf(stderr, "\n>>> Taste dr�cken <<<\n");

}

/*------------------------------------------------------------------*/

/**
 ** preview: Koordiniert alle Aktivit�ten wie Hilfstext anzeigen,
 **          eigentlichen Preview durchf�hren, Tastendr�cke aus-
 **          werten usw.
 **
 **/

void preview(PicBuf *picbuf)
{
    long    scancode;   /* Scancode der gedr�ckten Taste    */
    long    kbret  = 0; /* Stellung der Sondertasten        */
    boolean newdraw;    /* Neues Zeichnen n�tig?            */

    /* Ausgabe der Begr��ungsmeldung    */
    fprintf(stderr, "\n\n");
    fprintf(stderr, "ATARI-Preview\n");
    fprintf(stderr, "=============\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Bitte Taste dr�cken:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "<H>, <F1> oder <Help> f�r Hilfstext\n");
    fprintf(stderr, "<Q> oder <Esc>        f�r Abbruch\n");
    fprintf(stderr, "<beliebige Taste>     f�r Preview\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Hinweis:\n");
    fprintf(stderr, "Die Hilfe-Funktion ist auch w�hrend\n");
    fprintf(stderr, "des Previews aktiv\n");

    scancode = (Bconin(CON) >> 16) & 255;   /* Tastendruck abwarten */
                                            /* Scancode extrahieren */

    if (scancode != SC_Q && scancode != SC_ESC) {

        /* erstmalige Vorbesetzung der Variablen der    */
        /* verschiedenen Pixelsysteme                   */

        rx_factor = (int)((float)w_pixel / (float)h_pixel + 0.5);
        rx_factor = (rx_factor > 1) ? rx_factor : 1;
        ry_factor = (int)((float)h_pixel / (float)w_pixel + 0.5);
        ry_factor = (ry_factor > 1) ? ry_factor : 1;

        sx_min = 0;
        sx_max = w_screen * rx_factor;
        sy_min = 0;
        sy_max = h_screen * ry_factor;

        px_min = 0;
        px_max = picbuf->nc;
        py_min = 0;
        py_max = picbuf->nr;

        ox = 0;
        oy = 0;

        if (sx_max > px_max) {
            dx_min = (sx_max - px_max) / 2;
            dx_max = sx_max - dx_min - 1 - ((sx_max - px_max) % 2);
        } else {
            dx_min = sx_min;
            dx_max = sx_max - 1;
        }
        if (sy_max > py_max) {
            dy_min = (sy_max - py_max) / 2;
            dy_max = sy_max - dy_min - 1 - ((sy_max - py_max) % 2);
        } else {
            dy_min = sy_min;
            dy_max = sy_max - 1;
        }

        rx_min = dx_min / rx_factor;
        rx_max = dx_max / rx_factor;
        ry_min = dy_min / ry_factor;
        ry_max = dy_max / ry_factor;


        /* Graphikparameter zum Zeichnen vorbesetzen            */

        /* Clipping an Bildschirmgrenzen    */
        pxyarray[0] = 0;            pxyarray[1] = 0;
        pxyarray[2] = w_screen - 1; pxyarray[3] = h_screen - 1;
        vs_clip(vdi_handle, 1, pxyarray);

        /* Stil f�r Fl�chen: grau (s/w) oder gr�n (Farbe)       */
        vsf_perimeter(vdi_handle, 0);       /* kein Rahmen      */
        if(color_max < 4) {
            vsf_interior(vdi_handle, 2);    /* F�llstil: Muster */
            vsf_style(vdi_handle, 4);       /* Muster: grau     */
            vsf_color(vdi_handle, BLACK);   /* F�llfarbe        */
        } else {
            vsf_interior(vdi_handle, 1);    /* F�llstil: voll   */
            vsf_color(vdi_handle, GREEN);   /* F�llfarbe        */
        } /* endif */

        /* Stil f�r Linien festlegen */
        vsl_type(vdi_handle, SOLID);        /* Linienstil           */
        vsl_width(vdi_handle, 1);           /* L.breite (ungerade!) */
        vsl_ends(vdi_handle,0,0);           /* Linienenden          */
        vsl_color(vdi_handle, BLACK);       /* Linienfarbe          */


        /* Schleifenvorbereitung: Vom Begr��ungstext aus        */
        /* darf nur <H>, <Help> oder <F1> eine Bedeutung haben  */
        if (scancode != SC_H  &&  scancode != SC_HELP &&
            scancode != SC_F1)
            scancode = 0;
        /* es soll immer am Anfang einmal gezeichnet werden     */
        newdraw = TRUE;

        /* Tastaturabfrage-Schleife, bis Ende gew�nscht         */
        while (scancode != SC_Q && scancode != SC_ESC) {

            /* Hilfstext auf Wunsch ausgeben                    */

            if (scancode == SC_H  ||  scancode == SC_HELP ||
                scancode == SC_F1) {

                hilfe();
                newdraw = TRUE;

            } else {

                /* Graphik-Information auf Wunsch ausgeben          */

                if (scancode == SC_I) {

                    info();
                    newdraw = TRUE;

                } else {



                    /* gem�� letztem Tastendruck - wenn sinnvoll -  */
                    /* Bildausschnitt neu zeichnen                  */

                    /* Tastendruckauswertung    */
                    if (scancode == SC_PF_OBN || scancode == SC_PF_UTN) {

                        if (sy_max < py_max) {

                            if (scancode == SC_PF_OBN) {

                                if (oy > 0) {

                                    if ((kbret & KB_CONTROL) > 0) {

                                        oy -= sy_max / 8;

                                    } else {

                                        if (((kbret & KB_SHIFT_RTS) | (kbret & KB_SHIFT_LKS)) > 0) {

                                            oy -= 1;

                                        } else {

                                            oy -= sy_max;

                                        } /* endif */

                                    } /* endif */

                                    if (oy < 0) {

                                        oy = 0;

                                    } /* endif */

                                    newdraw = TRUE;

                                } /* endif */

                            } else {

                                if (oy < py_max - sy_max) {

                                    if ((kbret & KB_CONTROL) > 0) {

                                        oy += sy_max / 8;

                                    } else {

                                        if (((kbret & KB_SHIFT_RTS) | (kbret & KB_SHIFT_LKS)) > 0) {

                                            oy += 1;

                                        } else {

                                            oy += sy_max;

                                        } /* endif */

                                    } /* endif */

                                    if (oy > py_max - sy_max) {

                                        oy = py_max - sy_max;

                                    } /* endif */

                                    newdraw = TRUE;

                                } /* endif */

                            } /* endif */

                        } /* endif */

                    } else {

                        if (scancode == SC_PF_RTS || scancode == SC_C_PF_RTS ||
                            scancode == SC_PF_LKS || scancode == SC_C_PF_LKS) {

                            if (sx_max < px_max) {

                                if (scancode == SC_PF_LKS || scancode == SC_C_PF_LKS) {

                                    if (ox > 0) {

                                        if (scancode == SC_C_PF_LKS) {

                                            ox -= sx_max / 8;

                                        } else {

                                            if (((kbret & KB_SHIFT_RTS) | (kbret & KB_SHIFT_LKS))> 0) {

                                                ox -= 1;

                                            } else {

                                                ox -= sx_max;

                                            } /* endif */

                                        } /* endif */

                                        if (ox < 0) {

                                            ox = 0;

                                        } /* endif */

                                        newdraw = TRUE;

                                    } /* endif */

                                } else {

                                    if (ox < px_max - sx_max) {

                                        if (scancode == SC_C_PF_RTS) {

                                            ox += sx_max / 8;

                                        } else {

                                            if (((kbret & KB_SHIFT_LKS) | (kbret & KB_SHIFT_RTS)) > 0) {

                                                ox += 1;

                                            } else {

                                                ox += sx_max;

                                            } /* endif */

                                        } /* endif */

                                        if (ox > px_max - sx_max) {

                                            ox = px_max - sx_max;

                                        } /* endif */

                                        newdraw = TRUE;

                                    } /* endif */

                                } /* endif */

                            } /* endif */

                        } /* endif */

                    } /* endif */

                    if (newdraw == TRUE) {

                        zeichne(picbuf);
                        newdraw = FALSE;

                    } /* endif */

                } /* endif */

            } /* endif */

                /* Tastendruck abwarten, Scancode extrahieren   */
                scancode = (Bconin(CON) >> 16) & 255;
                kbret    = Kbshift(-1); /* Sondertasten abfr.   */

        } /* endwhile */


    } /* endif */

}

/*------------------------------------------------------------------*/


/**
 **  PicBuf_to_ATARI:    Hauptprogramm:
 **                      - Initialisierung und Beenden des GEM
 **                      - Aufruf der eigentlichen Preview-Funktionen
 **/

void    PicBuf_to_ATARI (PicBuf *picbuf, PAR *par)
{
    if (open_vwork() == TRUE) {

        Cursconf(0,1);          /* Cursor aus           */
        preview(picbuf);        /* Previewer aufrufen   */
        Cursconf(1,1);          /* Cursor ein           */
        close_vwork();

    } else {

        fprintf(stderr, "HP2xx - ATARI-Previewer\n");
        fprintf(stderr, "Fehler bei der GEM-Initialisierung!");
        exit(ERROR);

    }
}


