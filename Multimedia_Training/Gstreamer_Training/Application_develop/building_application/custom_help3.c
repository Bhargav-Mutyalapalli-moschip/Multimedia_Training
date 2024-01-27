#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  gboolean custom_flag1 = FALSE;
  gboolean custom_flag2 = FALSE;
  GOptionContext *ctx;
  GError *err = NULL;
  GOptionEntry entries[] = {
    { "custom-flag1", 'c', 0, G_OPTION_ARG_NONE, &custom_flag1, "Description for Custom Flag 1", NULL },
    { "custom-flag2", 'd', 0, G_OPTION_ARG_NONE, &custom_flag2, "Description for Custom Flag 2", NULL },
    { NULL }
  };

  ctx = g_option_context_new ("- Your application with custom flags");
  g_option_context_add_main_entries (ctx, entries, NULL);
  g_option_context_add_group (ctx, gst_init_get_option_group ());

  // Set a custom name for the default help option
  g_option_context_set_help (ctx, "my-help");

  if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
    g_print ("Failed to initialize: %s\n", err->message);
    g_clear_error (&err);
    g_option_context_free (ctx);
    return 1;
  }
  g_option_context_free (ctx);

  if (custom_flag1) {
    printf("Custom Flag 1 is set.\n");
    // Implement your custom logic for Custom Flag 1 here.
  }

  if (custom_flag2) {
    printf("Custom Flag 2 is set.\n");
    // Implement your custom logic for Custom Flag 2 here.
  }

  printf("Run me with --my-help to see the custom help message.\n");
  return 0;
}

