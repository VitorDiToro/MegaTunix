#include "../include/defines.h"
#include <events.h>
#include "../widgets/gauge.h"
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#ifndef M_PI 
#define M_PI 3.1415926535897932384626433832795 
#endif

static GtkWidget * gauge = NULL;
static gboolean hold_handlers = FALSE;


EXPORT gboolean create_new_gauge(GtkWidget * widget, gpointer data)
{
	GladeXML *xml = glade_get_widget_tree(widget);
	GtkWidget *parent = glade_xml_get_widget(xml,"gauge_frame");
	gauge = mtx_gauge_face_new();
	gtk_container_add(GTK_CONTAINER(parent),gauge);
	gtk_widget_show_all(parent);
	gtk_widget_set_sensitive(widget,FALSE);
	update_attributes(xml);
	return (TRUE);
}

EXPORT gboolean create_color_span(GtkWidget * widget, gpointer data)
{
	GtkWidget *dialog = NULL;
	GtkWidget *spinner = NULL;
	MtxColorRange *range = NULL;
	gfloat lbound = 0.0;
	gfloat ubound = 0.0;
	GladeXML *xml = glade_xml_new("gaugedesigner.glade", "range_dialog", NULL);
	printf("create_color_span\n");
	dialog = glade_xml_get_widget(xml,"range_dialog");
	if (!GTK_IS_WIDGET(dialog))
	{
		printf("dialog is fucked!\n");
		return FALSE;
	}

	/* Set the controls to sane ranges corresponding to the gauge */
	mtx_gauge_face_get_bounds(MTX_GAUGE_FACE(gauge),&lbound,&ubound);
	spinner = glade_xml_get_widget(xml,"range_lowpoint_spin");
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(spinner),lbound,ubound);
	spinner = glade_xml_get_widget(xml,"range_highpoint_spin");
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(spinner),lbound,ubound);

	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	switch (result)
	{
		case GTK_RESPONSE_APPLY:
			range = g_new0(MtxColorRange, 1);
			range->lowpoint = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(xml,"range_lowpoint_spin")));
			range->highpoint = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(xml,"range_highpoint_spin")));
			range->inset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(xml,"range_inset_spin")));
			range->lwidth = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(xml,"range_lwidth_spin")));
			gtk_color_button_get_color(GTK_COLOR_BUTTON(glade_xml_get_widget(xml,"range_colorbutton")),&range->color);
			mtx_gauge_face_set_color_range_struct(MTX_GAUGE_FACE(gauge),range);
			g_free(range);

			break;
		default:
			printf("abort\n");
			break;
	}
	gtk_widget_destroy(dialog);

	return (FALSE);
}

EXPORT gboolean spin_button_handler(GtkWidget *widget, gpointer data)
{
	gint tmpi;
	gfloat tmpf;
	gint handler = (gint)g_object_get_data(G_OBJECT(widget),"handler");
	tmpf = (gfloat)gtk_spin_button_get_value((GtkSpinButton *)widget);
	tmpi = (gint)(tmpf+0.00001);
	MtxGaugeFace *g = MTX_GAUGE_FACE(gauge);

	if (hold_handlers)
		return TRUE;

	switch (handler)
	{
		case MAJ_TICKS:
			mtx_gauge_face_set_major_ticks(g,tmpi);
			printf("major tick change\n");
			break;
		case MIN_TICKS:
			mtx_gauge_face_set_minor_ticks(g,tmpi);
			printf("minor tick change\n");
			break;
		case MAJ_TICK_LEN:
			mtx_gauge_face_set_major_tick_len(g,tmpf);
			printf("major tick LEN change\n");
			break;
		case MIN_TICK_LEN:
			mtx_gauge_face_set_minor_tick_len(g,tmpf);
			printf("minor tick LEN change\n");
			break;
		case START_ANGLE:
			mtx_gauge_face_set_lspan_rad(g,tmpf);
			printf("lspan change\n");
			break;
		case STOP_ANGLE:
			mtx_gauge_face_set_uspan_rad(g,tmpf);
			printf("uspan change\n");
			break;
		case LBOUND:
			mtx_gauge_face_set_lbound(g,tmpf);
			printf("lbound change\n");
			break;
		case UBOUND:
			mtx_gauge_face_set_ubound(g,tmpf);
			printf("ubound change\n");
			break;
		case PRECISION:
			mtx_gauge_face_set_precision(g,tmpf);
			printf("precision change\n");
			break;
		case TICK_INSET:
			mtx_gauge_face_set_tick_inset(g,tmpf);
			printf("tick inset change\n");
			break;
		case NEEDLE_WIDTH:
			mtx_gauge_face_set_needle_width(g,tmpf);
			printf("needle width change\n");
			break;
		case NEEDLE_TAIL:
			mtx_gauge_face_set_needle_tail(g,tmpf);
			printf("needle tail change\n");
			break;
		case NAME_SCALE:
			mtx_gauge_face_set_name_font_scale(g,tmpf);
			printf("name font scale change\n");
			break;
		case UNITS_SCALE:
			mtx_gauge_face_set_units_font_scale(g,tmpf);
			printf("units font scale change\n");
			break;
		case VALUE_SCALE:
			mtx_gauge_face_set_value_font_scale(g,tmpf);
			printf("value font scale change\n");
			break;
	}
	return (TRUE);
}


void update_attributes(GladeXML * xml)
{
	gfloat tmp1 = 0.0;
	gfloat tmp2 = 0.0;
	GtkWidget * widget = NULL;
	MtxGaugeFace *g = MTX_GAUGE_FACE(gauge);

	hold_handlers = TRUE;
	widget = glade_xml_get_widget(xml,"major_ticks_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_major_ticks(g));
	widget = glade_xml_get_widget(xml,"minor_ticks_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_minor_ticks(g));
	widget = glade_xml_get_widget(xml,"major_tick_len_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_major_tick_len(g));
	widget = glade_xml_get_widget(xml,"minor_tick_len_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_minor_tick_len(g));
	widget = glade_xml_get_widget(xml,"tick_inset_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_tick_inset(g));
	widget = glade_xml_get_widget(xml,"needle_width_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_needle_width(g));
	widget = glade_xml_get_widget(xml,"needle_tail_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_needle_tail(g));
	widget = glade_xml_get_widget(xml,"name_font_scale_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_name_font_scale(g));
	widget = glade_xml_get_widget(xml,"units_font_scale_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_units_font_scale(g));
	widget = glade_xml_get_widget(xml,"value_font_scale_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_value_font_scale(g));
	mtx_gauge_face_get_span_rad(g,&tmp1,&tmp2);
	widget = glade_xml_get_widget(xml,"start_angle_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),tmp1);
	widget = glade_xml_get_widget(xml,"stop_angle_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),tmp2);
	mtx_gauge_face_get_bounds(g,&tmp1,&tmp2);
	widget = glade_xml_get_widget(xml,"lbound_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),tmp1);
	widget = glade_xml_get_widget(xml,"ubound_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),tmp2);
	widget = glade_xml_get_widget(xml,"precision_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),mtx_gauge_face_get_precision(g));
	widget = glade_xml_get_widget(xml,"name_string_entry");
	gtk_entry_set_text(GTK_ENTRY(widget),mtx_gauge_face_get_name_string(g));
	widget = glade_xml_get_widget(xml,"units_string_entry");
	gtk_entry_set_text(GTK_ENTRY(widget),mtx_gauge_face_get_units_string(g));
	widget = glade_xml_get_widget(xml,"name_font_entry");
	gtk_entry_set_text(GTK_ENTRY(widget),mtx_gauge_face_get_name_font(g));
	widget = glade_xml_get_widget(xml,"units_font_entry");
	gtk_entry_set_text(GTK_ENTRY(widget),mtx_gauge_face_get_units_font(g));
	widget = glade_xml_get_widget(xml,"value_font_entry");
	gtk_entry_set_text(GTK_ENTRY(widget),mtx_gauge_face_get_value_font(g));
	widget = glade_xml_get_widget(xml,"antialiased_check");
	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(widget),mtx_gauge_face_get_antialias(g));
	widget = glade_xml_get_widget(xml,"name_color_button");
	gtk_color_button_set_color(GTK_COLOR_BUTTON(widget),mtx_gauge_face_get_color(g,COL_NAME_FONT));
	widget = glade_xml_get_widget(xml,"units_color_button");
	gtk_color_button_set_color(GTK_COLOR_BUTTON(widget),mtx_gauge_face_get_color(g,COL_UNIT_FONT));
	widget = glade_xml_get_widget(xml,"value_color_button");
	gtk_color_button_set_color(GTK_COLOR_BUTTON(widget),mtx_gauge_face_get_color(g,COL_VALUE_FONT));
	widget = glade_xml_get_widget(xml,"background_color_button");
	gtk_color_button_set_color(GTK_COLOR_BUTTON(widget),mtx_gauge_face_get_color(g,COL_BG));
	widget = glade_xml_get_widget(xml,"needle_color_button");
	gtk_color_button_set_color(GTK_COLOR_BUTTON(widget),mtx_gauge_face_get_color(g,COL_NEEDLE));
	widget = glade_xml_get_widget(xml,"major_tick_color_button");
	gtk_color_button_set_color(GTK_COLOR_BUTTON(widget),mtx_gauge_face_get_color(g,COL_MAJ_TICK));
	widget = glade_xml_get_widget(xml,"minor_tick_color_button");
	gtk_color_button_set_color(GTK_COLOR_BUTTON(widget),mtx_gauge_face_get_color(g,COL_MIN_TICK));
	hold_handlers = FALSE;
}

EXPORT gboolean entry_changed_handler(GtkWidget *widget, gpointer data)
{
	gint handler = (gint)g_object_get_data(G_OBJECT(widget),"handler");
	gchar * tmpbuf = NULL;
	tmpbuf = gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);

	MtxGaugeFace *g = MTX_GAUGE_FACE(gauge);

	if (hold_handlers)
		return TRUE;

	switch ((func)handler)
	{
		case NAME_STR:
			mtx_gauge_face_set_name_str(g,tmpbuf);
			printf("set name string\n");
			break;
		case UNITS_STR:
			mtx_gauge_face_set_units_str(g,tmpbuf);
			printf("set units str\n");
			break;
		case NAME_FONT:
			mtx_gauge_face_set_name_font(g,tmpbuf);
			printf("set name font \n");
			break;
		case VALUE_FONT:
			mtx_gauge_face_set_value_font(g,tmpbuf);
			printf("set value font \n");
			break;
		case UNITS_FONT:
			mtx_gauge_face_set_name_font(g,tmpbuf);
			printf("set units font \n");
			break;
		default:
			break;
	}
	return (TRUE);

}

EXPORT gboolean set_antialiased_mode(GtkWidget *widget, gpointer data)
{
	MtxGaugeFace *g = MTX_GAUGE_FACE(gauge);
	if (hold_handlers)
		return TRUE;

	mtx_gauge_face_set_antialias(g,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

	return TRUE;
}

EXPORT gboolean color_button_color_set(GtkWidget *widget, gpointer data)
{
	gint handler = (gint)g_object_get_data(G_OBJECT(widget),"handler");

	if (hold_handlers)
		return TRUE;
	GdkColor color;

	gtk_color_button_get_color(GTK_COLOR_BUTTON(widget),&color);
	mtx_gauge_face_set_color(MTX_GAUGE_FACE(gauge),handler,color);

	return TRUE;
}

