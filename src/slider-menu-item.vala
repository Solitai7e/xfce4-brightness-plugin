public class SliderMenuItem : Gtk.ImageMenuItem {
    public float value { get; set construct; default = 1; }

    private Gtk.Scale _scale = new Gtk.Scale.with_range(
        Gtk.Orientation.HORIZONTAL,
        0, 1, 0.01
    );


    private bool _on_button_event(Gdk.EventButton event)
    {
        int x, y;

        this.translate_coordinates(
            this._scale, (int) event.x, (int) event.y, out x, out y);


        Gtk.Allocation alloc;
        this._scale.get_allocation(out alloc);

        if (x > 0 && y > 0 && x < alloc.width && y < alloc.height)
            this._scale.event(event);


        return true;
    }


    construct {
        this.image = new BrightnessIcon(value);
        ((Gtk.Image) this.image).pixel_size = 20;

        this.add(_scale);

        this.bind_property(
            "value", this.image, "value",
            BindingFlags.BIDIRECTIONAL
        );

        this.bind_property(
            "value", this._scale.adjustment, "value",
            BindingFlags.BIDIRECTIONAL | BindingFlags.SYNC_CREATE
        );

        this.button_press_event.connect(this._on_button_event);
        this.button_release_event.connect(this._on_button_event);
    }
}
