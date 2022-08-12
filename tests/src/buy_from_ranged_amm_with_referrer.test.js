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
        "[Nano " + model.letter + "] Buy from Ranged AMM with refferer",
        zemu(model, async (sim, eth) => {
            const contract = new ethers.Contract(contractAddr, abi);

            // Constants used to create the transaction
            // TEST data: https://polygonscan.com/tx/0xac1b6735cc4c2b198ebbe682d9c3fea9675cff5c77e2320c70dc4fcf2c1472c9
            const market = "0x8f39a46078Fe90b20b3c6d90BdC8aBb9802616Be";
            const position = 0;
            const amount = parseEther("9");
            const expectedPayout = parseUnits("2.906753", 6);
            const slippage = parseEther("0.02");
            const referrer = "0xE21B80181304e3641424cdFb8376E73574ab4794";

            const { data } = await contract.populateTransaction.buyFromAMMWithReferrer(
                market,
                position,
                amount,
                expectedPayout,
                slippage,
                referrer
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
            await sim.navigateAndCompareSnapshots(".", model.name + "_buy_from_ranged_amm_referrer", [right_clicks, 0]);

            await tx;
        })
    );
});
