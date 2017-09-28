
# Papageno: Turn your keyboard into a magical musical instrument

Inspired by the highly useful thumb clusters that many modern ergonomic keyboards provide, the idea was born to assign as much functionality as possible to a restricted number of thumb keys. One way to achieve this are tap-dances as already supported by QMK for a while. Unfortunately tap-dances come with the restriction that a key has to be hit multiple times to trigger an action. Especially with a higher number of strokes it is difficult to hit a key the necessary number of times. It is e.g. difficult to distinguish between three and four strokes. Also it requires a significant amount of training to get used to muli-stroke tap-dances. 

All of this led to the idea to extend the concept of tap-dances to combinations of several keys. Why not trigger an action when several keys are hit in a predefined order. The requirement to recognize keystroke patterns motivated the development of [Papageno](https://github.com/noseglasses/papageno/). Papageno is an advanced pattern matching library whose features go far beyond only providing advanced tap-dances. Originally implemented as integrated in QMK, Papageno emerged to a stand-alone multi-platform library. Nevertheless, it still integrates seamlessly with QMK.

The impatient may directly jump to an example how to use [multi key patterns](https://github.com/noseglasses/noseglasses_qmk_layout/), e.g. on an ErgoDox EZ or Planck keyboard.

## Features
Some of Papageno's features are multi-key patterns, tap-dances and arbitrary leader sequences. Although all of these may be arranged and used independently, they are nonetheless compiled by Papageno into a common pattern matching tree. This search tree allows for very efficient pattern matching. As common for dynamically assembled tree data structures, this approach in its simplest form relies on dynamic memory allocation. Although many believe that dynamically allocated data structures are a no-go on embedded architectures, it is not that much of a burden as it seems at first glance. The main reason is that we mostly only allocate memory but do not free it during program execution, which automatically avoids RAM fragmentation. 

## Compression of data structures

Where the dynamic creation of tree data structures works well for a moderate amount of key patterns, it can be problematic when a large amount of patterns are defined or if many other features of QMK are used along with Papageno patterns. In such a case resources might be exhausted.

In such a case, Papageno's integrated compression mechanism helps to significantly reduce flash and RAM consumption. To achieve this, the pattern matching search tree is converted to a compile-time static data structure by a two stage compile process, thus, avoiding dynamic alloactions and shrinking binary size. Appart from the shear compactification of data structures, the reduced binary size mainly results from the fact that no code is required to establish the dynamic data structures. The respective functions are simply stripped from the binary.
The user is free to choose between both modes of operation.

## Compatibility with existing QMK features
The efficiency of the tree based pattern matching approach is one of the reasons why some features that were already part of QMK, such as tap-dances and leader sequences have been incorporated into Papageno. Another reason is that they were so simple and fun to implement based on Papageno.

Let's concentrate on the primary motivation, efficiency: If other advanced features provided by Papageno are used together with equivalent pre-existing QMK-features, flash memory consumption is significantly higher than when Papageno deals with all of it alone. 
Thus, although technically there are no reasons other than bloat of the generated firmware binary - Papageno cooperates well with other QMK features - it is recommended to use those features that are integrated in Papageno if Papageno is required anyway. 

When applied to [noseglasses'](https://github.com/noseglasses/noseglasses_qmk_layout/) keymap compiled for an ErgoDox EZ the compression reduces the size of the emerging hex-file by 2420 byte. The program storage saved can be used for other valuable QMK features.

## How it works together

Papageno-QMK is designed to be a wrapper to the rest of QMK. It first passes keystrokes through its own pattern matching engine and only if a series of keystrokes is identified as non-matching, they are passed over to the main QMK engine. Through this strong decoupling Papageno-QMK is robust and can be expected to work quite well with most other features provided by QMK.

For compatibility reasons, Papageno provides a layer mechanism that equals that of QMK. When defined, patterns are assigned to layers which makes them only active if their associated layer is activated.

It is most common to assign QMK keycodes as actions that are executed when patterns match keystroke sequences. If more advanced functionality is required, it is also possible to define arbitrary user callback functions that are supplied with user defined data.

## Example usage
The general use of Papageno-QMK is exemplified in noseglasses' [keymap](https://github.com/noseglasses/noseglasses_qmk_layout/). See also the file `process_keycode/process_papageno.h` where a number of C-macros are defined that simplify the specification of Papageno patterns. For a understanding of Papageno's algorithms and concepts, please see its [documentation](https://github.com/noseglasses/papageno/).


