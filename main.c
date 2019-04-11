#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include "functions.h"

int main(int argc, char **argv)
{

    GtkWidget *mainWindow = NULL;
    GtkWidget *scanproduct = NULL;
    GtkWidget *cartResume = NULL;
    GtkBuilder *builder = NULL;
    GtkButton *validateButton = NULL;
    GtkButton *showCart = NULL;
    GtkButton *returnCart = NULL;
    GError *error = NULL;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "../GladeFFW.glade", &error);

    if (error)
    {
        gint code = error->code;
        g_printerr("%s\n", error->message);
        g_error_free(error);
        return code;
    }

    GtkCssProvider *provider = gtk_css_provider_new ();
    GdkDisplay *display = gdk_display_get_default ();
    GdkScreen *screen = gdk_display_get_default_screen (display);

    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER); // ADD PROVIDER FOR SCREEN AND SET PRIORITY OF GRAPHICAL FROM CSS

    GFile *css_file = g_file_new_for_path("../css/global.css");

    gtk_css_provider_load_from_file(provider, css_file, &error); // LOAD OF CSS FILE

    g_object_unref (css_file);



    gtk_builder_connect_signals(builder, NULL);

    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "homeWindow"));
    validateButton = GTK_BUTTON(gtk_builder_get_object(builder, "validateButton"));
    showCart = GTK_BUTTON(gtk_builder_get_object(builder, "cartButton"));
    scanproduct = GTK_WIDGET(gtk_builder_get_object(builder, "scanWindow"));
    cartResume = GTK_WIDGET(gtk_builder_get_object(builder, "cartWindow"));
    returnCart = GTK_BUTTON(gtk_builder_get_object(builder, "returncartButton"));


    g_signal_connect(G_OBJECT(mainWindow), "destroy", (GCallback)OnDestroy, NULL);
    g_signal_connect(G_OBJECT(validateButton), "clicked", (GCallback)OpenScan, builder);
    g_signal_connect(G_OBJECT(scanproduct), "destroy", (GCallback)OnDestroy, NULL);
    g_signal_connect(G_OBJECT(showCart), "clicked", (GCallback)OpenCart, builder);
    g_signal_connect(G_OBJECT(cartResume), "destroy", (GCallback)OnDestroy, NULL);
    g_signal_connect(G_OBJECT(returnCart), "clicked", (GCallback) ReturnCart, builder);

    gtk_widget_show_all(mainWindow);

    gtk_main();

    return EXIT_SUCCESS;
}
