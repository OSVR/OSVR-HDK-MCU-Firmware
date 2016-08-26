/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_Toshiba_TC358870_ISR_h_GUID_F3111CEB_4EF8_4D9E_AA64_E90BCC1CBD01
#define INCLUDED_Toshiba_TC358870_ISR_h_GUID_F3111CEB_4EF8_4D9E_AA64_E90BCC1CBD01

// Internal Includes
#include "my_hardware.h"
#include "MacroUtils.h"

// Library/third-party includes
// - none

// ASF includes
#include <ioport.h>
#include <interrupt.h>

// Standard includes
#include <avr/interrupt.h>

#ifdef SVR_HAVE_TOSHIBA_TC358870
#define TC358870_ISR_NAME SVR_EXPAND_CAT3(TC358870_ADDR_SEL_INT_PORT, TC358870_PORT_INT, _vect)
// Yep, need secondary expansion again.
#define TC358870_ISR_IMPL(TC358870_ISR_NAME) ISR(TC358870_ISR_NAME)

// This would be the signature of your ISR.
#define TC358870_ISR() TC358870_ISR_IMPL(TC358870_ISR_NAME)

/// Temporary defines
#define TC358870_INT_MASK SVR_EXPAND_CAT3(INT, TC358870_PORT_INT, MASK)
#define TC358870_INT_LEVEL_GMASK SVR_EXPAND_CAT3(PORT_INT, TC358870_PORT_INT, LVL_gm)
#define TC358870_INT_LEVEL_IMPL(A, B, C, D, E)
#define TC358870_INT_LEVEL_VAL SVR_EXPAND_CAT5(PORT_INT, TC358870_PORT_INT, LVL_, TC358870_INT_LEVEL, _gc)
#define TC358870_INT_FLAG SVR_EXPAND_CAT3(PORT_INT, TC358870_PORT_INT, IF_bm)

/// Just call once - this will do one-time setup required for the MCU to handle interrupts.
static inline void Toshiba_TC358870_MCU_Ints_Init(void)
{
	// Get the port base struct.
	PORT_t *port = arch_ioport_pin_to_base(TC358870_ADDR_SEL_INT);

	// disable interrupts
	irqflags_t flags = cpu_irq_save();
	// Set appropriate value for INT1MASK (assuming TC358870_PORT_INT is 1)
	port->TC358870_INT_MASK = ioport_pin_to_mask(TC358870_ADDR_SEL_INT);

	// Set the PORT_INT1_LVL portion of INTCTRL to the right interrupt level (assuming TC358870_PORT_INT is 1)
	port->INTCTRL = (port->INTCTRL & ~TC358870_INT_LEVEL_GMASK) | TC358870_INT_LEVEL_VAL;

	// restore cpu flags.
	cpu_irq_restore(flags);
}

static inline __always_inline void Toshiba_TC358870_MCU_Ints_Clear_Flag(void)
{
	/// Clear the flag for our interrupt by writing 1 to it.
	/// @todo is = ok or should we be doing |= here?
	arch_ioport_pin_to_base(TC358870_ADDR_SEL_INT)->INTFLAGS = TC358870_INT_FLAG;
}

static inline __always_inline void Toshiba_TC358870_MCU_Ints_Suspend(void)
{
	/// Clear the intctrl for our interrupt
	/// @todo is this in fact atomic?
	arch_ioport_pin_to_base(TC358870_ADDR_SEL_INT)->INTCTRL &= ~TC358870_INT_LEVEL_GMASK;
}

static inline __always_inline void Toshiba_TC358870_MCU_Ints_Resume(void)
{
	/// OR's in our level - assumes that we're the only ones on that interrupt and that nobody monkeyed with it since
	/// the suspend.
	/// @todo is this in fact atomic?
	arch_ioport_pin_to_base(TC358870_ADDR_SEL_INT)->INTCTRL |= TC358870_INT_LEVEL_VAL;
}

#undef TC358870_INT_MASK
#undef TC358870_INT_LEVEL_GMASK
#undef TC358870_INT_LEVEL
#undef TC358870_INT_FLAG

#endif  // SVR_HAVE_TOSHIBA_TC358870

#endif  // INCLUDED_Toshiba_TC358870_ISR_h_GUID_F3111CEB_4EF8_4D9E_AA64_E90BCC1CBD01
