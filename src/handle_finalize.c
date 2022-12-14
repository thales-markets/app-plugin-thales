#include "thales_plugin.h"

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    switch (context->selectorIndex) {
        case BUY_FROM_AMM:
        case BUY_FROM_AMM_WITH_REFERRER:
        case SELL_TO_AMM:
            msg->numScreens = 3;
            break;
        case EXERCISE_POSITIONS:
        case EXERCISE_RANGED_POSITIONS:
            msg->numScreens = 1;
            break;
        default:
            msg->numScreens = 1;
            break;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
