# trueTools: spider

The stack **spider** of the **trueTools** collection
turned HyperCard stack cards into webpages
with one image screenshot stack card.

See also its [readme](../READMEs/trueTools-spider.pdf)
and the [overview of the trueTools](../stacks/trueTools.md).

## Stack Script

```
on controlKey key
  if key is 23 and short name of this stack ≠ spider then -- w
    put the optionKey is down into o
    if o then
      answer "Weave all cards of this Stack ?" with "Cancel","Weave"
      if it is "Cancel" then exit controlKey  --∆
    end if
    weaveStack not o
    if the result is not empty then
      answer the result
    end if
  else pass controlKey
end controlKey

on weaveStack cardOnly
  set cursor to busy
  if the version < 2.3 then
    return "Sorry, weaving requires HyperCard (Player) 2.3 or later."  --∆
  end if
  --
  if ":truePrint" & return is not in the stacksInUse then
    start using stack "truePrint"
    if the result is not empty then
      return "Sorry, weaving requires the truePrint stack."  --∆
    end if
  end if
  --
  lock screen
  lock messages
  go stack "spider"
  if hilite of btn "Mac" then put return into break
  else if hilite of btn "Unix" then put numToChar(10) into break
  else put return & numToChar(10) into break
  go back
  unlock messages
  unlock screen
  --
  put (value of word 2 of long name of this stack) && "ƒ:" ¬
    into destinationFolder
  if there is no folder destinationFolder then
    createFolder(destinationFolder)
    if the result is not empty then
      return "Could not create output folder." & return ¬
        & "CreateFolder" && the result  --∆
    end if
  end if
  --
  if cardOnly then
    weaveCard destinationFolder, break
    if the result is not empty then
      return the result  --∆
    end if
  else
    go to first card
    repeat for the number of cards
      if the mouse is down then exit repeat  --∆
      weaveCard destinationFolder, break
      if the result is not empty then return the result  --∆
      go next card
    end repeat
  end if
end weaveStack


on weaveCard destinationFolder, break
  --
  put weaveCardName() into cName
  put script of this card into sc
  put word 2 of sc into ext
  if ext≠"jpeg" and ext≠"gif" then put "gif" into ext
  put word 3 of sc into quality
  if ext is "jpeg" then
    if quality≠"very low" and quality≠"low" and quality≠"normal" ¬
      and quality ≠"high" and quality≠"very high" then
      put "normal" into quality
    end if
  else put quality ≠ "noninterlaced" into interlace
  --
  lock screen
  lock messages
  put empty into list
  repeat with i=the number of cd parts down to 1
    if visible of cd part i then
      set cursor to busy
      put empty into x
      put long name of this card into a
      put long name of this stack into s
      send mouseUp to cd part i
      if long name of this card ≠ a then
        if long name of this stack is s then
          put weaveCardName() & ".html" into x
        end if
        go back
      else if the result is not empty then
        put the result into x
      end if
      if x is not empty then
        put x && rect of cd part i & return after list
      end if
    end if
  end repeat
  --
  repeat with i=the number of bg parts down to 1
    if visible of bg part i then
      set cursor to busy
      put empty into x
      put long name of this card into a
      put long name of this stack into s
      send mouseUp to bg part i
      if long name of this card ≠ a then
        if long name of this stack is s then
          put weaveCardName() & ".html" into x
        end if
        go back
      else if the result is not empty then
        put the result into x
      end if
      if x is not empty then
        put x && rect of bg part i & return after list
      end if
    end if
  end repeat
  unlock messages
  unlock screen
  --
  set cursor to busy
  put destinationFolder & cName & ".html" into f
  open file f
  --
  write "<HTML>" & break & break & "<HEAD>" & break ¬
    & "<TITLE>" & short name of this card & "</TITLE>" & break ¬
    & "</HEAD>" & break & break to file f
  --
  write "<BODY BGCOLOR=" & quote & "#FFFFFF" & quote & ">" ¬
    & break & break to file f
  --
  write "<A HREF=" & quote & cName & ".map" & quote & ">" ¬
    & "<IMG SRC=" & quote & cName & "." & ext & quote ¬
    && "HEIGHT=" & height of card window ¬
    && "WIDTH=" & width of card window & break ¬
    & "USEMAP=" & quote & "#map" & quote ¬
    && "ISMAP BORDER=0></A>" & break & break to file f
  --
  write "<MAP NAME=" & quote & "map" & quote & ">" & break to file f
  repeat with i=1 to the number of lines of list
    write "<AREA SHAPE=" & quote & "rect" & quote && ¬
      "COORDS=" & quote & word 2 of line i of list & quote && ¬
      "HREF=" & quote & word 1 of line i of list & quote & ">" & ¬
      break to file f
  end repeat
  write "</MAP>" & break & break to file f
  write "</BODY>" & break & break & "</HTML>" & break to file f
  --
  close file f
  --
  put destinationFolder & cName & ".map" into f
  open file f
  repeat with i=1 to the number of lines of list
    write "rect" && word 1 of line i of list && ¬
      item 1 to 2 of word 2 of line i of list && ¬
      item 3 to 4 of word 2 of line i of list & break to file f
  end repeat
  close file f
  --
  set cursor to busy
  put trueScreenshot() into f
  if word one of f is not "ok" then
    return "Could not take screenshot." & return & f  --∆
  end if
  delete word one of f
  --
  set cursor to busy
  put "tell application" && quote & "clip2gif" & quote & return into s
  --
  put "save file" && quote & f & quote && "as " after s
  if ext is "jpeg" then put "JPEG" after s
  else put "GIF" after s
  --
  put " in file" && quote & destinationFolder & cName ¬
    & "." & ext & quote after s
  put " crop {" && rect of card window && "}" after s
  if ext is "jpeg" then put " quality" && quality after s
  --
  put " depth 8 transparency no scale 100" ¬
    && "colors palette input colors" after s
  if ext is "gif" then put " interlacing" && interlace after s
  put  return & "end tell" after s
  --
  if the optionKey is down then answer s
  do s as AppleScript
  --
  set cursor to busy
  eraseFile f
  if the result is not empty then
  return "Could not erase screen shot." ¬
    & return & "eraseFile" && the result  --∆
  end if
end weaveCard

function weaveCardName
  put short name of this card into a
  put empty into cName
  repeat with i=1 to the number of words of a
    put word i of a after cName
  end repeat
  return cName
end weaveCardName
```