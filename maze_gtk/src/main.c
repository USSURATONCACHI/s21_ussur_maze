#include <stdio.h>
#include <gtk/gtk.h>

void my_callback_function(GtkWidget *widget, gpointer data);
void destroy(GtkWidget *widget, gpointer data);
void greet(GtkWidget *widget, gpointer data);
static int counter = 0; 

int main(int argc, char** argv) {
    printf("Hello, world!\n");

    GtkWidget *window;
    GtkWidget *button;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    button = gtk_button_new_with_label("Click Me!");

    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    g_signal_connect(button, "clicked", G_CALLBACK(greet), "button");
    gtk_container_add(GTK_CONTAINER (window), button);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void greet( GtkWidget *widget, gpointer data )
{
   g_print ("Welcome to GTK\n");
   g_print ("%s clicked %d times\n",
      (char*)data, ++counter);
}

void destroy(GtkWidget *widget, gpointer data) {
   gtk_main_quit ();
}
