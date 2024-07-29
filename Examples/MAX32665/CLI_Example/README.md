## Description

A basic program to show how to use CLI library and design your own functions which can be called from serial terminal application over COM.\
This version of CLI application has three functions defined:\
a) cli_led_toogle: Toggles the led.\
b) cli_to_uppercase: Converts the word that is passed in lower-case to upper-case and sends back to host computer.\
c) cli_square: Squares the given number and returns back to the host computer.\

Note: This example is tested with EvKit_V1. So, you need to change [**board**](./.vscode/settings.json) variable in [**.vscode/settings.json**](./.vscode/settings.json) to **"board":"FTHR"** if you use MAX32666FTHR.

## Software

### Project Usage

Universal instructions on building, flashing, and debugging this project can be found in the **[MSDK User Guide](https://analogdevicesinc.github.io/msdk/USERGUIDE/)**.

### Project-Specific Build Notes

(None - this project builds as a standard example)

## Required Connections

-   Connect a USB cable between the PC and the MAX32666FTHR.
-   Open an terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1.

## Expected Output

The Console UART of the device will output these messages:

```
CLI Initialized! Enter 'help' to see a list of available commands.

$ help

CMD_LED_TOGGLE:
  Usage: [CMD_LED_TOGGLE] on command line
  Description: Changes led0 status

CMD_UPPERCASE:
  Usage: [CMD_UPPERCASE] [lowercase string]
  Description: Converts lower case characters to uppercase ones.

CMD_SQUARE:
  Usage: [CMD_SQUARE] [number to be squared]
  Description: multiples the number entered with itself.

$ CMD_LED_TOGGLE

$ Led toggled
CMD_UPPERCASE helloworld

$ HELLOWORLD
CMD_SQUARE 7

$ 49
```

