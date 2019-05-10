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
#define API_ENDPOINT "http://localhost:3000"


int OnDestroy(GtkWidget *pWidget, GlobalStruct *global_struct) {
    gtk_main_quit();

    return EXIT_SUCCESS;
}

GlobalStruct *InitStruct(GlobalStruct *global_struct, GtkBuilder *builder) {
    global_struct = malloc(sizeof(GlobalStruct));
    if (global_struct) {
        global_struct->mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "homeWindow"));//Init mainWindow
        global_struct->loginEntry = GTK_ENTRY(gtk_builder_get_object(builder, "idEntry"));
        global_struct->pwdEntry = GTK_ENTRY(gtk_builder_get_object(builder, "pwdEntry"));
        global_struct->scanproduct = GTK_WIDGET(gtk_builder_get_object(builder, "scanWindow"));
        global_struct->authError = GTK_LABEL(gtk_builder_get_object(builder, "authError"));
        global_struct->barrecodeEntry = GTK_ENTRY(gtk_builder_get_object(builder, "barrecodeEntry"));
        global_struct->quantityEntry = GTK_ENTRY(gtk_builder_get_object(builder, "quantityEntry"));
        global_struct->cartWindow = GTK_WIDGET(gtk_builder_get_object(builder, "cartWindow"));
        global_struct->listStore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore"));
        global_struct->scrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "scrolledWindow"));

    } else {
        printf("Memory not set\n");
        exit(1);
    }
    return global_struct;
}

int OpenScan(GtkWidget *valideButton, GlobalStruct *global_struct) {
    gtk_widget_show_all(global_struct->scanproduct);
    gtk_widget_hide(global_struct->mainWindow);

    return EXIT_SUCCESS;
}

void *OpenCart(GtkWidget *pWidget, GlobalStruct *global_struct) {
    gtk_widget_show_all(global_struct->cartWindow);
    gtk_widget_hide(global_struct->scanproduct);

    return EXIT_SUCCESS;
}


void *ReturnCart(GtkWidget *pWidget, GlobalStruct *global_struct) {
    gtk_widget_show_all(global_struct->scanproduct);
    gtk_widget_hide(global_struct->cartWindow);

    return EXIT_SUCCESS;
}


int ErrorLog(GtkLabel *authError, GlobalStruct *global_struct) {
    const gchar *errorAuth = "Login ou mot de passe invalide";
    gtk_label_get_text(global_struct->authError);
    gtk_label_set_text(global_struct->authError, errorAuth);

    return EXIT_SUCCESS;
}

int GetLog(GtkWidget *valideButton, GlobalStruct *global_struct) {
//    Bypass login
    OpenScan(valideButton, global_struct);

    const gchar *log = gtk_entry_get_text(GTK_ENTRY(global_struct->loginEntry));
    const gchar *pwd = gtk_entry_get_text(GTK_ENTRY(global_struct->pwdEntry));

    // CURL

    CURLcode curl_code;
    struct curl_slist *http_headers;
    long http_code = 0;

    http_headers = NULL;
    // Add header to list of strings
    http_headers = curl_slist_append(http_headers, "content-type: application/x-www-form-urlencoded");

    char body[256];
    sprintf(body, "email=%s&password=%s", log, pwd);

    CURL *curl_handle;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, API_ENDPOINT
            "/auth");
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.54.0");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, http_headers);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");

    curl_code = curl_easy_perform(curl_handle);
    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(http_headers);

    printf("\n%d\n", (int) curl_code);

    if (http_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK) {
        OpenScan(valideButton, global_struct);

        return EXIT_SUCCESS;
    } else {
        ErrorLog(global_struct->authError, global_struct);

        return EXIT_FAILURE;
    }
}

/*
 * Set product name and quantity in the cart
 */
int AddProduct(GlobalStruct *global_struct, product product) {

    GtkTreeIter iter;

    gtk_list_store_append(global_struct->listStore, &iter);
    gtk_list_store_set(global_struct->listStore, &iter,
                       QTY_COLUMN, (glong) product.quantity,
                       NAME_COLUMN, (gchar *) product.name,
                       -1);

    return EXIT_SUCCESS;
}

/*
 * Init the GtkTreeView
 */
GtkTreeView *createView(GlobalStruct *global_struct) {
    // Create new GtkListStore with types columns
    global_struct->listStore = gtk_list_store_new(N_COLUMNS,      // 2
                                                G_TYPE_LONG,    // quantity
                                                G_TYPE_STRING); // name

    // Creates a new GtkTreeView widget with the model initialized to global_struct->listStore
    global_struct->listView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(global_struct->listStore));

    // Creates a new GtkCellRendererText
    global_struct->cellRenderer = gtk_cell_renderer_text_new();
    // Creates GtkTreeViewColumn for "Quantity"/QTY_COLUMN
    global_struct->pColumn = gtk_tree_view_column_new_with_attributes("Quantity", global_struct->cellRenderer, "text",
                                                                    QTY_COLUMN, NULL);
    // Appends global_struct->pColumn to the list of columns
    gtk_tree_view_append_column(GTK_TREE_VIEW(global_struct->listView), global_struct->pColumn);

    // Repeat for "Name"/NAME_COLUMN
    global_struct->cellRenderer = gtk_cell_renderer_text_new();
    global_struct->pColumn = gtk_tree_view_column_new_with_attributes("Name", global_struct->cellRenderer, "text",
                                                                    NAME_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(global_struct->listView), global_struct->pColumn);

    // Add GtkTreeView to GtkScrolledWindow
    gtk_container_add(GTK_CONTAINER(global_struct->scrolledWindow), global_struct->listView);

    return EXIT_SUCCESS;
}


/*
 * Extract product values from GTK,
 * and prepare to add product to cart
 */
int add_to_cart(GtkWidget *addCart, GlobalStruct *global_struct) {

    struct product current_product;
    gchar *endPtr; // for strol

    const gchar *raw_code = gtk_entry_get_text(GTK_ENTRY(global_struct->barrecodeEntry));
    const gchar *raw_quantity = gtk_entry_get_text(GTK_ENTRY(global_struct->quantityEntry));

    // strol returns the converted int as a long int, else 0 is returned.
    current_product.code = strtol(raw_code, &endPtr, 10);
    current_product.quantity = strtol(raw_quantity, &endPtr, 10);

    if (current_product.code == 0 || current_product.quantity == 0) {
        return EXIT_FAILURE;
    } else {
        printf("%ld\n", current_product.code);
        printf("%ld\n", current_product.quantity);
    }

    // Add name and image_url to product struct
    if (get_product_info(&current_product) == EXIT_SUCCESS) {
        AddProduct(global_struct, current_product);
        printf("success");
        return EXIT_SUCCESS;
    } else {
        printf("fail");
        return EXIT_FAILURE;
    }
}

/*
 * Add name and image_url to product struct
 */

int get_product_info(product *product) {
    char *http_document;
    char url[URL_SIZE];

    json_t *json_document;
    json_error_t error;

    // Build URL
    snprintf(url, URL_SIZE, "https://world.openfoodfacts.org/api/v0/product/%ld.json", product->code);

    // Get JSON from API with CURL
    http_document = http_get(url);
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
        fprintf(stderr, "error: json_document is not an object (JSON type error)\n");
        json_decref(json_document);
        return 1;
    }

    // We'll extract these value from json_document
    json_t *product_json;
    int product_status;
//    char *product_name, *product_image_url = NULL;

    // Check status property of the json_document
    json_unpack(json_document, "{s:i}", "status", &product_status);
    // 1 is found, 0 is not found
    if (product_status != 1) {
        fprintf(stderr, "error: product not found.\n");
        return 1;
    }

    // Get product properties
    product_json = json_object_get(json_document, "product");
    if (!json_is_object(product_json)) {
        fprintf(stderr, "error: product is not an object\n");
        return 1;
    }

    json_unpack(product_json, "{s:s}", "product_name", &product->name);
    if (!product->name) {
        fprintf(stderr, "error: product.product_name was not found\n");
        return 1;
    }
    // It seems the string can be randomly NOT UTF-8.
    // This converts it to UTF-8 from whatever locale it's using.
    product->name = g_locale_to_utf8(product->name, -1, NULL, NULL, NULL);

    json_unpack(product_json, "{s:s}", "image_url", &product->image_url);
    if (!product->image_url) {
        fprintf(stderr, "error: product.product_image_url was not found\n");
        return 1;
    }

    printf("Product name: %s\n", product->name);
    printf("Product image: %s\n", product->image_url);


    // Free json_document
    json_decref(json_document);
    return 0;
}


/*
 * Struct used to build the result of the CURL http_get
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
 * Return document from GET HTTP request
 */
static char *http_get(const char *url) {
    // CURL init
    CURL *curl = NULL;
    CURLcode status; // Status of the CURL http_get
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

    // Make HTTP http_get
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
