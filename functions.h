#include <gtk/gtk.h>

typedef struct AppliStruct AppliStruct;
struct AppliStruct {
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

AppliStruct *InitStruct(AppliStruct *, GtkBuilder *);

int AddProduct(AppliStruct *appliStruct, product product);

int get_product_info(product *product);

int OnDestroy(GtkWidget *, AppliStruct *);

int OpenScan(GtkWidget *, AppliStruct *);

void *OpenCart(GtkWidget *, AppliStruct *);

void *ReturnCart(GtkWidget *, AppliStruct *);

GtkTreeView *createView(AppliStruct *);

int GetLog(GtkWidget *valideButton, AppliStruct *appliStruct);

static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream);

static char *http_get(const char *url);

int add_to_cart(GtkWidget *addCart, AppliStruct *appliStruct);
