import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models } from "./test.fixture";
import { ethers } from "ethers";
import { parseEther, parseUnits } from "ethers/lib/utils";

const contractAddr = "0xe8e022405505a9f2b0b7452c844f1e64423849fc";
const pluginName = "thales";
const testNetwork = "polygon";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + ".json";
const abi = require(abi_path);

// Test from constructed transaction
nano_models.forEach(function (model) {
    test(
        "[Nano " + model.letter + "] Sell to Ranged AMM",
        zemu(model, async (sim, eth) => {
            const contract = new ethers.Contract(contractAddr, abi);

            // Constants used to create the transaction
            // TEST data: https://polygonscan.com/tx/0xae81e73fad601907ad5ba4c5177815625eba6cd6130e53b485aa82ca93278eaa
            const market = "0xf8a811eCBc6A95Ebe05D922936b6b1833817793B";
            const position = 1;
            const amount = parseEther("4.48");
            const expectedPayout = parseUnits("1.084242", 6);
            const slippage = parseEther("0.02");

            const { data } = await contract.populateTransaction.sellToAMM(
                market,
                position,
                amount,
                expectedPayout,
                slippage
            );

            // Get the generic transaction template
            let unsignedTx = genericTx;
            // Modify `to` to make it interact with the contract
            unsignedTx.to = contractAddr;
            // Modify the attached data
            unsignedTx.data = data;
            // Modify the chain ID
            unsignedTx.chainId = 137;

            // Create serializedTx and remove the "0x" prefix
            const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

            const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

            const right_clicks = model.letter === "S" ? 9 : 7;

            // Wait for the application to actually load and parse the transaction
            await waitForAppScreen(sim);
            // Navigate the display by pressing the right button 10 times, then pressing both buttons to accept the transaction.
            // EDIT THIS: modify `10` to fix the number of screens you are expecting to navigate through.
            await sim.navigateAndCompareSnapshots(".", model.name + "_sell_to_ranged_amm", [right_clicks, 0]);

            await tx;
        })
    );
});
