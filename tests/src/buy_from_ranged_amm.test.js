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
        "[Nano " + model.letter + "] Buy from Ranged AMM",
        zemu(model, async (sim, eth) => {
            const contract = new ethers.Contract(contractAddr, abi);

            // Constants used to create the transaction
            // TEST data: https://polygonscan.com/tx/0xa23ed31f6ad2dece212373bbb27b1b5573e3bdd051afd9cb05f262a6616c8b94
            const market = "0x1AfD09FF3F5aeaC15b4F9734970AA48154F198c4";
            const position = 0;
            const amount = parseEther("1334.77");
            const expectedPayout = parseUnits("910.369524", 6);
            const slippage = parseEther("0.02");

            const { data } = await contract.populateTransaction.buyFromAMM(
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
            await sim.navigateAndCompareSnapshots(".", model.name + "_buy_from_ranged_amm", [right_clicks, 0]);

            await tx;
        })
    );
});
