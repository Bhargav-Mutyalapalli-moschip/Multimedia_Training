#include<gtk/gtk.h>
#include<stdio.h>
static void activate(GtkApplication *app,gpointer user_data)
{
	//gtkcode here along with app
	GtkWidget *win;
	win=gtk_application_window_new(app);
	gtk_widget_show_all(win);
	
}
int main(int argc,char *argv[])
{
	int ret;
	gtk_init(&argc,&argv);
	GtkApplication *app;
	app=gtk_application_new("Ram.Pranav",G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app,"activate",G_CALLBACK(activate),NULL);
	ret=g_application_run(G_APPLICATION(app),argc,argv);
	g_object_unref(app);
	return ret;

}


