# Levoit Vital Esphome component

Esphome component for the Levoit Vital **200S**, **200S pro** and **100S**.

As there was no component or firmware or anything for the Levoit Vital 200S i reverse engineered most of the protocol and made a esphome componend to use.
- This should also work with the Levoit Vital 200S Pro, as (i asked the manufacturer) the pro version is the exact same device but in another color.
- If you want to go deeper, i provided my current reverse engineering data ([command table.xlsx](https://github.com/user-attachments/files/23830099/command.table.xlsx)).


# How to?

## Disassemble:
1. To initially flash the Firmware or to make a backup, you must disassemble the device only slightly. It is enough to unscrew the 3 screws on the front of the device on its black top part. To view the screws, remove the white plastic screen in front of the HEPA filter.
2. After unscrewing, slightly pull the black top part towards you. First pull the left side up.
3. The Right side is then mostly stuck by a bracked (see Image below). you can losen it with a Long screwdriver, or if the gap is big enough with your finger.

<img src="https://github.com/user-attachments/assets/5dc789b5-80cd-4462-a1df-b1036ccd0e70" width="240"/>


4. Be cautious, as the cable that is connecting the Motor with the board is very short and can break the plastic connector (see Image below). Try to losen it first. If you reassemble the device later, just remove the cable from the cable Routing, this will give you enogh length to put the top plastic part in place again.

<img src="https://github.com/user-attachments/assets/7fb2a49d-cfcf-4f7d-909d-7339dc4bb23e" width="240"/>


## Flashsing preperation:
Solder cables to all 6 contact Pins of the esp.

<img src="https://github.com/user-attachments/assets/30160229-315b-493a-a895-d496809dc7b1" width="240"/>

* connect IO to GND
* also connect GND to you USB Adapter's GND
* connect RX to your USB adapter's TX
* connect TX to your USB adapter's RX
* connect Vcc3,3V to your USB adapter's VCC
* also connect the Vcc3,3V to EN

* For flashing the device or dumping the firmware you can leave the IO connected to GND.
* Also, make sure that EN is connected to 3,3V
* Also make sure, that you USB Adapter is set to 3,3V, as some of them also have a 5V functionality or Jumper.

## Flashing/backup

- Best practice is if you first backup the Firmware of the device.
- You can Dump the flash with esptool (Windows and linuks works both).

To make a complete backup you can use this command:
 `esptool --port your_port_here read-flash 0 ALL filename.bin `
 
I Used an older USB to Serial Adapter and could not backup with full Speed. If oyu have the same Problems, and the backup process breaks often, try to reduce the Baudrate, this takes very Long but is reliable. I used 19200

 `esptool --port your_port_here --baud 19200 read-flash 0 ALL filename.bin `


 After Dumping you can upload the firmware. Currently i do not provide a binary file for you, but yout can build and upload the component with esphome. 
 - The only thing you must configure in the yaml file are the firts 3 ids (or maybe leave it the way they are) and your wifi/ap data.




## Optional fan integration

If you want you can add this template to your home assitant to enable the fan power and level as a Homeassistant fan.
Thanks to Scags104 for the suggestion.

ids and names should be modified accordingly.
```
- fan:
  - unique_id: brap22_fan 
    name: "BRAP22 Fan"
    state: "{{ states('switch.brap22_power') }}"
    percentage: >-
      {% if is_state('switch.brap22_power', 'on') %}
        {% set level = states('select.brap22_fanlevel') | int(0) %}
        {{ level * 25 if level in [1,2,3,4] else 0 }}
      {% else %}
        0
      {% endif %}
    preset_mode: >-
      {% if is_state('switch.brap22_power', 'on') %}
        {{ states('select.brap22_fanmode') }}
      {% else %}
        None
      {% endif %}
    preset_modes:
      - "Manual"
      - "Automatic"
      - "Sleep"
      - "Pet"
    speed_count: 4
    optimistic: true
    turn_on:
      - service: switch.turn_on
        target:
          entity_id: switch.brap22_power
      - service: select.select_option
        target:
          entity_id: select.brap22_fanmode
        data:
          option: "Manual"
      - service: select.select_option
        target:
          entity_id: select.brap22_fanlevel
        data:
          option: "1"
    turn_off:
      - service: switch.turn_off
        target:
          entity_id: switch.brap22_power
    set_percentage:
      - service: switch.turn_on
        target:
          entity_id: switch.brap22_power
      - service: select.select_option
        target:
          entity_id: select.brap22_fanlevel
        data:
          option: >-
            {% set pct = percentage | int %}
            {{ 1 if pct <= 25 else 2 if pct <= 50 else 3 if pct <= 75 else 4 }}
    set_preset_mode:
      - service: switch.turn_on
        target:
          entity_id: switch.brap22_power
      - service: select.select_option
        target:
          entity_id: select.brap22_fanmode
        data:
          option: "{{ preset_mode }}"
```
