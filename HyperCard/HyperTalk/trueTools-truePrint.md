# trueTools: truePrint

This stack took screenshots of cards of HyperCard stacks
to the clipboard and also allowed to print them.

See also its [readme](../trueTools-truePrint.pdf).

## Stack Script

```
on controlKey key
  if key is 16 then -- p or any function key
    put the shiftKey is down into s
    put the optionKey is down into o
    truePrintCard not (s or o), o
    if the result is not empty then answer the result
  end if
  pass controlKey
end controlKey

on truePrintCard withDialogs, copyOnly
  --
  put withDialogs is true into withDialogs
  put copyOnly is true into copyOnly
  --
  if withDialogs then
    answer "Copy card picture to Clipboard and Print ?" ¬
      with "Cancel", "Copy Only", "Copy&Print"
      if it is "Cancel" then exit truePrintCard  --∆
    put it is "Copy Only" into copyOnly
  end if
  --
  set cursor to watch
  trueCopyRect rect of card window
  if the result is not empty then return the result  --∆
  --
  if not copyOnly then
    printPict "clipboard",, false, withDialogs
    if the result is not empty ¬
      and the result is not "Error : Printing aborted" then
      return "Could not print clipboard." ¬
        & return & "printPict" && the result  --∆
    end if
  end if
end truePrintCard

on trueCopyRect r
  put trueScreenShot() into f
  if word one of f is not "ok" then return f  --∆
  delete word one of f
  --
  pictToClip f,,,,,r
  if the result is not empty then
    return "Could not copy screen shot to clipboard." ¬
      & return & "pictToClip" && the result  --∆
  end if
  --
  eraseFile f
  if the result is not empty then
    return "Could not erase screen shot." ¬
      & return & "eraseFile" && the result  --∆
  end if
end trueCopyRect

function trueScreenshot
  put getBootVol() into v
  if v contains ":" then
    return "Could not get boot volume." & return & "getBootVol" && v  --∆
  end if
  put ":" after v
  --
  put getDir(v,"F","t=PICT") into a
  if a contains ":" then
    return "Could not get Directory." & return & "getDir" && a  --∆
  end if
  --
  put empty into IDs
  repeat with i=1 to the number of windows - the number of lines of the stacks
    if visible of window i then
      put id of window i & return after IDs
      hide window i
    end if
  end repeat
  --
  put menubarVisible() into wasVisible
  hide menubar
  set cursor to none
  doFKEY 3  -- take screen shot
  if wasVisible then show menubar
  --
  if IDs is not empty then
    repeat with i=the number of lines of IDs down to 1
      show window id line i of IDs
    end repeat
  end if
  --
  put getDir(v,"F","t=PICT") into b
  if b contains ":" then
    return "Could not get Directory." & return & "getDir" && b  --∆
  end if
  --
  put the number of lines of b into n
  if the number of lines of a + 1 ≠ n then
    return "Could not save screen shot."  --∆
  end if
  --
  repeat with i=1 to n
    if line i of b ≠ line i of a then exit repeat
  end repeat
  return "ok" && v & line i of b
end trueScreenshot
```
