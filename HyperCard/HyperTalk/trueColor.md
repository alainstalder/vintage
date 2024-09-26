# trueColor

Selected HyperTalk scripts from the trueColor stack.

See also its [readme](../trueColor.pdf)
and the [cards in the stack](../stacks/trueColor.md).

## Stack Script

This script implements the color functionality of trueColor.
There is an implementation in pure HyperTalk (using 3rd party XCMDs),
while if the system supported it, an XCMD written by me was used instead
(source of that is lost).

```
on openCard
  global trueColor
  if there is a fld "trueColor" then
    if line 2 of fld "trueColor" then
      if trueColor≠true then lock screen
      trueOpen false
      unlock screen
    end if
  else if trueColor is true then
    colorizeHC "dispose"
    put false into trueColor
    unlock screen
  end if
  pass openCard
end openCard

on closeCard
  global trueColor
  if there is a fld "trueColor" then
    if line 2 of fld "trueColor" then
      lock screen
      put true into trueColor
      trueClose
    end if
  end if
  pass closeCard
end closeCard

on openBackground
  global trueColor
  if there is a fld "trueColor" then
    if not line 2 of fld "trueColor" then
      if trueColor≠true then lock screen
      trueOpen false
      unlock screen
    end if
  else if trueColor is true then
    colorizeHC "dispose"
    put false into trueColor
    unlock screen
  end if
  pass openBackground
end openBackground

on closeBackground
  global trueColor
  if there is a fld "trueColor" then
    if not line 2 of fld "trueColor" then
      lock screen
      put true into trueColor
      trueClose
    end if
  end if
  pass closeBackground
end closeBackground

on resumeStack
  global trueColor
  if there is a fld "trueColor" then
    lock screen
    trueOpen false
    unlock screen
  end if
  pass resumeStack
end resumeStack

on suspendStack
  global trueColor
  if there is a fld "trueColor" then
    lock screen
    trueClose
    colorizeHC "dispose"
    put false into trueColor
    unlock screen
  end if
  pass suspendStack
end suspendStack

on closeStack
  global trueColor
  if there is a fld "trueColor" then
    trueClose
    colorizeHC "dispose"
    put false into trueColor
    unlock screen
  end if
  pass closeStack
end closeStack

on controlKey key
  if key is 20 then -- t
    if there is a fld "trueColor" then trueRemove
    else trueInstall
  else if key is 16 then -- p
    if ":truePrint" & return is not in the stacksInUse then
      start using stack "truePrint"
      if the result is empty then
        send "controlKey 16" to stack "truePrint"
        exit controlKey
      end if
    end if
  else if there is a fld "trueColor" then
    if key is 9       then trueToggle -- tab or i
    else if key is 2  then trueMode   -- b
    else if key is 1  then trueMove   -- a
    else if key is 6  then truePick   -- f
    else if key is 3  then trueCopy false -- c, Enter
    else if key is 24 then trueCopy true  -- x
    else if key is 22 then truePaste optionKey() is down  -- v
    else if key is 26 then trueUndo   -- z
    else if key is 5  then go to stack "trueHelp" in a new window -- e, Help
    else if key is 15 then truePastePrefs -- o
    else if key is 19 then  -- s
      if the optionKey is down then edit script of stack "trueColor"
      else trueHilite truePointed()
      end if
  end if
pass controlKey
end controlKey

--

on trueOpen flush
  put fld "trueColor" into t
  if line 1 of t and not line 3 of t then truePaint flush
end trueOpen

on trueClose
  put fld "trueColor" into t
  if line 1 of t and line 3 of t then trueErase
end trueClose

--

on truePaint flush
  global trueColor
  put true into trueColor
  --
  if the version ≥ 3.0 then
    truePaintScript
    exit truePaint  --∆
  end if
  if flush≠false then trueFlush
  put trueXFCN() into cmd
  if cmd is a number and cmd≠empty then
    if flush is false and cmd≠12 then -- try again with flushing
    truePaint
    exit truePaint  --∆
  end if
  beep
  if the version ≥ 2.2 then
    truePaintScript
  end if
  else do cmd
end truePaint

on trueFlush
  lock screen
  lock messages
  go to stack "trueColor"
  go back
  unlock messages
  unlock screen
end trueFlush

on truePaintScript
  put true into line 3 of fld "trueColor"
  put line 1 to 3 of fld "trueColor" into fld "trueColor"
  --
  repeat with i=1 to the number of bg parts
    put "bg part" && i into p
    if visible of p then trueFill p
  end repeat
  --
  if line 2 of fld "trueColor" then
    repeat with i=1 to the number of cd parts
      put "cd part" && i into p
      if visible of p then trueFill p
    end repeat
  end if
end truePaintScript

on trueErase
  put false into line 3 of fld "trueColor"
  put fld "trueColor" into t
  repeat with i=4 to the number of lines of t
    put line i of t into x
    do "get there is a" && item 1 of x
    if it then set icon of item 1 of x to item 2 of x
  end repeat
  colorizeHC "Erase"
end trueErase

on trueFill p, opt
  put line 1 to 2 of script of p into sc
  put (word 2 of screenList(cardScreen)>2) into f
  --
  if word 2 of line 1 of sc is "trueColor" then
    if f then colorizeHC "colorFill", rect of p, word 3 of line 1 of sc
    else colorizeHC "colorFill", rect of p, "65535,65535,65535"
  else if word 2 of line 2 of sc is "trueColor" then
    if f then colorizeHC "colorFill", rect of p, word 3 of line 2 of sc
    else colorizeHC "colorFill", rect of p, "65535,65535,65535"
  else if style of p ≠ transparent then
    colorizeHC "colorFill", rect of p, "65535,65535,65535"
  end if
  --
  if word 2 of line 1 of sc is "truePicture" then
    put line 1 of sc into x
  else if word 2 of line 2 of sc is "truePicture" then
    put line 2 of sc into x
  end if
  if word 2 of x is "truePicture" then
    put value of word 3 of x into s
    if word 4 of x is "dither" then
      colorizeHC "Add", s, rect of p, rect of p, "transparent with dithering"
    else
      colorizeHC "Add", s, topLeft of p, rect of p, "transparent"
    end if
  end if
  --
  if word 2 of name of p is "button" then
    if icon of p ≠ 0 then
      put width of p into w
      if w ≤ 32 then put left of p - (33 - w) div 2 into x
      else put left of p - (32 - w) div 2 into x
      put height of p into h
      if h ≤ 32 then put top of p - (33 - h) div 2 into y
      else put top of p - (32 - h) div 2 into y
      if showName of p then add -6 to y
      put x & "," & y into pos
      if opt is true then put "icon id" && trueIcon(p) into s
      else put "icon id" && icon of p into s
      --
      colorizeHC "Add", s, pos, rect of p, "transparent"
      if the result is empty and opt ≠ true then
        put return & word 1 to 2 of (name of p) && "id" && id of p ¬
          & "," & icon of p after fld "trueColor"
        set icon of p to "trueWhite"
      end if
    end if
  end if
end trueFill

--

on trueShow flush
  lock screen
  trueClose
  show target
  trueOpen flush
  unlock screen
end trueShow

on trueHide flush
  lock screen
  trueClose
  hide target
  trueOpen flush
  unlock screen
end trueHide

on trueShowQuick
  if there is a fld "trueColor" then
    lock screen
    show target
    put fld "trueColor" into t
    if line 1 of t and line 3 of t ¬
      and (line 2 of t or word 1 of name of target is "bkgnd") then
      put word 1 to 2 of name of target && "id" && id of target into p
      trueFill p,true
    end if
    unlock screen
  end if
end trueShowQuick

on trueHideQuick
  if there is a fld "trueColor" then
    lock screen
    hide target
    put fld "trueColor" into t
    if line 1 of t and line 3 of t ¬
      and (line 2 of t or word 1 of name of target is "bkgnd") then
      colorizeHC "colorFill", rect of target, "65535,65535,65535" -- white
    end if
    unlock screen
  end if
end trueHideQuick

--

on trueInstall
  if short name of this stack is "trueColor" then
    answer "Sorry, this stack cannot be colored."
    exit trueInstall  --∆
  end if
  --
  if (there is a bg btn "trueColor") and (the version ≥ 2.2) then
    do "put line 2 of bg btn" && quote&"trueColor"&quote && "into b"
    do "delete bg btn" && quote&"trueColor"&quote
  else put true into b
  --
  lock screen
  put the editBkgnd into temp
  set editBkgnd to true
  put the userlevel into u
  set userlevel to 5
  put the tool into t
  choose browse tool
  --
  doMenu "New Field"
  set userlevel to u
  set editBkgnd to temp
  choose t
  set name of last fld to "trueColor"
  set sharedText of fld "trueColor" to true
  set visible of fld "trueColor" to false
  set loc of fld "trueColor" to -100,-100
  put true & return & b & return & false into fld "trueColor"
  trueOpen
  unlock screen
end trueInstall

on trueRemove
  lock screen
  trueClose
  delete fld "trueColor"
  unlock screen
end trueRemove

--

on trueMode
  lock screen
  trueClose
  put not line 2 of fld "trueColor" into line 2 of fld "trueColor"
  trueOpen
  unlock screen
end trueMode

on trueToggle
  if there is a fld "trueColor" then
    put (the optionKey is down) into opt
    if opt and not line 1 of fld "trueColor" then put ticks() into t0
    else put 0 into t0
    put not line 1 of fld "trueColor" ¬
      into line 1 of fld "trueColor"
    --
    lock screen
    if line 1 of fld "trueColor" then
      truePaint true
    else
      trueErase
      if opt then colorizeHC "dispose"
    end if
    unlock screen
    --
    if t0≠0 then
      put trunc((ticks()-t0)/0.6)/100 into s
      if line 1 of fld "trueColor" then put empty into it
      else put "un" into it
      if s=1 then put empty into x else put "s" into x
      answer it & "painting took" && s && "second" & x & "."
    end if
  end if
end trueToggle

--

on trueSetProperty p,t,prop
  put script of p into s
  put 1 into i
  repeat 2 times
    if word 2 of line i of s is t then
      put true into found
      if prop is empty then
        delete line i of s
        subtract 1 from i
      else put "-"&"-" && t && prop into line i of s
    end if
    add 1 to i
  end repeat
  if found ≠ true then put "-"&"-" && t && prop & return before s
  set script of p to s
end trueSetProperty

function trueProperty p,t
  put script of p into s
  put empty into prop
  put 1 into i
  repeat 2 times
    if word 2 of line i of s is t then
      put word 3 to (number of words of line i of s) of line i of s ¬
        into prop
    end if
  end repeat
  return prop
end trueProperty

--

on trueMove
  set cursor to arrow
  wait until the mouse is down
  put truePointed() into p
  if p is empty then exit trueMove  --∆
  --
  put the mouseH into mh
  put the mouseV into mv
  put left of p - mh into dh
  put top of p - mv into dv
  put true into freeH
  put true into freeV
  --
  repeat while the mouse is down
    put the mouseH into mh1
    put the mouseV into mv1
    if mh1≠mh or mv1≠mv or the optionKey is down then
      if freeH and freeV and the shiftKey is down then
        if abs(mh1-mh)>abs(mv1-mv) then put false into freeV
        else put false into freeH
      end if
      put mh1 into mh
      put mv1 into mv
      lock screen
      send trueHideQuick to p
      if freeH then set left of p to dh + mh
      if freeV then set top of p to dv + mv
      if the optionKey is down then send trueShow to p
      else send trueShowQuick to p
      unlock screen
      set cursor to arrow
    end if
  end repeat
  send trueShow to p
end trueMove

on truePick
  put truePointed() into p
  if p is not empty then
    trueHilite p
    --
    if word 2 of screenList(cardScreen) ≤ 2 then
      answer "Color fillings will only show if you set your screen " ¬
        & "to at least 16 colors or grays." with "Cancel","Pick Color"
      if it is "Cancel" then exit truePick --∆
    end if
    --
    put trueProperty(p,"trueColor") into color
    if color is empty then put "65535,65535,65535" into color -- white
    get chooseColor(name of p,color)
    if it≠empty then
      if it is "65535,65535,65535" then put empty into it
      trueSetProperty p,"trueColor",it
      lock screen
      trueClose
      trueOpen
      unlock screen
    end if
  end if
end truePick

on truePastePrefs
  palette "truePasteColor", "21,52"
  palette "truePasteQuality", "113,52"
  palette "truePasteSize", "208,52"
end truePastePrefs

--

function truePointed
  put the mouseLoc into loc
  if the version < 2.2 then
    put trueXFCN("partlist") into a
    repeat with i=number of lines of a down to 1
      put line i of a into p
      if loc is within the rect of p and visible of p then return p
    end repeat
  else
    repeat with i=number of cd parts down to 1
      put "cd part" && i into p
      if loc is within the rect of p and visible of p then return p
    end repeat
    repeat with i=number of bg parts down to 1
      put "bg part" && i into p
      if loc is within the rect of (p) and visible of p then return p
    end repeat
  end if
  return empty
end truePointed

on trueHilite p
  put style of p into s
  if s is transparent or s is oval or s is opaque then
    lock screen
    set style of p to rectangle
    unlock screen
  end if
  hide p
  wait 20 ticks
  show p
  set style of p to s
end trueHilite

--

on trueStopUsing
  global wasInUse
  put value of word 2 of long name of this stack into s
  if s is in the stacksInUse then
    stop using this stack
    put true into wasInUse
  else put false into wasInUse
  stop using stack "trueColor"
end trueStopUsing

on trueStartUsing
  global wasInUse
  if wasInUse then start using this stack
  start using stack "trueColor"
end trueStartUsing

--

function trueIcon p
  if word 2 of name of p is "button" then
    if icon of p ≠ 0 then
      put fld "trueColor" into t
      if line 1 of t and line 3 of t then
        put word 1 to 2 of (name of p) && "id" && id of p into x
        repeat with i=4 to the number of lines of t
          if item 1 of line i of t is x then
            return item 2 of line i of t  --∆
          end if
        end repeat
      end if
      return icon of p  --∆
    end if
  end if
return empty
end trueIcon

function truePictName p
  get trueIcon(p)
  if it≠empty then return "icon id" && it  --∆
  --
  put trueProperty(p,"truePicture") into pictName
  if pictName ≠ empty then
    if char 1 of pictName is quote then -- dither
      put value of word 1 of pictName into pictName
    end if
    return pictName  --∆
  else
    if word 1 of name of p is "card" then put "cd " into s
    else put "bg " into s
    if word 2 of name of p is "button" then put "btn " after s
    else put "fld " after s
    put id of p after s
    if word 1 of s is "cd" then return s && short id of this card  --∆
    else return s && id of this bg  --∆
  end if
  return empty
end truePictName

--

function trueResID s, resType, resName
  put resources(s) into r
  if the result is not empty then answer "resources():" && the result
  put empty into id
  repeat with i=1 to the number of lines of r
    put line i of r into a
    if word 1 of a is resType ¬
      and word 4 to (number of words of a) of a is resName then
      put word 2 of a into id
      exit repeat  --∆
    end if
  end repeat
  return id
end trueResID

on trueSafePicture pictName
  put value of word 2 of long name of this stack into s
  put value of word 2 of long name of stack "trueColor" into t
  --
  put trueResID(s,"PICT",pictName) into id
  if id ≠ empty then
    put trueResID(t,"PICT","backup") into id1
    if id1 ≠ empty then
      resRemove t,"PICT",id1
      if the result is not empty then answer "resRemove:" && the result
    end if
    copyRes "PICT",id,s,t
    put the result into id
    get renameRes(t,id,"PICT",,"backup")
    if it is not empty then answer "renameRes:" && it
    resRemove s,"PICT",id
    if the result is not empty then answer "resRemove:" && the result
  end if
end trueSafePicture

--

on trueCopy cut
  put truePointed() into p
  if p is empty then exit trueCopy  --∆
  put truePictName(p) into pictName
  if pictName is empty then exit trueCopy  --∆
  --
  trueStopUsing
  put value of word 2 of long name of this stack into s
  put trueResID(s,"PICT",pictName) into id
  if id ≠ empty then
    trueHilite p
    pictToClip s,"res",pictName
    if the result is not empty then answer "PICTToClip:" && the result
  end if
  if cut then trueSafePicture pictName
  trueStartUsing
  if cut then
    if word 1 of pictName ≠ "icon" then
      trueSetProperty p,"truePicture",empty
    end if
    lock screen
    trueClose
    trueOpen
    unlock screen
  end if
end trueCopy

on trueUndo
  trueStopUsing
  put value of word 2 of long name of stack "trueColor" into t
  put trueResID(t,"PICT","backup") into id
  if id is empty then beep
  else
    pictToClip t,"res","backup"
    if the result is not empty then answer "PICTToClip:" && the result
  end if
  trueStartUsing
end trueUndo

on truePaste dither
  global truePasteColor,truePasteQuality,truePasteSize
  --
  if "PICT" is not in clipInfo() then exit truePaste  --∆
  put truePointed() into p
  if p is empty then exit truePaste  --∆
  put truePictName(p) into pictName
  if pictName is empty then exit truePaste  --∆
  --
  trueHilite p
  trueStopUsing
  trueSafePicture pictName
  --
  put "clipToPict" && word 2 of long name of this stack into cmd
  put ", " && quote & "res" & quote after cmd
  put ", " && quote & pictName & quote after cmd
  if truePasteSize is "shrink" then put "TRUE" into x
  else put "FALSE" into x
  put "," && quote & x & quote after cmd
  if truePasteSize is "shrink" or truePasteSize is "squeeze" then
    if trueXFCN("32-bit QuickDraw") then
      put rect of p into r
      put "," && quote & (1 + item 3 of r - item 1 of r) & ","¬
        & (1 + item 4 of r - item 2 of r) & quote after cmd
    else beep
  end if
  if truePasteSize is "clip" then
    put rect of p into r
    put "," && quote & "0,0," & (item 3 of r - item 1 of r) ¬
      & "," & (item 4 of r - item 2 of r) & quote after cmd
  end if
  if truePasteColor ≠ empty then
    put "," && quote & "d=" & truePasteColor & quote after cmd
  end if
  if truePasteQuality ≠ empty then
    if trueXFCN("QuickTime") then
      put "," && quote & "c=" & truePasteQuality & quote after cmd
    else beep
  end if
  --
  lock screen
  lock messages
  push card
  go to stack "trueColor"
  do cmd
  if the result is not empty then answer "ClipToPICT:" && the result
  go back
  pop card
  trueStartUsing
  put quote & pictName & quote into prop
  if dither then put space & "dither" after prop
  if word 1 of pictName ≠ "icon" then
    trueSetProperty p, "truePicture", prop
  end if
  trueClose
  trueOpen
  unlock messages
  unlock screen
end truePaste
```

## Background Script

The following script
that was attached to the background of the trueColor stack
started a slide show with credits on the individual cards of the stack
by virtually clicking the button "slides" on the first card of the stack.

```
on openStack
  go first card
  wait 90 ticks
  send mouseDown to button "slides"
  lock screen
  set style of button "help" of first card to transparent
  unlock screen with dissolve very slow
  pass openStack
end openStack

on closeStack
  set style of button "help" of first card to opaque
  pass closeStack
end closeStack
```

## Card Button "slides" Script

This button, when pressed manually or automatically
when the stack was opened via background script,
actually performed the slide show
by going from card to card with the then very beautiful
"dissolve" screen effect.

```
on mouseDown
  repeat for the number of cards of this bg
    lock screen
    go next card of this bg
    unlock screen with dissolve very slow
    put ticks() into t0
    repeat until ticks() - t0 > 70
      if the mouse is down then
        go first card of this bg
        exit mouseDown
      end if
    end repeat
  end repeat
end mouseDown
```

## Card Button "help" Script

This button with a "?" icon simply takes to the trueHelp stack.

```
on mouseUp
  go to stack "trueHelp" in a new window
end mouseUp
```
