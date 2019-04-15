#include <gtk/gtk.h>

typedef struct AppliStruct AppliStruct;
struct AppliStruct {
    GtkEntry *loginEntry;
    GtkEntry *pwdEntry;
    GtkWidget *mainWindow;
    GtkWidget *scanproduct;
    GtkLabel *authError;

};

AppliStruct *InitStruct(AppliStruct *, GtkBuilder *);

void *OnDestroy(GtkWidget *, AppliStruct *);

void *OpenScan(GtkWidget *, AppliStruct *);

void *OpenCart(GtkWidget *, gpointer);

void *ReturnCart(GtkWidget *, gpointer);

int *GetBarCode(GtkWidget *pwidget, gpointer pData);

int *GetQuantity(GtkWidget *, gpointer);

void *GetLog(GtkWidget *, AppliStruct *);

//void *GetPwd(GtkWidget *, gpointer);

static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream);

static char *request(const char *url);