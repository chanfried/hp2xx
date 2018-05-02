#ifdef ATARI
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

/**  V. 2.00
   Andreas Schwab (schwab@ls5.informatik.uni-dortmund.de)

   - Ausgaberoutine v�llig umgeschrieben, dadurch wesentlich schneller
   - Farbunterst�tzung
   - getestet auf allen Farbaufl�sungen des TT (incl. ST Hoch)
   - Bilder werden gestreckt statt gestaucht bei nicht-quadratischen
     Aufl�sungen (wurde wegen der Farbunterst�tzung n�tig)
 **
 ** V 2.10 HWW  "Blind" an die Aenderungen in hp2xx.h angepasst,
 **             denn ich habe keinen ATARI zum Testen zur Verfuegung!
 **/

/**
 **  Standard-Header f�r GEM-Programme:
 **/

#ifndef __PUREC__
/* Standard-Namen */
#include <osbind.h>
#include <aesbind.h>
#include <vdibind.h>
#else
#include <tos.h>
#include <aes.h>
#include <vdi.h>
#endif
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

typedef enum {			/* boolean definieren   */
	FALSCH,
	WAHR
} boolean;

#define CON     2		/* Console (VT-52)  */

/*
 *  weitere Definitionen:
 */

#define CLS     Eprintf("\033E")	/* Bildschirm l�schen   */

 /* Scancodes:           */
#define SC_H        35		/* H    \               */
#define SC_HELP     98		/* Help  >  gl. Fkt.    */
#define SC_F1       59		/* F1   /               */
#define SC_I        23		/* I f�r Info           */
#define SC_Q        16		/* Q    \ Programm-     */
#define SC_ESC      1		/* Esc  / abbruch       */
#define SC_PF_LKS   75		/* Pfeil links          */
#define SC_C_PF_LKS 115		/* Control Pfeil links  */
#define SC_PF_RTS   77		/* Pfeil rechts         */
#define SC_C_PF_RTS 116		/* Control Pfeil rechts */
#define SC_PF_OBN   72		/* Pfeil nach oben      */
#define SC_PF_UTN   80		/* Pfeil nach unten     */

 /* Sondertastenbits:    */
#define KB_SHIFT_RTS        1	/* Shift-rechts         */
#define KB_SHIFT_LKS        2	/* Shift-links          */
#define KB_CONTROL          4	/* Control              */
#define KB_ALTERNATE        8	/* Alternate            */

/*
 *  globale Variablen f�r GEM-Programme:
 */

int gl_apid;			/* Applikations-Identifikationsnummer   */

int phys_handle,		/* physikalisches Handle (GRAF_HANDLE)  */
 vdi_handle;			/* VDI-Handle (V_OPENVWK)               */

int gl_hchar,			/* H�he,                                */
 gl_wchar,			/* Breite eines Standard-Zeichens       */
 gl_hbox,			/* H�he, Breite der Box um ein          */
 gl_wbox;			/* Zeichen des Standard-Zeichensatzes   */

int w_text,			/* Anzahl Standard-Zeichen pro Zeile    */
 h_text;			/* Anzahl Zeilen                        */

int work_in[12],		/* Parameter�bergabe-Felder f�r         */
 work_out[57],			/* VDI-Aufrufe (inkl. V_OPENVWK)        */
 pxyarray[10];

int cntrl[12],			/* vom VDI und AES benutzte Parameter-  */
 initin[128],			/* �bergabefelder                       */
 ptsin[128], intout[128], ptsout[128];

int w_screen, h_screen;		/* Gr��e des Schirms insgesamt          */
int w_pixel, h_pixel;		/* Pixelbreite /-h�he in 1/1000 mm      */
int color_max;			/* gleichzeitig verf�gbare Farben       */
int color_palette;		/* Anzahl Farben insgesamt              */

/**
 **  Globale Variablen f�r den ATARI-Previewer
 **/

/* Gr��e des Schirms und Korrekturfaktoren f�r nicht quadratische Pixel */
int rx_min, rx_max, rx_factor, ry_min, ry_max, ry_factor;

/* eine Pixelreihe auf dem Schirm */
static Byte *rx_reihe;

/* Schirmausma�e nach Korrektur f�r nicht quadratische Pixel */
int sx_max, sy_max;

/* Darzustellender Bildausschnitt nach Korrektur f�r
   nicht quadratische Pixel */
int dx_min, dx_max, dy_min, dy_max;

/* Gr��e der Bitmap (in Pixeleinheiten) */
int px_max, py_max;

/* Offset zur Umrechnung vom p- ins d-System */
int ox, oy;

/**
 **  Funktionsprototypen f�r GEM-Initialisation:
 **/

boolean open_vwork(void);	/* �ffnet virtuele Workstation  */
void close_vwork(void);		/* Schlie�t virt. Workstation   */

/**
 **  HP2xx - Funktionsprototypen:
 **/

void preview(PicBuf *, PAR *);	/* Vorbelegungen, Tastendr�cke auswerten */

void hilfe(void);		/* Gibt Hilfstext aus       */

void info(void);		/* Bildschirmparameter-Info */

void zeichne(PicBuf *);		/* F�hrt Graphik aus        */

int lese_pixel(PicBuf *);	/* Liest einzeln. Pixel */
void zeichne_pixelreihe(int);	/* Zeichnet Pixelreihe  */

/*------------------------------------------------------------------*/

/**
 ** open_vwork:  �ffnet die Workstation, fragt wichtigste Kenndaten
 **              ab
 **/

static boolean open_vwork(void)
{
	int i;

	if ((gl_apid = appl_init()) != -1) {

		/* phys. Handle und Standard-Zeichengr��e erfragen  */

		phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox,
					  &gl_hbox);
		vdi_handle = phys_handle;

		/* work_in vorbesetzen, virtuelle Workstation auf   */
		/* Bildschirm �ffnen                                */

		work_in[0] = phys_handle;	/* Handle-Nr.       */
		for (i = 1; i < 10; work_in[i++] = 1);	/* alles Standard   */
		work_in[10] = 2;	/* RC-Koordinaten   */
		v_opnvwk(work_in, &vdi_handle, work_out);	/* Bildschirm �ffnen */

		/* Kenngr��en des Desktops abfragen */

		w_pixel = work_out[3];	/* Pixelbreite /-h�he       */
		h_pixel = work_out[4];	/*   in 1/1000 mm           */
		color_max = work_out[13];	/* gleichz.darstellb.Farb.  */
		color_palette = work_out[39];	/* verf�gbare Farben        */
		w_screen = work_out[0] + 1;	/* Bildschirmbreite /-h�he  */
		h_screen = work_out[1] + 1;	/*       in Pixeln          */
		vq_chcells(vdi_handle, &h_text, &w_text);	/* in Stand.zeichen */

		/* Maus abschalten (hier kein Maus-bedienbares Programm)    */

		graf_mouse(M_OFF, NULL);

		return (WAHR);
	} else
		return (FALSCH);
}

/*------------------------------------------------------------------*/

/**
 ** close_vwork: Schaltet die Maus wieder an,
 **              schlie�t die Workstation
 **              und die Applikation
 **/

static void close_vwork(void)
{
	graf_mouse(M_ON, NULL);
	v_clsvwk(vdi_handle);
	appl_exit();
}

static int last_color = -1;
/* Standard-Farbenbelegung wird vorausgesetzt */
static int xx2vdi[8] =
    { WHITE, BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW };

#ifdef __GNUC__
inline
#endif
static void set_line_color(int color)
{
	if (color != last_color) {
		vsl_color(vdi_handle, xx2vdi[color]);
		last_color = color;
	}
}

/*------------------------------------------------------------------*/

/**
 ** zeichne_pixelreihe:  Gibt eine Pixelreihe auf dem Schirm aus
 **			 mit Beachtung von ry_factor
 **/
static void zeichne_pixelreihe(int ry)
{
	register int start = rx_min;	/* Beginn des Linienst�cks      */
	register int pos;	/* x Position                   */
	int curr_color = rx_reihe[rx_min];
	int ry_n;

	for (pos = rx_min + 1; pos <= rx_max; pos++) {
		if (rx_reihe[pos] != curr_color) {
			if (curr_color != xxBackground) {
				set_line_color(curr_color);
				/* Linie(n) ausgeben */
				for (ry_n = 0; ry_n < ry_factor; ry_n++) {
					pxyarray[0] = start;	/* x1 */
					pxyarray[1] = ry + ry_n;	/* y1 */
					pxyarray[2] = pos - 1;	/* x2 */
					pxyarray[3] = ry + ry_n;	/* y2 */
					v_pline(vdi_handle, 2, pxyarray);
				}
			}
			start = pos;
			curr_color = rx_reihe[pos];
		}
	}
	if (curr_color != xxBackground) {
		set_line_color(curr_color);
		/* Linie(n) ausgeben */
		for (ry_n = 0; ry_n < ry_factor; ry_n++) {
			pxyarray[0] = start;	/* x1 */
			pxyarray[1] = ry + ry_n;	/* y1 */
			pxyarray[2] = rx_max;	/* x2 */
			pxyarray[3] = ry + ry_n;	/* y2 */
			v_pline(vdi_handle, 2, pxyarray);
		}
	}
}

/*------------------------------------------------------------------*/

/**
 ** zeichne: Steuert das eigentliche Darstellen der Graphik
 **
 **/

static void zeichne(PicBuf * picbuf)
{
	register int rx_n;	/* Z�hler zum "Sammeln" von Pixeln      */
	RowBuf *zeile;
	int px, py;
	int rx, ry;
	int color_index;

	v_clrwk(vdi_handle);	/* Bildschirm l�schen   */

	/* Ggf. graue / gr�ne R�nder am Bildschirmrand          */
	if (sx_max > px_max) {
		/* seitlichen Rand zeichnen */
		if (rx_min > 0) {
			/* linker Rand */
			pxyarray[0] = 0;	/* x1   */
			pxyarray[1] = 0;	/* y1   */
			pxyarray[2] = rx_min - 1;	/* x2   */
			pxyarray[3] = h_screen - 1;	/* y2   */
			v_bar(vdi_handle, pxyarray);
		}
		if (rx_max < w_screen - 1) {
			/* rechter Rand */
			pxyarray[0] = rx_max + 1;	/* x1   */
			pxyarray[1] = 0;	/* y1   */
			pxyarray[2] = w_screen - 1;	/* x2   */
			pxyarray[3] = h_screen - 1;	/* y2   */
			v_bar(vdi_handle, pxyarray);
		}
	}

	if (sy_max > py_max) {
		/* Rand oben/unten zeichnen */
		if (ry_min > 0) {
			/* oberer Rand */
			pxyarray[0] = 0;	/* x1   */
			pxyarray[1] = 0;	/* y1   */
			pxyarray[2] = w_screen - 1;	/* x2   */
			pxyarray[3] = ry_min - 1;	/* y2   */
			v_bar(vdi_handle, pxyarray);
		}
		if (ry_max < h_screen - 1) {
			/* unterer Rand */
			pxyarray[0] = 0;	/* x1   */
			pxyarray[1] = ry_max + 1;	/* y1   */
			pxyarray[2] = w_screen - 1;	/* x2   */
			pxyarray[3] = h_screen - 1;	/* y2   */
			v_bar(vdi_handle, pxyarray);
		}
	}
	/* Steuerung der Pixeldarstellung */
	py = oy + 1;
	for (ry = ry_min; ry <= ry_max; ry += ry_factor, py++) {
		zeile = get_RowBuf(picbuf, picbuf->nr - py);
		px = ox;
		for (rx = rx_min; rx <= rx_max; px++) {
			color_index = index_from_RowBuf(zeile, px, picbuf);
			for (rx_n = 0; rx_n < rx_factor && rx <= rx_max;
			     rx++, rx_n++)
				rx_reihe[rx] = color_index;
		}
		zeichne_pixelreihe(ry);
	}
}

/*------------------------------------------------------------------*/

/**
 ** hilfe:   Gibt Hilfstext aus
 **
 **/

static void hilfe(void)
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
	    "\n" ">>> Zur Programmfortsetzung bitte Taste dr�cken <<<";

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
	    "\n" ">>> Bitte Taste dr�cken <<<";

	CLS;
	if (w_text < 80)
		Eprintf("%s", hilfe40);
	else
		Eprintf("%s", hilfe80);
	Bconin(CON);
}

/*------------------------------------------------------------------*/

/**
 ** info:    Gibt Information �ber alle wichtigen Kenngr��en der
 **          aktuellen Bildschirmaufl�sung aus
 **
 **/

static void info(void)
{

	CLS;			/* Bildschirm l�schen   */

	Eprintf("Bildschirmkenngr��en-Info\n");
	Eprintf("=========================\n\n");

	Eprintf("Bildschirmbreite:  %4d\n", w_screen);
	Eprintf("-h�he [Pixel]:     %4d\n", h_screen);
	Eprintf("\n");
	Eprintf("Pixelbreite [�m]:  %4d\n", w_pixel);
	Eprintf("Pixelh�he   [�m]:  %4d\n", h_pixel);
	Eprintf(" ( Verh.(x / y) � %4d\n", rx_factor);
	Eprintf("   Verh.(y / x) � %4d )\n", ry_factor);
	Eprintf("\n");
	Eprintf("Buchstabenbreite:  %4d\n", gl_wchar);
	Eprintf("- h�he [Pixel]:    %4d\n", gl_hchar);
	Eprintf("\n");
	Eprintf("\"Box\"breite:       %4d\n", gl_wbox);
	Eprintf("\"Box\"h�he [Pixel]: %4d\n", gl_hbox);
	Eprintf("\n");
	Eprintf("Zeichen/Zeile:     %4d\n", w_text);
	Eprintf("Zeilen/Bildschirm: %4d\n", h_text);
	Eprintf("\n");
	Eprintf("Farbenzahl:        %4d\n", color_max);
	Eprintf("Farbennuancen:     %4d\n", color_palette);

	Eprintf("\n>>> Taste dr�cken <<<\n");
	Bconin(CON);
}

/*------------------------------------------------------------------*/

/**
 ** preview: Koordiniert alle Aktivit�ten wie Hilfstext anzeigen,
 **          eigentlichen Preview durchf�hren, Tastendr�cke aus-
 **          werten usw.
 **
 **/

static void preview(PicBuf * picbuf, int quiet)
{
	long scancode;		/* Scancode der gedr�ckten Taste    */
	long kbret = 0;		/* Stellung der Sondertasten        */
	boolean newdraw;	/* Neues Zeichnen n�tig?            */

	if (!quiet) {
		/* Ausgabe der Begr��ungsmeldung    */
		Eprintf("\n\n");
		Eprintf("ATARI-Preview\n");
		Eprintf("=============\n");
		Eprintf("\n");
		Eprintf("Bitte Taste dr�cken:\n");
		Eprintf("\n");
		Eprintf("<H>, <F1> oder <Help> f�r Hilfstext\n");
		Eprintf("<Q> oder <Esc>        f�r Abbruch\n");
		Eprintf("<beliebige Taste>     f�r Preview\n");
		Eprintf("\n");
		Eprintf("Hinweis:\n");
		Eprintf("Die Hilfe-Funktion ist auch w�hrend\n");
		Eprintf("des Previews aktiv\n");
		scancode = (Bconin(CON) >> 16) & 255;	/* Tastendruck abwarten */
	} else
		scancode = 0;

	if (scancode != SC_Q && scancode != SC_ESC) {
		/* erstmalige Vorbesetzung der Variablen der    */
		/* verschiedenen Pixelsysteme                   */

		if (w_pixel >= h_pixel) {
			rx_factor = 1;
			ry_factor = (w_pixel + h_pixel - 1) / h_pixel;
		} else {
			rx_factor = (h_pixel + w_pixel - 1) / w_pixel;
			ry_factor = 1;
		}

		sx_max = w_screen / rx_factor;
		sy_max = h_screen / ry_factor;

		px_max = picbuf->nc;
		py_max = picbuf->nr;

		ox = 0;
		oy = 0;

		if (sx_max > px_max) {
			dx_min = (sx_max - px_max) / 2;
			dx_max = dx_min + px_max - 1;
		} else {
			dx_min = 0;
			dx_max = sx_max - 1;
		}
		if (sy_max > py_max) {
			dy_min = (sy_max - py_max) / 2;
			dy_max = dy_min + py_max - 1;
		} else {
			dy_min = 0;
			dy_max = sy_max - 1;
		}

		rx_min = dx_min * rx_factor;
		rx_max = dx_max * rx_factor;
		ry_min = dy_min * ry_factor;
		ry_max = dy_max * ry_factor;

		/* Graphikparameter zum Zeichnen vorbesetzen            */

		/* Clipping an Bildschirmgrenzen    */
		pxyarray[0] = 0;
		pxyarray[1] = 0;
		pxyarray[2] = w_screen - 1;
		pxyarray[3] = h_screen - 1;
		vs_clip(vdi_handle, 1, pxyarray);

		/* Stil f�r Fl�chen: grau (s/w) oder gr�n (Farbe)       */
		vsf_perimeter(vdi_handle, 0);	/* kein Rahmen      */
		if (color_max < 4) {
			vsf_interior(vdi_handle, 2);	/* F�llstil: Muster */
			vsf_style(vdi_handle, 4);	/* Muster: grau     */
			vsf_color(vdi_handle, BLACK);	/* F�llfarbe        */
		} else {
			vsf_interior(vdi_handle, 1);	/* F�llstil: voll   */
			vsf_color(vdi_handle, GREEN);	/* F�llfarbe        */
		}

		/* Stil f�r Linien festlegen */
		vsl_type(vdi_handle, 1);	/* Linienstil           */
		vsl_width(vdi_handle, 1);	/* L.breite (ungerade!) */
		vsl_ends(vdi_handle, 0, 0);	/* Linienenden          */
		vsl_color(vdi_handle, BLACK);	/* Linienfarbe          */

		/* Schleifenvorbereitung: Vom Begr��ungstext aus        */
		/* darf nur <H>, <Help> oder <F1> eine Bedeutung haben  */
		if (scancode != SC_H && scancode != SC_HELP
		    && scancode != SC_F1)
			scancode = 0;
		/* es soll immer am Anfang einmal gezeichnet werden     */
		newdraw = TRUE;

		/* Tastaturabfrage-Schleife, bis Ende gew�nscht         */
		for (;;) {
			switch (scancode) {
			case SC_Q:
			case SC_ESC:
				return;

			case SC_H:
			case SC_HELP:
			case SC_F1:
				/* Hilfstext auf Wunsch ausgeben                    */
				hilfe();
				newdraw = TRUE;
				break;
			case SC_I:
				/* Graphik-Information auf Wunsch ausgeben          */
				info();
				newdraw = TRUE;
				break;

				/* gem�� letztem Tastendruck - wenn sinnvoll -  */
				/* Bildausschnitt neu zeichnen                  */
			case SC_PF_OBN:
				if (sy_max >= py_max)
					break;
				if (oy > 0) {
					if (kbret & KB_CONTROL)
						oy -= sy_max / 8;
					else if (kbret &
						 (KB_SHIFT_RTS |
						  KB_SHIFT_LKS))
						oy -= 1;
					else
						oy -= sy_max;
					if (oy < 0)
						oy = 0;
					newdraw = TRUE;
				}
				break;
			case SC_PF_UTN:
				if (sy_max >= py_max)
					break;
				if (oy < py_max - sy_max) {
					if (kbret & KB_CONTROL)
						oy += sy_max / 8;
					else if (kbret &
						 (KB_SHIFT_RTS |
						  KB_SHIFT_LKS))
						oy += 1;
					else
						oy += sy_max;
					if (oy > py_max - sy_max)
						oy = py_max - sy_max;
					newdraw = TRUE;
				}
				break;
			case SC_PF_RTS:
			case SC_C_PF_RTS:
				if (sx_max >= px_max)
					break;
				if (ox < px_max - sx_max) {
					if (scancode == SC_C_PF_RTS)
						ox += sx_max / 8;
					else if (kbret &
						 (KB_SHIFT_LKS |
						  KB_SHIFT_RTS))
						ox += 1;
					else
						ox += sx_max;
					if (ox > px_max - sx_max)
						ox = px_max - sx_max;
					newdraw = TRUE;
				}
				break;
			case SC_PF_LKS:
			case SC_C_PF_LKS:
				if (sx_max >= px_max)
					break;
				if (ox > 0) {
					if (scancode == SC_C_PF_LKS)
						ox -= sx_max / 8;
					else if (kbret &
						 (KB_SHIFT_RTS |
						  KB_SHIFT_LKS))
						ox -= 1;
					else
						ox -= sx_max;
					if (ox < 0)
						ox = 0;
					newdraw = TRUE;
				}
				break;
			}
			if (newdraw) {
				zeichne(picbuf);
				newdraw = FALSE;
			}
			/* Tastendruck abwarten, Scancode extrahieren   */
			scancode = (Bconin(CON) >> 16) & 255;
			kbret = Kbshift(-1);	/* Sondertasten abfr.   */
		}
	}
}

/*------------------------------------------------------------------*/

/**
 **  PicBuf_to_ATARI:    Hauptprogramm:
 **                      - Initialisierung und Beenden des GEM
 **                      - Aufruf der eigentlichen Preview-Funktionen
 **/

static int PicBuf_to_ATARI(GEN_PAR * pg, OUT_PAR * po)
{
	if (open_vwork()) {
		rx_reihe = (Byte *) malloc(h_screen);
		if (rx_reihe == NULL) {
			Eprintf("\nError: No mem for line buffer!\n");
			PError("PicBuf_to_ATARI");
			close_vwork();
			return ERROR;
		}

		(void) Cursconf(0, 1);	/* Cursor aus           */
		preview(po->picbuf, pg->quiet);	/* Previewer aufrufen   */
		(void) Cursconf(1, 1);	/* Cursor ein           */
		close_vwork();
	} else {
		Eprintf("HP2xx - ATARI-Previewer\n");
		Eprintf("Fehler bei der GEM-Initialisierung!");
		return ERROR;
	}
	if (rx_reihe != NULL)
		free(rx_reihe);
	return 0;
}
#endif
