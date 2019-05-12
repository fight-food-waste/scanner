#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#include <gtk/gtk.h>
#include <glib/gprintf.h>

#include "functions.h"

int main(int argc, char **argv) {

    GtkBuilder *builder = NULL;
    GtkButton *validateButton = NULL;
    GtkButton *validecartButton = NULL;
    GtkButton *showCart = NULL;
    GtkButton *returnCart = NULL;
    GtkButton *addCart = NULL;
    GError *error = NULL;
    GlobalStruct *global_struct = NULL;
    GtkWidget *tree_view_widget = NULL;
    GtkScrolledWindow *cart_scrolled_window = NULL;

    // Mandatory GTK init
    gtk_init(&argc, &argv);

    // Create new GtkBuild object
    builder = gtk_builder_new();
    // Parse GtkBuilder UI definition from gui.glade and merge it with the current contents of `builder`
    gtk_builder_add_from_file(builder, "../gui.glade", &error);
    if (error) {
        gint code = error->code;

        g_printerr("%s\n", error->message);

        g_error_free(error);

        return code;
    }


    // Init the global struct which will store data throughout the lifetime of the app
    global_struct = init_global_struct(builder);

    // Load CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER (css_provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
    GFile *css_file = g_file_new_for_path("../css/global.css");
    gtk_css_provider_load_from_file(css_provider, css_file, &error);
    g_object_unref(css_file);

    // Import some GTK objects from GtkBuilder
    validateButton = GTK_BUTTON(gtk_builder_get_object(builder, "validateButton"));
    showCart = GTK_BUTTON(gtk_builder_get_object(builder, "cartButton"));
    returnCart = GTK_BUTTON(gtk_builder_get_object(builder, "returncartButton"));
    addCart = GTK_BUTTON(gtk_builder_get_object(builder, "addcartButton"));
    validecartButton = GTK_BUTTON(gtk_builder_get_object(builder, "validecartButton"));
    cart_scrolled_window = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "scrolledWindow"));

    // Get the GtkTreeView widget with its columns
    tree_view_widget = create_gtk_tree_view(global_struct);

    // Add GtkTreeView to GtkScrolledWindow
    gtk_container_add(GTK_CONTAINER(cart_scrolled_window), tree_view_widget);

    // Destroy and quit the app when the close button is clicked
    g_signal_connect(global_struct->mainWindow, "destroy", (GCallback) destroy_and_quit, &global_struct);
    g_signal_connect(global_struct->scanproduct, "destroy", (GCallback) destroy_and_quit, &global_struct);

    // Connect buttons with callback functions
    g_signal_connect(validateButton, "clicked", (GCallback) GetLog, global_struct);
    g_signal_connect(showCart, "clicked", (GCallback) OpenCart, global_struct);
    g_signal_connect(addCart, "clicked", (GCallback) add_to_cart, global_struct);
    g_signal_connect(returnCart, "clicked", (GCallback) ReturnCart, global_struct);
    g_signal_connect(validecartButton, "clicked", (GCallback) send_cart, global_struct);


    gtk_widget_show_all(global_struct->mainWindow);

    gtk_main();

    return EXIT_SUCCESS;
}
