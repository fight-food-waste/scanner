#include <gtk/gtk.h>

typedef struct AppliStruct AppliStruct;
struct AppliStruct {
    GtkEntry *loginEntry;
    GtkEntry *pwdEntry;
    GtkWidget *mainWindow;
    GtkWidget *scanproduct;
    GtkLabel *authError;
    GtkWidget *cartResume;
    GtkEntry *barrecodeEntry;
    GtkEntry *quantityEntry;
    GtkLabel *refquantityLabel;
    GtkLabel *refproductLabel;

};

typedef struct product product;

struct product {
    long code;
    long quantity;
    char* name;
    char* image_url;
};

typedef struct http_params {
    CURLcode curl_code;
    struct curl_slist *http_headers;
    long http_code;
    char* body;
};

AppliStruct *InitStruct(AppliStruct *, GtkBuilder *);

int AddProduct(AppliStruct *appliStruct, product product);

int get_product_info(product *product);

int OnDestroy(GtkWidget *, AppliStruct *);

int OpenScan(GtkWidget *, AppliStruct *);

int OpenCart(GtkWidget *, gpointer);

int ReturnCart(GtkWidget *, gpointer);

//int *GetBarCode(GtkWidget *pwidget, gpointer pData);

//int *GetQuantity(GtkWidget *, gpointer);

int GetLog(GtkWidget *valideButton, AppliStruct *appliStruct);

//void *GetPwd(GtkWidget *, gpointer);

static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream);

static char *http_get(const char *url);

int add_to_cart(GtkWidget *addCart, AppliStruct *appliStruct);
