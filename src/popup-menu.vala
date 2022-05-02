[GtkTemplate (ui = "/a872cc4b9c1f/popup-menu.ui")]
public class PopupMenu : Gtk.Menu {
    public float value_red        { get; set construct; default = 1; }
    public float value_green      { get; set construct; default = 1; }
    public float value_blue       { get; set construct; default = 1; }
    public float value_brightness { get; set construct; default = 1; }

    public bool show_slider_red        { get; set construct; default = true; }
    public bool show_slider_green      { get; set construct; default = true; }
    public bool show_slider_blue       { get; set construct; default = true; }
    public bool show_slider_brightness { get; set construct; default = true; }


    public signal void activate_settings();


    [GtkCallback (name = "on_settings_button_activate")]
    private void _on_settings_button_activate()
    {
        this.activate_settings();
    }


    static construct {
        // HACK: Apparently GtkBuilder doesn't recognize classes that
        //       haven't been instantiated at least once elsewhere.
        //       Well fucking played GTK.
        new SliderMenuItem();
    }

    construct {
        this.show_all();
    }


    [CCode (type = "GtkWidget *")]
    public PopupMenu() {}
}
