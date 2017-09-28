
# Papageno: Turn your keyboard into a magical musical instrument

Most modern ergonomic keyboards come with thumb clusters motivated by the obvious under-utilization of the thumbs on traditional computer keyboards.

If there are dedicated thumb keys and the thumbs are the most powerful fingers it is reccomended to offload as much work as possible from the other fingers to the thumbs. In consequence this means that it is necesarry to assign as much functionality as possible to the typically restricted amount of reachable thumb keys. 

Tap-dances, an estimated feature that is part of QMK for a while now are one possible way to go, albeit not the perfect solution. This is because traditional tap-dances come with the restriction that a key needs to be hit multiple times to trigger an action. This can be difficult for a higher number of keystrokes of the same key, especially when one wants to distinguish between, say, three, four and five strokes. Even with a significant amount of training this can remain difficult, especially for non pianists.

So why not extend the concept of tap-dances to combinations of several keys. Wouldn't it be nice to trigger an action when several keys are hit in a predefined order. It is more simple to hit two keys in a row than the same key twice. Hitting two keys A and B in order A-B to generate a tabulator character and in order B-A to issue a shortcut that un-tabs a code line in you favorite text editor might be an example that hopefully demonstrates the power of this approach. It is both more easy from a neuromuscular point of view and quite intuitive in a sense that it assigns a forward-backward relation to tab and un-tab by the order of the two keys, especially provided key A is left of B (when used to reading and writing from left to right).

To allow the described to work, we need a mechanism that is capable to recognize complex keystroke patterns. This very requirement led to the development of [Papageno](https://github.com/noseglasses/papageno/), an advanced pattern matching library that provides way more functionality than advanced tap-dances only. Originally implemented as integrated in QMK, Papageno emerged to a stand-alone multi-platform library that still seamlessly integrates with QMK.

## Example usage

The impatient may directly jump to a QMK example that demonstrates how [Papageno](https://github.com/noseglasses/noseglasses_qmk_layout/) may be used to shift load from fingers to thumbs.

## Features
Appart from the advanced tap dances as already introduced, Papageno's main features are general multi-key patterns, chords, key clusters and arbitrary leader sequences. All these features are based on a common pattern matching tree. It allows for very efficient pattern matching without the need to reveal this implementation detail in the Papageno-QMK API. As common for dynamically assembled tree data structures, this approach relies on dynamic memory allocation. Although many believe that dynamically allocated data structures are a no-go on embedded architectures, such as those used in todays programmable keyboards, it is not that much of a hindrance as it seems at first glance. The main reason is that Papageno mainly only allocates memory but does not free it during program execution. This inherently avoids RAM fragmentation. 

## Compression of data structures

Papageno provides two modes of operation. The basic one of the two generates dynamic data structures during firmware execution on the keyboard. A more advanced mode of execution allows for a two stage compile process that compresses all data structures and tries to be as memory efficient as possible, both with respect to flash memory and RAM.

Where the dynamic creation of tree data structures works well for a moderate amount of key patterns, it can be problematic when a large amount of patterns is defined or if many other features of QMK are used along with Papageno. In such a case resources might be exhausted.

In such a case, Papageno's integrated compression mechanism helps to significantly reduce flash and RAM consumption. To achieve this, the pattern matching search tree is converted to a compile-time static data structure by a two stage compile process. We thus avoid dynamic alloactions and significantly shrink binary size. Appart from the shear compactification of data structures that also safes memory at runtime, the reduced binary size mainly results from the fact that no code is required to establish dynamic data structures during firmware execution. The respective C-functions can, therefore, simply be stripped from the binary.

## Compatibility with existing QMK features
The efficiency of the tree based pattern matching approach is one of the reasons why some features that were already part of QMK, such as tap-dances and leader sequences, have been incorporated into Papageno. Another reason is that they were so simple and fun to implement based on Papageno.

But let's concentrate on the primary motivation, efficiency. If other advanced features provided by Papageno are used together with equivalent pre-existing QMK-features, program memory consumption is significantly higher than when Papageno deals with all of the work alone. 
Technically there are no reasons other than bloat of the generated firmware binary. Papageno cooperates well with other QMK features. However, minimizing resource requirements is never a bad idea.

When applied to [noseglasses'](https://github.com/noseglasses/noseglasses_qmk_layout/) keymap compiled for an ErgoDox EZ, compression reduces the size of the emerging hex-file by 2420 byte. The program storage saved can e.g. be used for other valuable QMK features.

## How it all works together

Papageno-QMK is designed as a wrapper to the rest of QMK. It intercepts keystrokes and passes them through its own pattern matching engine. Only if a series of keystrokes is identified as non-matching, it is passed over to the main QMK engine. Through this strong decoupling Papageno-QMK is robust and may be expected to work quite well with most other features provided by QMK.

For compatibility reasons, Papageno provides a layer mechanism that is fairly similay to that of QMK. A their point of definition, patterns are assigned to layers. This causes them only to be active while their associated layer is activated. Fallthrough, works similar to assigning transparent keys to QMK keymaps.

## Actions
It is most common to emit QMK keycodes when a defined pattern matches a series of keystrokes.
If more advanced functionality is required, it is also possible to define arbitrary user callback functions that are supplied with user defined data.

## Example usage and further reading
The general use of Papageno-QMK is exemplified in noseglasses' [keymap](https://github.com/noseglasses/noseglasses_qmk_layout/). See also the file `process_keycode/process_papageno.h` where a number of C-macros are defined that simplify the specification of Papageno patterns. 

For a understanding of Papageno's algorithms and concepts, please see its [documentation](https://github.com/noseglasses/papageno/).


