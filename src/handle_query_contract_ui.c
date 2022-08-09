#include "thales_plugin.h"

// Set UI for "Market" screen.
static void set_market_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Beneficiary", msg->titleLength);

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    getEthAddressStringFromBinary(
        context->market,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        chainid);
}

// Set UI for the "Position" screen.
static void set_position_ui(ethQueryContractUI_t *msg) {
    strlcpy(msg->title, "Position", msg->titleLength);
    strlcpy(msg->msg, (char *) context->ticker, msg->msgLength);
}

// Set UI for the "Buy" screen.
static void set_buy_ui(ethQueryContractUI_t *msg) {
    strlcpy(msg->title, "Buy", msg->titleLength);

    uint8_t decimals = context->decimals;
    const char *ticker = context->ticker;

    amountToString(context->expected_payout,
                   sizeof(context->expected_payout),
                   decimals,
                   ticker,
                   msg->msg,
                   msg->msgLength);
}

// Set UI for "Pay" screen.
static void set_pay_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Pay", msg->titleLength);

    uint8_t decimals = context->decimals;
    const char *ticker = context->ticker;

    amountToString(context->expected_payout,
                   sizeof(context->expected_payout),
                   decimals,
                   ticker,
                   msg->msg,
                   msg->msgLength);
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    // EDIT THIS: Adapt the cases for the screens you'd like to display.
    switch (msg->screenIndex) {
        case 0:
            set_market_ui(msg);
            break;
        case 1:
            set_position_ui(msg, context);
            break;
        case 2:
            set_buy_ui(msg, context);
            break;
        case 3:
            set_pay_ui(msg, context);
            break;
        // Keep this
        default:
            PRINTF("Received an invalid screenIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}
