#include <gdk-pixbuf/gdk-pixbuf.h>

int main() {
    GdkPixbuf *pixbuf;// Assume you have a valid pixbuf
    GError *error = NULL;

    if (gdk_pixbuf_save("output.png", pixbuf, "png", &error)) {
        g_print("Image saved successfully.\n");
    } else {
        g_print("Error saving image: %s\n", error->message);
        g_error_free(error);
    }

    return 0;
}

