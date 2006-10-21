/*
 * Copyright (C) 2006 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 * and Christopher Mire (czb)
 *
 * Megasquirt gauge widget
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */


#include <config.h>
#ifndef MTX_GAUGE_FACE_H
#define MTX_GAUGE_FACE_H

G_BEGIN_DECLS

#define MTX_TYPE_GAUGE_FACE		(mtx_gauge_face_get_type ())
#define MTX_GAUGE_FACE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), MTX_TYPE_GAUGE_FACE, MtxGaugeFace))
#define MTX_GAUGE_FACE_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), MTX_GAUGE_FACE, MtxGaugeFaceClass))
#define MTX_IS_GAUGE_FACE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MTX_TYPE_GAUGE_FACE))
#define MTX_IS_GAUGE_FACE_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), MTX_TYPE_GAUGE_FACE))
#define MTX_GAUGE_FACE_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), MTX_TYPE_GAUGE_FACE, MtxGaugeFaceClass))

typedef struct _MtxGaugeFace		MtxGaugeFace;
typedef struct _MtxGaugeFaceClass	MtxGaugeFaceClass;
typedef struct _MtxColorRange		MtxColorRange;

struct _MtxColorRange
{
	gfloat lowpoint;
	gfloat highpoint;
	GdkColor color;
	gfloat lwidth;
	gfloat inset;
};

enum 
{
	COL_BG = 0,
	COL_NEEDLE,
	COL_MAJ_TICK,
	COL_MIN_TICK,
	COL_UNIT_FONT,
	COL_NAME_FONT,
	COL_VALUE_FONT,
	NUM_COLORS
}ColorIndex;


struct _MtxGaugeFace
{//public data
	GtkDrawingArea parent;
	GdkPixmap *pixmap;	/*! Update/backing pixmap */
	GdkPixmap *bg_pixmap;	/*! Static rarely changing pixmap */
	gint w;			/*! width */
	gint h;			/*! height */
	gdouble xc;		/*! X Center */
	gdouble yc;		/*! Y Center */
	gdouble radius;		/*! Radius of display */
#ifdef HAVE_CAIRO
	cairo_t *cr;		/*! Cairo context,  not sure if this is good
				   too hold onto or not */
#endif
	PangoLayout *layout;	/*! Pango TextLayout object */
	PangoFontDescription *font_desc;/*! Pango Font description */
	GdkGC * gc;		/*! Graphics Context for drawing */
	GdkColormap *colormap;	/*! Colormap for GC's */
	GdkColor colors[NUM_COLORS]; /*! Array of colors for specific
					     parts of a gauge object */
	gint precision;		/*! number of decimal places for val */
	gfloat start_deg; 	/*! GDK Start point in degrees (CCW) */
	gfloat stop_deg;	/*! GDK Stop point in degrees (CCW) */
	gfloat start_radian;	/*! CAIRO Start angle in radians (CW) */
	gfloat stop_radian;	/*! CAIRO Stop Angle in radians (CW) */
	gfloat value;		/*! Value represneting needle position */
	gfloat lbound;		/*! Lower Bound to clamp at */
	gfloat ubound;		/*! Upper Bound to Clamp at */
	gfloat span;		/*! Span from lbound to ubound */
	gchar * units_font;	/*! Units Textual font name */
	gchar * units_str;	/*! Units Text String */
	gfloat units_font_scale;/*! Units Font scale, % of 2xradius */
	gchar * value_font;	/*! Value Textual font name */
	gchar * value_str;	/*! Value Text String */
	gfloat value_font_scale;/*! Value Font scale, % of 2xradius */
	gchar * name_font;	/*! Name Textual font name */
	gchar * name_str;	/*! Name Text String */
	gfloat name_font_scale;	/*! Name Font scale, % of 2xradius */
	gboolean antialias;	/*! AA Flag (used in Cairo ONLY */
	gint major_ticks;	/*! Number of MAJOR ticks */
	gint minor_ticks;	/*! Number of MINOR ticks PER pair of major's*/
	gfloat tick_inset;	/*! Percentage of radius to start tickmark @ */
	gfloat major_tick_len;	/*! Major Tick length (% of radius 0-1.0) */
	gfloat minor_tick_len;	/*! Minor tick length (% of radius 0-1.0) */
	gfloat needle_width;	/*! % of radius Needle width @ spin axis */
	gfloat needle_tail;	/*! % of rad Length of "backside" of needle */
	gint needle_polygon_points;
	GdkPoint needle_coords[4];	/*! 4 point needle for now */
	GdkPoint last_needle_coords[4];	/*! 4 point needle for now */
	GArray *ranges;		/*! Array to contain the ranges */
};

struct _MtxGaugeFaceClass
{
	GtkDrawingAreaClass parent_class;
};

GType mtx_gauge_face_get_type (void) G_GNUC_CONST;
GtkWidget* mtx_gauge_face_new ();
void mtx_gauge_face_set_antialias (MtxGaugeFace *gauge, gboolean value);
gboolean mtx_gauge_face_get_antialias (MtxGaugeFace *gauge);
void mtx_gauge_face_set_value (MtxGaugeFace *gauge, gfloat value);
float mtx_gauge_face_get_value (MtxGaugeFace *gauge);
void mtx_gauge_face_set_color_range(MtxGaugeFace *gauge, gfloat, gfloat, GdkColor, gfloat, gfloat);
gint mtx_gauge_face_set_color_range_struct(MtxGaugeFace *gauge, MtxColorRange *);
GArray * mtx_gauge_face_get_ranges(MtxGaugeFace *gauge);
void mtx_gauge_face_set_name_str (MtxGaugeFace *gauge, gchar * str);
void mtx_gauge_face_set_units_str (MtxGaugeFace *gauge, gchar * str);
void mtx_gauge_face_set_precision(MtxGaugeFace *gauge, gint);
gint mtx_gauge_face_get_precision(MtxGaugeFace *gauge);
gchar * mtx_gauge_face_get_units_string (MtxGaugeFace *gauge);
gchar * mtx_gauge_face_get_name_string (MtxGaugeFace *gauge);
void mtx_gauge_face_set_bounds (MtxGaugeFace *gauge, gfloat value1, gfloat value2);
void mtx_gauge_face_set_lbound (MtxGaugeFace *gauge, gfloat );
void mtx_gauge_face_set_ubound (MtxGaugeFace *gauge, gfloat );
gboolean mtx_gauge_face_get_bounds (MtxGaugeFace *gauge, gfloat *value1, gfloat *value2);
void mtx_gauge_face_set_major_tick_len (MtxGaugeFace *gauge, gfloat );
gfloat mtx_gauge_face_get_major_tick_len (MtxGaugeFace *gauge);
void mtx_gauge_face_set_minor_tick_len (MtxGaugeFace *gauge, gfloat );
gfloat mtx_gauge_face_get_minor_tick_len (MtxGaugeFace *gauge);
void mtx_gauge_face_set_major_ticks (MtxGaugeFace *gauge, int ticks);
int mtx_gauge_face_get_major_ticks (MtxGaugeFace *gauge);
void mtx_gauge_face_set_minor_ticks (MtxGaugeFace *gauge, int ticks);
int mtx_gauge_face_get_minor_ticks (MtxGaugeFace *gauge);
gboolean mtx_gauge_face_get_span_rad (MtxGaugeFace *gauge, gfloat *start_radian, gfloat *stop_radian);
gboolean mtx_gauge_face_get_span_deg (MtxGaugeFace *gauge, gfloat *start_deg, gfloat *stop_deg);
void mtx_gauge_face_set_span_rad (MtxGaugeFace *gauge, gfloat start_radian, gfloat stop_radian);
void mtx_gauge_face_set_span_deg (MtxGaugeFace *gauge, gfloat start_deg, gfloat stop_deg);
void mtx_gauge_face_set_lspan_rad (MtxGaugeFace *gauge, gfloat start_radian);
void mtx_gauge_face_set_lspan_deg (MtxGaugeFace *gauge, gfloat start_deg);
void mtx_gauge_face_set_uspan_rad (MtxGaugeFace *gauge, gfloat stop_radian);
void mtx_gauge_face_set_uspan_deg (MtxGaugeFace *gauge, gfloat stop_deg);
void mtx_gauge_face_set_tick_inset (MtxGaugeFace *gauge, gfloat inset);
gfloat mtx_gauge_face_get_tick_inset (MtxGaugeFace *gauge);
void mtx_gauge_face_set_needle_width (MtxGaugeFace *gauge, gfloat width);
gfloat mtx_gauge_face_get_needle_width (MtxGaugeFace *gauge);
void mtx_gauge_face_set_needle_tail (MtxGaugeFace *gauge, gfloat width);
gfloat mtx_gauge_face_get_needle_tail (MtxGaugeFace *gauge);
void mtx_gauge_face_set_units_font (MtxGaugeFace *gauge, gchar * font_name);
gchar * mtx_gauge_face_get_units_font (MtxGaugeFace *gauge);
void mtx_gauge_face_set_value_font (MtxGaugeFace *gauge, gchar * font_name);
gchar * mtx_gauge_face_get_value_font (MtxGaugeFace *gauge);
void mtx_gauge_face_set_name_font (MtxGaugeFace *gauge, gchar * font_name);
gchar * mtx_gauge_face_get_name_font (MtxGaugeFace *gauge);
void mtx_gauge_face_set_units_font_scale (MtxGaugeFace *gauge, gfloat scale);
gfloat mtx_gauge_face_get_units_font_scale (MtxGaugeFace *gauge);
void mtx_gauge_face_set_name_font_scale (MtxGaugeFace *gauge, gfloat scale);
gfloat mtx_gauge_face_get_name_font_scale (MtxGaugeFace *gauge);
void mtx_gauge_face_set_value_font_scale (MtxGaugeFace *gauge, gfloat scale);
gfloat mtx_gauge_face_get_value_font_scale (MtxGaugeFace *gauge);
void mtx_gauge_face_set_color (MtxGaugeFace *gauge, gint index, GdkColor color);
GdkColor *mtx_gauge_face_get_color (MtxGaugeFace *gauge, gint index);

G_END_DECLS

#endif
