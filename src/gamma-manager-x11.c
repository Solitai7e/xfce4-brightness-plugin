#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/extensions/Xrandr.h>




static int find_last_non_clamped(CARD16 array[], int size)
{
    for (int i = size - 1; i > 0; i--)
        if (array[i] < 0xFFFF) return i;

    return 0;
}


RRCrtc *get_crtcs(Display *display, Window window, int *crtcs_count)
{
    RRCrtc *retval = NULL;




    XRRScreenResources *scrn_res = XRRGetScreenResources(display, window);
    if (!scrn_res) goto die_get_screen_resources;


    RRCrtc *crtcs = malloc(sizeof(RRCrtc) * scrn_res->ncrtc);
    if (!crtcs) goto die_alloc_crtcs;

    memcpy(crtcs, scrn_res->crtcs, sizeof(RRCrtc) * scrn_res->ncrtc);


    retval       = crtcs;
    *crtcs_count = scrn_res->ncrtc;




    if (!retval) free(crtcs);
die_alloc_crtcs:

    XRRFreeScreenResources(scrn_res);
die_get_screen_resources:


    return retval;
}

int get_crtc_gamma
(
    Display *display,
    RRCrtc crtc,

    float *out_brightness, float *out_red, float *out_green, float *out_blue
)
{
    int retval = 0;




    XRRCrtcGamma *crtc_gamma = XRRGetCrtcGamma(display, crtc);
    if (!crtc_gamma) goto die_get_crtc_gamma;

    CARD16 *red_raw   = crtc_gamma->red,
           *green_raw = crtc_gamma->green,
           *blue_raw  = crtc_gamma->blue;

    int size = crtc_gamma->size;
    if (size <= 0) goto die_invalid_size;


    // Black magic for converting the data to floats, taken from xrandr
    int last_red   = find_last_non_clamped(red_raw,   size);
    int last_green = find_last_non_clamped(green_raw, size);
    int last_blue  = find_last_non_clamped(blue_raw,  size);

    CARD16 *best_array = red_raw;
    int     last_best  = last_red;

    if (last_green > last_best) {
        last_best  = last_green;
        best_array = green_raw;
    }

    if (last_blue > last_best) {
        last_best  = last_blue;
        best_array = blue_raw;
    }

    if (last_best == 0) last_best = 1;

    int middle = last_best / 2;

    double i1 = (double) (middle + 1) / size;
    double v1 = (double) best_array[middle] / 65535;
    double i2 = (double) (last_best + 1) / size;
    double v2 = (double) best_array[last_best] / 65535;

    float brightness, red, green, blue;

    if (v2 < 0.0001) {
        brightness = 1;
        red        = 0;
        green      = 0;
        blue       = 0;
    }
    else {
        brightness = last_best + 1 == size ?
            v2 :
            exp((log(v2) * log(i1) - log(v1) * log(i2)) / log(i1 / i2));

        red = log((double) red_raw[last_red / 2] / brightness / 65535) /
              log((double) (last_red / 2 + 1) / size);

        green = log((double) green_raw[last_green / 2] / brightness / 65535) /
                log((double) (last_green / 2 + 1) / size);

        blue = log((double) blue_raw[last_blue / 2] / brightness / 65535) /
               log((double) (last_blue / 2 + 1) / size);
    }


    *out_brightness = brightness;
    *out_red        = red;
    *out_green      = green;
    *out_blue       = blue;

    retval = 1;




die_invalid_size:

    XRRFreeGamma(crtc_gamma);
die_get_crtc_gamma:


    return retval;
}

int set_crtc_gamma
(
    Display *display,
    RRCrtc crtc,

    float brightness, float red, float green, float blue
)
{
    int retval = 0;




    int size = XRRGetCrtcGammaSize(display, crtc);
    if (size <= 0 || size >= 65536) goto die_invalid_size;

    XRRCrtcGamma *crtc_gamma = XRRAllocGamma(size);
    if (!crtc_gamma) goto die_alloc_crtc_gamma;

    CARD16 *red_raw   = crtc_gamma->red,
           *green_raw = crtc_gamma->green,
           *blue_raw  = crtc_gamma->blue;


    // More black magic taken from xrandr
    red   = 1.0 / fmaxf(red,   0.001);
    green = 1.0 / fmaxf(green, 0.001);
    blue  = 1.0 / fmaxf(blue,  0.001);

    for (int i = 0; i < size; i++) {
        if (red == 1.0 && brightness == 1.0)
            red_raw[i] = (double) i / (double) (size - 1) * 65535.0;
        else
            red_raw[i] = 65535.0 * fmin(
                pow((double) i / (double) (size - 1), red) * brightness, 1.0);

        if (green == 1.0 && brightness == 1.0)
            green_raw[i] = (double) i / (double) (size - 1) * 65535.0;
        else
            green_raw[i] = 65535.0 * fmin(
                pow((double) i / (double) (size - 1), green) * brightness, 1.0);

        if (blue == 1.0 && brightness == 1.0)
            blue_raw[i] = (double) i / (double) (size - 1) * 65535.0;
        else
            blue_raw[i] = 65535.0 * fmin(
                pow((double) i / (double) (size - 1), blue) * brightness, 1.0);
    }


    XRRSetCrtcGamma(display, crtc, crtc_gamma);


    retval = 1;




    XRRFreeGamma(crtc_gamma);
die_alloc_crtc_gamma:

die_invalid_size:


    return retval;
}
