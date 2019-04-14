#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <glib/gprintf.h>
#include <tgmath.h>
#include "functions.h"

void *OnDestroy(GtkWidget *pWidget, AppliStruct *appliStruct)
{
    gtk_main_quit();
}

AppliStruct *InitStruct (AppliStruct *appliStruct, GtkBuilder* builder){
    appliStruct = malloc(sizeof(AppliStruct));
    if (appliStruct) {
        appliStruct->mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "homeWindow"));//Init mainWindow
        appliStruct->loginEntry = GTK_ENTRY(gtk_builder_get_object(builder, "idEntry"));
        appliStruct->pwdEntry = GTK_ENTRY(gtk_builder_get_object(builder, "pwdEntry"));
        appliStruct->scanproduct = GTK_WIDGET(gtk_builder_get_object(builder, "scanWindow"));
        appliStruct->authError = GTK_LABEL(gtk_builder_get_object(builder, "authError"));

    }else {
        printf("Memory not set");
        exit(1);
    }
    return appliStruct;
}

void *OpenScan(GtkWidget *valideButton, AppliStruct *appliStruct)
{
    gtk_widget_show_all(appliStruct->scanproduct);
    gtk_widget_hide(appliStruct->mainWindow);
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

int *GetBarreCode (GtkWidget *pwidget, gpointer pData){

    gchar *endPtr;

    const gchar *barrecode = gtk_entry_get_text(GTK_ENTRY(pData));

    long barrecode_value = strtol(barrecode, &endPtr, 10);

    printf ("%ld", barrecode_value);

}

int *GetQuantity (GtkWidget *pwidget, gpointer pData){

    gchar *endPtr;

    const gchar *quantity = gtk_entry_get_text(GTK_ENTRY(pData));

    long value_quantity = strtol(quantity, &endPtr, 10);

    printf ("\n%ld", value_quantity);

}

void *ErrorLog (GtkLabel *authError, AppliStruct *appliStruct){
    const gchar* errorAuth = "Login ou mot de passe invalide";
    gtk_label_get_text(appliStruct->authError);
    gtk_label_set_text(appliStruct->authError, errorAuth);



}
void *GetLog (GtkWidget *valideButton, AppliStruct *appliStruct){

    const gchar *log = gtk_entry_get_text(GTK_ENTRY(appliStruct->loginEntry));

    const gchar *pwd = gtk_entry_get_text(GTK_ENTRY(appliStruct->pwdEntry));
    const gchar *tab = "salut";

    if (log != NULL && pwd != NULL){
        if (strcmp( log, tab )== 0 && strcmp( pwd, tab )== 0){
            printf("les valeurs sont identiques");
            printf("%s\n", log);
            printf("%s\n", pwd);
            OpenScan(valideButton, appliStruct);

        }else{
            printf("Les valeurs sont incorrectes");
            ErrorLog(appliStruct->authError, appliStruct);


        };
    } else{
        printf("DEAD");

    }




}

/*void *GetPwd (GtkWidget *pWidget, gpointer pData) {

    const gchar *pwd = gtk_entry_get_text(GTK_ENTRY(pData));
    const gchar *tab = "salut";

    if (pwd != NULL){
        if (strcmp( pwd, tab )== 0){
            printf("les valeurs sont identiques");

        }else{
            printf("Les valeurs sont incorrectes");
        };
        printf("%s\n", pwd);

        return EXIT_SUCCESS;

    } else{
        printf("DEAD");

    }
    return 0;

}*/