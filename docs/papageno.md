
# Papageno: Turn your keyboard into a magical musical instrument

Inspired by the upcoming thumb clusters that many modern ergonomic keyboards provide, the idea was born to extend the idea of tap-dances to multiple keys. Why not trigger an action when several keys are hit in a predefined order that defines a pattern. This idea led to the development of [Papageno](https://github.com/noseglasses/papageno/), an advanced pattern matching library whose features rapidly grew far beyond advanced tap-dances. Originally implemented as integrated in QMK, Papageno emerged to a stand-alone multi-platform library. However, it still integrates seamlessly with QMK.

The impatient may directly jump to a description of the innovative use of multi key patterns [multi key patterns](https://github.com/noseglasses/noseglasses_qmk_layout/), e.g. on an ErgoDox or Planck keyboard.

## Features
The most prominent of Papageno's features are multi-key patterns, tap-dances and arbitrary leader sequences. Although all of these features are specified and used independently, they are nonetheless compiled into a common search tree. The emerging search tree allows for very efficient pattern matching. By means of an integrated compression mechanism the searach tree can be converted to a compile-time static data structure which reduces both Flash memory and SRAM consumption.

## Compatibility with existing QMK features
The efficiency of compressed tree approach is also the reason why some features that were already part of QMK, such as tap-dances and leader sequences have been incorporated into Papageno. Another reason is that they are just so simple to implement based on Papageno's general search tree.
If other advanced features provided by Papageno are used together with equivalent pre-existing QMK-features, it is recommended to use those versions that are integrated in Papageno instead of mixing different QMK-enhancements.  However, this is only important if resources such as program storage and SRAM memory are scarce. If applied to [noseglasses](https://github.com/noseglasses/noseglasses_qmk_layout/) keymap compiled for an ErgoDox EZ the compression reduces the size of the emerging hex-file by 2420 byte. The program storage saved can be used for other valuable features.

Papageno works as wrapper to QMK. It passes keystrokes through its own pattern matching engine and only if a series of keystrokes is identified as non-matching, they are passed over to the main QMK engine. Through this strong decoupling Papageno can be expected to work quite well with most other features provided by QMK.

For compatibility reasons, Papageno provides a layer mechanism that equals that of QMK. All defined patterns are assigned to a layer which makes them only active if the current layer matches.

It is common to assign QMK keycodes as actions that are executed when patterns match keystroke sequences. However, it is also possible to define user callback functions.

## Example usage
The general use of Papageno-QMK is exemplified in noseglasses' [keymap](https://github.com/noseglasses/noseglasses_qmk_layout/). See also the file `process_keycode/process_papageno.h` where a number of C-macros are defined that simplify the specification of Papageno patterns.


