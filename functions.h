#include <gtk/gtk.h>
#include <curl/curl.h>

typedef struct GlobalStruct GlobalStruct;
struct GlobalStruct {
    GtkEntry *loginEntry;
    GtkEntry *pwdEntry;
    GtkWidget *mainWindow;
    GtkWidget *scanproduct;
    GtkLabel *authError;
    GtkEntry *barrecodeEntry;
    GtkEntry *quantityEntry;
    GtkEntry *expirationDateEntry;
    GtkWidget *cartWindow;
    GtkListStore *list_store;
    char *token;
    GtkLabel *scanLabel;
    int bundle_id;
};

// Enum for GtkTreeView
enum {
    BARCODE_COLUMN,
    NAME_COLUMN,
    QTY_COLUMN,
    DATE_COLUMN,
    N_COLUMNS
};

/*
 * Use struct to pass around the product properties
 */
typedef struct product product;

struct product {
    long barcode;
    long quantity;
    char *name;
    char *expiration_date;
};

/*
 * Struct used to build the result of the CURL http_get
 * Contains a string (final data) and position/size of the string
 */
struct write_result {
    char *data;
    int pos;
};

void init_css();

GlobalStruct *init_global_struct(GtkBuilder *);

gboolean handle_keyboard_login(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

int add_product_to_cart(GlobalStruct *global_struct, product product);

int get_product_info(product *product);

int destroy_and_quit(GlobalStruct **);

int open_scanner_window(GtkWidget *widget, GlobalStruct *global_struct);

int open_cart_window(GtkWidget *widget, GlobalStruct *global_struct);

int return_to_cart(GtkWidget *widget, GlobalStruct *global_struct);

GtkWidget *create_gtk_tree_view(GlobalStruct *global_struct);

int login_user(GtkWidget *valideButton, GlobalStruct *global_struct);

static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream);

static char *http_get(const char *url);

int add_to_cart(GtkWidget *addCart, GlobalStruct *global_struct);

char *get_token(const gchar *email, const gchar *password, CURLcode *curl_code);

char *get_user_name(char *token);

char *send_cart(GtkWidget *, GlobalStruct *);

int create_bundle(char *token);

gboolean send_product_from_model(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data);

int send_product(GlobalStruct *global_struct, product product);

GtkListStore *init_list_store();

gboolean handle_keyboard_add_product(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
