#include <stdio.h>
#include <glib.h>

int main() {
    const gchar *input_string = "apple,banana,grape";
    const gchar *delimiter = ",";
    
    gchar **tokens = g_strsplit(input_string, delimiter, -1);
    
    for (gchar **t = tokens; *t != NULL; ++t) {
        printf("Token: %s\n", *t);
    }
    
    g_strfreev(tokens); // Free the memory allocated by g_strsplit
    
    return 0;
}

