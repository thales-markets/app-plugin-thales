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
        "[Nano " + model.letter + "] Buy from AMM with refferer",
        zemu(model, async (sim, eth) => {
            const contract = new ethers.Contract(contractAddr, abi);

            // Constants used to create the transaction
            // TEST data: https://polygonscan.com/tx/0x6574ca95f2932755f5a8f17497b693cd245379064095c86e59a5d21179bfcbef
            const market = "0x718Fe7c9E91B0D6F46b37b1d0760707b81aA5aAF";
            const position = 1;
            const amount = parseEther("5");
            const expectedPayout = parseUnits("3.505472", 6);
            const slippage = parseEther("0.02");
            const referrer = "0xe966C59c15566A994391F6226fee5bc0eF70F87A";

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
            await sim.navigateAndCompareSnapshots(".", model.name + "_buy_from_amm_referrer", [right_clicks, 0]);

            await tx;
        })
    );
});
