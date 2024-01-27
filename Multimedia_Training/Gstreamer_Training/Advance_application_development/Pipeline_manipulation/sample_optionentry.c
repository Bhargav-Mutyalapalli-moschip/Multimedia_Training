#include <stdio.h>
#include <glib.h>

static gboolean verbose = FALSE;
static gchar *input_file = NULL;
static gint count = 0;

static GOptionEntry options[] = {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Enable verbose output", NULL },
    { "input", 'i', 0, G_OPTION_ARG_STRING, &input_file, "Input file path", "FILE" },
    { "count", 'c', 0, G_OPTION_ARG_INT, &count, "Number of iterations", "COUNT" },
    { NULL }
};

int main(int argc, char **argv) {


	g_print("click here for options\n");
	g_print("  v -----> verbose\n i  -->input file path\n  c ---->count\n ");
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new("Example command-line options");
	g_option_context_add_main_entries(context, options, NULL);

	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_print("Option parsing failed: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	if (verbose) {
		g_print("Verbose mode enabled.\n");
	}

	if (input_file) {
		g_print("Input file: %s\n", input_file);
	}

	g_print("Count: %d\n", count);

	g_option_context_free(context);

    return 0;
}

