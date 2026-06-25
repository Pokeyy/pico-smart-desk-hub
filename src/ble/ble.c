#include "ble.h"

static le_notification_enabled;
static hci_con_handle_t con_handle;

//static int att_write_callback(hci_con_handle_t connection handle)

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    
    if (packet_type != HCI_EVENT_PACKET)    // check for event to report status update / data (device found)
        return;
    
    switch (hci_event_packet_get_type(packet)) {
        case SM_EVENT_JUST_WORKS_REQUEST:           // requests user to accept a Just Works pairing (connects via exchanging encryption keys in the bg)
            printf("Just works requested\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            break;  
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            le_notification_enabled = 0;
            break;
        case HCI_EVENT_LE_META:     // capture connection handle on connect
            if (hci_event_le_meta_get_subevent_code(packet) == HCI_SUBEVENT_LE_CONNECTION_COMPLETE)
                con_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
    }


}