/*******************************************************************************
 *   Ethereum 2 Deposit Application
 *   (c) 2020 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "os.h"
#include "cx.h"

#include "thales_plugin.h"

// *** Thales AMM contract methods ***
// Function: buyFromAMM(address market, uint8 position, uint256 amount, uint256 expectedPayout,
// uint256 additionalSlippage)
// Selector: 0x8875eb84
static const uint32_t THALES_BUY_FROM_AMM_SELECTOR = 0x8875eb84;
// Function: buyFromAMMWithReferrer(address market, uint8 position, uint256 amount, uint256
// expectedPayout, uint256 additionalSlippage, address _referrer)
// Selector: 0x9f916c9f
static const uint32_t THALES_BUY_FROM_AMM_WITH_REFERRER_SELECTOR = 0x9f916c9f;
// Function: sellToAMM(address market, uint8 position, uint256 amount, uint256 expectedPayout,
// uint256 additionalSlippage) Selector: 0x3ce1108d
static const uint32_t THALES_SELL_TO_AMM_SELECTOR = 0x3ce1108d;

// *** Thales Market contract methods ***
// Function: exerciseOptions()
// MethodID: 0x85149258
static const uint32_t THALES_EXERCISE_POSITION_SELECTOR = 0x85149258;

const uint32_t THALES_SELECTORS[NUM_THALES_SELECTORS] = {THALES_BUY_FROM_AMM_SELECTOR,
                                                         THALES_BUY_FROM_AMM_WITH_REFERRER_SELECTOR,
                                                         THALES_SELL_TO_AMM_SELECTOR,
                                                         THALES_EXERCISE_POSITION_SELECTOR};

const uint8_t AMM_POSITION_UP[INT256_LENGTH] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t AMM_POSITION_DOWN[INT256_LENGTH] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
};

// Function to dispatch calls from the ethereum app.
void dispatch_plugin_calls(int message, void *parameters) {
    switch (message) {
        case ETH_PLUGIN_INIT_CONTRACT:
            handle_init_contract(parameters);
            break;
        case ETH_PLUGIN_PROVIDE_PARAMETER:
            handle_provide_parameter(parameters);
            break;
        case ETH_PLUGIN_FINALIZE:
            handle_finalize(parameters);
            break;
        case ETH_PLUGIN_PROVIDE_INFO:
            handle_provide_token(parameters);
            break;
        case ETH_PLUGIN_QUERY_CONTRACT_ID:
            handle_query_contract_id(parameters);
            break;
        case ETH_PLUGIN_QUERY_CONTRACT_UI:
            handle_query_contract_ui(parameters);
            break;
        default:
            PRINTF("Unhandled message %d\n", message);
            break;
    }
}

// Calls the ethereum app.
void call_app_ethereum() {
    unsigned int libcall_params[3];
    libcall_params[0] = (unsigned int) "Ethereum";
    libcall_params[1] = 0x100;
    libcall_params[2] = RUN_APPLICATION;
    os_lib_call((unsigned int *) &libcall_params);
}

// Weird low-level black magic. No need to edit this.
__attribute__((section(".boot"))) int main(int arg0) {
    // Exit critical section
    __asm volatile("cpsie i");

    // Ensure exception will work as planned
    os_boot();

    // Try catch block. Please read the docs for more information on how to use those!
    BEGIN_TRY {
        TRY {
            // Low-level black magic.
            check_api_level(CX_COMPAT_APILEVEL);

            // Check if we are called from the dashboard.
            if (!arg0) {
                // Called from dashboard, launch Ethereum app
                call_app_ethereum();
                return 0;
            } else {
                // Not called from dashboard: called from the ethereum app!
                const unsigned int *args = (const unsigned int *) arg0;

                // If `ETH_PLUGIN_CHECK_PRESENCE` is set, this means the caller is just trying to
                // know whether this app exists or not. We can skip `dispatch_plugin_calls`.
                if (args[0] != ETH_PLUGIN_CHECK_PRESENCE) {
                    dispatch_plugin_calls(args[0], (void *) args[1]);
                }

                // Call `os_lib_end`, go back to the ethereum app.
                os_lib_end();
            }
        }
        FINALLY {
        }
    }
    END_TRY;

    // Will not get reached.
    return 0;
}
