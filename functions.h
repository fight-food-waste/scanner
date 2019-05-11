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
    GtkListStore *listStore;
    GtkWidget *listView;
    GtkCellRenderer *cellRenderer;
    GtkTreeViewColumn *pColumn;
    GtkScrolledWindow *scrolledWindow;
    char *token;
    GtkLabel *scanLabel;
};

// Enum for GtkTreeView
enum {
    QTY_COLUMN = 0,
    NAME_COLUMN,
    N_COLUMNS = 2

};

/*
 * Use struct to pass around the product properties
 */
typedef struct product product;

struct product {
    long code;
    long quantity;
    char *name;
    char *image_url;
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

int AddProduct(GlobalStruct *global_struct, product product);

int get_product_info(product *product);

int destroy_and_quit(GlobalStruct **);

int OpenScan(GtkWidget *, GlobalStruct *);

void *OpenCart(GtkWidget *, GlobalStruct *);

void *ReturnCart(GtkWidget *, GlobalStruct *);

GtkTreeView *createView(GlobalStruct *);

int GetLog(GtkWidget *valideButton, GlobalStruct *global_struct);

static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream);

static char *http_get(const char *url);

int add_to_cart(GtkWidget *addCart, GlobalStruct *global_struct);

char *get_token(gchar *email, gchar *password, GlobalStruct *global_struct);

char *get_user_name(GlobalStruct *global_struct);

char *send_cart(GtkWidget *, GlobalStruct *);

gboolean get_product_from_model(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data);