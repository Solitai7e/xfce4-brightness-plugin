#include <libxfce4panel/libxfce4panel.h>
#include <xfconf/xfconf.h>




static XfcePanelPlugin *plugin;
static XfconfChannel   *xfconf_channel;

static GtkWidget *panel_button;
static GtkWidget *panel_icon;
static GtkWidget *popup_menu;
static GtkWidget *settings_dialog;

static GammaManager *gamma_manager;




static void activate_popup_menu(void)
{
    gtk_menu_popup_at_widget(
        GTK_MENU(popup_menu), panel_button,

        xfce_panel_plugin_get_orientation(plugin) ?
            GDK_GRAVITY_NORTH_EAST :
            GDK_GRAVITY_SOUTH_WEST,

        GDK_GRAVITY_NORTH_WEST,
        NULL
    );

    xfce_panel_plugin_register_menu(plugin, GTK_MENU(popup_menu));
}


static void popup_menu_on_popped_up
(
    GtkMenu *self,
    gpointer flipped_rect, gpointer final_rect,
    gboolean flipped_x,    gboolean flipped_y,
    gpointer priv
)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(panel_button), TRUE);
}

static void popup_menu_on_deactivate(GtkMenuShell *menu, gpointer priv)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(panel_button), FALSE);
}


static gboolean panel_button_on_button_press_event
(
    GtkWidget      *button,
    GdkEventButton *event,
    gpointer        priv
)
{
    if (event->button != 1) return FALSE;

    activate_popup_menu();
    return TRUE;
}

static void popup_menu_on_activate_settings(PopupMenu *popup_menu)
{
    gtk_widget_show(settings_dialog);
}


static void plugin_on_free_data(XfcePanelPlugin *plugin, gpointer *priv)
{
    g_object_unref(gamma_manager);
    g_object_unref(settings_dialog);
    g_object_unref(popup_menu);
    g_object_unref(xfconf_channel);
}

static void plugin_on_size_changed(XfcePanelPlugin *plugin, gint size)
{
    gint icon_size = xfce_panel_plugin_get_icon_size(plugin);
    gtk_image_set_pixel_size(GTK_IMAGE(panel_icon), icon_size);
}


static void plugin_construct(XfcePanelPlugin *_plugin)
{
    plugin = _plugin;


    g_assert_true(xfconf_init(NULL));

    xfconf_channel = xfconf_channel_new_with_property_base(
        "xfce4-panel",
        xfce_panel_plugin_get_property_base(plugin)
    );


    panel_button = xfce_panel_create_toggle_button();
    gtk_widget_set_tooltip_text(panel_button, "Brightness");

    panel_icon = brightness_icon_new(1);
    gtk_container_add(GTK_CONTAINER(panel_button), panel_icon);

    gtk_widget_show_all(panel_button);
    gtk_container_add(GTK_CONTAINER(plugin), panel_button);


    popup_menu = popup_menu_new();
    gtk_menu_attach_to_widget(GTK_MENU(popup_menu), panel_button, NULL);


    settings_dialog = settings_dialog_new();


    GdkWindow *root_window = gtk_widget_get_window(GTK_WIDGET(plugin));
               root_window = gdk_window_get_effective_toplevel(root_window);

    gamma_manager = gamma_manager_x11_new(GDK_X11_WINDOW(root_window));


    gboolean apply_on_startup = xfconf_channel_get_bool(
        xfconf_channel,
        "/misc/apply-on-startup",
        TRUE
    );

    GBindingFlags ugh = apply_on_startup ? G_BINDING_SYNC_CREATE : 0;


    g_object_bind_property(popup_menu, "value-brightness", panel_icon, "value", G_BINDING_SYNC_CREATE);

    g_object_bind_property(settings_dialog, "value-show-red-slider",        popup_menu, "show-slider-red",        G_BINDING_SYNC_CREATE);
    g_object_bind_property(settings_dialog, "value-show-green-slider",      popup_menu, "show-slider-green",      G_BINDING_SYNC_CREATE);
    g_object_bind_property(settings_dialog, "value-show-blue-slider",       popup_menu, "show-slider-blue",       G_BINDING_SYNC_CREATE);
    g_object_bind_property(settings_dialog, "value-show-brightness-slider", popup_menu, "show-slider-brightness", G_BINDING_SYNC_CREATE);

    g_object_bind_property(popup_menu, "value-brightness", gamma_manager, "brightness", G_BINDING_BIDIRECTIONAL | ugh);
    g_object_bind_property(popup_menu, "value-red",        gamma_manager, "red",        G_BINDING_BIDIRECTIONAL | ugh);
    g_object_bind_property(popup_menu, "value-green",      gamma_manager, "green",      G_BINDING_BIDIRECTIONAL | ugh);
    g_object_bind_property(popup_menu, "value-blue",       gamma_manager, "blue",       G_BINDING_BIDIRECTIONAL | ugh);


    // HACK: xfconf won't accept G_TYPE_FLOATs for whatever reason
    xfconf_g_property_bind(xfconf_channel, "/values/brightness", G_TYPE_DOUBLE, popup_menu, "value-brightness");
    xfconf_g_property_bind(xfconf_channel, "/values/red",        G_TYPE_DOUBLE, popup_menu, "value-red");
    xfconf_g_property_bind(xfconf_channel, "/values/green",      G_TYPE_DOUBLE, popup_menu, "value-green");
    xfconf_g_property_bind(xfconf_channel, "/values/blue",       G_TYPE_DOUBLE, popup_menu, "value-blue");

    xfconf_g_property_bind(xfconf_channel, "/popup/show-red-slider",        G_TYPE_BOOLEAN, settings_dialog, "value-show-red-slider");
    xfconf_g_property_bind(xfconf_channel, "/popup/show-green-slider",      G_TYPE_BOOLEAN, settings_dialog, "value-show-green-slider");
    xfconf_g_property_bind(xfconf_channel, "/popup/show-blue-slider",       G_TYPE_BOOLEAN, settings_dialog, "value-show-blue-slider");
    xfconf_g_property_bind(xfconf_channel, "/popup/show-brightness-slider", G_TYPE_BOOLEAN, settings_dialog, "value-show-brightness-slider");
    xfconf_g_property_bind(xfconf_channel, "/misc/apply-on-startup",        G_TYPE_BOOLEAN, settings_dialog, "value-apply-on-startup");


    g_signal_connect(plugin, "free-data",    G_CALLBACK(plugin_on_free_data),    NULL);
    g_signal_connect(plugin, "size-changed", G_CALLBACK(plugin_on_size_changed), NULL);

    g_signal_connect(panel_button, "button-press-event", G_CALLBACK(panel_button_on_button_press_event), NULL);

    g_signal_connect(popup_menu, "popped-up",         G_CALLBACK(popup_menu_on_popped_up),         NULL);
    g_signal_connect(popup_menu, "deactivate",        G_CALLBACK(popup_menu_on_deactivate),        NULL);
    g_signal_connect(popup_menu, "activate-settings", G_CALLBACK(popup_menu_on_activate_settings), NULL);
}




XFCE_PANEL_PLUGIN_REGISTER(plugin_construct)
