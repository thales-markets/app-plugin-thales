#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
#define NUM_THALES_SELECTORS 1

// Name of the plugin.
#define PLUGIN_NAME "Thales"

extern const const uint32_t AMM_POSITION_UP[INT256_LENGTH];
extern const const uint32_t AMM_POSITION_DOWN[INT256_LENGTH];

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    BUY_FROM_AMM = 0,
} thalesSelector_t;

// Enumeration used to parse the smart contract data.
typedef enum {
    MARKET = 0,
    POSITION,
    AMOUNT,
    EXPECTED_PAYOUT,
    SLIPPAGE,
    UNEXPECTED_PARAMETER,
} parameter;

extern const uint32_t THALES_SELECTORS[NUM_THALES_SELECTORS];

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_t {
    // For display.
    uint8_t market[ADDRESS_LENGTH];
    uint8_t position[INT256_LENGTH];
    uint8_t amount[INT256_LENGTH];
    uint8_t expected_payout[INT256_LENGTH];
    uint8_t slippage[INT256_LENGTH];

    char ticker[MAX_TICKER_LEN];
    uint8_t decimals;
    uint8_t token_found;

    // For both parsing and display.
    thalesSelector_t selectorIndex;
} context_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);