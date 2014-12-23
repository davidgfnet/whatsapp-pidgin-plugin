/*
 * WhatsApp plugin for Pidgin
 *
 * For more information visit:
 *   https://github.com/davidgfnet/whatsapp-pidgin-plugin
 *
 * Copyright (C) 2014, David Guillen Fandos
 * 
 * Based on pidgin-xmpp-receipts:
 *  https://www.assembla.com/code/pidgin-xmpp-receipts/git/nodes
 *
 *
 */

#define DEBUG
#define PURPLE_PLUGINS
#define PLUGIN_VERSION "0.1"

#include <string.h>
#include "gtkplugin.h"
#include "version.h"
#include "gtkimhtml.h"
#include "gtkutils.h"

typedef struct {
	GtkTextBuffer *textbuffer;
	gint offset;
	gint lines;
	int reception;
} message_info;

static GHashTable * messages = NULL;

void add_message_iter(PurpleConnection *gc, const char* to, const gchar* messageid, int newlines)
{
	PurpleAccount *acct = purple_connection_get_account (gc);
	if (acct == 0) return;

	PurpleConversation *conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_ANY, to, acct);
	if (conv == 0) return;

	PidginConversation *gtkconv = PIDGIN_CONVERSATION(conv);
	GtkIMHtml *imhtml = GTK_IMHTML(gtkconv->imhtml);

	message_info *info = g_new(message_info, 1);

	info->textbuffer = imhtml->text_buffer;
	GtkTextIter location;
	gtk_text_buffer_get_end_iter(imhtml->text_buffer, &location);

	info->offset = gtk_text_iter_get_offset (&location);
	info->lines  = newlines;
	info->reception = -1;

	//Insert the location to the table, use messageid as key
	g_hash_table_insert(messages, strdup(messageid), info);
}

#define MESSAGE_SENT     0
#define MESSAGE_RECEIVED 1
#define MESSAGE_READ     2

void message_receipt_cb(PurpleConnection *gc, const char* id, int rtype)
{
	message_info* info = (message_info*) g_hash_table_lookup(messages, id);
	if (info == 0) return;

	int numticks = rtype - info->reception;
	info->reception = rtype;

	GtkTextIter location;
	gtk_text_buffer_get_iter_at_offset (info->textbuffer, &location, info->offset);
	gtk_text_iter_forward_lines (&location, info->lines);
	gtk_text_iter_forward_to_line_end (&location);

	while (numticks--)
		gtk_text_buffer_insert (info->textbuffer, &location, "✓", -1);

	if (info->reception >= MESSAGE_READ)
		g_hash_table_remove (messages, id);
}

static gboolean
deleting_conversation_remove_items(gpointer key, gpointer value, gpointer user_data)
{
	return (((message_info*)value)->textbuffer == user_data) ? TRUE : FALSE;
}

static void
deleting_conversation_cb(PurpleConversation *conv)
{
	g_hash_table_foreach_remove(messages,
								deleting_conversation_remove_items,
								GTK_IMHTML(PIDGIN_CONVERSATION(conv)->imhtml)->text_buffer);
            
}

// Store the message id and number of lines into the hash table
static void
msg_sending_cb(PurpleConnection *gc, const char * id, const char * who, const char * message)
{
	int newlines = 1;

	int i;
	for (i = 0; i < strlen(message); i++)
		if (message[i] == '\n')
			newlines++;

	add_message_iter(gc, who, id, newlines);
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
	PurplePlugin *whatsapp_protocol = purple_find_prpl("prpl-whatsapp");

	if (!whatsapp_protocol)
		return FALSE;
	
	messages = g_hash_table_new(g_str_hash, g_str_equal);

	purple_signal_connect(whatsapp_protocol, "whatsapp-sending-message", plugin, PURPLE_CALLBACK(msg_sending_cb), NULL);
	purple_signal_connect(whatsapp_protocol, "whatsapp-message-received", plugin, PURPLE_CALLBACK(message_receipt_cb), NULL);

    purple_signal_connect(purple_conversations_get_handle(), "deleting-conversation", plugin, PURPLE_CALLBACK(deleting_conversation_cb), NULL);

	return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
	return TRUE;
}

static PurplePluginInfo info =
{
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,                         /**< type           */
	PIDGIN_PLUGIN_TYPE,                             /**< ui_requirement */
	0,                                              /**< flags          */
	NULL,                                           /**< dependencies   */
	PURPLE_PRIORITY_LOWEST,                         /**< priority       */
	
	"whatsapp-pidgin-plugin",                       /**< id             */
	"WhatsApp Pidgin Plugin",                       /**< name           */
	PLUGIN_VERSION,                                 /**< version        */
	"WhatsApp plugin for Pidgin",                   /**  summary        */
	                                                /**  description    */
	"Displays reception information for WhatsApp messages",
	"David Guillen Fandos (david@davidgf.net)",     /**< author         */
	"http://davidgf.net",                           /**< homepage       */
	
	plugin_load,                                    /**< load           */
	plugin_unload,                                  /**< unload         */
	NULL,                                           /**< destroy        */
	NULL,                                           /**< ui_info        */
	NULL,                                           /**< extra_info     */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static void
init_plugin(PurplePlugin *plugin)
{
}

PURPLE_INIT_PLUGIN(whatsapp_plugin, init_plugin, info)

