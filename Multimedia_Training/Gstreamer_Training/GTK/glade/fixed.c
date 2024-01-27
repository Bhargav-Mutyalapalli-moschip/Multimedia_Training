#include<gtk/gtk.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include<ctype.h>

GtkWidget *win;
GtkWidget *button1;
GtkWidget *button2;
GtkWidget *button3;
GtkWidget *fixed1;
GtkWidget *label1;
GtkWidget *label2;
GtkWidget *label3;
GtkBuilder *builder;
void on_button1_clicked(GtkButton *b);
void on_button2_clicked(GtkButton *b);
void on_button3_clicked(GtkButton *b);
int main(int argc,char *argv[])
{
	gtk_init(&argc,&argv);

	builder=gtk_builder_new_from_file("part.glade");

	win=GTK_WIDGET(gtk_builder_get_object(builder,"window"));

	g_signal_connect(win,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	gtk_builder_connect_signals(builder,NULL);

	fixed1=GTK_WIDGET(gtk_builder_get_object(builder,"fixed1"));

	button1=GTK_WIDGET(gtk_builder_get_object(builder,"button1"));
	button2=GTK_WIDGET(gtk_builder_get_object(builder,"button2"));
	button3=GTK_WIDGET(gtk_builder_get_object(builder,"button2"));

	label1=GTK_WIDGET(gtk_builder_get_object(builder,"label1"));
	label2=GTK_WIDGET(gtk_builder_get_object(builder,"label2"));
	label3=GTK_WIDGET(gtk_builder_get_object(builder,"label3"));

	gtk_widget_show_all(win);

	gtk_main();
	return EXIT_SUCCESS;
}
void on_button1_clicked(GtkButton *b)
{
	gtk_label_set_text(GTK_LABEL(label1),(const gchar *)"hello");
}
void on_button2_clicked(GtkButton *b)
{
	gtk_label_set_text(GTK_LABEL(label2),(const gchar *)"hai");
}
void on_button3_clicked(GtkButton *b)
{
	gtk_label_set_text(GTK_LABEL(label3),(const gchar *)"namesthey");
}



