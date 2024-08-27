# RSC Drone Badge

<img src="./rsc-drone-badge.jpg" width="300">

A badge (pendant) in the shape of FPV drone which I designed for my friends at [Resistance Support Club](http://resistance.support). The collective organizes soldering workshops, the collected money goes to Ukrainian defenders.

## BOM (Bill of Material)
<table>
    <tr>
        <th>Part</th>
        <th>Value</th>
        <th>Package</th>
        <th>Part number</th>
        <th>Qty</th>
    </tr>
    <tr>
        <td>IC1</td>
        <td>ATTINY</td>
        <td>DIP8</td>
        <td>ATTINY25V-10PU</td>
        <td>1</td>
    </tr>
    <tr>
        <td>LD1, LD2, LD3, LD4,<br>LD5, LD6, LD7, LD8,<br>LD9, LD10, LD11, LD12,<br>LD13, LD14, LD15, LD16</td>
        <td>Yellow LED</td>
        <td>5mm THT</td>
        <td>N0Y64L35,<br>L-53SYD</td>
        <td>16</td>
    </tr>
    <tr>
        <td>Q1, Q2, Q3, Q4</td>
        <td>BC547</td>
        <td>TO92</td>
        <td>BC547A/B/C</td>
        <td>4</td>
    </tr>
    <tr>
        <td>R1, R2, R3, R4</td>
        <td>10K</td>
        <td>0207</td>
        <td>CF1/4W-10K</td>
        <td>4</td>
    </tr>
    <tr>
        <td>R5, R6, R7, R8</td>
        <td>10R</td>
        <td>0207</td>
        <td>CF1/4W-10R</td>
        <td>4</td>
    </tr>
    <tr>
        <td>C1</td>
        <td>100n</td>
        <td>2.54 ceramic</td>
        <td>CC-100N</td>
        <td>1</td>
    </tr>
    <tr>
        <td>S1</td>
        <td>Tactile button</td>
        <td>6x6mm</td>
        <td>TACT-64K-F,<br>B3F-1020</td>
        <td>1</td>
    </tr>
    <tr>
        <td>BAT</td>
        <td>Battery holder</td>
        <td>CR2032</td>
        <td>DS1092-04-B6P</td>
        <td>1</td>
    </tr>
</table>

Then you just need a regular CR2032 battery and ideally a 2mm nylon string from your local hardware store.

## Description

Custom designed PCB in Eagle, you can find both design files and gerber archive in the `pcb` subfolder. You can easily download the ZIP file with gerber and order the PCBs at your favorite PCB manufacturer. Recommended options are: Blue soldermask, ENIG surface.

The board contains ATtiny 25 MCU, you can find a precompiled HEX and shell upload script in `build` folder. Or you can compile it by yourself, eg. to run on another MCU like ATtiny 85. The source files are in the `code` folder, you'll need Arduino IDE with ATTinyCore for compiling them.

In `drawing` subfolder you can find board drawings in Adobe Illustrator format and a PDF with BOM and build instructions.

Слава Україні! 🇺🇦


## License

MIT, feel free to share!
