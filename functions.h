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

AppliStruct *InitStruct(AppliStruct *, GtkBuilder *);

int OnDestroy(GtkWidget *, AppliStruct *);

int OpenScan(GtkWidget *, AppliStruct *);

int OpenCart(GtkWidget *, gpointer);

int ReturnCart(GtkWidget *, gpointer);

//int *GetBarCode(GtkWidget *pwidget, gpointer pData);

//int *GetQuantity(GtkWidget *, gpointer);

int GetLog(GtkWidget *valideButton, AppliStruct *appliStruct);

//void *GetPwd(GtkWidget *, gpointer);

static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream);

static char *request(const char *url);

int GetProduct(GtkWidget *addCart, AppliStruct *appliStruct);
