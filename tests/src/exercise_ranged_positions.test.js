import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models } from "./test.fixture";
import { ethers } from "ethers";

const contractAddr = "0x75c7671d046268c11b5f55ba75df2b6d14774b1c";
const pluginName = "thales";
const testNetwork = "polygon";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + ".json";
const abi = require(abi_path);

// Test from constructed transaction
nano_models.forEach(function (model) {
    test(
        "[Nano " + model.letter + "] Exercise ranged positions",
        zemu(model, async (sim, eth) => {
            const contract = new ethers.Contract(contractAddr, abi);

            const { data } = await contract.populateTransaction.exercisePositions();

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

            const right_clicks = model.letter === "S" ? 5 : 5;

            // Wait for the application to actually load and parse the transaction
            await waitForAppScreen(sim);
            // Navigate the display by pressing the right button 10 times, then pressing both buttons to accept the transaction.
            // EDIT THIS: modify `10` to fix the number of screens you are expecting to navigate through.
            await sim.navigateAndCompareSnapshots(".", model.name + "_exercise_ranged_positions", [right_clicks, 0]);

            await tx;
        })
    );
});
