#include <pebble.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

Window *window;
TextLayer *text_layer;

enum result_keys {
  KEY_STATE = 1,
  KEY_NUM_PASSES = 2,
};

#define RING_BUFFER_SIZE 5
#define BUF_LEN 80

AppTimer * s_alarm_timer = NULL;
bool can_abort = false;

#define MAX_PASSES 20
int num_passes = 4;

#define my_assert(cond)				\
  do if (!(cond)) {				\
      APP_LOG(APP_LOG_LEVEL_ERROR, #cond);	\
    } while(0)

struct ring_buffer {
  double data[RING_BUFFER_SIZE];
  int end_index;
  double sum;
} rb_len;

void ring_buffer_init(struct ring_buffer *rb) {
  const double default_value = 1000;
  rb->sum = default_value*RING_BUFFER_SIZE;
  rb->end_index = 0;
  for (int i=0; i<RING_BUFFER_SIZE; ++i)
    rb->data[i] = default_value;
}

void ring_buffer_add(struct ring_buffer *rb, double value) {
  rb->sum -= rb->data[rb->end_index];
  rb->data[rb->end_index] = value;
  rb->sum += rb->data[rb->end_index];
  rb->end_index = (rb->end_index + 1)%RING_BUFFER_SIZE;
}
double ring_buffer_sum(struct ring_buffer *const rb) {
  return rb->sum;
}

static void inbox_received_callback(DictionaryIterator *iterator,
				    void *context) {
  // Read first item
  Tuple *t = dict_read_first(iterator);

  int state = -1;

  for (; t != NULL; t = dict_read_next(iterator)) {
    switch(t->key) {
    case KEY_STATE:
      my_assert(state == -1);
      state = (int)t->value->int32;
      my_assert(state != -1);
      break;
    case KEY_NUM_PASSES:
      num_passes = (int)t->value->int32;
      my_assert(1 <= num_passes && num_passes <= MAX_PASSES);
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
  }

  if (state != -1) {
    if (state == 0)
      APP_LOG(APP_LOG_LEVEL_ERROR, "Got state 0!");
    else if (state == 1)
      APP_LOG(APP_LOG_LEVEL_ERROR, "Got state 1!");
    else
      APP_LOG(APP_LOG_LEVEL_ERROR, "Got weird state!");
  }
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

void set_can_abort(void *data) {
  can_abort = true;
}

void alarm_phase() {
  static uint32_t segments[4*MAX_PASSES] = { 600, 100, 1300 };
  static const uint32_t segments_pattern[] = { 600, 100, 1300, 500 };

  size_t len = 3;
  for (int i=1; i<num_passes; ++i, ++len)
    segments[len] = segments_pattern[len%ARRAY_LENGTH(segments_pattern)];
  uint32_t total_duration = 0;
  for (size_t i=0; i<len; ++i)
    total_duration += segments[i];

  VibePattern pat = {
    .durations = segments,
    .num_segments = len,
  };
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start alarm phase");

  vibes_enqueue_custom_pattern(pat);

  can_abort = false;
  s_alarm_timer = app_timer_register(total_duration,
				     call_for_help_callback,
				     NULL);
  app_timer_register(1000, set_can_abort, NULL);
}

double my_sqrt(double x) {
  double l=0, r=x;
  while (r-l >= 1e-8) {
    double m=(l+r)/2;
    if (m*m > x)
      r = m;
    else
      l = m;
  }

  return l;
}

double hypot3(double x, double y, double z) {
  return my_sqrt(x*x + y*y + z*z);
}

void cancel_timer() {
  vibes_cancel();
  if (s_alarm_timer) {
    app_timer_cancel(s_alarm_timer);
    s_alarm_timer = NULL;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "cancel timer");
}

void accel_handler(AccelData *data, uint32_t num_samples) {
  //char buf[BUF_LEN];

  ring_buffer_add(&rb_len, hypot3(data->x, data->y, data->z));

  double avg_len = ring_buffer_sum(&rb_len)/RING_BUFFER_SIZE;
  // snprintf(buf, sizeof buf, "avg length=%d", (int)avg_len);
  // APP_LOG(APP_LOG_LEVEL_DEBUG, buf);

  const int free_fall_threshold = 350;
  const int intentional_shaking_threshold = 4500;

  if (avg_len < free_fall_threshold) {
    if (!s_alarm_timer)
      alarm_phase();
  } else if (avg_len > intentional_shaking_threshold) {
    if (can_abort)
      cancel_timer();
  }
}

void single_click_handler(ClickRecognizerRef recognizer, void *context) {
  cancel_timer();
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
  text_layer_set_text(text_layer, "WatchDog ready!");
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

 ring_buffer_init(&rb_len);

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
