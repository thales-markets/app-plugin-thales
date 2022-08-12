# Ledger Thales Plugin

[![Compilation & tests](https://github.com/thales-markets/app-plugin-thales/workflows/Code%20style%20check/badge.svg)](https://github.com/thales-markets/app-plugin-thales/actions/workflows/lint-workflow.yml)
[![Compilation & tests](https://github.com/thales-markets/app-plugin-thales/workflows/Compilation%20%26%20tests/badge.svg)](https://github.com/thales-markets/app-plugin-thales/actions?query=workflow:"Compilation+&+tests")

This is a plugin for the Ethereum application which helps parsing and displaying relevant information when signing a Thales transaction.

## Prerequisites

### Clone repositories

Clone the plugin to a new folder:

```shell
git clone https://github.com/thales-markets/app-plugin-thales.git
```

Then in the same folder clone two more repositories, which is the "plugin-tools" and "app-ethereum":

```shell
git clone https://github.com/LedgerHQ/plugin-tools.git                          # plugin-tools
git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum.git     # app-ethereum
```

### Connect to the container

Install [Docker](https://docs.docker.com/get-docker/) and [Docker compose](https://docs.docker.com/compose/install/).

In the same folder as above, simple type:

```shell
cd plugin-tools     # go to plugin-tools folder
./start.sh          # connect to the container
```

You are now connected to the container.
At the prompt, `ls` gives:

```shell
app-ethereum    plugin-tools
```

### Compile the Ethereum app

Still in the terminal, compile the Ethereum app:

```shell
cd app-ethereum     # go to app-ethereum folder
make                # compile the Ethereum app
```

If everything goes well, you should end with:

```shell
...
[LINK] bin/app.elf
```

## Documentation

It is STRONGLY recommended to follow the [plugin guide](https://developers.ledger.com/docs/dapp/nano-plugin/overview/) in order to better understand the flow and the context for plugins.

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://github.com/LedgerHQ/app-ethereum/blob/master/doc/ethapp_plugins.asc).

## Smart Contracts

Smart contracts covered by this plugin are:

| Network | Name          | Smart Contract                               |
| ------- | ------------- | -------------------------------------------- |
| Polygon | AMM           | `0x9b6d76B1C6140FbB0ABc9C4a348BFf4e4e8a1213` |
| Polygon | Ranged AMM    | `0xe8e022405505a9F2b0B7452C844F1e64423849fC` |
| Polygon | Market        | `0x7465c5d60d3d095443CF9991Da03304A30D42Eae` |
| Polygon | Ranged Market | `0x75c7671d046268c11b5f55bA75DF2B6d14774B1C` |

## Build

Go to the plugin-tools folder and run the `./start.sh` script.

```shell
cd plugin-tools     # go to plugin-tools folder
./start.sh          # run the script start.sh
```

The script will build a docker image and attach a console.
When the docker image is running go to the "app-plugin-thales" folder and build the `.elf` files.

```shell
cd app-plugin-thales/tests      # go to the tests folder in app-plugin-thales
./build_local_test_elfs.sh      # run the script build_local_test_elfs.sh
```

## Tests

To test the plugin go to the tests folder from the "app-plugin-thales" and run the script `test`

```shell
cd app-plugin-thales/tests      # go to the tests folder in app-plugin-thales
yarn test                       # run the script test
```

## Continuous Integration

The flow processed in [GitHub Actions](https://github.com/features/actions) is the following:

-   Code formatting with [clang-format](http://clang.llvm.org/docs/ClangFormat.html)
-   Compilation of the application for Ledger Nano S, Ledger Nano X and Ledger Nano S+ in [ledger-app-builder](https://github.com/LedgerHQ/ledger-app-builder)
