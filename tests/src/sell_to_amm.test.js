import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models } from "./test.fixture";
import { ethers } from "ethers";
import { parseEther, parseUnits } from "ethers/lib/utils";

const contractAddr = "0x9b6d76b1c6140fbb0abc9c4a348bff4e4e8a1213";
const pluginName = "thales";
const testNetwork = "polygon";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + ".json";
const abi = require(abi_path);

// Test from constructed transaction
nano_models.forEach(function (model) {
    test(
        "[Nano " + model.letter + "] Sell to AMM",
        zemu(model, async (sim, eth) => {
            const contract = new ethers.Contract(contractAddr, abi);

            // Constants used to create the transaction
            // TEST data: https://polygonscan.com/tx/0x00b9368023d51a5b706e39816f433944108bf2e1a6a5a1bb7fe28a559e3521f3
            const market = "0x6d5934c27E0b84Cf2434988C945114e43cf5CFd1";
            const position = 0;
            const amount = parseEther("1.74");
            const expectedPayout = parseUnits("1.463652", 6);
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
            await sim.navigateAndCompareSnapshots(".", model.name + "_sell_to_amm", [right_clicks, 0]);

            await tx;
        })
    );
});
