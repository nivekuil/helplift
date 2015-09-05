#include <pebble.h>
#include <stdio.h>

Window *window;
TextLayer *text_layer;

enum result_keys {
  KEY_STATE = 0,
};

#define my_assert(cond)				\
  do if (!(cond)) {				\
      APP_LOG(APP_LOG_LEVEL_ERROR, #cond);	\
    } while(0)

AppTimer * s_alarm_timer = NULL;

static void inbox_received_callback(DictionaryIterator *iterator,
				    void *context) {
  // Read first item
  Tuple *t = dict_read_first(iterator);

  int state = -1;

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_STATE:
      my_assert(state == -1);
      state = (int)t->value->int32;
      my_assert(state != -1);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }

  if (state == 0)
    APP_LOG(APP_LOG_LEVEL_ERROR, "Got state 0!");
  else if (state == 1)
    APP_LOG(APP_LOG_LEVEL_ERROR, "Got state 1!");
  else
    APP_LOG(APP_LOG_LEVEL_ERROR, "Got weird state!");

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator,
				   AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void call_for_help() {
  s_alarm_timer = NULL;

  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}

void call_for_help_callback(void *data) {
  call_for_help();
}

void alarm_phase() {
  // Vibe pattern: ON for 200ms, OFF for 100ms, ON for 400ms:
  static const uint32_t const segments[] = { 600, 100, 1300,
					     600, 100, 1300 };
  uint32_t total_duration = 0;
  for (size_t i=0; i<sizeof segments/sizeof *segments; ++i)
    total_duration += segments[i];

  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  vibes_enqueue_custom_pattern(pat);

  s_alarm_timer = app_timer_register(total_duration,
				     call_for_help_callback,
				     NULL);
}

void accel_handler(AccelData *data, uint32_t num_samples) {
  char buf[256];
  static int cnt;
  int x=data->x, y=data->y, z=data->z;
  int norm_sqr = x*x + y*y + z*z;
  if (norm_sqr < 900*900) {
    snprintf(buf, sizeof buf, "x,y,z: %d,%d,%d, norm_sqr=%d",
	     x, y, z, norm_sqr);
    APP_LOG(APP_LOG_LEVEL_DEBUG, buf);

    if (!s_alarm_timer) {
      alarm_phase();
    }
  }
  if (!(++cnt % 100)) {
    snprintf(buf, sizeof buf, "still alive: x,y,z: %d,%d,%d, norm_sqr=%d",
	     x, y, z, norm_sqr);
    APP_LOG(APP_LOG_LEVEL_DEBUG, buf);
  }
}

void single_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "down single lcick handler");
  // TODO: synchronization?
  if (s_alarm_timer) {
    app_timer_cancel(s_alarm_timer);
    s_alarm_timer = NULL;
  }
  vibes_cancel();
}

void config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_DOWN, single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, single_click_handler);
}

void handle_init(void) {
  // Create a window and text layer
  window = window_create();
  text_layer = text_layer_create(GRect(0, 0, 144, 154));

  // Set the text, font, and text alignment
  text_layer_set_text(text_layer, "HelpLift ready!");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  // Add the text layer to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

  // Push the window
  window_stack_push(window, true);

  accel_data_service_subscribe(1, accel_handler);

  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // single click / repeat-on-hold config:
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

 app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // App Logging!
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

void handle_deinit(void) {
  // Destroy the text layer
  text_layer_destroy(text_layer);

  // Destroy the window
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
