#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include "functions.h"

int main(int argc, char **argv)
{

    GtkWidget *mainWindow = NULL;
    GtkBuilder *builder = NULL;
    GtkButton *validateButton = NULL;
    GtkButton *showCart = NULL;
    GError *error = NULL;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "./GladeFFW.glade", &error);

    if (error)
    {
        gint code = error->code;
        g_printerr("%s\n", error->message);
        g_error_free(error);
        return code;
    }

    gtk_builder_connect_signals(builder, NULL);

    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "homeWindow"));
    validateButton = GTK_BUTTON(gtk_builder_get_object(builder, "validateButton"));
    showCart = GTK_BUTTON(gtk_builder_get_object(builder, "cartButton"));

    g_signal_connect(G_OBJECT(mainWindow), "destroy", (GCallback)OnDestroy, NULL);
    g_signal_connect(G_OBJECT(validateButton), "clicked", (GCallback)OpenScan, builder);
    g_signal_connect(G_OBJECT(showCart), "clicked", (GCallback)OpenCart, builder);

    gtk_widget_show_all(mainWindow);

    gtk_main();

    return EXIT_SUCCESS;
}
