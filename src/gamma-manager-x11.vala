[CCode (array_length_pos = 2.5, array_length_type = "int")]
private extern X.ID[]? get_crtcs(X.Display display, X.Window window);

private extern bool get_crtc_gamma
(
    X.Display display,
    X.ID      crtc,

    out float brightness, out float red, out float green, out float blue
);

private extern bool set_crtc_gamma
(
    X.Display display,
    X.ID      crtc,

    float brightness, float red, float green, float blue
);


public class GammaManagerX11 : GammaManager {
    public Gdk.X11.Window window { get; construct; }

    private unowned X.Display _display;
    private         X.ID[]    _crtcs;

    private void _on_update()
    {
        foreach (X.ID crtc in this._crtcs)
            set_crtc_gamma(
                this._display, crtc,
                this.brightness, this.red, this.green, this.blue
            );
    }

    construct {
        unowned var gdk_display = (Gdk.X11.Display) this.window.get_display();

        this._display = gdk_display.get_xdisplay();
        this._crtcs   = get_crtcs(this._display, this.window.get_xid());

        assert(this._crtcs != null);

        this.update.connect(this._on_update);
    }

    [CCode (type = "GammaManager *")]
    public GammaManagerX11(Gdk.X11.Window window)
    {
        Object(window: window);
    }
}
