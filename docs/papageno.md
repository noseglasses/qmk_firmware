
# Papageno: Turn your keyboard into a magical musical instrument

## Motivation

### Utilization of thumb keys on ergonomic keyboards

Most modern ergonomic keyboards come with thumb clusters. They are mainly motivated by the obvious under-utilization of the thumbs on traditional computer keyboards. 
As our thumbs are the most powerful fingers it is desirable to shift as much work as possible from the other fingers to the thumbs. The number of (reachable) thumb keys differs between keyboard designs. Typically there are between four and eight (reachable) and freely programmable thumb keys available.
This restriction in number means that when assigning keycodes or actions to the keys it is important to do it in a way that maximizes both, utilization and usability. In the past there have occured a significant number of innovative ideas that try to assign more than two keycodes or actions to a physical key.

### Existing features

Tap-dances, modifier keys, with and without one touch as well as keymap-layers are different ways to creatively assign work to thumb keys using QMK. They all have their specific advantages and drawbacks. A thorough analysis of the different ways to utilize and combine those could fill an article of its own. Therefore, we will concentrate on tap-dances to explain the enhancements that the use of Papageno introduces. 

Tap-dances theoretically allow to assign an infinite number of meanings to a single physical key. A specific number of keypresses triggers a specific action.
This is, however, already the greatest and probably the only disadvantage of "traditional" tap-dances. Hitting a key a given number of times in a row can be faily difficult to accomplish, especially when there are definitions for say, three, four and five strokes. This is even more difficult for non pianists.

### Multi-key tap-dances

As a keyboard comes with a multitude of keys, why not extend the concept of tap-dances to combinations of several keys? It is much easier to hit two, three or more keys in a defined order than to hit the same key a given number of times. Especially as that is precicesly what typists are trained to do.
 
The action triggered is supposed to replace the series of keycodes that is assigned with the keys when hit independently, same as it happens with tap-dances.

### Some numbers

Returning to where we came from, shifting work to thumb keys, this means that it would be useful to e.g. assign actions to combinations of two thumb keys that are assigned to the left and the right hand.
For a keyboard with two thumb keys for each hand, this would mean that we would end up with eight possible two-key combinations of thumb keys assigned to different hands. Hereby we assume that hitting two keys A and B in different orders is given different meanings. If we combine these eight combinations with the four possible actions assigned to keys when hit independently, this sums up to twelve different actions for four thumb keys.

It is up to the reader to compute the drastic increase in number if we would also consider three-key combinations.

### A simple real-live use case

Let's look at a brief example how this could be applied. Imagine editing some sort of source code. A hereby common task is to indent lines, either to improve readability or because the language requires indentation as part of its semantic. Often it is also necessary to un-indent lines of code. Most editors assign shortcuts to both operations that are not always quite simple to enter.
As (un-)indentation is a failry common operation when programming, it might be a good idea to assign this task to thumb-keys. 

We furtheron use the name A for one of the left hand thumb-keys and B for one of the right hand thumb keys. 
One possible solution for the given task would be to indent
 when keys A and B are hit in order A-B and un-indent when hit in order B-A. Both operations would be programmed to emit the required shortcut.
From a neuromuscular point of view this key-assignment strategy is a good choice as it is quite easy to learn. Moreover, it is intuitive in a sense that it assigns a forward-backward relation to indentation and un-indentation related to the order of the two keys being hit. This also helps memorizing new key assignments.

### Papageno - a pattern matching library

To bring all the above to live, we need a mechanism that is capable to recognize more or less complex keystroke patterns. This requirement, among others,
led to the development of [Papageno](https://github.com/noseglasses/papageno/), an advanced pattern matching library. Originally implemented as part of QMK, Papageno emerged to a stand-alone multi-platform library that still seamlessly integrates with QMK. It provides many different ways to define patterns, far beyond advanced tap-dances only.

## A QMK example

The impatient may directly jump to a QMK example keymap that demonstrates how [Papageno](https://github.com/noseglasses/noseglasses_qmk_layout/) may be used to efficiently shift load from fingers to thumbs.

## Features and Implementation

Papageno allows for the definition of general multi-key patterns, chords, key clusters and arbitrary leader sequences and more. All features are based on the same efficient and optimized pattern matching algorithm. To collaborate with QMK, Papageno provides its own QMK-wrapper API that allows to use most of its features in QMK keymap definitions.

The heart of Papageno is a search tree that is generated based on the definitions in the user-implemented QMK keymap code.
As typical for dynamically assembled tree data structures, this approach relies on dynamic memory allocation. Many believe that dynamically allocated data structures are a no-go on embedded architectures, such as those used in most programmable keyboards. Actually, it is not that much of a hindrance as Papageno mainly allocates memory but does not free it during program execution. This inherently avoids RAM fragmentation which would otherwise lead to an early exhaustion of RAM. To optimize the utilization of RAM, Papageno comes with an integrated copression mechanism that will be introduced in the following.

## Compression of data structures

Papageno provides two modes of operation. The basic one of the two generates dynamic data structures during firmware execution on the keyboard. A more advanced mode of execution allows for a two stage compile process that compresses all data structures and aims to be as memory efficient as possible, both with respect to flash memory and RAM.

Where the dynamic creation of tree data structures works well for a moderate amount of key patterns, it can be problematic when a large amount of patterns is defined or if many other features of QMK are used along with Papageno. In such a case the limited resources of the keyboard hardware might be a problem.

Papageno's integrated compression mechanism helps to significantly reduce flash and RAM consumption. For this to be achieved, the pattern matching search tree is converted to a compile-time static data structure during a two stage compile process. We, thus, avoid dynamic memory allocations and significantly shrink binary size. But appart from the shear compactification of data structures we also safe memory at runtime. This is because a reduction of binary size mainly results from the fact that no code is required to establish dynamic data structures during firmware execution. The respective C-functions are, therefore automatically stripped from the binary as part of the firmware build process.

## Compatibility with existing QMK features

The efficiency of Papageno's tree based pattern matching approach is one of the reasons why some features that were already part of QMK, such as tap-dances and leader sequences, have been incorporated into Papageno (Another reason is that they were fun to implement based on Papageno).
But let's concentrate on our primary motivation, efficiency.

If features provided by Papageno are used in common with equivalent pre-existing QMK-features, such as e.g. leader sequences, that are now also supported by Papageno, resource usage can be expected to be significantly higher than when Papageno deals with the given tasks alone. The reason is obvious, more program memory and probably more RAM is required to store binary code and variables for two or more modules instead of just one. 

Although optimized resource utilization is never a bad idea, Papageno cooperates well with other QMK features. 

When applied to [noseglasses'](https://github.com/noseglasses/noseglasses_qmk_layout/) keymap compiled for an ErgoDox EZ, Papageno's compression mechanism reduces the size of the emerging hex-file by 2420 byte. Program storage and RAM saved can e.g. be used for other valuable QMK features.

## QMK and Papageno - how it all works together

Papageno-QMK is designed as a wrapper to the rest of QMK. It intercepts keystrokes and passes them through its own pattern matching engine. Only if a series of keystrokes is identified as non-matching, it is passed over to the main QMK engine. QNK then sees these keystrokes as if they had just come from the most recent keyboard matrix scan.

Through this strong decoupling Papageno-QMK is robust and can be expected to work quite well with most other features provided by QMK.

For compatibility reasons, Papageno provides a layer mechanism that is fairly similay to that of QMK's keymaps. At their point of definition, patterns are assigned to layers. This causes them only to be active while their associated layer is activated. Fallthrough, works similar to assigning transparent keys to QMK keymaps.

## Actions

It is most common to emit QMK keycodes when a defined pattern matches a series of keystrokes.
If more advanced functionality is required, it is also possible to define arbitrary user callback functions that are supplied with user defined data.

## Introduction to Papageno' QMK API

### Key definitions

Papageno as integrated with QMK operates on key-positions or key-codes as building blocks for patterns. Using key-positions means that a physical key, i.d. the keyswitch at a given position on the keyboard is assigned with an identifier. Keycodes in contrast means that an QMK/TMK keycode is assigned an identifier that is used by Papageno for pattern matching. In the latter case, when a key is it, it is passed through QMK's keycode lookup to determine the assinged keycode that is in turn passed to Papageno's pattern matching engine.

To define a key-position, add the following code to your keymap. The example assumes that we want to assign names to the matrix keyposition 5,2 and 5,B which represent the left and right inner thumb keys on an ErgoDox EZ. 

Please note the macro parameter S, that is an auxiliary parameter that is necessary for implementation purposes. The given name S is arbitrarily chosen.

```C
#define LEFT_INNER_THUMB_KEY(S)     NG_KEYPOS(5, 2, S)
#define RIGHT_INNER_THUMB_KEY(S)     NG_KEYPOS(5, B, S)
```

All keys that are supposed to be used in Papageno patterns must be named in the demonstrated way.


### Initialization of the pattern matching engine

To set up all Papageno patterns it is recommended to define a dedicated initialization function. 

```C
void init_papageno(void)
{
   // Initialize the Papageno-QMK system
   //
   PPG_QMK_INIT
   
   // Define an indivieual timeout. Keystrokes whose time interval
   // exceeds the timeout are considered individually and not
   // as part of a pattern.
   //
   ppg_qmk_set_timeout_ms(200);
   
   // Left inner and right inner thumb key trigger emission of the 
   // enter keycode (KC_ENTER) if clustered, i.e. when hit one after
   // the other in arbitrary order.
   //
   PPG_QMK_KEYPOS_CLUSTER_ACTION_KEYCODE(
      0, // Layer 0, equivalent to the respective QMK layer
      KC_ENTER, // Keycode action
      // An unlimited numbero of matrix key-positions, follow.
      // Here we use the macros defined above.
      // . ...
      LEFT_INNER_THUMB_KEY, // left inner large thumb key
      RIGHT_INNER_THUMB_KEY  // right inner large thumb key
   );
   
   // ... more pattern definitions
  
   // Compile the Papageno search tree
   //
   PPG_QMK_COMPILE
}
```

### Connection with QMK

Our initialization function must be linked to the QMK system by overriding
the QMK-interface function `matrix_init_user`.

```C
void matrix_init_user(void) 
{ 
   init_papageno();
   
   // ... other initialization tasks
}
```

Two more interface function must be overridden to connect Papageno with QMK.
```C
void matrix_scan_user(void) 
{
   ppg_qmk_matrix_scan();
   
   // ... other matrix scan tasks
};
```
The overridden function `matrix_scan_user` ensures, amongst others, that pattern matching timeouts are correctly handled.

The actualy keypress handling is performed by an overriden version of `action_exec_user`.
```C
void action_exec_user(keyevent_t event)
{
   ppg_qmk_process_event(event);
   
   // ... other matrix scan tasks
}
```

If no other tasks are to be performed by the overridden QMK-interface functions, the overrides can be defined automatically by calling the macro `PPG_QMK_CONNECT` at global scope.

### A small working example

The following could be a `keymap.c` file that uses Papageno.
```C
#include "process_papageno.h"

// ... define your QMK-keycodes here

#define LEFT_INNER_THUMB_KEY(S) NG_KEYPOS(5, 2, S)
#define RIGHT_INNER_THUMB_KEY(S) NG_KEYPOS(5, B, S)

void init_papageno(void)
{
   // Initialize the Papageno-QMK system
   //
   PPG_QMK_INIT
   
   // Define an indivieual timeout. Keystrokes whose time interval
   // exceeds the timeout are considered individually and not
   // as part of a pattern.
   //
   ppg_qmk_set_timeout_ms(200);
   
   // Left inner and right inner thumb key trigger emission of the 
   // enter keycode (KC_ENTER) if clustered, i.e. when hit one after
   // the other in arbitrary order.
   //
   PPG_QMK_KEYPOS_CLUSTER_ACTION_KEYCODE(
      0, // Layer 0, equivalent to the respective QMK layer
      KC_ENTER, // Keycode action
      // An unlimited numbero of matrix key-positions, follow.
      // Here we use the macros defined above.
      // . ...
      LEFT_INNER_THUMB_KEY, // left inner large thumb key
      RIGHT_INNER_THUMB_KEY  // right inner large thumb key
   );
   
   // ... more pattern definitions
  
   // Compile the Papageno search tree
   //
   PPG_QMK_COMPILE
}

PPG_QMK_CONNECT
```

## Example usage and further reading

The introduction above just exemplifies one of the many features provided by Papageno. A complete documentation of Papageno's QMK-API goes beyond the scope of this document. 
The use of many of Papageno's features is exemplified with the help of noseglasses' [QMK-keymap](https://github.com/noseglasses/noseglasses_qmk_layout/). See also the file `process_keycode/process_papageno.h` in the root directory of QMK's source tree. It defines the complete Papageno-QMK API as C-functions and pre-processor macros, all of which are some sort of wrappers to the actual Papageno API [Papageno API](https://github.com/noseglasses/papageno/).