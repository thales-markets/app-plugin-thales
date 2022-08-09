#include "thales_plugin.h"

// EDIT THIS: Remove this function and write your own handlers!
static void handle_buy_from_amm(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case MARKET:  // market
            copy_address(context->market, msg->parameter, sizeof(context->market));
            context->next_param = POSITION;
            break;
        case POSITION:  // position
            copy_parameter(context->position, msg->parameter, sizeof(context->position));
            context->next_param = AMOUNT;
            break;
        case AMOUNT:  // amount
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = EXPECTED_PAYOUT;
            break;
        case EXPECTED_PAYOUT:  // expected payout
            copy_parameter(context->expected_payout,
                           msg->parameter,
                           sizeof(context->expected_payout));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    // EDIT THIS: adapt the cases and the names of the functions.
    switch (context->selectorIndex) {
        case BUY_FROM_AMM:
            handle_buy_from_amm(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}