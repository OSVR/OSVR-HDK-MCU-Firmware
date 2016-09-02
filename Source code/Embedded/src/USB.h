/*
 * USB.h
 *
 * Created: 10/23/2015 5:32:56 PM
 *  Author: Sensics
 */

#ifndef USB_H_
#define USB_H_

#include <usb_protocol_cdc.h>

#include <stdbool.h>
#include <stdint.h>

/*! \brief Opens the communication port
 * This is called by CDC interface when USB Host enable it.
 *
 * \retval true if cdc startup is successfully done
 */
bool main_cdc_enable(uint8_t port);

/*! \brief Closes the communication port
 * This is called by CDC interface when USB Host disable it.
 */
void main_cdc_disable(uint8_t port);

/// Called when somebody opens the CDC port
void main_cdc_config(uint8_t port, usb_cdc_line_coding_t* cfg);

void main_cdc_rx_notify(uint8_t port);

/// @brief Check to see if USB CDC is active.
bool usb_cdc_is_active(void);

/*! \brief Manages the leds behaviors
 * Called when a start of frame is received on USB line each 1ms.
 */
void main_sof_action(void);

/*! \brief Enters the application in low power mode
 * Callback called when USB host sets USB line in suspend state
 */
void main_suspend_action(void);

/*! \brief Turn on a led to notify active mode
 * Called when the USB line is resumed from the suspend state
 */
void main_resume_action(void);

/*! \brief Save new DTR state to change led behavior.
 * The DTR notify that the terminal have open or close the communication port.
 */
void main_cdc_set_dtr(uint8_t port, bool b_enable);

#endif /* USB_H_ */