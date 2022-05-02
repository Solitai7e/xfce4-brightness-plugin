[GtkTemplate (ui = "/a872cc4b9c1f/settings-dialog.ui")]
public class SettingsDialog : Gtk.Dialog {
    public bool value_show_red_slider        { get; set construct; default = true; }
    public bool value_show_green_slider      { get; set construct; default = true; }
    public bool value_show_blue_slider       { get; set construct; default = true; }
    public bool value_show_brightness_slider { get; set construct; default = true; }
    public bool value_apply_on_startup       { get; set construct; default = true; }

    construct {
        this.get_content_area().show_all();
        this.delete_event.connect(this.hide_on_delete);
    }

    [CCode (type = "GtkWidget *")]
    public SettingsDialog() {}
}
