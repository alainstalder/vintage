# trueTools: JPEGtoPICT

The stack **JPEGtoPICT** of the **trueTools** collection
allowed to convert JPEG images (and I think also GIFs)
to the PICT format that was used widely on Macs at the time,
using Apple's Quicktime via AppleScript.

See also its [readme](../READMEs/trueTools-JPEGtoPICT.pdf)
and the [overview of the trueTools](../stacks/trueTools.md).

## Stack Script

```
on JPEGtoPICT f, removeExt, creator, createIcons, showProgress
  --
  if not QuickTime() then
    return "Sorry, JPEGtoPICT requires QuickTime."  --∆
  end if
  --
  put GetDir(f,"F","t=JPEG") into a
  if a contains ":" then return "GetDir" && a  --∆
  put the number of lines of a into n
  if n is 0 then return "No JPEG files in the folder."  --∆
  --
  put f into m
  delete last char of m
  set itemDelimiter to ":"
  put last item of m into x
  put empty into last item of m
  set itemDelimiter to ","
  --
  put GetDir(m,"D") into list
  if word 1 of list is "Error" then return "GetDir" && list  --∆
  if x & "•" is not in list then put m & x & "•" & ":" into f2
  else
    repeat with i=2 to the number of lines of list + 1
      if x & "•" & i is not in list then exit repeat
    end repeat
  put m & x & "•" & i & ":" into f2
  end if
  --
  createFolder(f2)
  if the result is not empty then
    return "CreateFolder" && the result  --∆
  end if
  --
  put showProgress is true into showProgress
  if showProgress then
    lock screen
    put rect of btn "frame" into r
    set rect of btn "progress" to item 1 of r,item 2 of r, ¬
      item 1 of r, item 4 of r
    put item 3 of r - item 1 of r into w
    put empty into cd fld "progress"
    show btn "progress"
    show btn "frame"
    show btn "Stop"
    show cd fld "progress"
    unlock screen
  end if
  --
  repeat with i=1 to n
    set cursor to busy
    if showProgress then
      if the mouse is down and the mouseLoc is within ¬
        the rect of btn "Stop" then return f2 --∆
    end if
    put line i of a into p
    --
    PictToClip f & p
    if the result is not empty then
      return "PICTToClip" && the result  --∆
    end if
    --
    if removeExt≠false then
      set itemDelimiter to "."
      if last item of p is "jpg" or last item of p is "jpeg"
      then delete last item of p
      set itemDelimiter to ","
    end if
    if showProgress then put p into cd fld "progress"
    --
    ClipToPict f2 & p
    if the result is not empty then
      return "ClipToPICT" && the result  --∆
    end if
    --
    if creator is not empty then
      SetFInfo f2 & p,"t=PICT","c=" & creator
      if the result is not empty then
        return "SetFInfo" && the result  --∆
      end if
    end if
    --
    if createIcons is true then
      CreateCustomIcon f2 & p
      if the result is not empty then
        return "CreateCustomIcon" && the result
      end if
    end if
    --
    if showProgress then
      put rect of btn "progress" into r
      put item 1 of r + trunc(w*i/n) into item 3 of r
      set rect of btn "progress" to r
    end if
  end repeat
  return f2
end JPEGtoPICT
```

## Card Button "Convert..." Script

```
on mouseUp
  if the version < 2.3 then
    answer file "Please select a Folder by selecting a File within:"
    if it is empty then exit mouseUp  --∆
    set itemDelimiter to ":"
    delete last item of it
    set itemDelimiter to ","
    put it & ":" into f
  else
    answer folder "Please select a Folder:"
    if it is empty then exit mouseUp  --∆
    put it into f
    wait until the mouse is up
  end if
  --
  if hilite of btn "change creator" then put cd fld "creator" into c
  else put empty into c
  put hilite of btn "remove extension" into r
  put hilite of btn "create icons" into i
  --
  JPEGtoPICT f, r, c, i, true
  if "•" is not in the result then answer the result
  send restore to this card
end mouseUp
```