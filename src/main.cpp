#include "Application.h"

int main() {
    Application app;

    if (!app.Initialize()) {
        return -1;
    }

    return app.Run();
}
