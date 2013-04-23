#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0xD4, 0x8A, 0x4A, 0x14, 0x84, 0xF7, 0x45, 0xDA, 0x89, 0x1F, 0x4B, 0x2B, 0x2A, 0x64, 0x99, 0xF8 }
PBL_APP_INFO(MY_UUID,
             "SimplicITA", "Vivido",
             1, 0,
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer text_month_layer;
TextLayer text_day_layer;
TextLayer text_time_layer;

Layer line_layer;

static const char* const DAY[] = {
    "Domenica",
    "Lunedì",
    "Martedì",
    "Mercoledì",
    "Giovedì",
    "Venerdì",
    "Sabato"
};

static const char* const MONTH[] ={
    "Gennaio",
    "Febbraio",
    "Marzo",
    "Aprile",
    "Maggio",
    "Giugno",
    "Luglio",
    "Agosto",
    "Settembre",
    "Ottobre",
    "Novembre",
    "Dicembre"
};

static size_t append_string(char* buffer, const size_t length, const char* str)
{
    strncat(buffer, str, length);
    
    size_t written = strlen(str);
    return (length > written) ? written : length;
}

void month_to_words(PblTm* t, char* words, size_t length)
{
    size_t remaining = length;
    memset(words, 0, length);
    
    remaining -=append_string(words, length, MONTH[t->tm_mon]);
}


void month_to_words2(PblTm* t, char* words, size_t length)
{
    size_t remaining = length;
    memset(words, 0, length);
    
    string_format_time(words, sizeof(length), "%e ", t);
    remaining -=3;
    remaining -=append_string(words, remaining, MONTH[t->tm_mon]);
}

void day_to_words(PblTm* t, char* words, size_t length)
{
    size_t remaining = length;
    memset(words, 0, length);
    
    remaining -=append_string(words, length, DAY[t->tm_wday]);
}

static void update_time(PblTm* t)
{
    static char time_text[] = "00:00";
    static char month_text[] = "00 Xxxxxxxxx";
    static char day_text[] = "Xxxxxxxxx";

    day_to_words(t, day_text, sizeof(day_text));
    text_layer_set_text(&text_day_layer, day_text);
    
    month_to_words2(t, month_text, sizeof(month_text));
    text_layer_set_text(&text_month_layer, month_text);
    
    string_format_time(time_text, sizeof(time_text), "%R", t);
    text_layer_set_text(&text_time_layer, time_text);
}

void line_layer_update_callback(Layer *me, GContext* ctx)
{
    (void)me;
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
    graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));    
}

void handle_init(AppContextRef ctx) {
    (void)ctx;
    
    window_init(&window, "SimplicITA");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    
    resource_init_current_app(&APP_RESOURCES);
    
    text_layer_init(&text_day_layer, window.layer.frame);
    text_layer_set_text_color(&text_day_layer, GColorWhite);
    text_layer_set_background_color(&text_day_layer, GColorClear);
    layer_set_frame(&text_day_layer.layer, GRect(8, 40, 144-8, 168-40));
    text_layer_set_font(&text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
    layer_add_child(&window.layer, &text_day_layer.layer);

    text_layer_init(&text_month_layer, window.layer.frame);
    text_layer_set_text_color(&text_month_layer, GColorWhite);
    text_layer_set_background_color(&text_month_layer, GColorClear);
    layer_set_frame(&text_month_layer.layer, GRect(8, 68, 144-8, 168-68));
    text_layer_set_font(&text_month_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
    layer_add_child(&window.layer, &text_month_layer.layer);
      
    text_layer_init(&text_time_layer, window.layer.frame);
    text_layer_set_text_color(&text_time_layer, GColorWhite);
    text_layer_set_background_color(&text_time_layer, GColorClear);
    layer_set_frame(&text_time_layer.layer, GRect(7, 92, 144-7, 168-92));
    text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
    layer_add_child(&window.layer, &text_time_layer.layer);
    
    layer_init(&line_layer, window.layer.frame);
    line_layer.update_proc = &line_layer_update_callback;
    layer_add_child(&window.layer, &line_layer);
    
    PblTm t;
    get_time(&t);
    update_time(&t);
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t)
{
    (void)ctx;
    update_time(t->tick_time);
}

void pbl_main(void *params)
{
    PebbleAppHandlers handlers =
    {
        .init_handler = &handle_init,
        
        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
        
    };
    app_event_loop(params, &handlers);
}
