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
    GtkWidget *mainWindow = NULL;
    scanproduct = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *)pData, "scanWindow"));
    mainWindow = GTK_WIDGET (gtk_builder_get_object((GtkBuilder *)pData, "homeWindow"));
    gtk_widget_show_all(scanproduct);
    gtk_widget_hide(mainWindow);
}

void *OpenCart(GtkWidget *pWidget, gpointer pData)
{
    GtkWidget *cartResume = NULL;
    GtkWidget *scanproduct = NULL;
    cartResume = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *)pData, "cartWindow"));
    scanproduct = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *)pData, "scanWindow"));
    gtk_widget_show_all(cartResume);
    gtk_widget_hide(scanproduct);
}

void *ReturnCart (GtkWidget *pWidget, gpointer pData)
{

    GtkWidget *scanWindow = NULL;
    GtkWidget *cartWindow = NULL;

    scanWindow = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *)pData, "scanWindow"));
    cartWindow = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *)pData, "cartWindow"));
    gtk_widget_show_all(scanWindow);
    gtk_widget_hide(cartWindow);
}
