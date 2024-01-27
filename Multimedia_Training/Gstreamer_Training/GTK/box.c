#include<gtk/gtk.h>
#include<stdio.h>

int main(int argc,char *argv[])
{
	gtk_init(&argc,&argv);

	GtkWidget *win;
	GtkWidget *button1;
	GtkWidget *button2;
	GtkWidget *button3;
	GtkWidget *box;

        win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

	button1=gtk_button_new_with_label("Dharma rao");
	button2=gtk_button_new_with_label("lv");
	button3=gtk_button_new_with_label("bhavitha");

	gtk_widget_set_size_request(GTK_WIDGET(button1),200,150);
	gtk_widget_set_size_request(GTK_WIDGET(button2),0.0002,0.0009);

	gtk_container_add(GTK_CONTAINER(win),box);
	gtk_box_pack_start (GTK_BOX(box),button1,TRUE,TRUE,50);
	gtk_box_pack_start (GTK_BOX(box),button2,TRUE,TRUE,50);
	gtk_box_pack_start (GTK_BOX(box),button3,TRUE,TRUE,50);
	int ishome=gtk_box_get_homogeneous(GTK_BOX(box));
	printf("homo:%d\n",ishome);


	gtk_widget_show_all(win);


	gtk_main();
	return 0;
}

