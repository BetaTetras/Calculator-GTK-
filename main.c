#include <gtk/gtk.h>
#include <stdio.h>
#include "lib.c"

/////////////////// CSS /////////////////////////////////////
static const char* CSS =
    ".contour {"
    "  border: 2px dashed #3584e4;"             /* pointillés bleus */
    "  padding: 6px;"
    "  background-color: alpha(#3584e4, 0.08);"  /* fond bleu très pâle */
    "}"
    ".buttons {"
    "  background-color: #4b4b4b;"   // fond bleu plein
    "  color: white;"                // texte blanc
    "  font-size: 20px;"             // texte plus gros
    "  font-weight: bold;"           // en gras
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
/////////////////// buttons /////////////////////////////////////
static void on_buttons(GtkButton *button, gpointer data){
    GtkEditable *screen = GTK_EDITABLE(data);
    const char *labelOFButton = gtk_button_get_label(button);
    const char actualChar = labelOFButton[0];
    const char *actualCalculus = gtk_editable_get_text(screen);

    if(actualChar == 'C'){
        gtk_editable_set_text(screen,"0");
        return;
    }
    if(actualChar == '='){
        size_t numberOfOpe, numberOfNbr;
        int* tabOfNum = NULL;
        char* tabOfOpe = NULL;
        char* buffeur = NULL;
        double res;

        if(valideCalcule(actualCalculus) == 0
           && tokenisation(actualCalculus,&numberOfNbr,&tabOfNum,&numberOfOpe,&tabOfOpe) == 0
           && calcule(numberOfOpe,tabOfOpe,numberOfNbr,tabOfNum,&res) == 0
           && doubleToString(res,&buffeur) == 0){
            gtk_editable_set_text(screen,buffeur);
        }else{
            gtk_editable_set_text(screen,"error...");
        }
        free(tabOfNum);
        free(tabOfOpe);
        free(buffeur);
        return;
    }

    const char *base = (strcmp(actualCalculus,"error...") == 0) ? "0" : actualCalculus;
    char *newText;
    if(strcmp(base,"0") == 0 && isNumber(actualChar)){
        newText = g_strdup(labelOFButton);             // remplace le "0" de départ
    }else{
        newText = g_strdup_printf("%s%s",base,labelOFButton);
    }
    gtk_editable_set_text(screen,newText);
    g_free(newText);
}


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

    // On crée une grille qui vas servir d'endroit pour placée nos boutons
    GtkWidget *buttons = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(buttons), TRUE);      // toutes les lignes ont la même hauteur
    gtk_grid_set_column_homogeneous(GTK_GRID(buttons), TRUE);   // toutes les colonnes ont la même largeur
    // On crée les boutons
    GtkWidget *button_C = gtk_button_new_with_label("C");
    gtk_grid_attach(GTK_GRID(buttons),button_C,0,0,3,1);
    g_signal_connect(button_C,"clicked",G_CALLBACK(on_buttons),screen);

    GtkWidget *button_Zero = gtk_button_new_with_label("0");
    gtk_grid_attach(GTK_GRID(buttons),button_Zero,0,4,3,1);
    g_signal_connect(button_Zero,"clicked",G_CALLBACK(on_buttons),screen);

    GtkWidget *button_div = gtk_button_new_with_label("/");    
    gtk_grid_attach(GTK_GRID(buttons),button_div,3,0,1,1);
    g_signal_connect(button_div,"clicked",G_CALLBACK(on_buttons),screen);

    GtkWidget *button_mul = gtk_button_new_with_label("*");    
    gtk_grid_attach(GTK_GRID(buttons),button_mul,3,1,1,1);
    g_signal_connect(button_mul,"clicked",G_CALLBACK(on_buttons),screen);

    GtkWidget *button_add = gtk_button_new_with_label("+");    
    gtk_grid_attach(GTK_GRID(buttons),button_add,3,2,1,1);
    g_signal_connect(button_add,"clicked",G_CALLBACK(on_buttons),screen);

    GtkWidget *button_sub = gtk_button_new_with_label("-");    
    gtk_grid_attach(GTK_GRID(buttons),button_sub,3,3,1,1);
    g_signal_connect(button_sub,"clicked",G_CALLBACK(on_buttons),screen);

    GtkWidget *button_res = gtk_button_new_with_label("=");    
    gtk_grid_attach(GTK_GRID(buttons),button_res,3,4,1,1);
    g_signal_connect(button_res,"clicked",G_CALLBACK(on_buttons),screen);

    for(int i=1;i<10;i++){
        char label[2] = {'0'+i,'\0'};
        GtkWidget *button_number = gtk_button_new_with_label(label);
        gtk_grid_attach(GTK_GRID(buttons), button_number, (i-1)%3, 3-(i-1)/3, 1, 1);
        g_signal_connect(button_number,"clicked",G_CALLBACK(on_buttons),screen);
    }

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