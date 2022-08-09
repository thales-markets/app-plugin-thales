import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models, SPECULOS_ADDRESS, txFromEtherscan } from "./test.fixture";
import { ethers } from "ethers";
import { parseEther, parseUnits } from "ethers/lib/utils";

const contractAddr = "0x9b6d76b1c6140fbb0abc9c4a348bff4e4e8a1213";
const pluginName = "thales";
const abi_path = `../${pluginName}/abis/` + contractAddr + ".json";
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x0160b3aec12fd08e6be0040616c7c38248efb4413168a3372fc4d2db0e5961bb
// EDIT THIS: build your own test
// nano_models.forEach(function (model) {
//     test(
//         "[Nano " + model.letter + "] Swap Exact Eth For Tokens with beneficiary",
//         zemu(model, async (sim, eth) => {
//             // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xb27a69cd3190ad0712da39f6b809ecc019ecbc319d3c17169853270226d18a8a
//             const serializedTx = txFromEtherscan(
//                 "0x8875eb84000000000000000000000000a1b3d01f2837bbe5c2738341155a72e0d7a6e6340000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000002b5e3af16b1880000000000000000000000000000000000000000000000000000000000000296bbf700000000000000000000000000000000000000000000000000470de4df820000"
//             );

//             const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

//             const right_clicks = model.letter === "S" ? 12 : 6;

//             // Wait for the application to actually load and parse the transaction
//             await waitForAppScreen(sim);
//             // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
//             await sim.navigateAndCompareSnapshots(".", model.name + "_swap_exact_eth_for_tokens_with_beneficiary", [
//                 right_clicks,
//                 0,
//             ]);

//             await tx;
//         })
//     );
// });

// Test from constructed transaction
nano_models.forEach(function (model) {
    test(
        "[Nano " + model.letter + "] Buy from AMM",
        zemu(model, async (sim, eth) => {
            const contract = new ethers.Contract(contractAddr, abi);

            // Constants used to create the transaction
            // TEST data: https://polygonscan.com/tx/0xc89c4871212c13894c2c0446c2ae48571175cb3069f394e87273c4ab1bca1d4b
            const market = "0x718Fe7c9E91B0D6F46b37b1d0760707b81aA5aAF";
            const position = 0;
            const amount = parseEther("183.23");
            const expectedPayout = parseUnits("50.692244", 6);
            const slippage = parseEther("0.01");

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

            unsignedTx.chainId = 137;

            // Create serializedTx and remove the "0x" prefix
            const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

            const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

            const right_clicks = model.letter === "S" ? 7 : 5;

            // Wait for the application to actually load and parse the transaction
            await waitForAppScreen(sim);
            // Navigate the display by pressing the right button 10 times, then pressing both buttons to accept the transaction.
            // EDIT THIS: modify `10` to fix the number of screens you are expecting to navigate through.
            await sim.navigateAndCompareSnapshots(".", model.name + "_buy_from_amm", [right_clicks, 0]);

            await tx;
        })
    );
});
