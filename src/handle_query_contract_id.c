#include "thales_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(void *parameters) {
    ethQueryContractID_t *msg = (ethQueryContractID_t *) parameters;
    const context_t *context = (const context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

    switch (context->selectorIndex) {
        case BUY_FROM_AMM:
        case BUY_FROM_AMM_WITH_REFERRER:
            strlcpy(msg->version, "Buy from AMM", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case SELL_TO_AMM:
            strlcpy(msg->version, "Sell to AMM", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case EXERCISE_POSITION:
            strlcpy(msg->version, "Exercise position", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}