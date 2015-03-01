/*
 * This application shows how to use the Compass API to build a simple watchface
 * that shows where magnetic north is.
 *
 * The compass background image source is:
 *    <http://opengameart.org/content/north-and-southalpha-chanel>
 */

#include "pebble.h" 
  
#define BUFF 128
static AppSync sync;
static uint8_t sync_buffer[BUFF];
enum MsgKeys {
  angle = 0x0
};
  
static const GPathInfo ARROW_POINTS = {
  4, 
  (GPoint[]) { {0,-36}, {16,36}, {0,18}, {-16,36} }
}; 

static Window *window_main;
static Layer *layer_arrow;
static TextLayer *layer_alert_text;

static GPath *arrow;

static double offset_angle;

#define STATE_MENU 0
#define STATE_TRANS 1
#define STATE_NAV 2
#define STATE_TITLE 3
static int state = STATE_TITLE;
  
static int timer = 0;
static int maxtimer = 2;
  
static void sync_error(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}
static void sync_success(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) 
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Sync Success %s", new_tuple->value->cstring);
  if(state == STATE_TRANS)
  {
    GRect bounds = layer_get_frame(window_get_root_layer(window_main));
    GRect alert_bounds = GRect(0, -3, bounds.size.w, bounds.size.h / 7);
    text_layer_set_background_color(layer_alert_text, GColorClear);
    text_layer_set_text_color(layer_alert_text, GColorBlack);
    text_layer_set_font(layer_alert_text, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(layer_alert_text, GTextAlignmentLeft);
    layer_set_frame(text_layer_get_layer(layer_alert_text), alert_bounds);
    state = STATE_NAV;
  }
  
  if(state == STATE_NAV)
  {
    offset_angle = new_tuple->value->int32;
  
    gpath_rotate_to(arrow, TRIG_MAX_ANGLE/360 * offset_angle);
    layer_mark_dirty(layer_arrow);
  
    //text_layer_set_text(layer_alert_text, to_string(new_tuple->value->int32)); 
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(state == STATE_MENU)
  {
    text_layer_set_text(layer_alert_text, "lolz");
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(state == STATE_MENU)
  {
    text_layer_set_text(layer_alert_text, "lolzard"); 
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(state == STATE_MENU)
  {
    state = STATE_TRANS;
    text_layer_set_font(layer_alert_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text(layer_alert_text, "\nPlease begin walking to calibrate the compass."); 
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
  if(state == STATE_TITLE)
  {
    if(timer < maxtimer) timer++;
    if(timer == maxtimer)
    {
      text_layer_set_font(layer_alert_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
      text_layer_set_text(layer_alert_text, "Clear Saved Location\n\n\nSave Current Location\n\nNavigate to Saved Location");
      state = STATE_MENU;
    }  
  }
  
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void layer_arrow_update_callback(Layer *path, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, arrow);                                 
  GRect bounds = layer_get_frame(path);          
  GPoint path_center = GPoint(bounds.size.w / 2, bounds.size.h / 2);  
  graphics_fill_circle(ctx, path_center, 6);                  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, path_center, 4);                      
}

static void window_main_load(Window *window) {
  Layer *layer_window = window_get_root_layer(window);
  GRect bounds = layer_get_frame(layer_window);

  layer_arrow = layer_create(bounds);
  
  layer_set_update_proc(layer_arrow, layer_arrow_update_callback);
  layer_add_child(layer_window, layer_arrow);

  arrow = gpath_create(&ARROW_POINTS);

  GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
  gpath_move_to(arrow, center);

  layer_alert_text = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h / 7));
  GRect alert_bounds = GRect(0, 0, bounds.size.w, bounds.size.h);
  text_layer_set_background_color(layer_alert_text, GColorBlack);
  text_layer_set_text_color(layer_alert_text, GColorWhite);
  text_layer_set_text_alignment(layer_alert_text, GTextAlignmentCenter);
  layer_set_frame(text_layer_get_layer(layer_alert_text), alert_bounds);
  text_layer_set_font(layer_alert_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(layer_alert_text, "\n\nNice Little Tracker Experience");
  
  layer_add_child(layer_window, text_layer_get_layer(layer_alert_text));

  Tuplet initial_value[] = {
    TupletInteger(angle, 0)
  };
  
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_value, ARRAY_LENGTH(initial_value), sync_success, sync_error, NULL);
  app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
}

static void window_main_unload(Window *window) {
  text_layer_destroy(layer_alert_text);
  gpath_destroy(arrow);
  layer_destroy(layer_arrow);
}

static void init() {
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  window_main = window_create();
  window_set_window_handlers(window_main, (WindowHandlers) {
    .load = window_main_load,
    .unload = window_main_unload,
  });
  window_set_click_config_provider(window_main, click_config_provider);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  window_stack_push(window_main, true);
}

static void deinit() {
  window_destroy(window_main);
  tick_timer_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
} 