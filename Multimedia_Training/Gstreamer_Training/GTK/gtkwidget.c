#include<gtk/gtk.h>
#include<stdio.h>

int main(int argc,char *argv[])
{
	gtk_init(&argc,&argv);
	GtkWidget *win;
	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(win,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	gtk_widget_show_all(win);

	gtk_main();
	return 0;
}

