/*
 * Copyright (C) 2002-2011 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <configfile.h>
#include <defines.h>
#include <widgetmgmt.h>
#include <debugging.h>
#include <glade/glade.h>
#include <keyparser.h>
#include <stringmatch.h>
#include <tabloader.h>

extern gconstpointer *global_data;

/*!
 \brief populate_master() stores a pointer to all of the glade loaded 
 widgets into a master hashtable so that it can be recalled by name 
 anywhere in the program.
 \param widget, pointer to Widget
 \param user_data, pointer to ConfigFile structure
 */
G_MODULE_EXPORT void populate_master(GtkWidget *widget, gpointer user_data )
{
	gchar *name = NULL;
	gchar *fullname = NULL;
	gchar *prefix = NULL;
	GHashTable *dynamic_widgets = NULL;
	ConfigFile *cfg = (ConfigFile *) user_data;
	/*!
	 Populates a big master hashtable of all dynamic widgets so that 
	 various functions can do a lookup for the widgets name and get it's
	 GtkWidget * for manipulation.  We do NOT insert the topframe
	 widgets from the XML tree as if more than 1 tab loads there will 
	 be a clash, and there's no need to store the top frame widget 
	 anyways...
	 */
	if (GTK_IS_CONTAINER(widget))
		gtk_container_foreach(GTK_CONTAINER(widget),populate_master,user_data);
	if (!cfg_read_string(cfg,"global","id_prefix",&prefix))
		prefix = g_strdup("");

	name = (char *)glade_get_widget_name(widget);
	/*printf("name of widget stored is %s\n",name);*/

	if (name == NULL)
	{
		g_free(prefix);
		return;
	}
	if (g_strrstr((gchar *)name,"topframe"))
	{
		g_free(prefix);
		return;
	}
	dynamic_widgets = DATA_GET(global_data,"dynamic_widgets");
	if(!dynamic_widgets)
	{
		dynamic_widgets = g_hash_table_new_full(g_str_hash,g_str_equal,g_free,NULL);
		DATA_SET_FULL(global_data,"dynamic_widgets",dynamic_widgets,g_hash_table_destroy);
	}
	fullname = g_strdup_printf("%s%s",prefix,name);
	OBJ_SET_FULL(widget,"fullname",g_strdup(fullname),g_free);
	if (!g_hash_table_lookup(dynamic_widgets,fullname))
		g_hash_table_insert(dynamic_widgets,g_strdup(fullname),(gpointer)widget);
	else
		dbg_func(CRITICAL,g_strdup_printf(__FILE__": populate_master()\n\tKey %s  for widget %s from file %s already exists in master table\n",name,fullname,cfg->filename));

	g_free(prefix);
	g_free(fullname);
}


/*!
 \brief register_widget() adds a widget to the master hashtable (dynamic_widgets)
 \see dynamic_widgets
 \param name, Name of widget to store (any strings are allowed)
 \param widget, Pointer to the widget to be stored by name.
 \see deregister_widget
 */
G_MODULE_EXPORT void register_widget(gchar *name, GtkWidget * widget)
{
	GHashTable *dynamic_widgets = NULL;

	dynamic_widgets = DATA_GET(global_data,"dynamic_widgets");
	if(!dynamic_widgets)
	{
		dynamic_widgets = g_hash_table_new_full(g_str_hash,g_str_equal,g_free,NULL);
		DATA_SET_FULL(global_data,"dynamic_widgets",dynamic_widgets,g_hash_table_destroy);
	}
	if (g_hash_table_lookup(dynamic_widgets,name))
	{
		
		g_hash_table_replace(dynamic_widgets,g_strdup(name),(gpointer)widget);
		dbg_func(CRITICAL,g_strdup_printf(__FILE__": register_widget()\n\tWidget named \"%s\" already exists in master table replacing it!\n",name));
	}
	else
		g_hash_table_insert(dynamic_widgets,g_strdup(name),(gpointer)widget);
}


/*!
 \brief deregister_widget() removes a widget from the master hashtable (dynamic_widgets)
 \see dynamic_widgets
 \param name, Name of widget to remove (any strings are allowed)
 \returns TRUE on success removing, FALSE on failure removing 
 \see register_widget
 */
G_MODULE_EXPORT gboolean deregister_widget(gchar *name)
{
	GHashTable *dynamic_widgets = NULL;
	dynamic_widgets = DATA_GET(global_data,"dynamic_widgets");
	return (g_hash_table_remove(dynamic_widgets,name));
}


/*!
  \brief looks up the widget pointer given it's name
  \param name,  name of widget to find
  \returns pointer to the GtkWidget if found, or NULL
  */
G_MODULE_EXPORT GtkWidget * lookup_widget(const gchar * name)
{
	GHashTable *dynamic_widgets = NULL;
	GHashTable *widget_2_tab_hash = NULL;
	GtkWidget *widget = NULL;
	gchar *datamap = NULL;
	dynamic_widgets = DATA_GET(global_data,"dynamic_widgets");
	widget_2_tab_hash = DATA_GET(global_data,"widget_2_tab_hash");

	g_return_val_if_fail(name,NULL);
	g_return_val_if_fail(dynamic_widgets,NULL);
	g_return_val_if_fail(widget_2_tab_hash,NULL);

	if (g_hash_table_lookup_extended(dynamic_widgets,name,NULL,(gpointer *)&widget))
		return widget;
	else if (g_hash_table_lookup_extended(widget_2_tab_hash,name,NULL,(gpointer *)&datamap))
	{
		/* Load the tab this depends on and then search again! */
		if (handle_dependant_tab_load(datamap))
		{
			if (g_hash_table_lookup_extended(dynamic_widgets,name,NULL,(gpointer *)&widget))
				return widget;
		}
		else
			return NULL;

		return NULL;
	}
	else
		return NULL;
}

/*!
 \brief get_State() returns either TRUE or false based on the encoded value 
 passed across as a string.  The string is split up using g_strsplit, the 
 values are check for true/false and hte appropriate value is returned
 \param string, string to parse and dissect
 \param index, which one we want to check
 \returns the decoded state from the string
 */
G_MODULE_EXPORT gboolean get_state(gchar *string, gint index)
{
	gchar **tmpbuf = NULL;
	gboolean state = FALSE;
	gchar *tmpstr = NULL;

	tmpbuf = g_strsplit(string,",",0);
	tmpstr = g_ascii_strup(tmpbuf[index],-1);
	if (g_ascii_strcasecmp(tmpstr,"ENABLED") == 0)
		state = TRUE;
	if (g_ascii_strcasecmp(tmpstr,"DISABLED") == 0)
		state =  FALSE;
	g_free(tmpstr);
	g_strfreev(tmpbuf);
	return state;
}


/*!
  \brief  sets a widget sensitive or insensitive based on the status of
  dependant groups
  \param key, pointer to Widget
  \param data, unused
  */
G_MODULE_EXPORT void alter_widget_state(gpointer key, gpointer data)
{
	GtkWidget * widget = key;
	gchar * tmpbuf = NULL;
	const gchar * name = NULL;
	gchar ** groups = NULL;
	gint num_groups = 0;
	gint i = 0;
	gboolean value = FALSE;
	gboolean state = FALSE;
	MatchType type = AND;
	GHashTable *widget_group_states = NULL;

	if (!GTK_IS_WIDGET(widget))
		return;

	if (!OBJ_GET(widget,"bind_to_list"))
	{
		name = glade_get_widget_name(widget);
		printf(_("Error with widget %s, bind_to_list is null\n"),(name == NULL ? "undefined":name));
		return;
	}
	else
	        tmpbuf = (gchar *)OBJ_GET(widget,"bind_to_list");

	widget_group_states = DATA_GET(global_data,"widget_group_states");
	if (OBJ_GET(widget,"match_type"))
		type = (MatchType)OBJ_GET(widget,"match_type");
	groups = parse_keys(tmpbuf,&num_groups,",");
	state = TRUE;
	/*printf("setting state for %s in groups \"%s\" to:",(gchar *) OBJ_GET(widget,"name"),tmpbuf);*/
	for (i=0;i<num_groups;i++)
	{
		value = (GBOOLEAN)g_hash_table_lookup(widget_group_states,groups[i]);
		if (type == AND)
		{
			if (value == FALSE)
			{
				state = FALSE;
				break;
			}
		}
		else if (type == OR)
		{
			if (value == TRUE)
			{
				state = TRUE;
				break;
			}
			else
				state = FALSE;
		}
	}
	g_strfreev(groups);
	/*printf("%i\n",state);*/
	gtk_widget_set_sensitive(GTK_WIDGET(widget),state);
}



/*!
 \brief Calculate the bounding box for a string rendered with a widget's 
 default font. Set geo to a rect with 0,0 positioned on the left-hand 
 baseline.
 \param widget, pointer to the Widget in question
 \param text, the text we want to get the dimensions of
 \param geo, pointer to PangoRectangle representation of the text dimensions
 */
G_MODULE_EXPORT void get_geo( GtkWidget *widget, const char *text, PangoRectangle *geo )
{
	PangoLayout *layout;
	int width, height;

	layout = gtk_widget_create_pango_layout( widget, text );
	pango_layout_get_pixel_size( layout, &width, &height );
	g_object_unref( layout );

	/* FIXME ... we left/top to 0 for now.
	 *          */
	geo->width = width;
	geo->height = height;
}

/*!
 \brief Set a widget to a fixed size ... width in characters.
 \param widget, pointer to the widget
 \param nchars,  number of charactors this widget should size itself for
 */
G_MODULE_EXPORT void set_fixed_size( GtkWidget *widget, int nchars )
{
	PangoRectangle geo;

	/*! Statically using the font size is BAD PRACTICE and should use
	  the current theme settings,  but that has its own issues! */
	get_geo( widget, "8", &geo );
	gtk_widget_set_size_request( widget, geo.width * nchars, 
			geo.height );
}


/*!
  \brief, prevents a entry from being editable
  \param widget, pointer to Comboboxentry where we want to lock the entry
  */
G_MODULE_EXPORT void lock_entry(GtkWidget *widget)
{
	GtkComboBox *box = GTK_COMBO_BOX(widget);
	GtkEntry *entry = NULL;
	entry =  GTK_ENTRY (GTK_BIN (box)->child);
	if (GTK_IS_ENTRY(entry))
		gtk_editable_set_editable(GTK_EDITABLE(entry),FALSE);
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget),0);
}


/*
 \brief,  returns number of bytes for passed DataSize enumeration
 \param size, the enumeration of the size we want to get number of bytes this
 variable would take up in memory
 \returns the size in bytes that this would take up
 */
G_MODULE_EXPORT gint get_multiplier(DataSize size)
{
	gint mult = 0;

	switch (size)
	{
		case MTX_CHAR:
		case MTX_U08:
		case MTX_S08:
			mult = 1;
			break;
		case MTX_U16:
		case MTX_S16:
			mult = 2;
			break;
		case MTX_U32:
		case MTX_S32:
			mult = 4;
			break;
		default:
			break;
	}
	return mult;
}


/*!
  \brief debugging function to spit out the contents of a datalist attached to
  a widget pointer
  \param key_id, quark representation of a string
  \param data, the data associated with this quark
  \param user_data, unused
  */
G_MODULE_EXPORT void dump_datalist(GQuark key_id, gpointer data, gpointer user_data)
{
	const gchar * key = NULL;
	gfloat *val = NULL;
	key = g_quark_to_string(key_id);
	switch (translate_string((char *)key))
	{
		case MTX_STRING:
			printf("key %s, %s\n",key,(gchar *) data);
			break;
		case MTX_BOOL:
		case MTX_ENUM:
		case MTX_INT:
			printf("key %s, %i\n",key,(GINT)data);
			break;
		case MTX_FLOAT:
			val = (gfloat *) data;
			printf("key %s, %f\n",key,*val);
		default:
			printf("Key %s is complex, ptr %p\n",key,data);
			break;
	}
}


/*!
 \brief set_widget_sensitive() is used to set a widgets state.  This function
 exists because we call it from a g_list_foreach() whereas a straight call to
 gtk_widget_set_sensitive from there would result in typecheck warnings
 \param widget, pointer to widget to change sensitivity
 \param state, the state to set it to
 */
G_MODULE_EXPORT void set_widget_sensitive(gpointer widget, gpointer state)
{
        gtk_widget_set_sensitive(GTK_WIDGET(widget),(GBOOLEAN)state);
}


/*!
 \brief set_widget_active() is used to set a toggle buttonstate.  This function
 exists because we call it from a g_list_foreach() whereas a straight call to
 gtk_toggle_button_set_active from there would result in typecheck warnings
 \param widget, pointer to widget to change sensitivity
 \param state, the state to set it to.
 */
G_MODULE_EXPORT void set_widget_active(gpointer widget, gpointer state)
{
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),(GBOOLEAN)state);
}

