#include <gtk/gtk.h>


/////////////////// CSS /////////////////////////////////////
static const char* CSS =
    ".contour {"
    "  border: 2px dashed #3584e4;"             /* pointillés bleus */
    "  padding: 6px;"
    "  background-color: alpha(#3584e4, 0.08);"  /* fond bleu très pâle */
    "}";

static void on_startup(GApplication *app, gpointer data) {
    (void)app;    // paramètres imposés par le signal, mais inutiles ici :
    (void)data;   // (void) évite les warnings « unused parameter »

    GtkCssProvider *css = gtk_css_provider_new();   // un « fournisseur de style »
    gtk_css_provider_load_from_string(css, CSS);    // on lui donne notre texte CSS
    gtk_style_context_add_provider_for_display(     // on l'applique à tous les widgets
        gdk_display_get_default(),                  // de l'écran,
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);   // en priorité sur le thème
    g_object_unref(css);                            // GTK garde sa propre référence
}
////////////////////////////////////////////////////////


// Crée juste un label
static GtkWidget *creer_titre(const char *texte) {
    GtkWidget *titre = gtk_label_new(texte);
    gtk_widget_set_halign(titre, GTK_ALIGN_FILL);
    gtk_widget_add_css_class(titre, "contour");
    return titre;
}

static void activate(GtkApplication *app, gpointer data) {
    (void)data;

    // On crée la fenêtre
    GtkWidget *win = gtk_application_window_new(app);
    // Définition de sa taile (250x350)
    // GTK_WINDOW(win) -> Conversion GtkWidget to GtkWindow
    gtk_window_set_default_size(GTK_WINDOW(win),250,350);

    /* ======== La page ========
     * Une boîte verticale, sans contour, qui range les 2 sections.
     * C'est l'unique enfant de la fenêtre. */
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);  // 5 px entre ses enfants
    gtk_window_set_child(GTK_WINDOW(win), page);

    /* ======== Création des boxs ========
     * On veut séparée notre page en deux partie pour
     * avoir une premier partie qui permet de voir 
     * ce que l'on a écrit comment calcule et l'autre
     * pour choisir quelle nombre et opérateur que l'on
     * veut */
    
    // Set de l'écran de la caluclatrice :
    // - Text de départ = 0
    // - Pas de saisie dans l'écran
    // - Texte aligné a droite
    // - Pas de barre de saisie
    GtkWidget *screen = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(screen), "0");
    gtk_editable_set_editable(GTK_EDITABLE(screen), FALSE); 
    gtk_editable_set_alignment(GTK_EDITABLE(screen), 1.0);   
    gtk_widget_set_can_focus(screen, FALSE);

    
    GtkWidget *buttons = gtk_grid_new();
    // On définit leur taille
    gtk_widget_set_size_request(screen, -1, 80);   // screen : 80 px de haut (largeur libre)
    gtk_widget_set_vexpand(buttons, TRUE);          // buttons : prend tout le reste    
    // On les ajoute a la fenêtre
    gtk_box_append(GTK_BOX(page), screen);
    gtk_box_append(GTK_BOX(page), buttons); 

    // Affichage de notre fenêtre
    gtk_window_present(GTK_WINDOW(win));
}

int main(int argc, char **argv) {
    g_setenv("GTK_CSD", "0", FALSE);

    // Création de notre application
    GtkApplication *app = gtk_application_new("com.exemple.demo", G_APPLICATION_DEFAULT_FLAGS);
    
    // Set le css provider
    g_signal_connect(app, "startup", G_CALLBACK(on_startup), NULL);
    // Activation 
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Lancement de l'app
    return g_application_run(G_APPLICATION(app), argc, argv);
}