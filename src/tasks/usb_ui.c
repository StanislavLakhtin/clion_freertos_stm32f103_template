//
// Created by Stanislav Lakhtin on 08/11/2019.
//

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <defs.h>
#include <string.h>
#include <printf.h>
#include <stdlib.h>

#define EQUAL             0x00
char buffer[20];

#define SHIFTLEFT(CMND, SIZE)     memcpy(buffer, buffer + SIZE, sizeof(buffer) - SIZE)
#define CMP_CMND_TEXT(X)          memcmp(buffer, X, strlen(X)) == EQUAL

#define IDLE              0x00
#define OW_CMND           0x01
#define OW_SHOW_SUBCMND   0x02
#define OW_SHOW_ROMS      0x03
#define OW_SHOW_TEMP      0x04
#define OW_SHOW_TEMP_ID   0x05

static const char *CMNDS[] = {"IDLE", "ow", "show", "roms", "temp", "id_select"};

static const char *introText =
    "OneWire Helper \n"
    "By EduTerra.PRO Lab\n";

/* Буфер для пользовательского ввода */

uint8_t current_state = IDLE;
uint8_t selected_option = 0x00;

static void view_station_state(void) {
  printf("\n=========BME280 Sensor Data Start===========\n");
  printf("Temp: %f C\n", state.bme280.temperature);
  printf("Hum: %d %%\n", state.bme280.humidity);
  printf("Press: %d Pa\n", state.bme280.pressure);
  printf("Press: %f mmHg\n", (float) state.bme280.pressure * 0.00750061683f);
  printf("===========BME280 Sensor Data End==============\n");
}

static void trim_left(char *str, uint8_t len) {
  uint8_t i = 0;
  while (i < len && (str[i] == ' ' || str[i] == '\t')) {
    i += 1;
  }
  if (i < len) {
    memcpy(str, str + i, len - i);
  }
}

static void clear_input_buffer() {
  for (uint8_t i = 0; i < (uint8_t) sizeof(buffer); i++)
    buffer[i] = 0x00;
}

static void prompt(void) {
  clear_input_buffer();
  printf("\n> ");
  usb_getline(buffer, sizeof(buffer));
}

static void state_machine() {
  uint8_t analize = TRUE;
  while (analize) {
    trim_left(buffer, sizeof(buffer));
    switch (current_state) {
      case IDLE:
        if (CMP_CMND_TEXT(CMNDS[OW_CMND])) {
          current_state = OW_CMND;
          SHIFTLEFT(OW_CMND, strlen(CMNDS[OW_CMND]));
        } else {
          clear_input_buffer();
          analize = FALSE;
        }
        break;
      case OW_CMND:
        if (CMP_CMND_TEXT(CMNDS[OW_SHOW_SUBCMND])) {
          current_state = OW_SHOW_SUBCMND;
          SHIFTLEFT(OW_SHOW_SUBCMND, strlen(CMNDS[OW_SHOW_SUBCMND]));
        } else
          analize = FALSE;
        break;
      case OW_SHOW_SUBCMND:
        if (CMP_CMND_TEXT(CMNDS[OW_SHOW_ROMS])) {
          current_state = OW_SHOW_ROMS;
          SHIFTLEFT(OW_SHOW_SUBCMND, strlen(CMNDS[OW_SHOW_ROMS]));
        } else if (CMP_CMND_TEXT(CMNDS[OW_SHOW_TEMP])) {
          current_state = OW_SHOW_TEMP;
          SHIFTLEFT(OW_SHOW_SUBCMND, strlen(CMNDS[OW_SHOW_TEMP]));
        } else
          analize = FALSE;
        break;
      case OW_SHOW_TEMP:
        selected_option = atoi(buffer);
        current_state = OW_SHOW_TEMP_ID;
        analize = FALSE;
        break;
      case OW_SHOW_TEMP_ID:
        analize = FALSE;
        break;
      default:
        clear_input_buffer();
        analize = FALSE;
        break;
    }
  }
}

static void print_hex(uint8_t *buffer, int len, uint8_t backorder) {
  printf("[");
  uint8_t n = 0;
  while  (n < len ) {
    uint8_t p =  backorder ? len-n-1 : n;
    printf("%.2X", buffer[p]);
    if (n != (len - 1))
      printf(".");
    else
      printf("]");
    n++;
  }
}

static void show_roms(void) {
  SemaphoreHandle_t xOwInUse = get_ow_semaphore();
  if (xSemaphoreTake(xOwInUse, (TickType_t) 0) == pdTRUE) {
    if (state.ow.bus->state.devicesQuantity != 0x00) {
      printf("\ntotal known sensors are:\n (ID) [FAMILY] [x.x.x.x.x.x] [CRC]\n ---------");
      for (uint8_t i = 0; i < state.ow.bus->state.devicesQuantity; i++) {
        RomCode *rom = ow_get_rom(i);
        printf("\n (%.2d) [%.2X] ", i, rom->family);
        print_hex(rom->code, 6, TRUE);
        printf(" [%.2X]", rom->crc);
        if (rom->family == 0x28)
          printf(" -- DS18B20 Temp sensor device");
      }
    }
  }
  xSemaphoreGive(xOwInUse);
}

static void show_temp(uint8_t id) {
  SemaphoreHandle_t xOwInUse = get_ow_semaphore();
  if (xSemaphoreTake(xOwInUse, (TickType_t) 0) == pdTRUE) {
    if (id > state.ow.bus->state.devicesQuantity) {
      printf("\n ERROR: There are no rom with ID: %d", id);
    } else {
      if (state.ow.bus->rom[id].family == 0x28) {
        printf("\nDS18B20 ID: %d, CODE: ", id);
        print_hex(state.ow.bus->rom[id].code, 6, TRUE);
        printf("\n Temp (Celsius) = %.4f\n", id, state.ow.last_temp[id]);
      } else {
        printf("\n ERROR: Family code device is %X. It's not DS18B20 device.\n", state.ow.bus->rom[id].family);
      }
    }
    xSemaphoreGive(xOwInUse);
  }
}

void usb_ui(void *arg __attribute((unused))) {
  printf("%s", introText);
  while (true) {
    prompt();
    printf("\ninput: %s", buffer);
    state_machine();
    printf("\ncurrent state: %s", CMNDS[current_state]);
    switch (current_state) {
      case OW_CMND: {
        printf("\n show ...");
        break;
      }
      case OW_SHOW_ROMS: {
        show_roms();
        current_state = IDLE;
        clear_input_buffer();
        break;
      }
      case OW_SHOW_TEMP: {
        printf("\nselect ID from the list below\n");
        show_roms();
        break;
      }
      case OW_SHOW_TEMP_ID: {
        clear_input_buffer();
        show_temp(selected_option);
        current_state = IDLE;
        break;
      }
      case OW_SHOW_SUBCMND:
        printf("\n roms ...\n temp ...\n dev_count\n conf");
        break;
      default:
        printf("\n you are in '%s' state", CMNDS[current_state]);
        break;
      case IDLE:
        printf("\n ow -- OneWire Bus commands\n bme -- BME280 Sensor commands\n");
        clear_input_buffer();
        break;
    }
    taskYIELD();
  }
}