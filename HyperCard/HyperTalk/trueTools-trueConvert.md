# trueTools: trueConvert

This stack converts stacks colorized with trueColor
to ones colorized with Apple's Color Tools (AddColor XCMD),
at the time still hoping for a HyperCard 3.0 sometime
with native color support
which would have likely only supported color 
added  via Apple's Color Tools.

See also its [readme](../trueTools-trueConvert.pdf).

## Stack Script

```
on trueConvert s
  global truePasteColor, iconList
  --
  if truePasteColor is empty then
    put word 2 of screenList(cardScreen) into depth
  else put truePasteColor into depth
  --
  put value of word 2 of long name of stack s into s
  if there is no stack s & "•" then put s & "•" into s1
  else
    put 2 into i
    repeat while there is a stack s & "•" & i
      add 1 to i
    end repeat
    put s & "•" & i into s1
  end if
  --
  fileCopy s, s1
  if the result is not s1 then
    return "FileCopy" && the result --∆
  end if
  put s1 into s
  --
  put value of word 2 of long name of this stack into t
  --
  copyRes "XCMD",9207,t,s
  copyRes "BITs",1000,t,s
  copyRes "STR#",8000,t,s
  copyRes "STR#",8001,t,s
  copyRes "STR#",8002,t,s
  copyRes "STR#",8003,t,s
  copyRes "STR#",8004,t,s
  copyRes "STR#",8005,t,s
  copyRes "STR#",8006,t,s
  --
  put resources(s) into r
  if the result is not empty then
    return "resources()" && the result  --∆
  end if
  --
  put empty into iconList
  repeat with i=1 to the number of lines of r
    put line i of r into a
    if word 1 of a is "PICT" and word 4 to 5 of a is "icon id" then
      put word 4 to 6 of a & return after iconList
    end if
  end repeat
  --
  put script of stack s into sc
  --
  put trueInsertCmd(sc,"openCard","Send colorMe to this card") into sc
  put trueInsertCmd(sc,"closeCard","lock screen") into sc
  put trueInsertCmd(sc,"colorMe","AddColor colorCard,stamp,0",false) into sc
  put trueInsertCmd(sc,"openStack","AddColor install," & depth) into sc
  put trueInsertCmd(sc,"closeStack","AddColor remove") into sc
  --
  put trueInsertCmd(sc,"trueShow","show the target", false) into sc
  put trueInsertCmd(sc,"trueHide","hide the target", false) into sc
  put trueInsertCmd(sc,"trueShowQuick","show the target", false) into sc
  put trueInsertCmd(sc,"trueHideQuick","hide the target", false) into sc
  --
  set script of stack s to sc
  --
  put long name of this card into c
  lock messages
  closeCard
  closeBackground
  closeStack
  go to stack s
  AddColor install,depth
  --
  put empty into bgList
  repeat with i=1 to the number of cards of this stack
  set cursor to busy
  if the mouse is down then exit repeat  --∆
  lock screen
  go card i of this stack
  --
  if there is a fld "trueColor" then
    delete fld "trueColor"
    put id of this bg & return after bgList
    repeat with j=1 to the number of bg parts
      set cursor to busy
      put "bg part" && j into p
      if visible of p then
        trueConvertPart p
        if the result is not empty then return the result  --∆
      end if
    end repeat
  end if
  --
  if id of this bg is in bgList then
    repeat with j=1 to the number of cd parts
      set cursor to busy
      put "cd part" && j into p
      if visible of p then
        trueConvertPart p
        if the result is not empty then return the result  --∆
      end if
    end repeat
  end if
  --
  AddColor colorCard,stamp,30
  unlock screen
  end repeat
  --
  wait 30 ticks
  AddColor remove
  go c
  openStack
  openBackground
  openCard
  unlock messages
end trueConvert


on trueConvertPart p
  global iconList
  --
  if word 1 of name of p is "card" then put "cd" into cdbg
  else put "bg" into cdbg
  --
  put trueProperty(p,"trueColor") into c
  if c ≠ empty then trueSetProperty p,"trueColor",empty
  else put "65535,65535,65535" into c
  --
  if c ≠ "65535,65535,65535" or style of p ≠ transparent then
    if style of p is transparent then
      addColor addRect, cdbg, rect of p, c, 0, -1
    else if word 2 of name of p is "button" then
      addColor addButton, cdbg, id of p, c, 0, -1
    else addColor addField, cdbg, id of p, c, 0, -1
    if the result is not empty then return the result  --∆
  end if
  --
  put trueProperty(p,"truePicture") into x
  if x ≠ empty then trueSetProperty p,"truePicture",empty
  --
  put value of word 1 of x into s
  if word 2 of x is "dither" then
    addColor addPict, cdbg, s, rect of p, "t", -1
  else addColor addPict, cdbg, s, topLeft of p, "t", -1
  if the result is not empty then return the result  --∆
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
      put "icon id" && icon of p into s
      if s is in iconList then
        set icon of p to "trueWhite"
        addColor addPict, cdbg, s, pos, "t", -1
        if the result is not empty then return the result  --∆
      end if
    end if
  end if
end trueConvertPart

function trueInsertCmd sc, h, cmd, p
  put offset("on" && h, sc) into x
  if x is 0 then
    if last char of sc is not return then put return after sc
    put return after sc
    put "on" && h & return after sc
    put "  " & cmd & return after sc
    if p≠false then put "  pass" && h & return after sc
    put "end" && h & return after sc
  else
    put x + 3 + length of h into x
    put return & "  " & cmd & return into char x of sc
  end if
  return sc
end trueInsertCmd
```

## Card Button "Convert Stack..."

```
on mouseUp
  if the version < 2.2 then
    put "Sorry, this conversion requires HyperCard (Player)"¬
      && "2.2 or later." into s
    answer s
    exit mouseUp  --∆
  end if
  --
  answer file "Please select a Stack:" of type "Stack"
  if it is empty then exit mouseUp  --∆
  put it into s
  put "Please make sure you read the Important Note before" ¬
    & " distributing any converted stack." into a
  answer a with "Cancel", "Convert"
  if it is "Cancel" then exit mouseUp  --∆
  --
  put long name of this card into c
  trueConvert s
  if the result is not empty then
    if the result is "Error: AddColor incorrectly installed." then
    answer the result & return & "(Not enough Memory ?)"
    else answer the result
    go c
  end if
end mouseUp
```