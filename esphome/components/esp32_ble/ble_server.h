#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"
#include "ble_service.h"
#include "ble_characteristic.h"
#include "ble_uuid.h"
#include "ble_advertising.h"
#include <map>

#include "queue.h"

#ifdef ARDUINO_ARCH_ESP32

#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>

namespace esphome {
namespace esp32_ble {

class BLEServiceComponent {
 public:
  virtual void setup_service();
};

class BLEServer : public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void teardown();

  void set_manufacturer(const std::string manufacturer) { this->manufacturer_ = manufacturer; }
  void set_model(const std::string model) { this->model_ = model; }

  BLEService *create_service(const uint8_t *uuid, bool advertise = false);
  BLEService *create_service(uint16_t uuid, bool advertise = false);
  BLEService *create_service(const std::string uuid, bool advertise = false);
  BLEService *create_service(ESPBTUUID uuid, bool advertise = false, uint16_t num_handles = 15, uint8_t inst_id = 0);

  esp_gatt_if_t get_gatts_if() { return this->gatts_if_; }
  uint32_t get_connected_client_count() { return this->connected_clients_; }
  std::map<uint16_t, void *> get_clients() { return this->clients_; }
  BLEAdvertising *get_advertising() { return this->advertising_; }

  void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

  void register_service_component(BLEServiceComponent *component) { this->service_components_.push_back(component); }

 protected:
  bool create_device_characteristics_();
  void setup_server_();

  void add_client_(uint16_t conn_id, void *client) {
    this->clients_.insert(std::pair<uint16_t, void *>(conn_id, client));
  }
  bool remove_client_(uint16_t conn_id) { return this->clients_.erase(conn_id) > 0; }

  std::string manufacturer_;
  optional<std::string> model_;
  esp_gatt_if_t gatts_if_{0};
  BLEAdvertising *advertising_;

  uint32_t connected_clients_{0};
  std::map<uint16_t, void *> clients_;

  std::vector<BLEService *> services_;
  BLEService *device_information_service;

  std::vector<BLEServiceComponent *> service_components_;

  SemaphoreHandle_t register_lock_;
};

extern BLEServer *global_ble_server;

}  // namespace esp32_ble
}  // namespace esphome

#endif
