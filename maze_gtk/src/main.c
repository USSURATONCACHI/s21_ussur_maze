#include <stdio.h>
#include <gtk/gtk.h>

void my_callback_function(GtkWidget *widget, gpointer data);

int main(int argc, char** argv) {
    printf("Hello, world!\n");

    GtkWidget *window;
    GtkWidget *button;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    button = gtk_button_new_with_label("Click Me!");
}

void destroy(GtkWidget *widget, gpointer data) {
   gtk_main_quit ();
}
