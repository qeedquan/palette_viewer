#include "app.h"

int main(int argc, char *argv[])
{
    App *app;

    app = new App(argc, argv);
    return app->exec();
}
