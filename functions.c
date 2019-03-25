#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include "functions.h"

void *OnDestroy(GtkWidget *pWidget, gpointer pData)
{
    gtk_main_quit();
}

void *OpenScan(GtkWidget *pWidget, gpointer pData)
{
    GtkWidget *scanproduct = NULL;
    scanproduct = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *)pData, "scanWindow"));
    gtk_widget_show_all(scanproduct);
}

void *OpenCart(GtkWidget *pWidget, gpointer pData)
{
    GtkWidget *cartResume = NULL;
    cartResume = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *)pData, "cartWindow"));
    gtk_widget_show_all(cartResume);
}
