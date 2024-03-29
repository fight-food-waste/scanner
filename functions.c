#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <curl/curl.h>
#include <jansson.h>

#include "functions.h"


#define RESULT_SIZE (256 * 1024)  /* CURL result, 256 KB*/
#define URL_SIZE 256
#define API_ENDPOINT "http://ffw.local/api"

/*
 * Clean memory and quit GTK
 */
int destroy_and_quit(GlobalStruct **global_struct) {
    gtk_main_quit();

    free(*global_struct);

    return EXIT_SUCCESS;
}

/*
 * Import CSS into GTK
 */
void init_css() {
    GError *error = NULL;

    GtkCssProvider *css_provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER (css_provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
    GFile *css_file = g_file_new_for_path("../style.css");
    gtk_css_provider_load_from_file(css_provider, css_file, &error);
    g_object_unref(css_file);

    if (error) {
        gint code = error->code;

        g_printerr("%s\n", error->message);

        g_error_free(error);
    }
}

/*
 * Initalize global struct
 */
GlobalStruct *init_global_struct(GtkBuilder *builder) {
    GlobalStruct *global_struct = (GlobalStruct *) malloc(sizeof(GlobalStruct));

    if (global_struct) {
        // Add objects to the global struct that we will need at multiple places
        global_struct->mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "homeWindow"));//Init mainWindow
        global_struct->loginEntry = GTK_ENTRY(gtk_builder_get_object(builder, "idEntry"));
        global_struct->pwdEntry = GTK_ENTRY(gtk_builder_get_object(builder, "pwdEntry"));
        global_struct->scanproduct = GTK_WIDGET(gtk_builder_get_object(builder, "scanWindow"));
        global_struct->authError = GTK_LABEL(gtk_builder_get_object(builder, "authError"));
        global_struct->scanLabel = GTK_LABEL(gtk_builder_get_object(builder, "scanLabel"));
        global_struct->barrecodeEntry = GTK_ENTRY(gtk_builder_get_object(builder, "barrecodeEntry"));
        global_struct->quantityEntry = GTK_ENTRY(gtk_builder_get_object(builder, "quantityEntry"));
        global_struct->expirationDateEntry = GTK_ENTRY(gtk_builder_get_object(builder, "expirationDateEntry"));
        global_struct->cartWindow = GTK_WIDGET(gtk_builder_get_object(builder, "cartWindow"));
        global_struct->list_store = init_list_store();
        global_struct->token = NULL;
        global_struct->bundle_id = 0;
    } else {
        exit(EXIT_FAILURE);
    }
    return global_struct;
}

/*
 * Validate form when return is pressed
 */
gboolean handle_keyboard_login(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GlobalStruct *global_struct = user_data;

    if (event->keyval == GDK_KEY_Return) {

        login_user(NULL, global_struct);

        return TRUE;
    }
    return FALSE;
}

/*
 * Add product to cart when return is pressed
 */
gboolean handle_keyboard_add_product(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

    GlobalStruct *global_struct = user_data;

    if (event->keyval == GDK_KEY_Return) {

        add_to_cart(NULL, global_struct);

        return TRUE;
    }
    return FALSE;
}

/*
 * Open scanner window
 */
int open_scanner_window(GtkWidget *widget, GlobalStruct *global_struct) {
    gtk_widget_show_all(global_struct->scanproduct);
    gtk_widget_hide(global_struct->mainWindow);

    return EXIT_SUCCESS;
}

/*
 * Open cart window
 */
int open_cart_window(GtkWidget *widget, GlobalStruct *global_struct) {
    gtk_widget_show_all(global_struct->cartWindow);
    gtk_widget_hide(global_struct->scanproduct);

    return EXIT_SUCCESS;
}

/*
 * Return to scan window from the cart window
 */
int return_to_cart(GtkWidget *widget, GlobalStruct *global_struct) {
    gtk_widget_show_all(global_struct->scanproduct);
    gtk_widget_hide(global_struct->cartWindow);

    return EXIT_SUCCESS;
}

/*
 * Callback function called by CURL
 * Copies `size * nmemb` bytes from the `buffer` to the `write_result` struct (stream)
 * (`size` is always = 1 so nmemb is basically the chunk size)
 */
static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream) {
    int chunk_size = size * nmemb;

    // Get pointer to stream (final result)
    struct write_result *result = (struct write_result *) stream;

    // Final result should be <= RESULT_SIZE
    if (result->pos + chunk_size >= RESULT_SIZE - 1) {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    // Copy `chunk_size` bytes from `buffer` to `result->data` at `result->pos` position of the array
    memcpy(result->data + result->pos, buffer, chunk_size);
    // Increment ending position by the number of copied bytes
    result->pos += chunk_size;

    // Returns number of copied bytes
    return chunk_size;
}

/*
 * Get token from API and open scanner window
 */
int login_user(GtkWidget *valideButton, GlobalStruct *global_struct) {

    const gchar *log = gtk_entry_get_text(global_struct->loginEntry);
    const gchar *pwd = gtk_entry_get_text(global_struct->pwdEntry);

    CURLcode curl_code;

    global_struct->token = get_token(log, pwd, &curl_code);

    if (global_struct->token) {

        char *user_name = NULL;

        user_name = get_user_name(global_struct->token);

        char user_welcome_msg[100];
        sprintf(user_welcome_msg, "Welcome, %s", user_name);

        gtk_label_set_text(global_struct->scanLabel, user_welcome_msg);

        open_scanner_window(valideButton, global_struct);

        return EXIT_SUCCESS;
    } else if (curl_code == CURLE_COULDNT_CONNECT) {
        const gchar *errorAuth = "Couldn't connect to API.";
        gtk_label_set_text(global_struct->authError, errorAuth);

        return EXIT_FAILURE;
    } else if (curl_code == CURLE_OK) {
        const gchar *errorAuth = "Wrong credentials.";
        gtk_label_set_text(global_struct->authError, errorAuth);

        return EXIT_FAILURE;
    } else {
        const gchar *errorAuth = "Something went wrong.";
        gtk_label_set_text(global_struct->authError, errorAuth);

        return EXIT_FAILURE;
    }
}

/*
 * Get the user's first name from the API
 */
char *get_user_name(char *token) {
    CURLcode curl_code;
    struct curl_slist *http_headers;
    long http_code = 0;
    char *user_name = NULL;

    char token_header[100];

    sprintf(token_header, "Authorization: Bearer %s", token);

    http_headers = NULL;
    // Add header to list of strings
    http_headers = curl_slist_append(http_headers, "content-type: application/x-www-form-urlencoded");
    http_headers = curl_slist_append(http_headers, "Accept: application/json");
    http_headers = curl_slist_append(http_headers, token_header);

    char *data = NULL; // HTTP document

    // Final result has to be <= RESULT_SIZE
    data = malloc(RESULT_SIZE);
    if (!data)
        goto error;

    // Struct used to build the final result (`data`)
    struct write_result write_result = {
            .data = data,
            .pos = 0
    };

    CURL *curl_handle;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, API_ENDPOINT
            "/user");
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.54.0");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, http_headers);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_response); // Defines callback function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &write_result);

    curl_code = curl_easy_perform(curl_handle);
    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(http_headers);

    if (http_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK) {
        // Properly end string
        data[write_result.pos] = '\0';

        json_t *json_token;
        json_error_t error;

        json_token = json_loads(data, 0, &error);
        free(data);

        if (!json_token) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return NULL;
        }

        json_unpack(json_token, "{s:s}", "first_name", &user_name);
        if (!user_name) {
            fprintf(stderr, "error: product.product_name was not found\n");
            return NULL;
        }

        // It seems the string can be randomly NOT UTF-8.
        // This converts it to UTF-8 from whatever locale it's using.
        user_name = g_locale_to_utf8(user_name, -1, NULL, NULL, NULL);


        // Free json_token
        json_decref(json_token);

        return user_name;
    } else {
        goto error;
    }

    error:
    // Clean memory
    if (data)
        free(data);
    curl_global_cleanup();
    return NULL;
}


/*
 * Get the user token from the API
 */
char *get_token(const gchar *email, const gchar *password, CURLcode *curl_code) {

    struct curl_slist *http_headers;
    long http_code = 0;
    gchar *token = NULL;

    http_headers = NULL;
    // Add header to list of strings
    http_headers = curl_slist_append(http_headers, "content-type: application/x-www-form-urlencoded");
    http_headers = curl_slist_append(http_headers, "Accept: application/json");

    char body[256];
    sprintf(body, "email=%s&password=%s", email, password);

    char *data = NULL; // HTTP document

    // Final result has to be <= RESULT_SIZE
    data = malloc(RESULT_SIZE);
    if (!data)
        goto error;

    // Struct used to build the final result (`data`)
    struct write_result write_result = {
            .data = data,
            .pos = 0
    };

    CURL *curl_handle;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, API_ENDPOINT
            "/login");
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.54.0");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, http_headers);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_response); // Defines callback function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &write_result);

    *curl_code = curl_easy_perform(curl_handle);
    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(http_headers);

    if (http_code == 200 && *curl_code != CURLE_ABORTED_BY_CALLBACK) {
        // Properly end string
        data[write_result.pos] = '\0';

        json_t *json_token;
        json_error_t error;

        json_token = json_loads(data, 0, &error);
        free(data);

        if (!json_token) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return NULL;
        }

        json_unpack(json_token, "{s:s}", "token", &token);
        if (!token) {
            fprintf(stderr, "error: product.product_name was not found\n");
            return NULL;
        }

        // It seems the string can be randomly NOT UTF-8.
        // This converts it to UTF-8 from whatever locale it's using.
        token = g_locale_to_utf8(token, -1, NULL, NULL, NULL);

        // Free json_token
        json_decref(json_token);

        return token;
    } else {
        goto error;
    }

    error:
    // Clean memory
    if (data)
        free(data);
    if (curl_handle)
        curl_global_cleanup();
    return NULL;
}

/*
 * Add a row for a product in the cart
 */
int add_product_to_cart(GlobalStruct *global_struct, product product) {

    GtkTreeIter iter;

    gtk_list_store_append(global_struct->list_store, &iter);
    gtk_list_store_set(global_struct->list_store, &iter,
                       BARCODE_COLUMN, (glong) product.barcode,
                       NAME_COLUMN, (gchar *) product.name,
                       QTY_COLUMN, (glong) product.quantity,
                       DATE_COLUMN, (gchar *) product.expiration_date,
                       -1);

    return EXIT_SUCCESS;
}

/*
 * Init the GtkListStore that will be used to store products
 */
GtkListStore *init_list_store() {
    // Create new GtkListStore with typed columns
    GtkListStore *list_store = gtk_list_store_new(N_COLUMNS,      // 3
                                                  G_TYPE_LONG,    // quantity
                                                  G_TYPE_STRING,  // name
                                                  G_TYPE_LONG,    // barcode
                                                  G_TYPE_STRING); // expiration date

    return list_store;
}

/*
 * Init the GtkTreeView widget that will be used to display the GtkListStore
 *
 */
GtkWidget *create_gtk_tree_view(GlobalStruct *global_struct) {

    // Create new empty columns
    GtkTreeViewColumn *barcode_column, *name_column, *quantity_column, *date_column;
    // Create a new GtkCellRendererText to render text in a cell
    GtkCellRenderer *cell_renderer = gtk_cell_renderer_text_new();

    GtkWidget *tree_view_widget;

    // Create a new GtkTreeView widget with the model initialized to the GtkListStore
    tree_view_widget = gtk_tree_view_new_with_model(GTK_TREE_MODEL(global_struct->list_store));

    // Fill the GtkTreeViewColumns
    barcode_column = gtk_tree_view_column_new_with_attributes("Barcode", cell_renderer, "text",
                                                              BARCODE_COLUMN, NULL);
    name_column = gtk_tree_view_column_new_with_attributes("Name", cell_renderer, "text",
                                                           NAME_COLUMN, NULL);
    quantity_column = gtk_tree_view_column_new_with_attributes("Quantity", cell_renderer, "text",
                                                               QTY_COLUMN, NULL);
    date_column = gtk_tree_view_column_new_with_attributes("Expiration date", cell_renderer, "text",
                                                           DATE_COLUMN, NULL);

    // Append columns to the list of columns aka the GtkTreeView
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view_widget), barcode_column);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view_widget), name_column);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view_widget), quantity_column);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view_widget), date_column);

    return tree_view_widget;
}

/*
 * Extract product info from the GtkEntry
 * Get product name from the OFF API
 * Call function to add the product to the cart
 */
int add_to_cart(GtkWidget *widget, GlobalStruct *global_struct) {

    struct product current_product;
    gchar *endPtr; // for strol

    const gchar *raw_code = gtk_entry_get_text(global_struct->barrecodeEntry);
    const gchar *raw_quantity = gtk_entry_get_text(global_struct->quantityEntry);
    const gchar *raw_expiration_date = gtk_entry_get_text(global_struct->expirationDateEntry);

    // strol returns the converted int as a long int, else 0 is returned.
    current_product.barcode = strtol(raw_code, &endPtr, 10);
    current_product.quantity = strtol(raw_quantity, &endPtr, 10);
    current_product.expiration_date = (char *) raw_expiration_date;

    if (current_product.barcode == 0 || current_product.quantity == 0 || current_product.expiration_date == 0) {
        return EXIT_FAILURE;
    }

    // Add name and image_url to product struct
    if (get_product_info(&current_product) == EXIT_SUCCESS) {

        // Add to product to cart
        add_product_to_cart(global_struct, current_product);

        // Reset GtkEntries
        gtk_entry_set_text(global_struct->barrecodeEntry, "");
        gtk_entry_set_text(global_struct->quantityEntry, "");
        gtk_entry_set_text(global_struct->expirationDateEntry, "");

        // Set focus on the barcode GtkEntry
        gtk_widget_grab_focus(GTK_WIDGET(global_struct->barrecodeEntry));

        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

/*
 * Get the product's name and add it the struct
 */
int get_product_info(product *product) {
    char *http_document;
    char url[URL_SIZE];

    json_t *json_document;
    json_error_t error;

    // Build URL
    snprintf(url, URL_SIZE, "https://world.openfoodfacts.org/api/v0/product/%ld.json", product->barcode);

    // Get JSON from API with CURL
    http_document = http_get(url);
    if (!http_document)
        return EXIT_FAILURE;

    json_document = json_loads(http_document, 0, &error);
    free(http_document);

    if (!json_document) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return EXIT_FAILURE;
    }

    // Open Food Facts returns a JSON object
    if (!json_is_object(json_document)) {
        fprintf(stderr, "error: json_document is not an object (JSON type error)\n");
        json_decref(json_document);
        return EXIT_FAILURE;
    }

    // We'll extract these value from json_document
    json_t *product_json;
    int product_status;

    // Check status property of the json_document
    json_unpack(json_document, "{s:i}", "status", &product_status);
    // 1 is found, 0 is not found
    if (product_status != 1) {
        fprintf(stderr, "error: product not found.\n");
        return EXIT_FAILURE;
    }

    // Get product properties
    product_json = json_object_get(json_document, "product");
    if (!json_is_object(product_json)) {
        fprintf(stderr, "error: product is not an object\n");
        return EXIT_FAILURE;
    }

    json_unpack(product_json, "{s:s}", "product_name", &product->name);
    if (!product->name) {
        fprintf(stderr, "error: product.product_name was not found\n");
        return EXIT_FAILURE;
    }
    // It seems the string can be randomly NOT UTF-8.
    // This converts it to UTF-8 from whatever locale it's using.
    product->name = g_locale_to_utf8(product->name, -1, NULL, NULL, NULL);

    // Free json_document
    json_decref(json_document);
    return EXIT_SUCCESS;
}

/*
 * Generic CURL wrapper to make a GET HTTP request.
 * Returns the HTTP document
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

/*
 * Create and bundle, iterate trough the cart, send each product and close bundle
 */
char *send_cart(GtkWidget *widget, GlobalStruct *global_struct) {

    global_struct->bundle_id = create_bundle(global_struct->token);

    if (global_struct->bundle_id == 0) {
        // error
        return NULL;
    }

    // Iterate trough the GtkTreeModel, aka the list's rows
    gtk_tree_model_foreach(GTK_TREE_MODEL(global_struct->list_store), send_product_from_model, global_struct);

    // Clear cart
    gtk_list_store_clear(global_struct->list_store);

    return NULL;
}

/*
 * Create a bundle via the API and returns its id
 */
int create_bundle(char *token) {
    CURLcode curl_code;
    struct curl_slist *http_headers;
    long http_code = 0;
    int bundle_id = 0;

    char token_header[100];

    sprintf(token_header, "Authorization: Bearer %s", token);

    http_headers = NULL;
    // Add header to list of strings
    http_headers = curl_slist_append(http_headers, "content-type: application/x-www-form-urlencoded");
    http_headers = curl_slist_append(http_headers, "Accept: application/json");
    http_headers = curl_slist_append(http_headers, token_header);

    char *data = NULL; // HTTP document

    // Final result has to be <= RESULT_SIZE
    data = malloc(RESULT_SIZE);
    if (!data)
        goto error;

    // Struct used to build the final result (`data`)
    struct write_result write_result = {
            .data = data,
            .pos = 0
    };

    CURL *curl_handle;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, API_ENDPOINT
            "/bundles");
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.54.0");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, http_headers);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_response); // Defines callback function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &write_result);

    curl_code = curl_easy_perform(curl_handle);
    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(http_headers);

    if (http_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK) {
        // Properly end string
        data[write_result.pos] = '\0';

        json_t *json_bundle;
        json_error_t error;

        json_bundle = json_loads(data, 0, &error);
        free(data);

        if (!json_bundle) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return 0;
        }

        json_unpack(json_bundle, "{s:i}", "id", &bundle_id);
        if (!bundle_id) {
            fprintf(stderr, "error: product.product_name was not found\n");
            return 0;
        }

        // Free json_bundle
        json_decref(json_bundle);

        return bundle_id;
    } else {
        goto error;
    }

    error:
    // Clean memory
    if (data)
        free(data);
    curl_global_cleanup();
    return 0;
}

/*
 * Get product from model and send it to the API
 */
gboolean send_product_from_model(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data) {
    product current_product;
    GlobalStruct *global_struct = user_data;

    // Get product from model
    gtk_tree_model_get(model, iter,
                       BARCODE_COLUMN, &current_product.barcode,
                       NAME_COLUMN, &current_product.name,
                       QTY_COLUMN, &current_product.quantity,
                       DATE_COLUMN, &current_product.expiration_date,
                       -1);

    send_product(global_struct, current_product);

    return FALSE;
}

/*
 * Send product to API
 */
int send_product(GlobalStruct *global_struct, product product) {

    CURLcode curl_code;
    struct curl_slist *http_headers;
    long http_code = 0;
    char *data = NULL; // HTTP document
    http_headers = NULL;
    char body[256];

    char token_header[100];
    sprintf(token_header, "Authorization: Bearer %s", global_struct->token);

    // Add header to list of strings
    http_headers = curl_slist_append(http_headers, "content-type: application/x-www-form-urlencoded");
    http_headers = curl_slist_append(http_headers, "Accept: application/json");
    http_headers = curl_slist_append(http_headers, token_header);

    sprintf(body, "name=%s&barcode=%ld&quantity=%ld&bundle_id=%d&expiration_date=%s", product.name,
            product.barcode, product.quantity, global_struct->bundle_id, product.expiration_date);

    CURL *curl_handle;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, API_ENDPOINT
            "/products");
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.54.0");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, http_headers);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");

    curl_code = curl_easy_perform(curl_handle);
    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(http_headers);


    if (http_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK) {

        return EXIT_SUCCESS;
    } else {
        goto error;
    }

    error:
    // Clean memory
    if (data)
        free(data);
    if (curl_handle)
        curl_global_cleanup();
    return EXIT_FAILURE;
}