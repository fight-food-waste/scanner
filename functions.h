#include <gtk/gtk.h>

typedef struct GlobalStruct GlobalStruct;
struct GlobalStruct {
    GtkEntry *loginEntry;
    GtkEntry *pwdEntry;
    GtkWidget *mainWindow;
    GtkWidget *scanproduct;
    GtkLabel *authError;
    GtkEntry *barrecodeEntry;
    GtkEntry *quantityEntry;
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
};

/*
 * Struct used to build the result of the CURL http_get
 * Contains a string (final data) and position/size of the string
 */
struct write_result {
    char *data;
    int pos;
};

GlobalStruct *init_global_struct(GtkBuilder *);

gboolean handle_keyboard_login(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

int AddProduct(GlobalStruct *global_struct, product product);

int get_product_info(product *product);

int destroy_and_quit(GlobalStruct **);

int OpenScan(GtkWidget *, GlobalStruct *);

void *OpenCart(GtkWidget *, GlobalStruct *);

void *ReturnCart(GtkWidget *, GlobalStruct *);

GtkWidget *create_gtk_tree_view(GlobalStruct *global_struct);

int GetLog(GtkWidget *valideButton, GlobalStruct *global_struct);

static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream);

static char *http_get(const char *url);

int add_to_cart(GtkWidget *addCart, GlobalStruct *global_struct);

char *get_token(const gchar *email, const gchar *password, GlobalStruct *global_struct);

char *get_user_name(char *token);

char *send_cart(GtkWidget *, GlobalStruct *);

int create_bundle(char *token);

gboolean get_product_from_model(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data);

int send_product(GlobalStruct *global_struct, product product);

GtkListStore *init_list_store();

int close_bundle(char* token, int bundle_id);

gboolean handle_keyboard_add_product(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
