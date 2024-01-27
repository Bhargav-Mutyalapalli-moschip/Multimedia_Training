#include<gtk/gtk.h>
#include<stdio.h>

int main(int argc,char *argv[])
{
	const char *title;
	int width,height;
	gtk_init(&argc,&argv);
	GtkWidget *win;
	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win),"Ram Pranav");

	gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_MOUSE);

	gtk_window_set_default_size(GTK_WINDOW(win),500,400);

	gtk_window_set_resizable(GTK_WINDOW(win),FALSE);

	gtk_window_set_decorated(GTK_WINDOW(win),TRUE);

//	gtk_window_maximize(GTK_WINDOW(win));
        title=gtk_window_get_title(GTK_WINDOW(win));
	printf("The title:%s\n",title);
        gtk_window_get_size(GTK_WINDOW(win),&width,&height);
	printf("The width:%d\n",width);
	printf("The height:%d\n",height);

	printf("the window is decorated? %d\n",gtk_window_get_decorated(GTK_WINDOW(win)));
	printf("the window is maximized or not? %d\n",gtk_window_is_maximized(GTK_WINDOW(win)));
	printf("the window is active or not? %d\n",gtk_window_is_active(GTK_WINDOW(win)));


	g_signal_connect(win,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	gtk_widget_show_all(win);

	gtk_main();
	return 0;
}

