#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "config.h"
#include "http.h"
	
#if ANDROID
	#define MY_UUID { 0x85, 0x58, 0xF1, 0x59, 0xC9, 0xBD, 0x4F, 0x66, 0xAC, 0x5E, 0x83, 0x29, 0xE1, 0x2A, 0xD5, 0x7F }
#else
	#define MY_UUID HTTP_UUID
#endif

PBL_APP_INFO(MY_UUID,
             "E-Thermostaat", "Erik Wieberdink",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer text_layer;

ActionBarLayer action_bar_layer;
BitmapLayer status_layer;

HeapBitmap button_image_up;
HeapBitmap button_image_down;
HeapBitmap button_image_setup;

HeapBitmap button_image_status0;
HeapBitmap button_image_status1;
HeapBitmap button_image_status2;
HeapBitmap button_image_status3;



void http_failed(int32_t cookie, int http_status, void* context) {
	if (cookie == HTTP_TEMP_ID || cookie == 0)
	{
		text_layer_set_text(&text_layer, http_get_error(http_status));
	}
}

void http_success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if (cookie == HTTP_TEMP_ID)
	{
		// Get temperature
		Tuple* data_tuple = dict_find(received, MSG_TEMP_CURRENT);
		
		char* temperatuur = data_tuple->value->cstring;
		
		text_layer_set_text(&text_layer, temperatuur);

		// Look up type setting
		
		data_tuple = dict_find(received, MSG_TEMP_STATUS);
		
		switch (data_tuple->value->int16)
		{
			case 0:
				bitmap_layer_set_bitmap(&status_layer, &button_image_status0.bmp);
				break;
			case 1:				
				bitmap_layer_set_bitmap(&status_layer, &button_image_status1.bmp);
				break;
			case 2:				
				bitmap_layer_set_bitmap(&status_layer, &button_image_status2.bmp);
				break;
			case 5:				
				bitmap_layer_set_bitmap(&status_layer, &button_image_status3.bmp);
				break;
			default:				
				bitmap_layer_set_bitmap(&status_layer, &button_image_status0.bmp);
				break;
		}
		
	    layer_mark_dirty((Layer*)&status_layer);
	}
}






void get_temp(int action)
{
  	// Build the HTTP request
	DictionaryIterator *body;
	
	HTTPResult result = http_out_get(SERVER_URL, HTTP_TEMP_ID, &body);
	if(result != HTTP_OK) {
		return;
    }
	else
	{	
		dict_write_int32(body, MSG_TYPE, MSG_TYPE_TEMP);
		dict_write_int32(body, MSG_TEMP_ACTION, action);
		
		result = http_out_send();
		if(result != HTTP_OK) {
			return;
		}

		return;
	}
}






void temp_up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
	
	get_temp(MSG_ACTION_UP);
}
void temp_select_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;

	get_temp(MSG_ACTION_STATUS);
}
void temp_select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;

	get_temp(MSG_ACTION_FIXED);
}
void temp_down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;

	get_temp(MSG_ACTION_DOWN);
}
void temp_click_config_provider(ClickConfig **config, Window *window) {
    (void)window;
    
    config[BUTTON_ID_UP]->click.handler = (ClickHandler) temp_up_single_click_handler;
    config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;
    
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) temp_select_click_handler;
    config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;
    
    config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) temp_select_long_click_handler;
    config[BUTTON_ID_SELECT]->long_click.delay_ms = 700;
	
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) temp_down_single_click_handler;
    config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}




void init_temp_window()
{
    window_set_background_color(&window, GColorWhite);

	http_register_callbacks((HTTPCallbacks){
		.failure = http_failed,
		.success = http_success
	}, 0);


    // Init resources
    resource_init_current_app(&APP_RESOURCES);
    
    // Load some bitmaps
    heap_bitmap_init(&button_image_up, RESOURCE_ID_IMAGE_BUTTON_UP);
    heap_bitmap_init(&button_image_setup, RESOURCE_ID_IMAGE_BUTTON_SETUP);
    heap_bitmap_init(&button_image_down, RESOURCE_ID_IMAGE_BUTTON_DOWN);

	heap_bitmap_init(&button_image_status0, RESOURCE_ID_IMAGE_TEMP_AV);
	heap_bitmap_init(&button_image_status1, RESOURCE_ID_IMAGE_TEMP_COMFORT);
	heap_bitmap_init(&button_image_status2, RESOURCE_ID_IMAGE_TEMP_REST);
	heap_bitmap_init(&button_image_status3, RESOURCE_ID_IMAGE_TEMP_VT);
	
	text_layer_init(&text_layer, GRect(10, 10, 130, 30));
	text_layer_set_text_alignment(&text_layer, GTextAlignmentLeft);
	text_layer_set_text(&text_layer, "Loading...");
	text_layer_set_font(&text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	layer_add_child(&window.layer, &text_layer.layer);	

	bitmap_layer_init(&status_layer, GRect(30, 35, 64, 64));    
    layer_add_child(&window.layer, &status_layer.layer);
	
    action_bar_layer_init(&action_bar_layer);
    action_bar_layer_set_click_config_provider(&action_bar_layer, (ClickConfigProvider) temp_click_config_provider);
    action_bar_layer_set_icon(&action_bar_layer, BUTTON_ID_UP, &button_image_up.bmp);
    action_bar_layer_set_icon(&action_bar_layer, BUTTON_ID_SELECT, &button_image_setup.bmp);
    action_bar_layer_set_icon(&action_bar_layer, BUTTON_ID_DOWN, &button_image_down.bmp);	
    action_bar_layer_add_to_window(&action_bar_layer, &window);	
	
	get_temp(MSG_ACTION_NONE);
}







void handle_init(AppContextRef ctx) {
  (void)ctx;

  	window_init(&window, "E-Thermostaat");
 	window_stack_push(&window, true /* Animated */);

	init_temp_window();
}

void handle_deinit(AppContextRef ctx)
{
    heap_bitmap_deinit(&button_image_up);
    heap_bitmap_deinit(&button_image_setup);
    heap_bitmap_deinit(&button_image_down);

	heap_bitmap_deinit(&button_image_status0);
	heap_bitmap_deinit(&button_image_status1);
	heap_bitmap_deinit(&button_image_status2);
	heap_bitmap_deinit(&button_image_status3);

    window_deinit(&window);
}









void pbl_main(void *params) {
  	PebbleAppHandlers handlers = {
    	.init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 512,
				.outbound = 256
		    }
		}
    };
  	app_event_loop(params, &handlers);
}
