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
    // Parse GtkBuilder UI definition from gui.xml and merge it with the current contents of `builder`
    gtk_builder_add_from_file(builder, "../gui.xml", &error);
    if (error) {
        gint code = error->code;

        g_printerr("%s\n", error->message);

        g_error_free(error);

        return code;
    }

    // Init the global struct which will store data throughout the lifetime of the app
    global_struct = init_global_struct(builder);

    init_css();

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
    g_signal_connect(validateButton, "clicked", (GCallback) login_user, global_struct);
    g_signal_connect(showCart, "clicked", (GCallback) open_cart_window, global_struct);
    g_signal_connect(returnCart, "clicked", (GCallback) return_to_cart, global_struct);
    g_signal_connect(validecartButton, "clicked", (GCallback) send_cart, global_struct);

    // Validate login form by pressing enter
    // First, enable the login window (a GtkWidget) to receive key press events
    gtk_widget_add_events(global_struct->mainWindow, GDK_KEY_PRESS_MASK);
    // Then, for each keypress in this widget, call handle_keyboard_login()
    g_signal_connect (G_OBJECT (global_struct->mainWindow), "key_press_event", G_CALLBACK (handle_keyboard_login), global_struct);

    // Same, to add a product to the cart
    gtk_widget_add_events(global_struct->scanproduct, GDK_KEY_PRESS_MASK);
    g_signal_connect (G_OBJECT (global_struct->scanproduct), "key_press_event", G_CALLBACK (handle_keyboard_add_product), global_struct);


    gtk_widget_show_all(global_struct->mainWindow);

    gtk_main();

    return EXIT_SUCCESS;
}
