public abstract class GammaManager : Object {
    public float brightness { get; set construct; default = 1; }
    public float red        { get; set construct; default = 1; }
    public float green      { get; set construct; default = 1; }
    public float blue       { get; set construct; default = 1; }

    public signal void update();

    construct {
        this.notify.connect(spec => {
            switch (spec.name) {
                case "brightness":
                case "red":
                case "green":
                case "blue":
                    this.update();
                    break;
            }
        });
    }
}
