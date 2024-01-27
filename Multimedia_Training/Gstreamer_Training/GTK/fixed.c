#include<gtk/gtk.h>
#include<stdio.h>

int main(int argc,char *argv[])
{
	gtk_init(&argc,&argv);

	GtkWidget *win;
	GtkWidget *button1;
	GtkWidget *button2;
	GtkWidget *fixed;

        win=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_default_size(GTK_WINDOW(win),500,400);

	fixed=gtk_fixed_new();

	button1=gtk_button_new_with_label("Pranav");
	button2=gtk_button_new_with_label("Rakesh");

	gtk_fixed_put(GTK_FIXED(fixed),button1,0,0);
	gtk_fixed_put(GTK_FIXED(fixed),button2,200,0);

	g_signal_connect(win,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	gtk_container_add(GTK_CONTAINER(win),fixed);
	gtk_widget_show_all(win);

	gtk_main();
	return 0;
}

