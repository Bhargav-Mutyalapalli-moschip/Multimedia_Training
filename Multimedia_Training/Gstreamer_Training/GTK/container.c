#include<gtk/gtk.h>
#include<stdio.h>

int main(int argc,char *argv[])
{
	gtk_init(&argc,&argv);

	GtkWidget *win;
	GtkWidget *button1;
	GtkWidget *button2;

        win=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	button1=gtk_button_new_with_label("button1");
	button2=gtk_button_new_with_label("Rakesh vachadu");

	gtk_container_add(GTK_CONTAINER(win),button1);
	gtk_container_remove(GTK_CONTAINER(win),button1);
	gtk_container_add(GTK_CONTAINER(win),button2);

	gtk_widget_show_all(win);


	gtk_main();
	return 0;
}

