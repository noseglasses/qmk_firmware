
# Papageno: Turn your keyboard into a magical musical instrument

## Motivation

### Thumb keys on ergonomic keyboards
Most modern ergonomic keyboards come with thumb clusters. They are mainly motivated by the obvious under-utilization of the thumbs on traditional computer keyboards. 
As our thumbs are the most powerful fingers it is desirable to shift as much work as possible from the other fingers to the thumbs. The number of (reachable) thumb keys differs between keyboard designs. Typically there are between four and eight (reachable) and freely programmable thumb keys available.
This restriction in number means that when assigning keycodes or actions to the keys it is important to do it in a way that maximizes both, utilization and usability. In the past there have occured a significant number of innovative ideas that try to assign more than two keycodes or actions to a physical key.

### Drawbacks of existing solutions
Tap-dances, e.g. are one possible way to go. They theoretically allow to assign an infinite number of meanings to a single physical key. However, "traditional" tap-dances come with the restriction that a key needs to be hit multiple times to trigger an action. This can be difficult faily difficult to accomplish, especially when there are definitions for say, three, four and five strokes. Even with a significant amount of training it remains difficult to hit a given key a specific number of times in a short time interval. This is even more difficult for non pianists.

### Multi-key tap-dances
Why not extend the concept of tap-dances to combinations of several keys? It is much easier to hit two, three or more keys in a defined order, especially as that is precicesly what typists are trained to do.
Of course, this implies that the keystrokes are consumed when the assigned action is triggered. So the action is supposed to replace the series of keycodes that is assigned with the keys when hit independently.
Returning to where we came from, the thumb keys, this means that it would be useful to e.g. assign actions to combinations of two thumb keys that are assigned to the left and the right hand.

### Some numbers
For a keyboard with two thumb keys for each hand, this would mean that we would end up with eight possible two-key combinations of thumb keys assigned to different hands. Hereby we assume that hitting two keys A and B in different orders has different meanings. If we combine these eight possibilities with the four possible actions assigned to keys it hit independently, this sums up to twelve different possibilities.
It is up to you to compute the increase in number if we would also consider three-key combinations.

### A simple real-live use case
Let's look at a brief example how this could be used. Imagine editing some sort of source code. A common task is to indent lines, either to improve readability or because the language uses indentation as part of its semantic. Sometimes it is necessary to un-indent a line of code. Most editors assign shortcuts to both operations that are more or less simple to enter.
As it is a failry common operation, it might be a good idea to perform indentation and un-indentation using thumb-keys. 

We now assign the name A to one of the left hand thumb-keys and B to one of the right hand thumb keys. 
One efficient solution for the given indentation/un-indentation task would be to trigger indentation 
 when keys A and B are hit in order A-B and un-indentation when hit in order B-A. Both operations would of coures emit the shortcut necessary.
This key-assignment is quite optimal from a neuromuscular point of view as it is very easy to learn. It is, moreover, quite intuitive in a sense that it assigns a forward-backward relation to tab and un-tab by the order of the two keys being hit.

### Papageno - why it is there
To bring the above example to live, we need a mechanism that is capable to recognize more or less complex keystroke patterns. This requirement, among others,
led to the development of [Papageno](https://github.com/noseglasses/papageno/), an advanced pattern matching library. Originally implemented as integrated in QMK, Papageno emerged to a stand-alone multi-platform library that still seamlessly integrates with QMK. During its development it underwent quite an evolution that broadened its range of features in a way that goes far beyond advanced tap-dances.

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


