#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_uptime_layer;
static TextLayer *s_test;
static bool up =true;
static int s_uptime =0;
static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  static char s_uptime_buffer[32];
  
  snprintf(s_uptime_buffer,sizeof(s_uptime_buffer),"Uptime: %ds    %d",s_uptime,up);
  text_layer_set_text(s_uptime_layer,s_uptime_buffer);
  text_layer_set_text_alignment(s_uptime_layer, GTextAlignmentRight);

  if(s_uptime == 10)
    {
    up=false;
  }
  if(s_uptime == 0){
    up=true;
  }
  if(up==true)
    {
    s_uptime++;
  }
  else{
    s_uptime--;
  }
  
}

static void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  s_uptime_layer = text_layer_create(GRect(0,0,window_bounds.size.w,window_bounds.size.h));
  text_layer_set_text_alignment(s_uptime_layer,GTextAlignmentCenter);
  text_layer_set_text(s_uptime_layer,"Uptime: 0s");
  layer_add_child(window_layer,text_layer_get_layer(s_uptime_layer));
}

static void main_window_unload(Window *window){
  text_layer_destroy(s_uptime_layer);
}

static void init(void) {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  // Subscribe to TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit(void){
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}