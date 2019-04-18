#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <curl/curl.h>
#include <jansson.h>

#include "functions.h"


#define RESULT_SIZE (256 * 1024)  /* CURL result, 256 KB*/
#define URL_SIZE 256


void *OnDestroy(GtkWidget *pWidget, AppliStruct *appliStruct) {
    gtk_main_quit();
}

AppliStruct *InitStruct(AppliStruct *appliStruct, GtkBuilder *builder) {
    appliStruct = malloc(sizeof(AppliStruct));
    if (appliStruct) {
        appliStruct->mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "homeWindow"));//Init mainWindow
        appliStruct->loginEntry = GTK_ENTRY(gtk_builder_get_object(builder, "idEntry"));
        appliStruct->pwdEntry = GTK_ENTRY(gtk_builder_get_object(builder, "pwdEntry"));
        appliStruct->scanproduct = GTK_WIDGET(gtk_builder_get_object(builder, "scanWindow"));
        appliStruct->authError = GTK_LABEL(gtk_builder_get_object(builder, "authError"));

    } else {
        printf("Memory not set\n");
        exit(1);
    }
    return appliStruct;
}

void *OpenScan(GtkWidget *valideButton, AppliStruct *appliStruct) {
    gtk_widget_show_all(appliStruct->scanproduct);
    gtk_widget_hide(appliStruct->mainWindow);
}

void *OpenCart(GtkWidget *pWidget, gpointer pData) {
    GtkWidget *cartResume = NULL;
    GtkWidget *scanproduct = NULL;

    cartResume = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *) pData, "cartWindow"));
    scanproduct = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *) pData, "scanWindow"));

    gtk_widget_show_all(cartResume);
    gtk_widget_hide(scanproduct);
}

void *ReturnCart(GtkWidget *pWidget, gpointer pData) {

    GtkWidget *scanWindow = NULL;
    GtkWidget *cartWindow = NULL;

    scanWindow = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *) pData, "scanWindow"));
    cartWindow = GTK_WIDGET(gtk_builder_get_object((GtkBuilder *) pData, "cartWindow"));

    gtk_widget_show_all(scanWindow);
    gtk_widget_hide(cartWindow);
}

int *GetBarCode(GtkWidget *pwidget, gpointer pData) {

    gchar *endPtr;
    const gchar *barrecode = gtk_entry_get_text(GTK_ENTRY(pData));
//    long barrecode_value = strtol(barrecode, &endPtr, 10);

//    printf("%ld", barrecode_value);

    char *http_document;
    char url[URL_SIZE];

    json_t *json_document;
    json_error_t error;

    // Build URL
    snprintf(url, URL_SIZE, "https://world.openfoodfacts.org/api/v0/product/%s.json", barrecode);

    // Get JSON from API with CURL
    http_document = request(url);
    if (!http_document)
        return 1;

    json_document = json_loads(http_document, 0, &error);
    free(http_document);

    if (!json_document) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return 1;
    }

    // Open Food Facts returns a JSON object
    if (!json_is_object(json_document)) {
        fprintf(stderr, "error: json_document is not an oject (JSON type error)\n");
        json_decref(json_document);
        return 1;
    }

    // We'll extract these value from json_document
    json_t *product;
    int product_status;
    char *product_name, *product_image_url = NULL;

    // Check status property of the json_document
    json_unpack(json_document, "{s:i}", "status", &product_status);
    // 1 is found, 0 is not found
    if (product_status != 1) {
        fprintf(stderr, "error: product not found.\n");
        return 1;
    }

    // Get product properties
    product = json_object_get(json_document, "product");
    if (!json_is_object(product)) {
        fprintf(stderr, "error: product is not an object\n");
        return 1;
    }

    json_unpack(product, "{s:s}", "product_name", &product_name);
    if (!product_name) {
        fprintf(stderr, "error: product.product_name was not found\n");
        return 1;
    }
    json_unpack(product, "{s:s}", "image_url", &product_image_url);
    if (!product_image_url) {
        fprintf(stderr, "error: product.product_image_url was not found\n");
        return 1;
    }

    printf("Product name: %s\n", product_name);
    printf("Product image: %s\n", product_image_url);

    // Free json_document
    json_decref(json_document);
    return 0;
}

int *GetQuantity(GtkWidget *pwidget, gpointer pData) {

    gchar *endPtr;
    const gchar *quantity = gtk_entry_get_text(GTK_ENTRY(pData));
    long value_quantity = strtol(quantity, &endPtr, 10);

    printf("%ld\n", value_quantity);

}

void *ErrorLog(GtkLabel *authError, AppliStruct *appliStruct) {
    const gchar *errorAuth = "Login ou mot de passe invalide";
    gtk_label_get_text(appliStruct->authError);
    gtk_label_set_text(appliStruct->authError, errorAuth);
}

void *GetLog(GtkWidget *valideButton, AppliStruct *appliStruct) {

    const gchar *log = gtk_entry_get_text(GTK_ENTRY(appliStruct->loginEntry));
    const gchar *pwd = gtk_entry_get_text(GTK_ENTRY(appliStruct->pwdEntry));
    const gchar *tab = "salut";

    // CURL

    CURLcode ret;
    CURL *hnd;
    struct curl_slist *slist1;

    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "content-type: application/x-www-form-urlencoded");

    char body[256];
    sprintf(body, "email=%s&password=%s", log, pwd);

    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_URL, "http://localhost:3000/auth");
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.54.0");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");

    ret = curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);
    hnd = NULL;
    curl_slist_free_all(slist1);
    slist1 = NULL;

    //printf ("%d\n", (int) ret);

    // end CURL

    if (log != NULL && pwd != NULL) {
        if (strcmp(log, tab) == 0 && strcmp(pwd, tab) == 0) {
            printf("les valeurs sont identiques\n");
            printf("%s\n", log);
            printf("%s\n", pwd);
            OpenScan(valideButton, appliStruct);
        } else {
            printf("Les valeurs sont incorrectes\n");
            ErrorLog(appliStruct->authError, appliStruct);
        }
    } else {
        printf("DEAD\n");
    }
}

/*
 * Struc used to build the result of the CURL request
 * Contains a string (final data) and position/size of the string
 */
struct write_result {
    char *data;
    int pos;
};

/*
 * Callback function called by CURL
 * Copies `size * nmemb` bytes from the `buffer` to the `write_result` struct (stream)
 * (`size` is always = 1)
 */
static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream) {
    // Get pointer to stream (final result)
    struct write_result *result = (struct write_result *) stream;

    // Final result should be <= RESULT_SIZE
    if (result->pos + size * nmemb >= RESULT_SIZE - 1) {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    // Copy `size * nmemb` bytes from `buffer` to `result->data` at `result->pos` position of the array
    memcpy(result->data + result->pos, buffer, size * nmemb);
    // Increment ending position by the number of copied bytes
    result->pos += size * nmemb;

    // Returns number of copied bytes
    return size * nmemb;
}

/*
 * Takes an URL and returns the HTTP document
 */
static char *request(const char *url) {
    // CURL init
    CURL *curl = NULL;
    CURLcode status; // Status of the CURL request
    char *data = NULL; // HTTP document
    long code; // HTTP response code

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl)
        goto error;

    // Final result has to be <= RESULT_SIZE
    data = malloc(RESULT_SIZE);
    if (!data)
        goto error;

    // Struct used to build the final result (`data`)
    struct write_result write_result = {
            .data = data,
            .pos = 0
    };

    // Set CURL parameters
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response); // Defines callback function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);

    // Make HTTP request
    status = curl_easy_perform(curl);
    if (status != 0) {
        fprintf(stderr, "error: unable to request data from %s:\n", url);
        fprintf(stderr, "%s\n", curl_easy_strerror(status));
        goto error;
    }

    // Check HTTP response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if (code != 200) {
        fprintf(stderr, "error: server responded with code %ld\n", code);
        goto error;
    }

    // Clean memory
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    // Properly end string
    data[write_result.pos] = '\0';

    // Returns HTTP document
    return data;

    error:
    // Clean memory
    if (data)
        free(data);
    if (curl)
        curl_easy_cleanup(curl);
    curl_global_cleanup();
    return NULL;
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