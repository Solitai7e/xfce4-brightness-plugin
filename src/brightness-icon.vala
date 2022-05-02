private string brightness_to_icon_name(float value)
{
    return value < 0.1 ? "display-brightness-off-symbolic"    :
           value < 0.3 ? "display-brightness-low-symbolic"    :
           value < 0.8 ? "display-brightness-medium-symbolic" :
                         "display-brightness-high-symbolic";
}

public class BrightnessIcon : Gtk.Image {
    public float value { get; set construct; default = 1; }

    construct {
        this.bind_property(
            "value", this, "icon-name",
            BindingFlags.SYNC_CREATE,

            (binding, src, ref dest) => {
                dest.set_string(brightness_to_icon_name((float) src));
                return true;
            }
        );
    }

    [CCode (type = "GtkWidget *")]
    public BrightnessIcon(float brightness = 1)
    {
        Object(value: brightness);
    }
}
