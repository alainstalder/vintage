# Vintage Software by me

Public Open Source / Freeware software I,
Alain Stalder (*1966 in Switzerland),
wrote and published in the past
and which is no longer maintained,
nor usually the systems it ran on.
Vintage…

## trueColor/trueTools for HyperCard <span style="color:lightgray">(1996-97)</span>

HyperCard was a great tool, it originally came for free with every Mac.
But it was only black and white and there never was a real color version.

**trueColor** made an external command (XCMD) called "ColorizeHC"
much more intuitive to use,
with keyboard and mouse.

I am particularly fond of the associated help documentation
in the included **trueHelp** stack,
where you could also try the features live,
here its title page:

[![image](trueHelp.jpg)](https://TODO)

The trueColor stack itself was visually quite small,
just showed credits for my part and used XCMDs:

[![image](trueColor.jpg)](https://TODO)

After writing and publishing trueColor,
I also wrote a collection of further HyperCard tools
called simply **trueHelp**.

This was all Freeware
and by and large Open Source
since scripts, images, icons, etc. in HyperCard
were readable by all.
The only thing that had not been released as source
was a speedup I later implemented by using a compiled XCMD
to implement the core functionality faster than by script,
and unfortunately its source code (C, CodeWarrior compiler) is lost.

You can still run trueColor and trueTools today (at least in 2024)
by running HyperCard in a Mac emulator like Mini vMac.

Some things you can find here under the [HyperCard directory](https://TODO):

* trueColor 1.03 original distribution: [trueColor-103.hqx](https://TODO)
* trueHelp 1.03 original distribution: [trueTools-103.hqx](https://TODO)
* Pages with intro of:
  * The [**trueColor**](https://TODO) stack
  * The [**trueHelp**](https://TODO) stack (don't miss!)
  * All [**trueTools**](https://TODO) stacks
* [**READMEs**](https://TODO) of all components, rendered to PDFs
* [**Hypertalk scripts**](https://TODO) of:
  * The [**trueColor**](https://TODO) stack (core color functionality)
  * The [**vue**](https://TODO) stack of trueTools (an image viewer, 
    one of the most beautiful small pieces of software I ever wrote)
  * [**More**](https://TOOD) stacks, cards, buttons of trueColor/trueTools

Some general info around HyperCard and especially color:

* [How HyperCard got its Color Back](https://orangejuiceliberationfront.com/how-hypercard-got-its-color-back/) <span style="color:lightgray">(uliwitness 2016)</span>

By the way, I wrote these things during the 1.5 years
I spent as a postdoc in physics at McGill University in Montréal, Canada,
a place I personally never felt even remotely at home.

## Delphi/astrolib for PalmOS <span style="color:lightgray">(2000-2002)</span>

Shortly after springbreak 2000,
I started to write a small astrology program
(what you would today call an app)
called **Delphi** for PalmOS handheld devices,
and made the first release in October 2000.

[![image](delphi.jpg)](https://TODO)

Delphi was Freeware but not Open Source
and unfortunately its source code is lost.

But what has been preserved is the source code
of the Open Source library called **astrolib**
which I wrote and was used by Delphi.
It calculates astronomical positions of
sun, moon, planets up to Pluto
(Pluto was considered a planet back then),
plus the true lunar nodes,
and it calculates time zones based on
UNIX tz data for all 350 tz locations
plus about 2000 additional locations.

I did not use any code from the UNIX tz data,
but wrote that myself to fit into the small frame of PalmOS,
also compressing the tz data into a condensed binary format.
Also regarding astronomical calculations,
I had to put function parameters into a static array
because the stack was too small on these devices.
Also, optimizing performance in Delphi when drawing charts
was quite a challenge these times;
I wrote my own routines for circles and dotted lines, etc.,
and I created my own font for the various symbols.

Overall, of course, to fit a birth chart onto such a small screen,
which was only 160 x 160 pixels back then, was quite a challenge,
and I am proud of the result.

Some things you can find here under the [PalmOS directory](https://TODO):

* [**ZIP file of the Website**](https://TODO) for Delphi 2.01 + astrolib 1.03 in 2002\
  (contains also the respective distributions of Delphi/astrolib)
* From that website:
  * [**Title page**](PalmOS/extracts/web/index.md) with blown up images (without links)
  * [**Delphi User's Manual page**](PalmOS/extracts/web/delphi-users-manual.md) with blown up images (without links)
  * [**astrolib page**](https://TODO) rendered to Markdown
* [**Source code of astrolib**](https://TODO)
  (C with macros to make it read similar to Modula-2,
  which made a difference before syntax coloring in editors)

By the way, Delphi was quite successful,
there were thousands of downloads and quite a few support requests.
While maybe my main reason for writing it had ironically been
to put hashes and signatures of some documents I had written
(around themes that are now at exactphilosophy.net)
into the About Screens of different versions of Delphi.