#include "thales_plugin.h"

static void handle_buy_from_amm(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case MARKET:  // market
            copy_address(context->market, msg->parameter, sizeof(context->market));
            context->next_param = POSITION;
            break;
        case POSITION:  // position
            if (memcmp(msg->parameter, AMM_POSITION_UP, INT256_LENGTH) == 0) {
                strlcpy(context->ticker, "UP ", sizeof(context->ticker));
            } else if (memcmp(msg->parameter, AMM_POSITION_DOWN, INT256_LENGTH) == 0) {
                strlcpy(context->ticker, "DOWN ", sizeof(context->ticker));
            } else {
                strlcpy(context->ticker, "??? ", sizeof(context->ticker));
            }
            context->decimals = WEI_TO_ETHER;
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
            context->next_param = SLIPPAGE;
            break;
        case SLIPPAGE:  // slippage
            context->next_param = REFERRER;
            break;
        case REFERRER:  // referrer
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_exercise_position(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
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

    switch (context->selectorIndex) {
        case BUY_FROM_AMM:
        case BUY_FROM_AMM_WITH_REFERRER:
            handle_buy_from_amm(msg, context);
            break;
        case EXERCISE_POSITION:
            handle_exercise_position(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}